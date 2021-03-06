#include "queue_single_lane.h"


void queue_single_lane::initSingleLane(std::string lineName, int nRois,
                                       queueROIPolygon *queueROIsPolygon,
                                       queueROIPolygon analysisROI, colourRecognition *colourR,
                                       typeRecognitionLBP *typeR) {

    m_iNqueueROIs = nRois;
    //m_piQueueCounter = new int[5];
    memset(m_iQueueCounter, 0, m_iNqueueROIs * sizeof(int));

    m_iQueueLenght = 0;


    // Przypisanie listy ROI (utworzenie)
    for (int i = 0; i < m_iNqueueROIs; i++) {
        queue_roi qROI(queueROIsPolygon[i]);
        m_queue_ROIs.push_back(qROI);
    }

    // Utworzenie obiektu do rozpoznawania pojazdow
    m_pvehicleAnalysis = new vAnalysis(colourR, typeR);
    m_pvehicleAnalysis->init(analysisROI);
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

#ifdef DRAW_QUEUE
        m_queue_ROIs[i].draw(image_vis, image_grey);
#endif
    }

    // Uruchomienie kroku analizy...
    // TODO ew. integracja 1 i 2
    // 1. Analiza czy tło pod pojazdem nie trzeba uaktualnic
    //m_pvehicleAnalysis->bgModelUpdate(image_rgb, image_grey, movementMask, edgeMask);
    // 2. Wykrywanie obecnosci pojazdu
    //bool bCarDetected = m_pvehicleAnalysis->detectVehiclePrecence(movementMask, edgeMask);
    // 3. Jesli wykryto pojazd
    //if (bCarDetected) {
    //	std::cout<<"CAR DETECTED" << std::endl;
    //	m_pvehicleAnalysis->step(image_rgb, image_grey, image_lbp, movementMask, edgeMask);

    //}

#ifdef DRAW_ANALYSIS
    //m_pvehicleAnalysis->draw(image_vis);
#endif

    // debug
    char buf[100];
    sprintf(buf, "Q = %d", m_iQueueLenght);
    coord BL = m_queue_ROIs[0].getBL();

    cv::rectangle(image_vis,
                  cv::Rect(BL.first, BL.second,
                           75, 25), cv::Scalar(0, 0, 0), CV_FILLED);
    cv::putText(image_vis, buf, cv::Point(BL.first + 10,
                                          BL.second + 20), 1, 1,
                cv::Scalar(255, 255, 255));

    // Wyrysowanie dlugosci kolejki


    // Wypisanie dlugosci kolejki (na razie na konsole)
#ifdef ROI_DEBUG
    std::cout << "QL = " << m_iQueueLenght << std::endl;
#endif
}
