#ifndef MATHELPER_H
#define MATHELPER_H

#include <iostream>
#include <fstream>
#include "log.hpp"
#include "opencv2/opencv.hpp"

class MatHelper
{

public:
    static bool writeMatBinary(std::ofstream &ofs, const cv::Mat &out_mat)
    {
        if (!ofs.is_open())
        {
            return false;
        }
        if (out_mat.empty())
        {
            int s = 0;
            ofs.write((const char *)(&s), sizeof(int));
            return true;
        }
        int type = out_mat.type();
        ofs.write((const char *)(&out_mat.rows), sizeof(int));
        ofs.write((const char *)(&out_mat.cols), sizeof(int));
        ofs.write((const char *)(&type), sizeof(int));
        ofs.write((const char *)(out_mat.data), out_mat.elemSize() * out_mat.total());

        return true;
    }

    static bool SaveMatBinary(const std::string &filename, const cv::Mat &output)
    {
        std::ofstream ofs(filename, std::ios::binary);
        return writeMatBinary(ofs, output);
    }

    static bool readMatBinary(std::ifstream &ifs, cv::Mat &in_mat)
    {
        if (!ifs.is_open())
        {
            return false;
        }

        int rows, cols, type;
        ifs.read((char *)(&rows), sizeof(int));
        if (rows == 0)
        {
            return true;
        }
        ifs.read((char *)(&cols), sizeof(int));
        ifs.read((char *)(&type), sizeof(int));

        in_mat.release();
        in_mat.create(rows, cols, type);
        ifs.read((char *)(in_mat.data), in_mat.elemSize() * in_mat.total());

        return true;
    }

    static bool LoadMatBinary(const std::string &filename, cv::Mat &output)
    {
        std::ifstream ifs(filename, std::ios::binary);
        return readMatBinary(ifs, output);
    }
};

#endif