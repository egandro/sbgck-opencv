#include "detector.hpp"
#include "assetdetection.hpp"

bool Detector::calibrateReferenceFrame(Mat &frame, Board &board) {
    Log(typelog::INFO) << "Detector calibrateReferenceFrame";

    Asset detectedBoard;

    if (AssetDetection::detectAsset(frame, board.asset, detectedBoard))
    {
        Log(typelog::DEBUG) << "board detected - verifying";
        Asset tempBoard;

        Asset asset(board.asset.getDefault().image);
        Board boardTemp(asset);

        // the second test we do without the homography
        if (AssetDetection::detectAsset(detectedBoard.getDefault().image, boardTemp.asset, tempBoard))
        {
            Log(typelog::DEBUG) << "board detected";
            // imshow("detectedBoard", detectedBoard.getDefault().image);
            // imshow("tempBoard", tempBoard.getDefault().image);
            // waitKey();
            detectedBoard.getDefault().image.copyTo(board.frameBoardEmpty);
            return true;
        }
    }

    return false;
}
