#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

struct Frame {
    std::vector<unsigned char> bytes;
    cv::Mat image;
    unsigned long long id = 1;
};

#endif

