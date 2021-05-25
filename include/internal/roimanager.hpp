#ifndef ROIMANAGER_H
#define ROIMANAGER_H

#include "../log.hpp"
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
    RoiManager()
    {
    }

    RoiManager(const RoiManager &value)
    {
        for (std::size_t i = 0; i < value.circles.size(); ++i)
        {
            RegionCircle r = value.circles[i];
            circles.push_back(r);
        }

        for (std::size_t i = 0; i < value.rects.size(); ++i)
        {
            RegionRect r = value.rects[i];
            rects.push_back(r);
        }

        for (std::size_t i = 0; i < value.polys.size(); ++i)
        {
            RegionPoly r = value.polys[i];
            polys.push_back(r);
        }
    }

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

    std::string getRegion(const Point p);

    std::string getRegion(const Rect r);

    bool addToMask(Mat &mask, std::string areaName = "");
};

#endif