#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
// https://stackoverflow.com/questions/11238918/s-isreg-macro-undefined
#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m)&S_IFMT) == S_IFDIR)
#endif
#include <iostream>
#include <fstream>
#include <regex>
#include <nlohmann/json.hpp>
#include <log.hpp>

#include "board.hpp"
#include "token.hpp"
#include "camera.hpp"
#include "imagedetection.hpp"

using json = nlohmann::json;

structlog LOGCFG = {};

// config json
/*
{
    "name": "arctic",
    "camUrl": "http://zte:8080/video",
    "boardFile": "../sbgck-dev-game/game/boards/Arctic-base.png",
    "boardMapFile": "../sbgck-dev-game/game/boards/Arctic-base.json",
    "colorCheckerFile": "../sbgck-dev-game/game/assets/color_checker.png",
    "outFolder": "./build",
    "tokenGeometry": "Circle",
    "color": "255,0,0"
}
*/

class MyConfig
{
public:
    string boardFile;
    string boardMapFile;
    string camUrl;
    string colorCheckerFile;
    string name;
    string outFolder;
    Geometry tokenGeometry;
    Scalar color;
} myConfig;

static bool parseConfig(const char *fileName);
static bool checkOutDir();

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_token_detect_loop";
    LOGCFG.headers = true;
    LOGCFG.level = DEBUG;

    if (argc != 2)
    {
        Log(DEBUG) << "usage: " << argv[0] << " <ConfigFile>";
        return -1;
    }

    if (!parseConfig(argv[1]) || !checkOutDir())
        return 1;

    // Open Camera
    CameraConfig cfg = {
        IPCamera,
        myConfig.camUrl};

    // open the camera
    Camera cam(cfg);

    // open the board

    Board board;
    board.asset = Asset(myConfig.boardFile);
    board.roiManager.initFromJsonFile(myConfig.boardMapFile);

    Board boardColorChecker;
    boardColorChecker.asset = Asset(myConfig.colorCheckerFile);

    // imshow("colorChecker", colorChecker);
    // waitKey();

    Token token;
    token.geometry = myConfig.tokenGeometry;
    token.color = myConfig.color;

    Mat emptyFrame;
    Mat colorCheckerFrame;

    while(true) {
        Mat frame = cam.getFrame();
        // Log(DEBUG) << "read frame";
        // imshow("frame", frame);
        // waitKey();

        if(emptyFrame.size().height == 0) {
            Asset detectedBoard;
            if(ImageDetection::detectBoard(frame, board, detectedBoard)) {
                Log(DEBUG) << "board detected";
                emptyFrame = detectedBoard.getDefault().image;
            }
            continue;
        }

        // does not work
        // maybe we have to go the CCM route (I hate this!)

        // if(colorCheckerFrame.size().height == 0) {
        //     // Log(DEBUG) << "read frame";
        //     // imshow("frame", frame);
        //     // imshow("boardColorChecker", boardColorChecker.asset.getDefault().image);
        //     // waitKey();
        //     Asset detectedBoard;
        //     if(ImageDetection::detectBoard(frame, boardColorChecker, detectedBoard)) {
        //         Log(DEBUG) << "boardColorChecker detected";
        //         colorCheckerFrame = detectedBoard.getDefault().image;
        //         imshow("colorCheckerFrame", colorCheckerFrame);
        //         waitKey();
        //         break;
        //     }
        //     continue;
        // }

        Asset detectedBoard;
        if(!ImageDetection::detectBoard(frame, board, detectedBoard)) {
            continue;
        }

        Mat mask = Mat(board.asset.getDefault().image.size().height, board.asset.getDefault().image.size().width, CV_8UC1, Scalar(0, 0, 0));
        //if(board.roiManager.addToMask(mask /*, "#yard"*/)) {
        if(board.roiManager.addToMask(mask, "#controlPost")) {
            Mat copy;
            detectedBoard.getDefault().image.copyTo(copy);

            Mat region;
            bitwise_and(copy, copy, region, mask);
            imshow("region", region);
            imshow("board", board.asset.getDefault().image);
            waitKey(0);
        }
    }



    // Load Assets, Board, Map, color calibration card (todo make this in software)

    // ** PREPARATION ***
    // check if saved state exist - if not
    // get Initial calibration image
    // get color checker image
    // save this state - to outFolder

    // Load saved state

    // ** Main Loop **
    // capture frame
    // diff frame
    // apply color correction
    // detect token
    // print ROI

    return 0;
}

static bool parseConfig(const char *fileName)
{
    ifstream ifs(fileName);

    if (ifs.fail())
    {
        Log(ERROR) << fileName << " could not be opened";
        return false;
    }

    string jsonStr((std::istreambuf_iterator<char>(ifs)),
                   (std::istreambuf_iterator<char>()));
    ifs.close();

    json j = json::parse(jsonStr.c_str());
    //Log(DEBUG) << j.dump(4);

    // unchecked!
    myConfig.boardFile = j["boardFile"].get<std::string>();
    myConfig.boardMapFile = j["boardMapFile"].get<std::string>();
    myConfig.camUrl = j["camUrl"].get<std::string>();
    myConfig.colorCheckerFile = j["colorCheckerFile"].get<std::string>();
    myConfig.name = j["name"].get<std::string>();
    myConfig.outFolder = j["outFolder"].get<std::string>();

    string tokenGeometry = j["tokenGeometry"].get<std::string>();
    if (tokenGeometry== "Triangle")
    {
        myConfig.tokenGeometry = Geometry::Triangle;
    }
    else if (tokenGeometry== "Square")
    {
        myConfig.tokenGeometry = Geometry::Square;
    }
    else if (tokenGeometry== "Rect")
    {
        myConfig.tokenGeometry = Geometry::Rect;
    }
    else if (tokenGeometry== "Hexagon")
    {
        myConfig.tokenGeometry = Geometry::Hexagon;
    }
    else if (tokenGeometry== "Pentagon")
    {
        myConfig.tokenGeometry = Geometry::Pentagon;
    }
    else if (tokenGeometry== "Circle")
    {
        myConfig.tokenGeometry = Geometry::Circle;
    }

    string color = j["color"].get<std::string>();

    // strip whitespace
    std::regex r("\\s+");
    color = std::regex_replace(color, r, "");

    std::vector<int> vect;
    std::stringstream ss(color);

    for (int i; ss >> i;)
    {
        vect.push_back(i);
        if (ss.peek() == ',')
            ss.ignore();
    }

    // we need to convert this into BRG from RGB
    myConfig.color = Scalar(vect.at(2), vect.at(0), vect.at(1));

    return true;
}

static bool checkOutDir()
{
    struct stat sb;

    if (stat(myConfig.outFolder.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        return true;
    }

    Log(ERROR) << "outfolder: " << myConfig.outFolder << " does not exists ";
    return false;
}
