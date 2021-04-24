#include <iostream>
#include <nlohmann/json.hpp>
#include <log.hpp>

using json = nlohmann::json;

structlog LOGCFG = {};

// config json
/*
{
    "name": "arctic",
    "camUrl": "http://zte:8080/video",
    "boardFile": "../../sbgck-dev-game/game/boards/Arctic-base.png",
    "boardRegionFile": "../../sbgck-dev-game/game/boards/Arctic-base.json",
    "colorCheckerFile": "../../sbgck-dev-game/game/assets/color_checker.png",
    "playerTokenFile": "../../sbgck-dev-game/game/assets/token_a.png",
    "outFolder": "./temp"
}
*/

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char*)"main_token_detect_loop";
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
