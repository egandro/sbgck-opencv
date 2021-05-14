#include <stdlib.h>
#include "tokenshape.hpp"

// main ideas from here: https://www.youtube.com/watch?v=2FYm3GOonhk&t=5933s
vector<ShapeLocation> TokenShape::detectShape(const Mat frame, const Token &token)
{
    //Log(INFO) << "ImageShape detectShape";

    vector<ShapeLocation> result;
    Mat imgGray, imgBlur, imgCanny, imgDil, imgErode, imgContours;

    if(frame.channels() > 1) {
        cvtColor(frame, imgGray, COLOR_BGR2GRAY);
    } else {
        imgGray = frame;
    }

    // preprocessing
    GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
    Canny(imgBlur, imgCanny, 25, 75);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(imgCanny, imgDil, kernel);

    // imshow("frame", frame);
    // imshow("imgGray", imgGray);
    // imshow("imgBlur", imgBlur);
    // imshow("imgCanny", imgCanny);
    // imshow("imgDil", imgDil);
    // waitKey();

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    // https://stackoverflow.com/questions/37691479/opencv-detection-of-rectangle-or-pentagon
    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.size() == 0)
    {
        return result;
    }

    for (unsigned int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]); // area of the contours we detected - here we need the min dimensions
        //if (area < 1000 || area > 5000)         // get the 1000/5000 from somewhere
        //   continue;
        if (area < 800 || area > 5000)
            continue;

        ShapeLocation loc;

        // approximate polygon
        double peri = arcLength(contours[i], true);
        // this uses 0.01 - https://stackoverflow.com/questions/37691479/opencv-detection-of-rectangle-or-pentagon
        // this uses 0.04 - https://stackoverflow.com/questions/60177653/how-to-detect-an-octagonal-shape-in-python-and-opencv
        double epsilon = 0.04 * peri; // original value was 0.02

        // epsilon = 7.5; // suggestion was 3.0
        // int s = contours[i].size();
        // https://docs.opencv.org/3.4/dc/dcf/tutorial_js_contour_features.html < try convexHull here for circle?
        approxPolyDP(contours[i], loc.contours, epsilon, true); // makes octagons of circles

        // bounding box
        loc.boundRect = boundingRect(loc.contours);

        //int rawCor = (int)contours[i].size();
        int objCor = (int)loc.contours.size();
        Geometry objectType = Geometry::None;

        if (objCor == 3)
        {
            objectType = Geometry::Triangle;
        }
        else if (objCor == 4)
        {
            float aspRatio = (float)loc.boundRect.width / (float)loc.boundRect.height;
            if (aspRatio > 0.95 && aspRatio < 1.05)
            {
                objectType = Geometry::Square;
            }
            else
            {
                objectType = Geometry::Rect;
            }
        }
        else if (objCor == 5)
        {
            objectType = Geometry::Pentagon;
        }
        else if (objCor == 6)
        {
            objectType = Geometry::Hexagon;
        }
        else if (objCor > 6)
        {
            objectType = Geometry::Circle;
        }

        if(objectType == Geometry::None) {
            continue;
        }

        if(objectType != token.geometry) {
            continue;
        }

        loc.found = true;
        result.push_back(loc);
    }

    return result;
}