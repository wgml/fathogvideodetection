#include "queue_roi.h"

// ------------------------------------------------------------------------------------------------------------
// queue_roi
// ------------------------------------------------------------------------------------------------------------
queue_roi::queue_roi(queueROIPolygon queueROI) {


	m_queueROI = queueROI;
	m_edgeThreshold = queueROI.edgeTh;
	m_movementThreshold = queueROI.movementTh;

	// FIXED !!!
	m_darkAreasThreshold = 100;

	m_bMovementDetctedCnt = 0;
	m_bEdgeDetctedCnt = 0;

	// Obliczenie parametrow linii
	computeROIlineParams();

	// Inicjalizacja obrazka
	m_darkAreas = cv::Mat::zeros(m_ROIBbox.height, m_ROIBbox.width, CV_8U);

}
queue_roi::~queue_roi() {

}

// ------------------------------------------------------------------------------------------------------------
// step
// ------------------------------------------------------------------------------------------------------------
bool queue_roi::step(cv::Mat movementMask, cv::Mat edgeMask, cv::Mat image_grey){

    // debug (wizualizacja)
    //cv::imshow("ROI - EDGE", edgeMask(m_ROIBbox));
    //cv::imshow("ROI - MOVMENT", movementMask(m_ROIBbox));
    //cv::imshow("ROI - DARK", m_darkAreas);
    //cv::waitKey(0);


	m_movementCounter = 0;
	m_edgeCounter = 0;
	m_darkAreasCounter =0;
	bool inside[4];

	int grayCounter = 0;
	int pixelCounter = 0;

	for (int jj = m_ROIBbox.y; jj < m_ROIBbox.y + m_ROIBbox.height; jj++) {
		for (int ii = m_ROIBbox.x; ii < m_ROIBbox.x + m_ROIBbox.width; ii++) {

			// Testy na linie (czy jestesmy wewnatrz obszaru)
			for (int k = 0; k <= 3; k++) {
				float e = m_lineParams[k].cos_alfa * ii + m_lineParams[k].sin_alfa * jj - m_lineParams[k].p;

				if (m_lineParams[k].gOrL)
					inside[k] = (e > 0) ? true : false;
				else
					inside[k] = (e > 0) ? false : true;
			}

            // Jesli jestesmy wewnatrz obszaru
			if (inside[0] && inside[1] && inside[2] && inside[3]) {

				if (movementMask.at<uchar>(jj, ii) > 0)
					m_movementCounter++;

				if (edgeMask.at<uchar>(jj, ii) > 0)
					m_edgeCounter++;

				if (m_darkAreas.at<uchar>(jj-m_ROIBbox.y, ii-m_ROIBbox.x) > 0)
					m_darkAreasCounter++;


                // Do obliczania sredniej szarosci
				grayCounter += image_grey.at<uchar>(jj, ii);
				// Licznik piskeli (do obliczania sredniej)
				pixelCounter++;

			}
		}
	}

    // Srednia wartosc szarosci
	m_meanGreyValue = grayCounter / pixelCounter;
	// debug
	std::cout << "m_meanGreyValue "<< m_meanGreyValue <<std::endl;

	// Jesli wykryto ruch
	if (m_movementCounter > m_movementThreshold)
		m_bMovementDetcted = true;
	else
		m_bMovementDetcted = false;

    // Filtr medianowy ruchu
	m_bMovementDetcted = median1DFilter(m_bMovementDetcted, m_bMovementDetctedCnt);

	// Wykryto krawedzie
	if (m_edgeCounter > m_edgeThreshold)
		m_bEdgeDetcted = true;
	else
		m_bEdgeDetcted = false;

    // Filtr medianowy krawedzi
	m_bEdgeDetcted = median1DFilter(m_bEdgeDetcted, m_bEdgeDetctedCnt);


	// Wykryto ciemny obszar
	if (m_darkAreasCounter > m_darkAreasThreshold)
		m_bDarkAreaDetcted = true;
	else
		m_bDarkAreaDetcted = false;

    m_bDarkAreaDetcted = median1DFilter(m_bDarkAreaDetcted, m_bDarkAreaDetctedCnt);



	// Zwraca, czy jest zatrzymany pojazd
	return !m_bMovementDetcted && m_bEdgeDetcted && m_bDarkAreaDetcted;
	// debug
}


// ------------------------------------------------------------------------------------------------------------
// draw
// ------------------------------------------------------------------------------------------------------------
void queue_roi::draw(cv::Mat image_vis, cv::Mat image_grey){

	// ROI (testowo)
	//cv::rectangle(image_vis, m_ROIBbox, cv::Scalar(255, 0, 0));

	// Testowo magiczna binaryzacja

	// Tylko obwodka

	// Definicja koloru czerwonego
	cv::Vec<uchar, 3> vRED;
	cv::Vec<uchar, 3> vGREEN;
	cv::Vec<uchar, 3> vBLUE;


	vGREEN[0] =0; vGREEN[1] = 255; vGREEN[2] =0;
	vRED[0] = 0; vRED[1] = 0; vRED[2] = 255;
	vBLUE[0] = 255; vBLUE[1] = 0; vBLUE[2] = 0;



	bool inside[4];

	for (int jj = m_ROIBbox.y; jj < m_ROIBbox.y + m_ROIBbox.height; jj++) {
		for (int ii = m_ROIBbox.x; ii < m_ROIBbox.x + m_ROIBbox.width; ii++) {

			// Testy na linie
			for (int k = 0; k <= 3; k++) {
				float e = m_lineParams[k].cos_alfa * ii + m_lineParams[k].sin_alfa * jj - m_lineParams[k].p;

				if (m_lineParams[k].gOrL)
					inside[k] = (e > 0) ? true : false;
				else
					inside[k] = (e > 0) ? false : true;

			}


            // Czy w srodku danego obszru
			if (inside[0] && inside[1] && inside[2] && inside[3]) {
				cv::Vec<uchar, 3> p = image_vis.at<vec_uchar_3>(jj, ii);

				/*
				if (!m_bMovementDetcted && m_bEdgeDetcted)
					p[2] = 255;
				else {
					if (m_bMovementDetcted)
						p[0] = 255;
					if (m_bEdgeDetcted)
						p[1] = 255;
					if (m_bDarkAreaDetcted)
						p[2] = 255;

				}
                */
				if (image_grey.at<uchar>(jj, ii) < m_meanGreyValue*0.5)
					m_darkAreas.at<uchar>(jj - m_ROIBbox.y, ii - m_ROIBbox.x) = 255;
                else
                    m_darkAreas.at<uchar>(jj - m_ROIBbox.y, ii - m_ROIBbox.x) = 0;

                // Tymczasowe do konfiguracji
                //image_vis.at<vec_uchar_3>(jj, ii) = p;

				if (m_bMovementDetcted)
                    image_vis.at<vec_uchar_3>(jj, ii) = 0.75*p + 0.25*vRED;
				else
                    if (m_bEdgeDetcted)
                        image_vis.at<vec_uchar_3>(jj, ii) = 0.75*p + 0.25*vBLUE;
                    else
                        image_vis.at<vec_uchar_3>(jj, ii) = 0.75*p + 0.25*vGREEN;

			}



		}

	}

	// debug
	//cv::waitKey(0);

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


	//cv::waitKey(0);

}

// ------------------------------------------------------------------------------------------------------------
//! Wyznaczanie parametrow dla linii
// ------------------------------------------------------------------------------------------------------------
void queue_roi::computeROIlineParams(){


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

	// ------------------------------------------------------------------------------------------------------------
	// Wyrysowaywanie poszczegolnych linii
	// ------------------------------------------------------------------------------------------------------------
	singleLineParam(0, m_queueROI.TL.x, m_queueROI.TL.y, m_queueROI.BL.x, m_queueROI.BL.y, true);   // TL BL
	singleLineParam(1, m_queueROI.TR.x, m_queueROI.TR.y, m_queueROI.BR.x, m_queueROI.BR.y, false);  // TR BR
	singleLineParam(2, m_queueROI.TR.x, m_queueROI.TR.y, m_queueROI.TL.x, m_queueROI.TL.y, true);  // TR TL
	singleLineParam(3, m_queueROI.BL.x, m_queueROI.BL.y, m_queueROI.BR.x, m_queueROI.BR.y, false);  // BR BL
}

// ------------------------------------------------------------------------------------------------------------
//! Wyznaczanie parametrow dla linii
// ------------------------------------------------------------------------------------------------------------
void queue_roi::singleLineParam(int idx, float x1, float y1, float x2, float y2, bool lor) {

	// TODO - to trzeba sprawdzic, czy tak jest
	float alpha;
	if (y1 != y2) {
		alpha = atan2f(x1 - x2, y2 - y1);
	}
	else {
		alpha = CV_PI / 2;
	}

	m_lineParams[idx].sin_alfa = sin(alpha);
	m_lineParams[idx].cos_alfa = cos(alpha);
	m_lineParams[idx].p = x1*m_lineParams[idx].cos_alfa + y1*m_lineParams[idx].sin_alfa;
	m_lineParams[idx].gOrL = lor;

}

// ------------------------------------------------------------------------------------------------------------
//! median1DFilter
// ------------------------------------------------------------------------------------------------------------
bool queue_roi::median1DFilter(bool res, int & medianCounter ) {

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
