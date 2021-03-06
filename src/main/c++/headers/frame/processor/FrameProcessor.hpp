//
// Created by dialight on 26.11.16.
//

#ifndef OPENCVCLIENT_FRAMEPROCESSOR_HPP
#define OPENCVCLIENT_FRAMEPROCESSOR_HPP

#include <opencv2/opencv.hpp>

class FrameProcessor {

public:
    virtual ~FrameProcessor() {}

    virtual void init(cv::Mat &frame) = 0;
    virtual void handle(cv::Mat &frame) = 0;

};


#endif //OPENCVCLIENT_FRAMEPROCESSOR_HPP
