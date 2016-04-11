//
// Created by vka on 11.04.16.
//

#include <opencv2/imgproc.hpp>

#include "lbp/lbp.h"
#include "frameImageProcessing.h"

void frameImageProcessing::step(Mat image_rgb, Mat image_gray) {
    this->image_gray_prev = this->image_gray;
    this->image_rgb_prev = this->image_rgb;
    this->image_rgb = image_rgb.clone();
    this->image_gray = image_gray.clone();
}

void frameImageProcessing::getMovementMask(Mat &mask) {
    mask = image_gray - image_gray_prev;
}

void frameImageProcessing::getEdgeMask(Mat &mask) {
    int low_threshold = 15;
    int ratio = 3;
    int kernel_size = 3;

    cv::blur(image_gray, mask, cv::Size(3, 3));
    cv::Canny(mask, mask, low_threshold, low_threshold * ratio, kernel_size);
}

void frameImageProcessing::getLBP(Mat &lbp) {
    lbp = Mat(image_gray.size(), CV_32FC1);
    libfacerec::olbp(image_gray, lbp);
}

void frameImageProcessing::getGaussianImage(Mat &image_gauss) {
    cv::Size ksize = cv::Size(3, 3);
    double sigmaX = 1;
    double sigmaY = 1;

    image_gauss = Mat(image_gray.size(), CV_32FC1);
    cv::GaussianBlur(image_gray, image_gauss, ksize, sigmaX, sigmaY);
}
