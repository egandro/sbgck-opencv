#ifndef ROIMANAGER_H
#define ROIMANAGER_H

#include "log.hpp"
#include "opencv2/opencv.hpp"

#include "roi/regioncircle.hpp"
#include "roi/regionpoly.hpp"
#include "roi/regionrect.hpp"

using namespace cv;

class RoiManager
{
private:
    std::list<RegionCircle> circles;
    std::list<RegionRect> rects;
    std::list<RegionPoly> polygons;

    bool parseCircle(const std::string areaName, const std::string coords, RegionCircle &circle);
    bool parseRect(const std::string areaName, const std::string coords, RegionRect &rect);
    bool parsePoly(const std::string areaName, const std::string coords, RegionPoly &rect);

public:
    void initFromJsonString(const std::string jsonStr);
};

#endif