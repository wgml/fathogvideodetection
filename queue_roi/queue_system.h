#ifndef QUEUE_SYSTEM_H
#define QUEUE_SYSTEM_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "queue_single_lane.h"
#include "processing/queueROIPolygon.h"

class queueSystem {

public:
    queueSystem(int iLanes, int *iqueueROIs, int *gtQueue, std::string *names,
                queueROIPolygon **queueROIs);

    ~queueSystem();

    void step(cv::Mat &image_grey, cv::Mat &image_rgb, cv::Mat &image_lbp, cv::Mat &movementMask,
              cv::Mat &edgeMask, cv::Mat &image_vis);

private:

    int m_iNsingleLanes; //!< liczba analizowanych pasow ruchu
    int *m_piQueueROIs; //!< liczba obszarow zainteresownia w ramach danej linii

    cv::Mat image_rgb_prev;

    std::vector<queue_single_lane> m_queueSingleLanes;

};

#endif
