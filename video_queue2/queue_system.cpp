#include "queue_system.h"

queueSystem::queueSystem(int iLanes, int *iqueueROIs, int *gtQueue, std::string *names,
                         queueROIPolygon **queueROIs, queueROIPolygon *analysisROIs) {

    m_iNsingleLanes = iLanes;                        // ile pasow ruchu
    m_piQueueROIs = new int[m_iNsingleLanes];       // ile ROI na danym pasie


    for (int i = 0; i < m_iNsingleLanes; i++) {
        m_piQueueROIs[i] = iqueueROIs[i];

        queue_single_lane qsl;
        qsl.initSingleLane(names[i], iqueueROIs[i], queueROIs[i], analysisROIs[i], &m_colourR,
                           &m_typeR);       // inicjalizacja pojedynczego pasa
        m_queueSingleLanes.push_back(qsl);
    }
}

queueSystem::~queueSystem() {
    delete[] m_piQueueROIs;
    m_queueSingleLanes.clear();
}

// ------------------------------------------------------------------------------------------------------------
//! step
// Tutaj bedzie ew. integracja informacji dla calego systemu linii detekcji
// Uwaga. Koncepcja wizualizacja -> kazda linia "sama siebie rysuje".
// ------------------------------------------------------------------------------------------------------------
void queueSystem::step(cv::Mat &image_grey, cv::Mat &image_rgb, cv::Mat &image_lbp,
                       cv::Mat &movementMask, cv::Mat &edgeMask, cv::Mat &image_vis) {
    cv::Mat darkAreaMask;

    // Wywolanie obslugi dla kazdego pasa osobno
    for (int i = 0; i < m_iNsingleLanes; i++) {
        m_queueSingleLanes[i].step(image_grey, image_rgb, image_lbp, movementMask, edgeMask,
                                   darkAreaMask, image_vis);
    }
    // Kopiowanie poprzedniego
    image_rgb.copyTo(image_rgb_prev);
}


