#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "roimanager.hpp"
#include "log.hpp"

using json = nlohmann::json;

bool RoiManager::parseCircle(const std::string areaName, const std::vector<int> coords, RegionCircle &circle)
{
    // <area shape="circle" coords="112,144,47" target="#circle" href="#circle" />
    if (coords.size() != 3)
    {
        return false;
    }

    circle.areaName = areaName;
    circle.center.x = coords.at(0);
    circle.center.y = coords.at(1);
    circle.radius = coords.at(2);

    return true;
}

bool RoiManager::parseRect(const std::string areaName, const std::vector<int> coords, RegionRect &rect)
{
    // <area shape="poly" coords="230,503,114,504,115,679,300,684,301,609,227,607" target="#poly" href="#poly" />
    if (coords.size() != 4)
    {
        return false;
    }

    rect.areaName = areaName;
    rect.tl.x = coords.at(0);
    rect.tl.y = coords.at(1);
    rect.br.x = coords.at(2);
    rect.br.y = coords.at(3);

    return true;
}

bool RoiManager::parsePoly(const std::string areaName, const std::vector<int> coords, RegionPoly &poly)
{
    // <area shape="poly" coords="230,503,114,504,115,679,300,684,301,609,227,607" target="#poly" href="#poly" />
    if (coords.size() < 6 || (coords.size() % 2) != 0)
    { // we need 3 points and they have to be pairs
        return false;
    }

    poly.areaName = areaName;

    int last = 0;
    for (std::size_t i = 0; i < coords.size(); i++)
    {
        if (i % 2 == 1)
        {
            Point p;
            p.x = last;
            p.y = coords[i];
            poly.points.push_back(p);
        }
        last = coords[i];
    }

    return true;
}

bool RoiManager::initFromJsonFile(const std::string fileName)
{
    Log(typelog::INFO) << "RoiManager initFromFile " << fileName;

    ifstream ifs(fileName);

    if (ifs.fail())
    {
        Log(typelog::ERR) << fileName << " could not be opened";
        return false;
    }

    string jsonStr((std::istreambuf_iterator<char>(ifs)),
                   (std::istreambuf_iterator<char>()));
    ifs.close();

    return initFromJsonString(jsonStr);
}

bool RoiManager::initFromJsonString(const std::string jsonStr)
{
    Log(typelog::INFO) << "RoiManager initFromJsonString";
    circles.clear();
    rects.clear();
    polys.clear();

    // https://github.com/nlohmann/json

    json j = json::parse(jsonStr);

    if (!j["map"].empty() && j["map"].size() < 1)
        return true;

    for (unsigned int i = 0; i < j["map"].size(); i++)
    {
        std::string shape = "";
        std::string coords = "";
        std::string area = "";

        if (!j["map"][i]["shape"].empty())
        {
            shape = j["map"][i]["shape"].get<std::string>();
        }

        if (!j["map"][i]["coords"].empty())
        {
            coords = j["map"][i]["coords"].get<std::string>();
        }

        if (!j["map"][i]["area"].empty())
        {
            area = j["map"][i]["area"].get<std::string>();
        }

        // strip whitespace
        std::regex r("\\s+");
        coords = std::regex_replace(coords, r, "");

        std::vector<int> vect;
        std::stringstream ss(coords);

        for (int i; ss >> i;)
        {
            vect.push_back(i);
            if (ss.peek() == ',')
                ss.ignore();
        }

        std::transform(shape.begin(), shape.end(), shape.begin(), ::tolower);
        if (shape == "circle")
        {
            RegionCircle circle;
            if (parseCircle(area, vect, circle))
            {
                circles.push_back(circle);
            }
            else
            {
                return false;
            }
        }
        else if (shape == "rect")
        {
            RegionRect rect;
            if (parseRect(area, vect, rect))
            {
                rects.push_back(rect);
            }
            else
            {
                return false;
            }
        }
        else if (shape == "poly")
        {
            RegionPoly poly;
            if (parsePoly(area, vect, poly))
            {
                polys.push_back(poly);
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}

bool RoiManager::isInsideRegion(const Point p, std::string &areaName)
{
    Log(typelog::INFO) << "RoiManager isInsideRegion (point)";

    for (std::size_t i = 0; i < circles.size(); i++)
    {
        RegionCircle r = circles.at(i);
        if (r.isInside(p))
        {
            areaName = r.areaName;
            return true;
        }
    }

    for (std::size_t i = 0; i < rects.size(); i++)
    {
        RegionRect r = rects.at(i);
        if (r.isInside(p))
        {
            areaName = r.areaName;
            return true;
        }
    }

    for (std::size_t i = 0; i < polys.size(); i++)
    {
        RegionPoly r = polys.at(i);
        if (r.isInside(p))
        {
            areaName = r.areaName;
            return true;
        }
    }

    return false;
}

bool RoiManager::isInsideRegion(const Rect r, std::string &areaName)
{
    Log(typelog::INFO) << "RoiManager isInsideRegion (rect)";

    // TL
    Point p = r.tl();
    if (isInsideRegion(p, areaName))
    {
        return true;
    }

    // TR
    p = r.tl();
    p.x += r.width;
    if (isInsideRegion(p, areaName))
    {
        return true;
    }

    // BL
    p = r.tl();
    p.y += r.height;
    if (isInsideRegion(p, areaName))
    {
        return true;
    }

    // BR
    p = r.tl();
    p.x += r.width;
    p.y += r.height;
    if (isInsideRegion(p, areaName))
    {
        return true;
    }

    return false;
}

bool RoiManager::addToMask(Mat &mask, std::string areaName)
{
    Log(typelog::INFO) << "RoiManager addToMask";
    bool found = false;
    Scalar color = Scalar(255, 255, 255); // white

    for (std::size_t i = 0; i < circles.size(); i++)
    {
        RegionCircle r = circles.at(i);
        if (areaName.empty() || r.areaName == areaName)
        {
            found = true;
            // https://stackoverflow.com/questions/18886083/how-fill-circles-on-opencv-c/18886330
            circle(mask, r.center, r.radius, color, -1);
        }
    }

    for (std::size_t i = 0; i < rects.size(); i++)
    {
        RegionRect r = rects.at(i);
        if (areaName.empty() || r.areaName == areaName)
        {
            found = true;
            rectangle(mask, r.tl, r.br, color, -1);
        }
    }

    for (std::size_t i = 0; i < polys.size(); i++)
    {
        RegionPoly r = polys.at(i);
        if (areaName.empty() || r.areaName == areaName)
        {
            found = true;

            // https://stackoverflow.com/questions/8281239/drawing-polygons-in-opencv/18106418
            const Point *elementPoints[1] = {&r.points[0]};
            int numberOfPoints = (int)r.points.size();

            fillPoly(mask, elementPoints, &numberOfPoints, 1, color);
        }
    }

    return found;
}

std::string RoiManager::getRegion(const Point p) {
    // Log(typelog::INFO) << "RoiManager getRegion (rect)";

    for (std::size_t i = 0; i < circles.size(); i++)
    {
        RegionCircle r = circles.at(i);
        if (r.isInside(p))
        {
            return r.areaName;
        }
    }

    for (std::size_t i = 0; i < rects.size(); i++)
    {
        RegionRect r = rects.at(i);
        if (r.isInside(p))
        {
            return r.areaName;
        }
    }

    for (std::size_t i = 0; i < polys.size(); i++)
    {
        RegionPoly r = polys.at(i);
        if (r.isInside(p))
        {
            return r.areaName;
        }
    }

    return "";
}

std::string RoiManager::getRegion(const Rect r) {
    // Log(typelog::INFO) << "RoiManager getRegion (rect)";

    std::string result;

    // TL
    Point p = r.tl();
    result = getRegion(p);
    if (!result.empty())
    {
        return result;
    }

    // TR
    p = r.tl();
    result = getRegion(p);
    if (!result.empty())
    {
        return result;
    }

    // BL
    p = r.tl();
    p.y += r.height;
    result = getRegion(p);
    if (!result.empty())
    {
        return result;
    }

    // BR
    p = r.tl();
    p.x += r.width;
    p.y += r.height;
    result = getRegion(p);
    if (!result.empty())
    {
        return result;
    }

    return "";
}