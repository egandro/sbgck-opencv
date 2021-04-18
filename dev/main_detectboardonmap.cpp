#include <ctype.h>
#include <opencv2/opencv.hpp>

#include "assetmanager.hpp"
#include "camera.hpp"
#include "imagedetection.hpp"

#include "log.hpp"

using namespace cv;

structlog LOGCFG = {};

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_detectboardonmap";
    LOGCFG.headers = true;
    LOGCFG.level = DEBUG;

    if (argc != 4)
    {
        Log(DEBUG) << "usage: " << argv[0] << " <ImageFileOrUrl> <BoardFile> <OutFile>";
        return -1;
    }

    Asset board = AssetManager::addBoard(argv[2]);
    AssetManager::setCurrentBoard(board);

    CameraMode mode = DebugFile;

    std::string urlOrFileName(argv[1]);

    std::string lower(urlOrFileName);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower.rfind("http", 0) == 0)
    {
        mode = IPCamera;
    }

    CameraConfig cfg = {
        mode,
        urlOrFileName
    };

    Camera camPtr(cfg);
    Mat mat = camPtr.getFrame();
    Asset frame = Asset(mat);

    Asset detectedBoard = ImageDetection::detectBoard(mat, (*AssetManager::getCurrentBoard()));

    //imshow("Good Matches & Object detection", detectedBoard.getDefault().image);
    //waitKey();

    Mat image = detectedBoard.getDefault().image;
    if(image.empty()) {
        Log(WARN) << "got empty board back";
    } else {
        imwrite(argv[3], image);
    }

    return 0;
}
