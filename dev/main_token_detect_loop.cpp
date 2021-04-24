#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
// https://stackoverflow.com/questions/11238918/s-isreg-macro-undefined
#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m)&S_IFMT) == S_IFDIR)
#endif
#include <iostream>
#include <fstream>
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

class MyConfig
{
public:
    string boardFile;
    string boardRegionFile;
    string camUrl;
    string colorCheckerFile;
    string name;
    string outFolder;
    string playerTokenFile;
} myConfig;

static void parseConfig(const char *fileName);
static bool checkOutDir();

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_token_detect_loop";
    LOGCFG.headers = true;
    LOGCFG.level = DEBUG;

    if (argc != 2)
    {
        Log(DEBUG) << "usage: " << argv[0] << " <ConfigFile>";
        return -1;
    }

    parseConfig(argv[1]);
    if (!checkOutDir())
        return 1;

    return 0;
}

static void parseConfig(const char *fileName)
{
    ifstream ifs(fileName);
    string jsonStr((std::istreambuf_iterator<char>(ifs)),
                   (std::istreambuf_iterator<char>()));
    ifs.close();

    json j = json::parse(jsonStr.c_str());
    //Log(DEBUG) << j.dump(4);

    // unchecked!
    myConfig.boardFile = j["boardFile"].get<std::string>();
    myConfig.boardRegionFile = j["boardRegionFile"].get<std::string>();
    myConfig.camUrl = j["camUrl"].get<std::string>();
    myConfig.colorCheckerFile = j["colorCheckerFile"].get<std::string>();
    myConfig.name = j["name"].get<std::string>();
    myConfig.outFolder = j["outFolder"].get<std::string>();
    myConfig.playerTokenFile = j["playerTokenFile"].get<std::string>();
}

static bool checkOutDir()
{
    struct stat sb;

    if (stat(myConfig.outFolder.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        return true;
    }

    Log(ERROR) << "outfolder: " << myConfig.outFolder << " does not exists ";
    return false;
}
