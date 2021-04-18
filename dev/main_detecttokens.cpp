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
    LOGCFG.prefix = (char *)"main_detecttokens";
    LOGCFG.headers = true;
    LOGCFG.level = DEBUG;

    const std::string boardFile = "/home/pi/projects/pics/board.png";
    const std::string urlOrFileName("/home/pi/projects/pics/frame_arctic_a_in_control_post.png");
    //const std::string urlOrFileName("/home/pi/projects/pics/frame_dummy.png");
    const std::string assetTokenAFile = "/home/pi/projects/pics/token_a.png";
    const std::string assetTokenBFile = "/home/pi/projects/pics/token_b.png";
    const std::string assetTokenCFile = "/home/pi/projects/pics/token_c.png";

    Asset tokenA = AssetManager::addAsset(assetTokenAFile.c_str());
    Asset tokenB = AssetManager::addAsset(assetTokenBFile.c_str());
    Asset tokenC = AssetManager::addAsset(assetTokenCFile.c_str());

    Asset board = AssetManager::addBoard(boardFile.c_str());
    AssetManager::setCurrentBoard(board);

    CameraMode mode = DebugFile;
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
    Mat image = detectedBoard.getDefault().image;

    if(!image.empty()) {
        imwrite("/tmp/board.png", image);
        Rect r = ImageDetection::detectTemplate(detectedBoard, tokenA);
        Log(INFO) << "tokenA: " << r;
    }

    return 0;
}
