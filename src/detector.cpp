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

bool Detector::queryTokens(DetectorTokenConfig &cfg)
{
    Log(typelog::INFO) << "Detector queryTokens";

    Log(typelog::INFO) << "Detector queryTokens detecting board";
    Asset detectedBoard;
    if (!AssetDetection::detectAsset(*(cfg.frame), cfg.board->asset, detectedBoard))
    {
        Log(typelog::ERR) << "detectedBoard failed";
        return false;
    }

    Mat boardImage = detectedBoard.getDefault().image;

    // we might have to apply the histogram of the frameBoardEmpty
    // to the detected board to avoid unstable colors

    Mat diff = ImageDiff::removeBackground(boardImage, cfg.board->frameBoardEmpty);
    // imshow("detectedBoard", detectedBoard.getDefault().image);
    // imshow("diff", diff);
    // waitKey(0);

    if (cfg.showAllROIs)
    {
        Mat roiMask = Mat(boardImage.size().height, boardImage.size().width, CV_8UC1, Scalar(0, 0, 0));
        if (cfg.board->roiManager.addToMask(roiMask /*, "#yard"*/))
        {
            Mat roiImage;
            boardImage.copyTo(roiImage);

            Mat rois;
            bitwise_and(roiImage, roiImage, rois, roiMask);

            imshow("rois", rois);
            waitKey(0);
        }
    }

    for (size_t i = 0; i < cfg.tokens.size(); i++)
    {
        Token token = *(cfg.tokens[i]);

        if (token.tokenDetector == TokenDetector::None)
        {
            Log(typelog::WARN) << "token has no token detector set - skiping " << token.name;
            continue;
        }
        if (token.tokenDetector == TokenDetector::Asset)
        {
            Log(typelog::WARN) << "token asset detector is not implemented, yet " << token.name;
            continue;
        }

        //Mat color = TokenColor::detectColor(boardImage, token);
        Mat color = TokenColor::detectColor(diff, token);
        if (cfg.showColorDiff)
        {
            //imshow("boardImage", boardImage);
            imshow("diff", diff);
            imshow("color (per token)", color);
            waitKey(0);
        }

        //Mat mask = diff;
        Mat mask = color;

        /// shape detection
        const vector<ShapeLocation> locs = TokenShape::detectShape(mask, token);

        if (cfg.showContours)
        {
            Mat contourImage;
            boardImage.copyTo(contourImage);

            for (size_t i = 0; i < locs.size(); i++)
            {
                ShapeLocation loc = locs[i];

                // we need a vector of vector of points
                vector<vector<Point>> conPoly;
                conPoly.push_back(loc.contours);

                // draw contour according to the approxymated polygon
                drawContours(contourImage, conPoly, 0, Scalar(255, 0, 255), 2);

                // bounding box
                // rectangle(contourImage, loc.boundRect.tl(), loc.boundRect.br(), Scalar(0, 255, 0), 2);

                string name = token.name;
#ifdef DRAW_COLOR
                // text what we detected
                name = getGeometryString(token.geometry);
                name += "(";
                name += std::to_string((int)token.color[2]);
                name += ",";
                name += std::to_string((int)token.color[0]);
                name += ",";
                name += std::to_string((int)token.color[1]);
                name += ")";
#endif
                putText(contourImage, name, {loc.boundRect.x, loc.boundRect.y - 5}, FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 69, 255), 1);
            }
            imshow("contourImage", contourImage);
            waitKey(0);
        }

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
                std::string regionName = cfg.board->roiManager.getRegion(loc.boundRect);

                if (!regionName.empty())
                {
                    Log(typelog::DEBUG) << "token (" << token.name << ") is in region: " << regionName;
                }
                else
                {
                    Log(typelog::DEBUG) << "token (" << token.name << ")";
                }

                if (cfg.ROIs.size() > 0)
                {
                    if (find(cfg.ROIs.begin(), cfg.ROIs.end(), regionName) == cfg.ROIs.end())
                    {
                        // user want's to filter and ROI doesn't match
                        continue;
                    }
                }

                // the result
                pair<std::string, std::string> item;
                item.first = regionName;
                item.second = token.name;
                cfg.result.push_back(item);
            }
        }
    }

    return true;
}
