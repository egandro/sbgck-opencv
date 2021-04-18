#include <opencv2/opencv.hpp>

#include "assetmanager.hpp"

#include "log.hpp"

using namespace cv;

structlog LOGCFG = {};

int main(int argc, char **argv)
{
    LOGCFG.prefix = (char *)"main_assetmanager";
    LOGCFG.headers = true;
    LOGCFG.level = DEBUG;

    if (argc != 2)
    {
        Log(DEBUG) << "usage: " << argv[0] << " <ImageFile>";
        return -1;
    }

    Asset board = AssetManager::addBoard(argv[1]);
    AssetManager::setCurrentBoard(board);

    AssetManager::addAsset(argv[1]);
    AssetManager::addAsset(argv[1]);
    AssetManager::addAsset(argv[1]);

    AssetManager::release();
    return 0;
}
