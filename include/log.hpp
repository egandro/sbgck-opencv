/*
 * File:   Log.h
 * Author: Alberto Lepe <dev@alepe.com>
 *
 * Created on December 1, 2015, 6:00 PM
 */

// https://stackoverflow.com/questions/5028302/small-logger-class

#ifndef LOG_H
#define LOG_H

#include <iostream>

using namespace std;

enum class typelog
{
    DEBUG,
    INFO,
    WARN,
    ERR,
    __DISABLED
};

struct structlog
{
    bool headers = false;
    typelog level = typelog::__DISABLED;
    char *prefix = NULL;
};

extern structlog LOGCFG;

class Log
{
public:
    Log() {}
    Log(typelog type)
    {
        msglevel = type;
        if (LOGCFG.prefix)
        {
            operator<< (LOGCFG.prefix);
            if (LOGCFG.headers)
            {
                operator<< (" ");
            }
        }
        if (LOGCFG.headers)
        {
            operator<<(getLabel(type));
        }

        if (LOGCFG.prefix || LOGCFG.headers)
        {

            operator<<(": ");
        }
    }
    ~Log()
    {
        if (opened)
        {
            cout << endl;
        }
        opened = false;
    }
    template <class T>
    Log &operator<<(const T &msg)
    {
        if (msglevel >= LOGCFG.level)
        {
            cout << msg;
            opened = true;
        }
        return *this;
    }

private:
    bool opened = false;
    typelog msglevel = typelog::DEBUG;
    inline string getLabel(typelog type)
    {
        string label = "";
        switch (type)
        {
        case typelog::DEBUG:
            label = "DEBUG";
            break;
        case typelog::INFO:
            label = "INFO";
            break;
        case typelog::WARN:
            label = "WARN";
            break;
        case typelog::ERR:
            label = "ERR";
            break;
        default:
            break;
        }
        return label;
    }
};

#endif /* LOG_H */