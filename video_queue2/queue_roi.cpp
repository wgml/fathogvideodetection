#include "queue_roi.h"

queue_roi::queue_roi(queueROIPolygon queueROI) {


    m_edgeThreshold = queueROI.edgeTh;
    m_movementThreshold = queueROI.movementTh;

    // FIXED !!!
    m_darkAreasThreshold = 100;

    m_bMovementDetctedCnt = 0;
    m_bEdgeDetctedCnt = 0;

    // Obliczenie parametrow linii
    computeROIlineParams(queueROI);

    // Inicjalizacja obrazka
    m_darkAreas = cv::Mat::zeros(m_ROIBbox.height, m_ROIBbox.width, CV_8U);

}

bool queue_roi::step(cv::Mat movementMask, cv::Mat edgeMask, cv::Mat image_grey) {

    m_movementCounter = 0;
    m_edgeCounter = 0;
    m_darkAreasCounter = 0;
    int grayCounter = 0;
    int pixelCounter = 0;

    for (int jj = m_ROIBbox.y; jj < m_ROIBbox.y + m_ROIBbox.height; jj++) {
        for (int ii = m_ROIBbox.x; ii < m_ROIBbox.x + m_ROIBbox.width; ii++) {
            if (roiPolygon.isPointInside(std::pair<int, int>{ii, jj})) {
                if (movementMask.at<uchar>(jj, ii) > 0)
                    m_movementCounter++;

                if (edgeMask.at<uchar>(jj, ii) > 0)
                    m_edgeCounter++;

                if (m_darkAreas.at<uchar>(jj - m_ROIBbox.y, ii - m_ROIBbox.x) > 0)
                    m_darkAreasCounter++;

                // Do obliczania sredniej szarosci
                grayCounter += image_grey.at<uchar>(jj, ii);
                // Licznik piskeli (do obliczania sredniej)
                pixelCounter++;
            }
        }
    }

    // Srednia wartosc szarosci
    m_meanGreyValue = grayCounter / (pixelCounter + 1);
    // debug
#ifdef ROI_DEBUG
    std::cout << "m_meanGreyValue "<< m_meanGreyValue <<std::endl;
#endif

    // Jesli wykryto ruch
    m_bMovementDetcted = m_movementCounter > m_movementThreshold;

    // Filtr medianowy ruchu
    m_bMovementDetcted = median1DFilter(m_bMovementDetcted, m_bMovementDetctedCnt);

    // Wykryto krawedzie
    m_bEdgeDetcted = m_edgeCounter > m_edgeThreshold;

    // Filtr medianowy krawedzi
    m_bEdgeDetcted = median1DFilter(m_bEdgeDetcted, m_bEdgeDetctedCnt);


    // Wykryto ciemny obszar
    m_bDarkAreaDetcted = m_darkAreasCounter > m_darkAreasThreshold;

    m_bDarkAreaDetcted = median1DFilter(m_bDarkAreaDetcted, m_bDarkAreaDetctedCnt);

    // Zwraca, czy jest zatrzym any pojazd
    return !m_bMovementDetcted && m_bEdgeDetcted && m_bDarkAreaDetcted;
}

void queue_roi::draw(cv::Mat image_vis, cv::Mat image_grey) {
    cv::Vec<uchar, 3> vRED;
    cv::Vec<uchar, 3> vGREEN;
    cv::Vec<uchar, 3> vBLUE;


    vGREEN[0] = 0;
    vGREEN[1] = 255;
    vGREEN[2] = 0;
    vRED[0] = 0;
    vRED[1] = 0;
    vRED[2] = 255;
    vBLUE[0] = 255;
    vBLUE[1] = 0;
    vBLUE[2] = 0;

    for (int jj = m_ROIBbox.y; jj < m_ROIBbox.y + m_ROIBbox.height; jj++) {
        for (int ii = m_ROIBbox.x; ii < m_ROIBbox.x + m_ROIBbox.width; ii++) {
            if (roiPolygon.isPointInside(std::pair<int, int>{ii, jj})) {
                cv::Vec<uchar, 3> p = image_vis.at<vec_uchar_3>(jj, ii);
                if (image_grey.at<uchar>(jj, ii) < m_meanGreyValue * 0.5)
                    m_darkAreas.at<uchar>(jj - m_ROIBbox.y, ii - m_ROIBbox.x) = 255;
                else
                    m_darkAreas.at<uchar>(jj - m_ROIBbox.y, ii - m_ROIBbox.x) = 0;

                if (m_bMovementDetcted)
                    image_vis.at<vec_uchar_3>(jj, ii) = 0.75 * p + 0.25 * vRED;
                else if (m_bEdgeDetcted)
                    image_vis.at<vec_uchar_3>(jj, ii) = 0.75 * p + 0.25 * vBLUE;
                else
                    image_vis.at<vec_uchar_3>(jj, ii) = 0.75 * p + 0.25 * vGREEN;
            }
        }
    }
#ifdef QUEUE_DEBUG
    // Wypisanie informacji tekstowych
    ///*
    char buf[100];
    sprintf(buf,"M = %d|%d", m_movementCounter,m_movementThreshold);
    //cv::putText(image_vis, buf, cv::Point(m_ROIBbox.x, m_ROIBbox.y+15),1,1, cv::Scalar(255, 255, 255));
    sprintf(buf,"E = %d|%d", m_edgeCounter,m_edgeThreshold);
    //cv::putText(image_vis, buf, cv::Point(m_ROIBbox.x, m_ROIBbox.y + 30), 1, 1, cv::Scalar(255, 255, 255));
    //*/

    std::cout<< "M = " << m_movementCounter << " | " <<m_movementThreshold   << std::endl;
    std::cout<< "E = " << m_edgeCounter<< " | " << m_edgeThreshold           << std::endl;
    std::cout<< "D = " << m_darkAreasCounter<< " | " << m_darkAreasThreshold << std::endl;
#endif
}

void queue_roi::computeROIlineParams(queueROIPolygon m_queueROI) {


    // ------------------------------------------------------------------------------------------------------------
    // Wyznacznie prostokata otaczajacego
    // ------------------------------------------------------------------------------------------------------------
    int xx1 = m_queueROI.TL.x > m_queueROI.BL.x ? m_queueROI.BL.x : m_queueROI.TL.x;
    int yy1 = m_queueROI.TL.y > m_queueROI.TR.y ? m_queueROI.TR.y : m_queueROI.TL.y;

    int xx2 = m_queueROI.TR.x > m_queueROI.BR.x ? m_queueROI.TR.x : m_queueROI.BR.x;
    int yy2 = m_queueROI.BL.y > m_queueROI.BR.y ? m_queueROI.BL.y : m_queueROI.BR.y;

    m_ROIBbox.x = xx1;
    m_ROIBbox.y = yy1;
    m_ROIBbox.width = xx2 - xx1;
    m_ROIBbox.height = yy2 - yy1;

    std::vector<wgml::point> points;
    points.push_back(wgml::point{m_queueROI.TL.x, m_queueROI.TL.y});
    points.push_back(wgml::point{m_queueROI.TR.x, m_queueROI.TR.y});
    points.push_back(wgml::point{m_queueROI.BR.x, m_queueROI.BR.y});
    points.push_back(wgml::point{m_queueROI.BL.x, m_queueROI.BL.y}); // wild ordering hack appears
    roiPolygon = wgml::Polygon{points};
}

bool queue_roi::median1DFilter(bool res, int &medianCounter) {

    if (res)
        medianCounter++;
    else
        medianCounter--;

    if (medianCounter > MEDIAN_1D_SIZE)
        medianCounter = MEDIAN_1D_SIZE;

    if (medianCounter < 0)
        medianCounter = 0;

    return medianCounter > MEDIAN_1D_SIZE / 2;
}
