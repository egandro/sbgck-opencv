#ifndef _COMPONENTMANAGER_HPP
#define _COMPONENTMANAGER_HPP

#include <vector>
#include "opencv2/opencv.hpp"
#include "log.hpp"
#include "board.hpp"

class ComponentManager
{
    ComponentManager(const ComponentManager &value) {}

public:
    ComponentManager() {}

    ~ComponentManager() {}

    bool loadBoard(const unsigned char *imageData,
                   const int imageDataLen,
                   string boardMapJson = "", string boardName = "")
    {
        Asset asset(imageData, imageDataLen);
        Board board(asset);
        board.name = boardName;
        board.roiManager.initFromJsonString(boardMapJson);
    }
};

#endif