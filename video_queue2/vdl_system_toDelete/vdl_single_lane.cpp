#include "vdl_single_lane.h"



// Constructor
// ------------------------------------------------------------------------------------------
vdlSingleLane::vdlSingleLane() {
}

// destructor
// ------------------------------------------------------------------------------------------
vdlSingleLane::~vdlSingleLane() {

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
		std::cout <<"Inicjalizacja VDL" << i << std::endl;
		sprintf(buf, "_%d",i);
		vdl v;
		v.initVdl(cv::Point(laneStartX[i], laneStartY[i]), cv::Point(laneEndX[i], laneEndY[i]), LINE_H, laneName + buf);
		v.setThresholds(SAD_TH,HE_TH,CENSUS_TH,VE_TH);
		m_vdls.push_back(v);
	}

	m_iCarCounter = 0;
}


// ------------------------------------------------------------------------------------------
// step
// ------------------------------------------------------------------------------------------
int vdlSingleLane::step(const cv::Mat & image_grey,const  cv::Mat & image_rgb,const  cv::Mat & image_rgb_gauss,const  cv::Mat & movementMask,const  cv::Mat & edgeMask) {

	// UWaga to dziala jako tako, ale tylko dla jednej linii
	bool bCarDetected;
	for (int i=0; i < m_iNvdls; i++) {
        // debug
        std::cout <<"Obliczenia dla linii VDL" << i << std::endl;
		//m_iCarCounter = m_vdls[i].step(image_grey, image_rgb, image_rgb_gauss, bCarDetected); // To jest takie prostackie podejscie na raize
    }

	// Tutaj bedzie analiza wynikow z jednego pasa
	// Gromadzenie detekcji itp.

	m_iCarCounterPrev = m_iCarCounter;
	return m_iCarCounter;
}


// ------------------------------------------------------------------------------------------
// wyrysowanie VDL
// ------------------------------------------------------------------------------------------
void vdlSingleLane::drawVDL(cv::Mat & image_rgb){
	// wyrysowanie wszystkich linii detekcji
	for (int i=0; i < m_iNvdls; i++)
		m_vdls[i].drawVDL(image_rgb);
}


