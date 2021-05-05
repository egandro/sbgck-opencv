#ifndef ROIMANAGER_H
#define ROIMANAGER_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

#include "regioncircle.hpp"
#include "regionpoly.hpp"
#include "regionrect.hpp"

using namespace cv;

class RoiManager
{
private:
    std::vector<RegionCircle> circles;
    std::vector<RegionRect> rects;
    std::vector<RegionPoly> polys;

    bool parseCircle(const std::string areaName, const std::vector<int> coords, RegionCircle &circle);
    bool parseRect(const std::string areaName, const std::vector<int> coords, RegionRect &rect);
    bool parsePoly(const std::string areaName, const std::vector<int> coords, RegionPoly &poly);

public:
    ~RoiManager()
    {
        circles.clear();
        rects.clear();
        polys.clear();
    }

    bool initFromJsonFile(const std::string fileName);

    bool initFromJsonString(const std::string jsonStr);

    bool isInsideRegion(const Point p, std::string &areaName);

    bool isInsideRegion(const Rect r, std::string &areaName);

    bool addToMask(Mat &mask, std::string areaName="");
};

#endif