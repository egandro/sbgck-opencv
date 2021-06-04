#include <algorithm>
#include "detector.hpp"
#include "assetdetection.hpp"
#include "imagediff.hpp"
#include "tokenshape.hpp"
#include "tokencolor.hpp"

bool Detector::calibrateReferenceFrame(Mat &frame, Board &board, const double histogramCorrelationMin)
{
    Log(typelog::INFO) << "Detector calibrateReferenceFrame";

    Asset detectedBoard;

    if (AssetDetection::detectAsset(frame, board.asset, detectedBoard, false, histogramCorrelationMin))
    {
        Log(typelog::DEBUG) << "board detected - verifying";
        Asset tempBoard;

        Asset asset(board.asset.getDefault().image);
        Board boardTemp(asset);

        // the second test we do without the homography
        if (AssetDetection::detectAsset(detectedBoard.getDefault().image, boardTemp.asset, tempBoard, false, histogramCorrelationMin))
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
                    Log(typelog::DEBUG) << " detected a token (" << token.name << ") but it's not in a ROI";
                    // we don't need this
                    continue;
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

bool Detector::detectReferenceImage(Mat &frame, Asset &reference, Mat &result, const double histogramCorrelationMin)
{
    Log(typelog::INFO) << "Detector detectReferenceImage";

    Asset detectedAsset;

    if (AssetDetection::detectAsset(frame, reference, detectedAsset, false, histogramCorrelationMin))
    {
        Log(typelog::DEBUG) << "asset detected - verifying";
        Asset tempAsset;

        Asset asset(reference.getDefault().image);
        Board boardTemp(asset);

        // the second test we do without the homography
        if (AssetDetection::detectAsset(detectedAsset.getDefault().image, boardTemp.asset, tempAsset, false, histogramCorrelationMin))
        {
            Log(typelog::DEBUG) << "asset detected";
            // imshow("detectedAsset", detectedAsset.getDefault().image);
            // imshow("tempAsset", tempAsset.getDefault().image);
            // waitKey();
            detectedAsset.getDefault().image.copyTo(result);
            return true;
        }
    }

    return false;
}

bool Detector::calibrateColorMap(Mat &frame, Asset &reference,
                                 Mat &destination,
                                 std::vector<std::vector<Scalar>> referenceColors,
                                 ColorMap &result, const int border,
                                 const double segmentPercentage)
{
    Log(typelog::INFO) << "Detector calibrateColorMap";

    if (reference.getDefault().image.empty())
    {
        Log(typelog::ERR) << "reference image is empty";
        return false;
    }

    reference.getDefault().image.copyTo(destination);

    const int max_y = (int)referenceColors.size();
    if (max_y == 0)
    {
        return true;
    }

    const int max_x = (int)referenceColors[0].size();
    if (max_x == 0)
    {
        return true;
    }

    if (segmentPercentage < 0.0 || segmentPercentage > 1.0)
    {
        Log(typelog::ERR) << "Detector calibrateColorMap - segmentPercentage must be between 0.0 and 1.0";
        return false;
    }

    Mat cameraColorMapImage;

    // detect the reference colormap image without a histogram (that's because we are doing that)
    if (!Detector::detectReferenceImage(frame, reference, cameraColorMapImage, 0.0))
    {
        Log(typelog::ERR) << "can't detect reference color map in frame";
        return false;
    }

    cameraColorMapImage.copyTo(destination);

    const int segment_x = (int)((double)(reference.getDefault().image.cols - 2 * border) / (double)max_x);
    const int segment_y = (int)((double)(reference.getDefault().image.rows - 2 * border) / (double)max_y);

    const int segment_x_size = (int)((double)segment_x * (double)segmentPercentage);
    const int segment_y_size = (int)((double)segment_y * (double)segmentPercentage);

    const int offset_x = (int)((double)(segment_x - segment_x_size) / (double)2);
    const int offset_y = (int)((double)(segment_y - segment_y_size) / (double)2);

    for (int y = 0; y < max_y; y++)
    {
        for (int x = 0; x < max_x; x++)
        {
            // cut a rect from the detected frame
            Rect rect(x * segment_x + offset_x + border, y * segment_y + offset_y + border, segment_x_size, segment_y_size);

            // get the mean color
            Mat image_roi = cameraColorMapImage(rect);
            Scalar meanColor = mean(image_roi);

            if ((size_t)y >= referenceColors.size())
            {
                // out of bound read
                continue;
            }

            if ((size_t)x >= referenceColors[y].size())
            {
                // out of bound read
                continue;
            }

            // original color
            Scalar refColor = referenceColors[y][x];

            // map the colors
            result.setMappedColor(refColor, meanColor);


            Log(typelog::INFO) << "Mapping refColor: " << refColor << " to: " << meanColor << " x: " << x << " y: " << y;

            //rectangle(destination, rect, cv::Scalar(0, 255, 0));
            //Rect rect2(x * segment_x + border, y * segment_y + border, segment_x, segment_y);
            //rectangle(reference.getDefault().image, rect2, cv::Scalar(255, 0, 255));
            //rectangle(reference.getDefault().image, rect2, meanColor, FILLED);

            rectangle(destination, rect, meanColor, FILLED);
            rectangle(destination, rect, cv::Scalar(255, 0, 255));
        }
    }

    return true;
}
