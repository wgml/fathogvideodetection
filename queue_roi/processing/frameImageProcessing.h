//
// Created by vka on 11.04.16.
//

#ifndef OPENCVHELLOWORLD_FRAMEIMAGEPROCESSING_H
#define OPENCVHELLOWORLD_FRAMEIMAGEPROCESSING_H

#include <opencv2/core/mat.hpp>

using cv::Mat;

class frameImageProcessing {

public:
    frameImageProcessing(int width, int height)
            : width(width),
              height(height) {
        image_gray = Mat::zeros(image_gray.size(), CV_32FC1);
        image_gray_prev = Mat::zeros(image_gray.size(), CV_32FC1);
        image_rgb = Mat::zeros(image_gray.size(), CV_32FC3);
        image_rgb_prev = Mat::zeros(image_gray.size(), CV_32FC3);
    }

    ~frameImageProcessing() = default;

    void step(Mat image_rgb, Mat image_gray);
    void getMovementMask(Mat &mask);
    void getEdgeMask(Mat &mask);
    void getLBP(Mat &lbp);
    void getGaussianImage(Mat &image_gauss);

private:
    const int width;
    const int height;

    Mat image_gray;
    Mat image_gray_prev;
    Mat image_rgb;
    Mat image_rgb_prev;
};


#endif //OPENCVHELLOWORLD_FRAMEIMAGEPROCESSING_H
