#include <iostream>
#include <nlohmann/json.hpp>
#include <log.hpp>

using json = nlohmann::json;

structlog LOGCFG = {};

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char*)"main_json_test";
    LOGCFG.headers = true;
    LOGCFG.level = DEBUG;

    // https://github.com/nlohmann/json

    auto j = json::parse("{ \"happy\": true, \"name\": \"foo\", \"pi\": 3.141 }");

    // explicit conversion to string
    std::string s = j.dump();
    Log(DEBUG) << s;

    // serialization with pretty printing
    // pass in the amount of spaces to indent
    Log(DEBUG) << j.dump(4);


    float pi = j["pi"].get<float>();
    bool happy = j["happy"].get<bool>();
    std::string name = j["name"].get<std::string>();


    Log(DEBUG) << pi << " " <<  happy << " " <<  name;

    return 0;
}
