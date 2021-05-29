#include "detector.hpp"
#include "assetdetection.hpp"
#include "imagediff.hpp"
#include "tokenshape.hpp"
#include "tokencolor.hpp"

bool Detector::calibrateReferenceFrame(Mat &frame, Board &board)
{
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

bool Detector::queryTokens(Mat &frame, Board &board,
                           std::vector<const Token *> tokens, std::vector<string> ROIs, std::vector<pair<std::string, std::string>> &result)
{
    Log(typelog::INFO) << "Detector queryTokens";

    Asset detectedBoard;
    if (!AssetDetection::detectAsset(frame, board.asset, detectedBoard))
    {
        Log(typelog::ERR) << "detectedBoard failed";
        return false;
    }

    Mat boardImage = detectedBoard.getDefault().image;

    // we might have to apply the histogram of the frameBoardEmpty
    // to the detected board to avoid unstable colors

    Mat diff = ImageDiff::removeBackground(boardImage, board.frameBoardEmpty);
    imshow("detectedBoard", detectedBoard.getDefault().image);
    imshow("diff", diff);
    waitKey(0);

    return false;
}
