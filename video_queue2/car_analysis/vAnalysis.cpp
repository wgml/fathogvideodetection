#include "vAnalysis.h"
#include <ctime>

// ------------------------------------------------------------------------------------------------------------
// vAnalysis
// ------------------------------------------------------------------------------------------------------------
vAnalysis::vAnalysis(colourRecognition *colourR, typeRecognitionLBP *typeR) {

    mp_colourR = colourR;
    mp_typeR = typeR;

}


// ------------------------------------------------------------------------------------------------------------
// ~vAnalysis
// ------------------------------------------------------------------------------------------------------------
vAnalysis::~vAnalysis() {


}

// ------------------------------------------------------------------------------------------------------------
// init
// ------------------------------------------------------------------------------------------------------------
void vAnalysis::init(queueROIPolygon analysisROI) {


	m_analysisROI = analysisROI;
	m_edgeThreshold = m_analysisROI.edgeTh;
	m_movementThreshold = m_analysisROI.movementTh;

	m_bMovementDetctedCnt = 0;
	m_bEdgeDetctedCnt = 0;



	// Obliczenie parametrow linii
	computeROIlineParams();

	// utworzenie pustego modelu tla
	//m_bgModel = cv::Mat::zeros(m_ROIBbox.height, m_ROIBbox.width, CV_8UC3);
	m_bgModel = cv::Mat::zeros(m_ROIBbox.height, m_ROIBbox.width, CV_8U);


	// utworzenie maski na obiekty pierwszoplanowe
	m_objectMask = cv::Mat::zeros(m_ROIBbox.height, m_ROIBbox.width, CV_8U);
	// utworzenie maski na obszary jednorodne
	m_uniformAreaMask = cv::Mat::zeros(m_ROIBbox.height, m_ROIBbox.width, CV_8U);

	m_vehiclePresent_prev = false;
	m_vehiclePresent = false;

	//debug
	std::cout <<"Analysis: "<< m_ROIBbox.height << "   " << m_ROIBbox.width << std::endl;


}

// ------------------------------------------------------------------------------------------------------------
// step
// Krok analizy tj. detekcji kolotu i typu pojazdu
// ------------------------------------------------------------------------------------------------------------
void vAnalysis::step(const cv::Mat & image_rgb,const cv::Mat & image_gray,const cv::Mat & image_lbp,const cv::Mat & movementMask,const cv::Mat & edgeMask) {

	// Zapisanie biezacego ROI
	// ++ pewnie trzeba bedzie to do pliku zapisac...
	image_rgb(m_ROIBbox).copyTo(m_currentROI);
	image_gray(m_ROIBbox).copyTo(m_currentROIgrey);
	image_lbp(m_ROIBbox).copyTo(m_currentROIlbp);


	// TODO odejmowanie tla
	// pobranie probek koloru, LBP i wszystkie inne gadzety...

	// Odejmowanie tla (w odcieniach szarosci)
	//diff_th_rgb(m_currentROI, m_bgModel, m_ROIMask,  m_objectMask);
	diff_th_grey(m_currentROIgrey, m_bgModel, m_ROIMask,  m_objectMask);

	// Obliczanie maski range
	//rangeFilterTh(m_currentROIgrey, m_uniformAreaMask, RANGE_TH);


	// Iloczyn maski obiektow i range
	//cv::Mat colourMask = m_objectMask & m_uniformAreaMask;


	// Rozpoznawanie koloru
	std::string colour =mp_colourR->recognizeColour(m_currentROI, m_objectMask);


    // Rozpoznawnie typu samochodu:
    std::string type = mp_typeR->recognizeType(m_currentROIlbp);



	std::cout << "KOLOR = " << colour << std::endl;
	std::cout << "TYP = "   << type << std::endl;


	// debug
	cv::putText(m_currentROI, colour, cv::Point(5, 20), 1, 1, cv::Scalar(255, 255, 255));

	cv::imshow("m_currentROI", m_currentROI);
	cv::imshow("m_bgModel", m_bgModel);
	cv::imshow("m_objectMask", m_objectMask);
	cv::imshow("m_ROIMask", m_ROIMask);
	//cv::waitKey(0);

	// +++ zapisDoPliku
	char buf[100];
	time_t t = time(0);                             // get time now
	sprintf(buf,"carSamples/%d_cROI.png", t);       // utworz nazwe
	cv::imwrite(buf, m_currentROI);                 // zapisz plik
	//sprintf(buf, "carSamples/%d_oROI.png", t);
	//cv::imwrite(buf, m_objectMask);
	//sprintf(buf, "carSamples/%d_mROI.png", t);
	//cv::imwrite(buf, m_ROIMask);



	//cv::imshow("m_uniformAreaMask", m_uniformAreaMask);



}

// ------------------------------------------------------------------------------------------------------------
// bgModelUpdate
// ------------------------------------------------------------------------------------------------------------
void vAnalysis::bgModelUpdate(cv::Mat image_rgb, cv::Mat image_gray, cv::Mat movementMask, cv::Mat edgeMask){

	// zerowanie licznikow ruchu i krawedzi
	m_movementCounter = 0;
	m_edgeCounter = 0;

	// zliczenie odpowiedzi z ruchem i krawdzami
	for (int jj = m_ROIBbox.y; jj < m_ROIBbox.y + m_ROIBbox.height; jj++) {
		for (int ii = m_ROIBbox.x; ii < m_ROIBbox.x + m_ROIBbox.width; ii++) {

			if (m_ROIMask.at<uchar>(jj - m_ROIBbox.y, ii - m_ROIBbox.x) > 0) {
				if (movementMask.at<uchar>(jj, ii) > 0)
					m_movementCounter++;
				if (edgeMask.at<uchar>(jj, ii) > 0)
					m_edgeCounter++;
			}
		}
	}


	// czy jest ruch + filtracja medianowa
	if (m_movementCounter > m_movementThreshold)
		m_bMovementDetcted = true;
	else
		m_bMovementDetcted = false;

	//m_bMovementDetcted = medianbinary1DFilter(m_bMovementDetcted, m_bMovementDetctedCnt);

	// czy sa krawedzie (jest pojazd) + filtracja medianowa
	if (m_edgeCounter > m_edgeThreshold)
		m_bEdgeDetcted = true;
	else
		m_bEdgeDetcted = false;

    // debug
    //std::cout << "E = " << m_edgeCounter << " | " << m_edgeThreshold << std::endl;
    //std::cout << "M = " << m_movementCounter << " | " << m_movementThreshold << std::endl;


	//m_bEdgeDetcted = medianbinary1DFilter(m_bEdgeDetcted, m_bEdgeDetctedCnt);

	// Decyzja, czy uaktualniac obraz tla
	// jesli nia ma krawedzi i nie maa ruchu
	if (!m_bEdgeDetcted && !m_bMovementDetcted) {
		// aktualizacja modelu tla
		image_gray(m_ROIBbox).copyTo(m_bgModel);

		// debug
		//cv::imshow("m_bgModel", m_bgModel);
		//cv::waitKey(0);
	}
}

// ------------------------------------------------------------------------------------------------------------
// detectVehiclePrecence
// ------------------------------------------------------------------------------------------------------------
bool vAnalysis::detectVehiclePrecence(cv::Mat movementMask, cv::Mat edgeMask) {

	// Nieoptymalne - > ROI
	cv::Mat mMaskROI;
	cv::Mat eMaskROI;

	bool vehicle;

	movementMask(m_ROIBbox).copyTo(mMaskROI);
	edgeMask(m_ROIBbox).copyTo(eMaskROI);


	// Recznie (-10 do -5) ostatnich linii
	int sumEdgeA = 0;
	int sumMovA = 0;

	for (int j = mMaskROI.rows - 16; j < mMaskROI.rows - 9; j++) {
		for (int i = 0; i < mMaskROI.cols; i++) {
			if (m_ROIMask.at<uchar>(j, i) > 0) {
				if (mMaskROI.at<uchar>(j, i) > 0) sumMovA++;
				if (eMaskROI.at<uchar>(j, i) > 0) sumEdgeA++;
			}
		}
	}

    // Recznie (-5 do 0) ostatnich linii
	int sumEdgeB = 0;
	int sumMovB = 0;

	for (int j = mMaskROI.rows - 8; j < mMaskROI.rows; j++) {
		for (int i = 0; i < mMaskROI.cols; i++) {
			if (m_ROIMask.at<uchar>(j, i) > 0) {
				if (mMaskROI.at<uchar>(j, i) > 0) sumMovB++;
				if (eMaskROI.at<uchar>(j, i) > 0) sumEdgeB++;
			}
		}
	}



	//cv::waitKey(0);

	// detekcja - warunki na obecnosc pojazdu (pierwsze pojawienie sie)
	// staly prog
	if (sumMovA > sumMovB && sumMovA > 50 ) {
		m_vehiclePresent = true;
	}
	if (((sumMovB > sumMovA) && (sumMovB > 50 && sumMovA < 10) )|| (sumMovA == 0 && sumMovB == 0)) {
		m_vehiclePresent = false;
	}

    // wykrywanie narastajacego zbocza (obecnosci pojazdu)
	if (m_vehiclePresent == true && m_vehiclePresent_prev == false)
		vehicle = true;
	else
		vehicle = false;

	m_vehiclePresent_prev = m_vehiclePresent;

	// debug (wypisywane wartosci)
	//std::cout << "M = " << sumMovA << " | " << sumMovB << std::endl;
	//std::cout << "E = " << sumEdgeA << " | " << sumEdgeB << std::endl;
	//std::cout << m_vehiclePresent << " | " << m_vehiclePresent_prev << " | " << vehicle << std::endl;
	//cv::waitKey(0);

	return vehicle;
}

// ------------------------------------------------------------------------------------------------------------
// draw
// ------------------------------------------------------------------------------------------------------------
void vAnalysis::draw(cv::Mat image_vis){

	// ROI (testowo)
	//cv::rectangle(image_vis, m_ROIBbox, cv::Scalar(255, 0, 0));

	// Tylko obwodka

	cv::Vec<uchar, 3> vRED;
	vRED[0] = 0; vRED[1] = 0; vRED[2] = 255;
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


			// tylko jesli wystepuje ruch
			if (inside[0] && inside[1] && inside[2] && inside[3]) {
				cv::Vec<uchar, 3> p = image_vis.at<vec_uchar_3>(jj, ii);
				if (m_bMovementDetcted || m_bEdgeDetcted)
					p[2] = 255;
				image_vis.at<vec_uchar_3>(jj, ii) = p;
			}



		}

	}

	// Wypisanie informacji tekstowych
	///*
#ifdef DRAW
	char buf[100];
	sprintf(buf, "M = %d|%d", m_movementCounter, m_movementThreshold);
	cv::putText(image_vis, buf, cv::Point(m_ROIBbox.x, m_ROIBbox.y + 15), 1, 1, cv::Scalar(255, 255, 255));
	sprintf(buf, "E = %d|%d", m_edgeCounter, m_edgeThreshold);
	cv::putText(image_vis, buf, cv::Point(m_ROIBbox.x, m_ROIBbox.y + 30), 1, 1, cv::Scalar(255, 255, 255));
#endif

}


// ------------------------------------------------------------------------------------------------------------
//! Wyznaczanie parametrow dla linii
// ------------------------------------------------------------------------------------------------------------
void vAnalysis::computeROIlineParams(){


	// ------------------------------------------------------------------------------------------------------------
	// Wyznacznie prostokata otaczajacego
	// ------------------------------------------------------------------------------------------------------------
	int xx1 = m_analysisROI.TL.x > m_analysisROI.BL.x ? m_analysisROI.BL.x : m_analysisROI.TL.x;
	int yy1 = m_analysisROI.TL.y > m_analysisROI.TR.y ? m_analysisROI.TR.y : m_analysisROI.TL.y;

	int xx2 = m_analysisROI.TR.x > m_analysisROI.BR.x ? m_analysisROI.TR.x : m_analysisROI.BR.x;
	int yy2 = m_analysisROI.BL.y > m_analysisROI.BR.y ? m_analysisROI.BL.y : m_analysisROI.BR.y;

	m_ROIBbox.x = xx1;
	m_ROIBbox.y = yy1;
	m_ROIBbox.width = xx2 - xx1;
	m_ROIBbox.height = yy2 - yy1;

	// ------------------------------------------------------------------------------------------------------------
	// Wyrysowaywanie poszczegolnych linii
	// ------------------------------------------------------------------------------------------------------------
	// Linia gorna w masce jest nieco krotsza ...
	singleLineParam(0, m_analysisROI.TL.x, m_analysisROI.TL.y, m_analysisROI.BL.x, m_analysisROI.BL.y, true);   // TL BL
	singleLineParam(1, m_analysisROI.TR.x, m_analysisROI.TR.y-5, m_analysisROI.BR.x, m_analysisROI.BR.y-5, false);  // TR BR
	singleLineParam(2, m_analysisROI.TR.x, m_analysisROI.TR.y, m_analysisROI.TL.x, m_analysisROI.TL.y, true);  // TR TL
	singleLineParam(3, m_analysisROI.BL.x, m_analysisROI.BL.y, m_analysisROI.BR.x, m_analysisROI.BR.y, false);  // BR BL


	// ------------------------------------------------------------------------------------------------------------
	// Utworzenie maski
	// ------------------------------------------------------------------------------------------------------------
	m_ROIMask = cv::Mat::zeros(m_ROIBbox.height, m_ROIBbox.width, CV_8U);
	bool inside[4];
	int jjm = 0;
	int iim = 0;
	for (int jj = m_ROIBbox.y, jjm=0; jj < m_ROIBbox.y + m_ROIBbox.height; jj++, jjm++) {
		for (int ii = m_ROIBbox.x, iim=0; ii < m_ROIBbox.x + m_ROIBbox.width; ii++, iim++) {

			// Testy na linie
			for (int k = 0; k <= 3; k++) {
				float e = m_lineParams[k].cos_alfa * ii + m_lineParams[k].sin_alfa * jj - m_lineParams[k].p;

				if (m_lineParams[k].gOrL)
					inside[k] = (e > 0) ? true : false;
				else
					inside[k] = (e > 0) ? false : true;

			}


			// tylko jesli wystepuje ruch
			if (inside[0] && inside[1] && inside[2] && inside[3]) {

				//std::cout << jjm << "|" << iim << std::endl;
				m_ROIMask.at <uchar> (jjm, iim) = 255;

			}



		}

	}

	singleLineParam(1, m_analysisROI.TR.x, m_analysisROI.TR.y, m_analysisROI.BR.x, m_analysisROI.BR.y, false);  // TR BR
	// debbug
	//cv::imshow("m_ROIMask", m_ROIMask);
	//cv::waitKey(0);



}

// ------------------------------------------------------------------------------------------------------------
//! Wyznaczanie parametrow dla linii
// ------------------------------------------------------------------------------------------------------------
void vAnalysis::singleLineParam(int idx, float x1, float y1, float x2, float y2, bool lor) {

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
bool vAnalysis::medianbinary1DFilter(bool res, int & medianCounter) {

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

// ------------------------------------------------------------------------------------------------
// diff_th_rgb
// - segmentacja w RGB
// - wariant z odlegloscia maksymalna -> trzeba zobaczyc jak to sie sprawdza i ew. skorygowac (ale to jest detal raczej)
// ------------------------------------------------------------------------------------------------
void vAnalysis::diff_th_rgb(cv::Mat A, cv::Mat B, cv::Mat roiMask, cv::Mat  & mask) {

	// Petla po obrazie
	for (int j = 0; j < A.rows; j++) {
		for (int i = 0; i < A.cols; i++) {

			// Je�li jestemy wewn�trz ROI
			if (roiMask.at<uchar>(j, i) > 0) {

				// Pobranie pixela z dwoch obrazkow
				cv::Vec<uchar, 3> p = A.at<vec_uchar_3>(j, i);
				cv::Vec<uchar, 3> b = B.at<vec_uchar_3>(j, i);


				// Obliczanie odleglosci (w RGB)
				double dR = fabs(double(p[0]) - double(b[0]));
				double dG = fabs(double(p[1]) - double(b[1]));
				double dB = fabs(double(p[2]) - double(b[2]));

				// Wybranie maksymalnej
				double temp;
				if (dR >= dG)
					temp = dR;
				else
					temp = dG;

				double max;
				if (dB > temp)
					max = dB;
				else
					max = temp;

				// Segmentacja
				//if (dY > m_YT | dCb > m_CbT | dCr > m_CrT)
				if (max > RGB_TH) {
					mask.at<vec_uchar_1>(j, i) = 255;
				} else
					mask.at<vec_uchar_1>(j, i) = 0;

			}
		}
	}
}

// ------------------------------------------------------------------------------------------------
// diff_th_rgb
// - segmentacja w RGB
// - wariant z odlegloscia maksymalna -> trzeba zobaczyc jak to sie sprawdza i ew. skorygowac (ale to jest detal raczej)
// ------------------------------------------------------------------------------------------------
void vAnalysis::diff_th_grey(cv::Mat A, cv::Mat B, cv::Mat roiMask, cv::Mat  & mask) {

	// Petla po obrazie
	for (int j = 0; j < A.rows; j++) {
		for (int i = 0; i < A.cols; i++) {

			// Je�li jestemy wewnatrz ROI
			if (roiMask.at<uchar>(j, i) > 0) {

				// Pobranie pixela z dwoch obrazkow
				cv::Vec<uchar, 1> p = A.at<vec_uchar_1>(j, i);
				cv::Vec<uchar, 1> b = B.at<vec_uchar_1>(j, i);


				// Obliczanie odleglosci (w RGB)
				double dG = fabs(double(p[0]) - double(b[0]));


				// Segmentacja
				if (dG > GREY_TH) {
					mask.at<vec_uchar_1>(j, i) = 255;
				} else
					mask.at<vec_uchar_1>(j, i) = 0;

			}
		}
	}
}



// ------------------------------------------------------------------------------------------------
// range
// - to nie jest optymalne !!
// ------------------------------------------------------------------------------------------------
void vAnalysis::rangeFilterTh(cv::Mat IG, cv::Mat  & mask, int threshold) {

	double min, max, range;
	cv::Point mini, maxi;

	for (int j = 1; j < IG.rows - 1; j++) {
		for (int i = 1; i < IG.cols - 1; i++) {

			cv::Rect r(i - 1, j - 1, 3, 3);
			cv::Mat C = IG(r);
			cv::minMaxLoc(C, &min, &max,&mini,&maxi);
			range = max - min;

			if (range < threshold) {
				mask.at<uchar>(j, i) = 255;
			}
			else {
				mask.at<uchar>(j, i) = 0;
			}


		}
	}

}
