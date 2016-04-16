// Metody detekcji krawedzie na FPGA
// Tomasz Kryjak
// Laboratroium Biocybernetyki, AGH
// 21.01.2014r.

#ifndef EDGEDETECTION_FPGA
#define EDGEDETECTION_FPGA

#include <opencv2/core/core.hpp>

void sobelEdge(const cv::Mat input,  cv::Mat & sobelX, cv::Mat & sobelY, cv::Mat &sobelM);

void sobelEdgeThresholdedMagintude(const cv::Mat & input, cv::Mat &sobelMB, int threshold);
void sobelEdgeThresholdedMagintude_RGB(const cv::Mat input, cv::Mat &sobelMB, int threshold);

void sobelEdge_RGB(const cv::Mat input, cv::Mat &sobelR, cv::Mat &sobelG, cv::Mat &sobelB);

void advEdgeDetection(const cv::Mat ER, const cv::Mat EG, const cv::Mat EB, int stdThreshold, int threshold, cv::Mat &edgeMask);

void prewittEdge(const cv::Mat input,  cv::Mat & sobelX, cv::Mat & sobelY, cv::Mat &sobelM);

void scale_L_Component(const cv::Mat input,  cv::Mat output);




#endif
