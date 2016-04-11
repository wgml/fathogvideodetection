#include "queue_single_lane.h"

queue_single_lane::queue_single_lane() {
}

queue_single_lane::~queue_single_lane() {
}

void queue_single_lane::initSingleLane(std::string lineName, int nRois,
                                       queueROIPolygon *queueROIsPolygon) {

    m_iNqueueROIs = nRois;
    //m_piQueueCounter = new int[5];
    memset(m_iQueueCounter, 0, m_iNqueueROIs * sizeof(int));

    m_iQueueLenght = 0;

    // Przypisanie listy ROI (utworzenie)
    for (int i = 0; i < m_iNqueueROIs; i++) {
        queue_roi qROI(queueROIsPolygon[i]);
        m_queue_ROIs.push_back(qROI);
    }
}


void queue_single_lane::step(cv::Mat &image_grey, cv::Mat &image_rgb, cv::Mat &image_lbp,
                             cv::Mat &movementMask, cv::Mat &edgeMask, cv::Mat &darkAreaMask,
                             cv::Mat &image_vis) {

    // Wywolanie przetarzania dla wszystkich kolejek
    for (int i = 0; i < m_iNqueueROIs; i++) {

        bool res;

        res = m_queue_ROIs[i].step(movementMask, edgeMask, image_grey);

        // resetuj dlugosc kolejki
        if (i == 0 && !res) {
            m_iQueueLenght = 0;
            memset(m_iQueueCounter, 0, m_iNqueueROIs * sizeof(int));
        }
        else {
            if (i == m_iQueueLenght && res) { // analizujemy dane pole
                m_iQueueCounter[m_iQueueLenght]++;
                // prog
                if (m_iQueueCounter[m_iQueueLenght] >
                    30) { // jesli jest dluzej niz to inkrementujemy dlugosc kolejki
                    m_iQueueLenght++;
                }
            }
        }
    }

    // debug
    char buf[100];
    sprintf(buf, "Q = %d", m_iQueueLenght);
    cv::rectangle(image_vis,
                  cv::Rect(m_queue_ROIs[0].m_queueROI.BL.x, m_queue_ROIs[0].m_queueROI.BL.y - 35,
                           75, 25), cv::Scalar(0, 0, 0), CV_FILLED);
    cv::putText(image_vis, buf, cv::Point(m_queue_ROIs[0].m_queueROI.BL.x + 10,
                                          m_queue_ROIs[0].m_queueROI.BL.y - 15), 1, 1,
                cv::Scalar(255, 255, 255));
}
