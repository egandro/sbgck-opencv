#include <nlohmann/json.hpp>

#include "runtimeprop.hpp"
#include <sstream>
#include <vector>

using json = nlohmann::json;

class RuntimePropJsonParser
{
    json j;

    json findKey(std::string key)
    {
        std::stringstream ss(key);
        std::string subKey;
        json current = j;

        while (getline(ss, subKey, '.'))
        {
            if (current[subKey].empty())
            {
                return current[subKey];
            }
            current = current[subKey];
        }

        return current;
    }

public:
    void init(std::string jsonStr)
    {
        j = json::parse(jsonStr);
    }

    std::string getString(std::string key, std::string defaultValue)
    {
        json current = findKey(key);

        if (!current.empty())
        {
            return current.get<std::string>();
        }

        return defaultValue;
    }

    int getInt(std::string key, int defaultValue)
    {
        json current = findKey(key);

        if (!current.empty())
        {
            return current.get<int>();
        }

        return defaultValue;
    }

    double getDouble(std::string key, double defaultValue)
    {
        json current = findKey(key);

        if (!current.empty())
        {
            return current.get<double>();
        }

        return defaultValue;
    }

    bool getBool(std::string key, bool defaultValue)
    {
        json current = findKey(key);

        if (!current.empty())
        {
            return current.get<bool>();
        }

        return defaultValue;
    }
};

static RuntimePropJsonParser parser;

void RuntimeProp::init(std::string jsonStr)
{
    parser.init(jsonStr);
}

std::string RuntimeProp::getString(std::string key, std::string defaultValue)
{
    return parser.getString(key, defaultValue);
}

int RuntimeProp::getInt(std::string key, int defaultValue)
{
    return parser.getInt(key, defaultValue);
}

double RuntimeProp::getDouble(std::string key, double defaultValue)
{
    return parser.getDouble(key, defaultValue);
}

bool RuntimeProp::getBool(std::string key, bool defaultValue)
{
    return parser.getBool(key, defaultValue);
}