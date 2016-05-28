#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "queue_roi.h"
#include "car_analysis/vAnalysis.h"

#include <iostream>


class queue_single_lane {
public:
    queue_single_lane() = default;

    ~queue_single_lane() = default;

    void initSingleLane(std::string lineName, int nRois, queueROIPolygon *queueROIsPolygon,
                        queueROIPolygon analysisROI, colourRecognition *colourR,
                        typeRecognitionLBP *typeR);

    void step(cv::Mat &image_grey, cv::Mat &image_rgb, cv::Mat &image_lbp, cv::Mat &movementMask,
              cv::Mat &edgeMask, cv::Mat &darkAreaMask, cv::Mat &image_vis);

private:
    int m_iNqueueROIs;
    int m_iGt;

    int m_iQueueLenght;
    int m_iQueueCounter[10];

    std::vector<queue_roi> m_queue_ROIs;

    vAnalysis *m_pvehicleAnalysis;                 // modul analizy pojazdu (kolor, typ, itp.)

};

