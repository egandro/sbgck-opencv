#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "areamanager.hpp"
#include "log.hpp"

using json = nlohmann::json;

std::list<Circle> AreaManager::circles;
std::list<Square> AreaManager::squares;
std::list<Poly> AreaManager::polygons;

bool AreaManager::parseCircle(const std::string areaName, const std::string coords, Circle &circle) {
    return false;
}

bool AreaManager::parseSquare(const std::string areaName, const std::string coords, Square &square) {
    return false;
}

bool AreaManager::parsePoly(const std::string areaName, const std::string coords, Poly &poly) {
    return false;
}


void AreaManager::initFromJsonString(const std::string jsonStr) {
    Log(INFO) << "AreaManager initFromJsonString";
    AreaManager::circles.clear();
    AreaManager::squares.clear();
    AreaManager::polygons.clear();


    // https://github.com/nlohmann/json

    json j = json::parse(jsonStr);

    if (!j["map"].empty() && j["map"].size() > 0)
        return;

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

        // <area shape="poly" coords="230,503,114,504,115,679,300,684,301,609,227,607" target="#yard" href="#yard" />
        // <area shape="rect" coords="883,212,1040,374" target="#controlPost" href="#controlPost" />
        // <area shape="circle" coords="112,144,47" target="#xxx" href="#xxx" />

        std::vector<cv::Point2f> points;
        if (coords.length() > 0)
        {
            std::istringstream input;
            input.str(coords);

            vector<string> strings;
            std::string s;
            float x = 0;
            float y = 0;
            int k = 1;

            while (std::getline(input, s, ','))
            {
                if (k % 2)
                {
                    x = std::stof(s);
                }
                else
                {
                    y = std::stof(s);

                    cv::Point2f p;
                    p.x = x;
                    p.y = y;
                    points.push_back(p);
                }
                k++;
            }
        }

        Log(DEBUG) << "shape              : " << shape;
        Log(DEBUG) << "coords (as Points) : " << points;
        Log(DEBUG) << "area               : " << area;
        Log(DEBUG) << "-------------------";
    }
}

