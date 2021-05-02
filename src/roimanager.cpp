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
    Log(INFO) << "RoiManager initFromFile " << fileName;

    ifstream ifs(fileName);

    if(ifs.fail()) {
      Log(ERROR) << fileName << " could not be opened";
      return false;
    }

    string jsonStr((std::istreambuf_iterator<char>(ifs)),
                   (std::istreambuf_iterator<char>()));
    ifs.close();

    return initFromJsonString(jsonStr);
}

bool RoiManager::initFromJsonString(const std::string jsonStr)
{
    Log(INFO) << "RoiManager initFromJsonString";
    circles.clear();
    rects.clear();
    polys.clear();

    // https://github.com/nlohmann/json

    json j = json::parse(jsonStr);

    if (!j["map"].empty() && j["map"].size() < 1)
        return true;

    for (int i = 0; i < j["map"].size(); i++)
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

bool RoiManager::isInsideCircle(const Point p, const RegionCircle &circle)
{
    Log(INFO) << "RoiManager isInsideCircle";

    // https://stackoverflow.com/questions/481144/equation-for-testing-if-a-point-is-inside-a-circle
    double dx = circle.center.x - p.x;
    double dy = circle.center.y - p.y;
    dx *= dx;
    dy *= dy;
    double distanceSquared = dx + dy;
    double radiusSquared = circle.radius * circle.radius;

    return distanceSquared <= radiusSquared;
}

bool RoiManager::isInsideRect(const Point p, const RegionRect &rect)
{
    Log(INFO) << "RoiManager isInsideRect";

    // https://www.geeksforgeeks.org/check-if-a-point-lies-on-or-inside-a-rectangle-set-2/
    if (p.x > rect.tl.x && p.x < rect.br.x && p.y > rect.tl.y && p.y < rect.br.y)
        return true;

    return false;
}

bool RoiManager::isInsidePoly(const Point p, const RegionPoly &poly)
{
    Log(INFO) << "RoiManager isInsidePoly";

    vector<Point> points = poly.points;
    size_t i, j, nvert = points.size();
    bool c = false;

    // https://stackoverflow.com/questions/11716268/point-in-polygon-algorithm
    for (i = 0, j = nvert - 1; i < nvert; j = i++)
    {
        if (((points[i].y >= p.y) != (points[j].y >= p.y)) &&
            (p.x <= (points[j].x - points[i].x) * (p.y - points[i].y) / (points[j].y - points[i].y) + points[i].x))
            c = !c;
    }

    return c;
}

bool RoiManager::isInsideRegion(const Point p, std::string &areaName)
{
    Log(INFO) << "RoiManager isInsideRegion (point)";

    for (std::size_t i = 0; i < circles.size(); i++)
    {
        RegionCircle r = circles.at(i);
        if (isInsideCircle(p, r))
        {
            areaName = r.areaName;
            return true;
        }
    }

    for (std::size_t i = 0; i < rects.size(); i++)
    {
        RegionRect r = rects.at(i);
        if (isInsideRect(p, r))
        {
            areaName = r.areaName;
            return true;
        }
    }

    for (std::size_t i = 0; i < polys.size(); i++)
    {
        RegionPoly r = polys.at(i);
        if (isInsidePoly(p, r))
        {
            areaName = r.areaName;
            return true;
        }
    }

    return false;
}

bool RoiManager::isInsideRegion(const Rect r, std::string &areaName)
{
    Log(INFO) << "RoiManager isInsideRegion (rect)";

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