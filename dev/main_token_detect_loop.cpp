#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
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
#include "assetdetection.hpp"
#include "imagediff.hpp"
#include "tokenshape.hpp"
#include "tokencolor.hpp"
#include "mathelper.hpp"

using json = nlohmann::json;

structlog LOGCFG = {};

// config json
/*
{
    "name": "arctic",
    "url": "http://zte:8080/video",
    "boardFile": "../sbgck-dev-game/game/boards/Arctic-base.png",
    "boardMapFile": "../sbgck-dev-game/game/boards/Arctic-base.json",
    "colorCheckerFile": "./tests/images/color_checker.png",
    "outFolder": "./build",
    "tokens": [
        {
            "name": "Red Pentagon",
            "tokenGeometry": "Pentagon",
            "color": "232,50,59",
        }
    ]
}
*/

class MyConfig
{
public:
    string boardFile;
    string boardMapFile;
    string url;
    string colorCheckerFile;
    string name;
    string outFolder;
    vector<Token> tokens;
} myConfig;

static bool parseConfig(const char *fileName);
static bool checkOutDir();

inline bool exists_test(const std::string &name)
{
    ifstream f(name.c_str());
    return f.good();
}

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_token_detect_loop";
    LOGCFG.headers = true;
    LOGCFG.level = typelog::DEBUG;

    if (argc != 2)
    {
        Log(typelog::DEBUG) << "usage: " << argv[0] << " <ConfigFile>";
        return -1;
    }

    if (!parseConfig(argv[1]) || !checkOutDir())
        return 1;

    // camera config
    CameraConfig cfg(CameraMode::IPCamera, myConfig.url);

    // open the camera
    Camera cam;
    if (!cam.open(cfg))
    {
        Log(typelog::ERR) << "can't open camera";
        return 1;
    }

    // open the board
    Asset assetBoard(myConfig.boardFile);
    Board board(assetBoard);
    board.roiManager.initFromJsonFile(myConfig.boardMapFile);

    Asset assetColorChecker(myConfig.colorCheckerFile);
    Board boardColorChecker(assetColorChecker);

    // imshow("colorChecker", colorChecker);
    // waitKey();

    Mat colorCheckerFrame;

    // debug hack
    if (exists_test(myConfig.outFolder + "/frameBoardEmpty.png"))
    {
        board.frameBoardEmpty = imread(myConfig.outFolder + "/frameBoardEmpty.png", IMREAD_COLOR);
    }

    if (exists_test(myConfig.outFolder + "/homography.homo"))
    {
        // not an image (!)
        MatHelper::LoadMatBinary(myConfig.outFolder + "/homography.homo", board.asset.homography);
        //Log(typelog::INFO) <<  "restored homography : \n" << board.homography;
    }

    int empty = 0;

    while (true)
    {
        Mat frame;
        if (!cam.getFrame(frame))
        {
            Log(typelog::DEBUG) << "frame from camera is empty";
            empty++;
            int sleepTime = 1;
#ifdef _WIN32
            Sleep(sleepTime * 1000);
#else
            sleep(sleepTime);
#endif
            if (empty > 3)
            {
                Log(typelog::DEBUG) << "can't read from camera - resetting";
                cam.reset();
            }
            continue;
        }
        empty = 0;

        // create empty reference board if it does not exist
        if (board.frameBoardEmpty.size().height == 0)
        {
            Asset detectedBoard;
            if (AssetDetection::detectAsset(frame, board.asset, detectedBoard))
            {
                Log(typelog::DEBUG) << "board detected - verifying";
                Asset tempBoard;

                Board boardTemp(assetBoard);

                // the second test we do without the homography
                if (AssetDetection::detectAsset(detectedBoard.getDefault().image, boardTemp.asset, tempBoard))
                {
                    Log(typelog::DEBUG) << "board detected";

                    // imshow("detectedBoard", detectedBoard.getDefault().image);
                    // imshow("tempBoard", tempBoard.getDefault().image);
                    // waitKey();

                    detectedBoard.getDefault().image.copyTo(board.frameBoardEmpty);
                    imwrite(myConfig.outFolder + "/frameBoardEmpty.png", board.frameBoardEmpty);
                    MatHelper::SaveMatBinary(myConfig.outFolder + "/homography.homo", board.asset.homography);
                }
            }
            continue;
        }

        // does not work
        // maybe we have to go the CCM route (I hate this!)

        // if(colorCheckerFrame.size().height == 0) {
        //     // Log(typelog::DEBUG) << "read frame";
        //     // imshow("frame", frame);
        //     // imshow("boardColorChecker", boardColorChecker.asset.getDefault().image);
        //     // waitKey();
        //     Asset detectedBoard;
        //     if(ImageDetection::detectBoard(frame, boardColorChecker, detectedBoard)) {
        //         // Log(typelog::DEBUG) << "boardColorChecker detected";
        //         // colorCheckerFrame = detectedBoard.getDefault().image;
        //         // imshow("colorCheckerFrame", colorCheckerFrame);
        //         // waitKey();
        //         // break;

        //         Log(typelog::DEBUG) << "board boardColorChecker - verifying";
        //         Asset tempBoard;

        //         Board boardTemp;
        //         boardTemp.asset = Asset(boardColorChecker.asset.getDefault().image);

        //         // the second test we do without the homography
        //         if (ImageDetection::detectBoard(detectedBoard.getDefault().image, boardTemp, tempBoard))
        //         {
        //             Log(typelog::DEBUG) << "board boardColorChecker";

        //             imshow("detectedBoard", detectedBoard.getDefault().image);
        //             imshow("tempBoard", tempBoard.getDefault().image);
        //             waitKey();

        //             detectedBoard.getDefault().image.copyTo(colorCheckerFrame);
        //             imwrite(myConfig.outFolder + "/frameColorChecker.png", colorCheckerFrame);

        //             continue;
        //         }
        //     }
        //     continue;
        // }

        Asset detectedBoard;
        if (!AssetDetection::detectAsset(frame, board.asset, detectedBoard))
        {
            continue;
        }

        Mat boardImage = detectedBoard.getDefault().image;

        // we might have to apply the histogram of the frameBoardEmpty
        // to the detected board to avoid unstable colors

        Mat diff = ImageDiff::removeBackground(boardImage, board.frameBoardEmpty);
        // imshow("detectedBoard", detectedBoard.getDefault().image);
        // imshow("diff", diff);
        // // imwrite("./diff.png", diff);
        // waitKey(0);


        for (auto itToken = std::begin(myConfig.tokens); itToken != std::end(myConfig.tokens); ++itToken) {

            //Mat color = TokenColor::detectColor(boardImage, token);
            Mat color = TokenColor::detectColor(diff, (*itToken));
            // imshow("diff", diff);
            // imshow("color", color);
            // waitKey(0);

            //Mat mask = diff;
            Mat mask = color;

            /// shape detection
            const vector<ShapeLocation> locs = TokenShape::detectShape(mask, (*itToken));

            // // ROIs
            // Mat roiMask = Mat(boardImage.size().height, boardImage.size().width, CV_8UC1, Scalar(0, 0, 0));
            // if (board.roiManager.addToMask(roiMask /*, "#yard"*/))
            // {
            //     //if(board.roiManager.addToMask(mask, "#livingRoom")) {
            //     Mat copy;
            //     detectedBoard.getDefault().image.copyTo(copy);

            //     Mat res;
            //     bitwise_and(copy, copy, res, roiMask);
            //     boardImage = res;

            //     // imshow("res", res);
            //     // imshow("detectedBoard", detectedBoard.getDefault().image);
            //     // waitKey(0);
            // }

            // Using a for loop with iterator
            for (auto it = std::begin(locs); it != std::end(locs); ++it)
            {
                // we need a vector of vector of points
                vector<vector<Point>> conPoly;
                conPoly.push_back((*it).contours);

                // draw contour according to the approxymated polygon
                drawContours(boardImage, conPoly, 0, Scalar(255, 0, 255), 2);

                // bounding box
                // rectangle(boardImage, (*it).boundRect.tl(), (*it).boundRect.br(), Scalar(0, 255, 0), 2);

                // // text what we detected
                // string name = getGeometryString((*itToken).geometry);
                // name += "(";
                // name += std::to_string((int)(*itToken).color[2]);
                // name += ",";
                // name += std::to_string((int)(*itToken).color[0]);
                // name += ",";
                // name += std::to_string((int)(*itToken).color[1]);
                // name += ")";

                string name = (*itToken).name;
                putText(boardImage, name, {(*it).boundRect.x, (*it).boundRect.y - 5}, FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 69, 255), 1);
            }

            if (locs.size() > 0)
            {
                // imshow("detectedBoard", boardImage);
                // imshow("diff", diff);
                // imshow("mask with color", mask);
                // waitKey();
                Log(typelog::DEBUG) << "detected " << locs.size() << " tokens";

                for (std::vector<ShapeLocation>::const_iterator it = locs.begin();
                    it != locs.end();
                    ++it)
                {
                    std::string regionName = board.roiManager.getRegion((*it).boundRect);
                    if (!regionName.empty())
                    {
                        Log(typelog::DEBUG) << "token (" << (*itToken).name << ") is in region: " << regionName;
                    } else {
                        Log(typelog::DEBUG) << "token (" << (*itToken).name << ")";
                    }
                }
            }
        }

        // imshow("detectedBoard", boardImage);
        // waitKey();
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
        Log(typelog::ERR) << fileName << " could not be opened";
        return false;
    }

    string jsonStr((std::istreambuf_iterator<char>(ifs)),
                   (std::istreambuf_iterator<char>()));
    ifs.close();

    json j = json::parse(jsonStr.c_str());
    //Log(typelog::DEBUG) << j.dump(4);

    // unchecked!
    myConfig.boardFile = j["boardFile"].get<std::string>();
    myConfig.boardMapFile = j["boardMapFile"].get<std::string>();
    myConfig.url = j["url"].get<std::string>();
    myConfig.colorCheckerFile = j["colorCheckerFile"].get<std::string>();
    myConfig.name = j["name"].get<std::string>();
    myConfig.outFolder = j["outFolder"].get<std::string>();

    for (json::iterator it = j["tokens"].begin(); it != j["tokens"].end(); ++it) {

        Token token;

        string tokenGeometry = (*it)["tokenGeometry"].get<std::string>();

        if (tokenGeometry == "Triangle")
        {
            token.geometry = Geometry::Triangle;
        }
        else if (tokenGeometry == "Square")
        {
            token.geometry = Geometry::Square;
        }
        else if (tokenGeometry == "Rect")
        {
            token.geometry = Geometry::Rect;
        }
        else if (tokenGeometry == "Pentagon")
        {
            token.geometry = Geometry::Pentagon;
        }
        else if (tokenGeometry == "Hexagon")
        {
            token.geometry = Geometry::Hexagon;
        }
        else if (tokenGeometry == "Circle")
        {
            token.geometry = Geometry::Circle;
        }

        if(token.geometry == Geometry::None) {
            continue;
        }

        string color = (*it)["color"].get<std::string>();

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

        // we need to convert this into BGR from RGB
        token.color = Scalar(vect.at(2), vect.at(1), vect.at(0));
        string name = (*it)["name"].get<std::string>();
        token.name = name;

        myConfig.tokens.push_back(token);
    }

    return true;
}

static bool checkOutDir()
{
    struct stat sb;

    if (stat(myConfig.outFolder.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        return true;
    }

    Log(typelog::ERR) << "outfolder: " << myConfig.outFolder << " does not exists ";
    return false;
}
