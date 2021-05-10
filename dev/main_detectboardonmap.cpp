#include <ctype.h>
#include <opencv2/opencv.hpp>
#include <log.hpp>

#include "camera.hpp"
#include "imagedetection.hpp"

using namespace cv;

structlog LOGCFG = {};

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_detectboardonmap";
    LOGCFG.headers = true;
    LOGCFG.level = typelog::DEBUG;

    if (argc != 4)
    {
        Log(typelog::DEBUG) << "usage: " << argv[0] << " <ImageFileOrUrl> <BoardFile> <OutFile>";
        return -1;
    }

    Board board;
    board.asset = Asset(argv[2]);

    CameraMode mode = CameraMode::DebugFile;

    std::string urlOrFileName(argv[1]);

    std::string lower(urlOrFileName);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower.rfind("http", 0) == 0)
    {
        mode = CameraMode::IPCamera;
    }

    CameraConfig cfg = {
        mode,
        urlOrFileName
    };

    Camera camPtr(cfg);
    Mat frame = camPtr.getFrame();

    Asset detectedBoard;
    ImageDetection::detectBoard(frame, board, detectedBoard);

    //imshow("Good Matches & Object detection", detectedBoard.getDefault().image);
    //waitKey();

    Mat image = detectedBoard.getDefault().image;
    if(image.empty()) {
        Log(typelog::WARN) << "got empty board back";
    } else {
        imwrite(argv[3], image);
    }

    return 0;
}
