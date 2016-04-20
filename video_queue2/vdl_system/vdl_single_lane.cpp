#include "vdl_single_lane.h"


// ------------------------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------------------------
vdlSingleLane::vdlSingleLane() {

	for (int i = 0; i < VDL_MAX_VDL_ON_LANE; i++) {
		m_tSpeedEstimation[i] = 0;
		m_iCarCounter[i] = 0;
	}
	std::cout <<"vdlSingleLane" << std::endl;
}

// ------------------------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------------------------
vdlSingleLane::~vdlSingleLane() {
    std::cout <<"~vdlSingleLane" << std::endl;
}

// ------------------------------------------------------------------------------------------
// initSinglelane
// ------------------------------------------------------------------------------------------
void vdlSingleLane::initSingleLane(std::string laneName, int nVdls, int gt, int *laneStartX, int *laneStartY, int *laneEndX, int *laneEndY){

	m_iNvdls = nVdls;					// ile lini vdl
	m_iGt = gt;							// wartosc referencyjna dla zliczenia
	char buf[100];					    // pomocniczy bufor tekstowy

    // tworzenie kolejnych linii VDL
	for (int i=0; i < m_iNvdls; i++) {

		sprintf(buf, "_%d",i);  // dodanie numeru do nazwy

		m_vdls[i].initVdl(cv::Point(laneStartX[i], laneStartY[i]), cv::Point(laneEndX[i], laneEndY[i]), VDL_LINE_HEIGHT, laneName+buf);   // inicjalizacja
		m_vdls[i].setThresholds(VDL_SAD_TH, VDL_SX_TH, VDL_CENSUS_TH, 0);                                                        // ustawienie progow
		m_iCarCounter[i] = 0;

	}
	std::cout <<"vdlSingleLane::initSingleLane " << laneName <<std::endl;
}


// ------------------------------------------------------------------------------------------
// step
// UWAGA - tu jest zalozenie, ze sa maksymalnie tylko !!3!! linie detekcji
// ------------------------------------------------------------------------------------------
int vdlSingleLane::step(const cv::Mat & image_grey,const cv::Mat & image_rgb, const cv::Mat & image_rgb_gauss,  int iFrame) {

	bool bCarDetected;                      // czy auto zostalo wykryte

	double dt1, dt2, dx1, dx2, v1, v2, v;   // zmienne zwiazane z wyliczaniem predkosci

    // petla do VDL'ach
	for (int i = 0; i < m_iNvdls; i++) {

        // uruchomienie analizy na poszczegolnych VDL'ach
		m_iCarCounter[i] = m_vdls[i].step(image_grey, image_rgb,image_rgb_gauss, bCarDetected);

        // jesli wykryto samochod
		if (bCarDetected) {
		    // zapamietanie czasu
			m_tSpeedEstimation[i] = iFrame;

			// wykrycie czy jestesmy na tej ostatniej linii
			if (i == m_iNvdls - 1) {
				// Obliczamy dwie predkosci

                // Jesli na kazdej z linii nastapil pomiar oraz jest on prawidłowy.
				if (m_tSpeedEstimation[1] > 0 && m_tSpeedEstimation[0] > 0 && m_tSpeedEstimation[2] > 0 &&
					m_tSpeedEstimation[1] > m_tSpeedEstimation[0] && m_tSpeedEstimation[2] > m_tSpeedEstimation[1] ) {

					// Roznica w czasie detekcji
					dt1 = m_tSpeedEstimation[1]- m_tSpeedEstimation[0];
					dt2 = m_tSpeedEstimation[2] - m_tSpeedEstimation[1];
					// Roznica w odleglosi pomiedzy liniami
					dx1 = m_vdls[1].m_lStart.y - m_vdls[0].m_lStart.y;
					dx2 = m_vdls[2].m_lStart.y - m_vdls[1].m_lStart.y;
					// Poszczegolne predkosci
					v1 = dx1 / dt1;
					v2 = dx2 / dt2;
					// Predkosc srednia
					v = (v1 + v2) / 2;
				}
				else {
					v = -1;
				}
				//czyczenie
				m_tSpeedEstimation[0] = 0;
				m_tSpeedEstimation[1] = 0;
				m_tSpeedEstimation[2] = 0;


				//std::cout << "Speed = " << v << std::endl;
				//cv::waitKey(0);
			}
		}
	}

	//debug wypisanie
	std::cout << "Speed ";
	for (int i = 0; i < m_iNvdls; i++) {
		std::cout <<m_tSpeedEstimation[i] << " ";

	}
	std::cout << std::endl;

	std::cout << "Count ";
	for (int i = 0; i < m_iNvdls; i++) {
	 		std::cout <<m_iCarCounter[i] << " ";

	}
	std::cout << std::endl;


	// Tutaj bedzie analiza wynikow z jednego pasa
	// Gromadzenie detekcji itp.

    // Roboczo ...
	return m_iCarCounter[0];
}


// ------------------------------------------------------------------------------------------
// wyrysowanie VDL
// ------------------------------------------------------------------------------------------
void vdlSingleLane::drawVDL(cv::Mat & image_rgb){

	// wyrysowanie wszystkich linii detekcji
#ifdef DRAW_VDL
    for (int i=0; i < m_iNvdls; i++)
		m_vdls[i].drawVDL(image_rgb);
#endif

}


