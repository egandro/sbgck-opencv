#include "strategy.hpp"

string getGeometryString(Geometry geometry)
{
    string res = "unknown";
    switch (geometry)
    {
    case Geometry::None:
        res = "None";
        break;
    case Geometry::Triangle:
        res = "Triangle";
        break;
    case Geometry::Square:
        res = "Square";
        break;
    case Geometry::Rect:
        res = "Rect";
        break;
    case Geometry::Hexagon:
        res = "Hexagon";
        break;
    case Geometry::Pentagon:
        res = "Pentagon";
        break;
    case Geometry::Circle:
        res = "Circle";
        break;
    }

    return res;
}
