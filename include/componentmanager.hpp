#ifndef _COMPONENTMANAGER_HPP
#define _COMPONENTMANAGER_HPP

#include <vector>
#include "opencv2/opencv.hpp"
#include "log.hpp"
#include "board.hpp"

class ComponentManager
{
    ComponentManager(const ComponentManager &value) {}
    vector<Board> boards;

public:
    ComponentManager() {}

    ~ComponentManager() {}

    bool loadBoard(const unsigned char *imageData,
                   const int imageDataLen,
                   string boardMapJson = "", string boardName = "")
    {
        Asset asset(imageData, imageDataLen);
        if( asset.getDefault().image.empty())
            return false;

        Board board(asset);
        board.name = boardName;
        if( !boardMapJson.empty() && board.roiManager.initFromJsonString(boardMapJson))
            return false;

        return true;
    }
};

#endif