#ifndef RUNTIMEPROP_H
#define RUNTIMEPROP_H

#include "log.hpp"

class RuntimeProp {
public:
    static void init(std::string jsonStr);

    static std::string getString(std::string key, std::string defaultValue);
    static int getInt(std::string key, int defaultValue);
    static double getDouble(std::string key, double defaultValue);
    static bool getBool(std::string key, bool defaultValue);
};

#endif