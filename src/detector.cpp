#include <algorithm>
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

    Log(typelog::INFO) << "Detector queryTokens detecting board";
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
    // imshow("detectedBoard", detectedBoard.getDefault().image);
    // imshow("diff", diff);
    // waitKey(0);

    for (size_t i = 0; i < tokens.size(); i++)
    {
        Token token = *(tokens[i]);

        //Mat color = TokenColor::detectColor(boardImage, token);
        Mat color = TokenColor::detectColor(diff, token);
        imshow("frame", frame);
        imshow("detectedBoard", detectedBoard.getDefault().image);
        imshow("diff", diff);
        imshow("color", color);
        waitKey(0);

        //Mat mask = diff;
        Mat mask = color;

        /// shape detection
        const vector<ShapeLocation> locs = TokenShape::detectShape(mask, token);

#ifdef DRAW_ROIS
        // ROIs
        Mat roiMask = Mat(boardImage.size().height, boardImage.size().width, CV_8UC1, Scalar(0, 0, 0));
        if (board.roiManager.addToMask(roiMask /*, "#yard"*/))
        {
            //if(board.roiManager.addToMask(mask, "#livingRoom")) {
            Mat copy;
            detectedBoard.getDefault().image.copyTo(copy);

            Mat res;
            bitwise_and(copy, copy, res, roiMask);
            boardImage = res;

            // imshow("res", res);
            // imshow("detectedBoard", detectedBoard.getDefault().image);
            // waitKey(0);
        }
#endif

#ifdef DRAW_CONTOURS_AND_TEXTS
        // Using a for loop with iterator
        for (size_t i = 0; i < locs.size(); i++)
        {
            ShapeLocation loc = locs[i];

            // we need a vector of vector of points
            vector<vector<Point>> conPoly;
            conPoly.push_back(loc.contours);

            // draw contour according to the approxymated polygon
            drawContours(boardImage, conPoly, 0, Scalar(255, 0, 255), 2);

            // bounding box
            // rectangle(boardImage, loc.boundRect.tl(), loc.boundRect.br(), Scalar(0, 255, 0), 2);

            // text what we detected
            string name = getGeometryString(token.geometry);
            name += "(";
            name += std::to_string((int)token.color[2]);
            name += ",";
            name += std::to_string((int)token.color[0]);
            name += ",";
            name += std::to_string((int)token.color[1]);
            name += ")";

            string name = token.name;
            putText(boardImage, name, {loc.boundRect.x, loc.boundRect.y - 5}, FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 69, 255), 1);
        }
#endif

        if (locs.size() > 0)
        {
            // imshow("detectedBoard", boardImage);
            // imshow("diff", diff);
            // imshow("mask with color", mask);
            // waitKey();
            Log(typelog::DEBUG) << "detected " << locs.size() << " tokens";

            for (size_t i = 0; i < locs.size(); i++)
            {
                ShapeLocation loc = locs[i];
                std::string regionName = board.roiManager.getRegion(loc.boundRect);

                if (!regionName.empty())
                {
                    Log(typelog::DEBUG) << "token (" << token.name << ") is in region: " << regionName;
                }
                else
                {
                    Log(typelog::DEBUG) << "token (" << token.name << ")";
                }

                if (ROIs.size() > 0)
                {
                    if (find(ROIs.begin(), ROIs.end(), regionName) == ROIs.end())
                    {
                        // user want's to filter and ROI doesn't match
                        continue;
                    }
                }

                // the result
                pair<std::string, std::string> item;
                item.first = regionName;
                item.second = token.name;
                result.push_back(item);
            }
        }
    }

    return true;
}
