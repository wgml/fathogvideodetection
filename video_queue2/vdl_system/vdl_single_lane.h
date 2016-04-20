#ifndef SINGLE_LANE
#define SINGLE_LANE

#include "../globals.h"
#include "vdl.h"

class vdlSingleLane {
public:

	// Konstuktor
	vdlSingleLane();

	// Destruktor
	~vdlSingleLane();

	// Inicjalizcja jednej linii
	void initSingleLane(std::string laneName, int nVdls, int gt, int *laneStartX, int *laneStartY, int *laneEndX, int *laneEndY);

	// Krok algorytmu dla linii
	int step(const cv::Mat & image_grey,const cv::Mat & image_rgb, const cv::Mat & image_rgb_gauss,  int iFrame);

	// Wyrysowanie linii VDL
	void drawVDL(cv::Mat & image_rgb);


	int m_iNvdls;			//!< liczba wirtualnych linii detekcji
	int m_iGt;			    //!< ground truth -- liczba pojazdow, ktore faktycznie powinny zostac zliczone

	//int m_iCarCounter;		 //!< licznik samochodow
	//int m_iCarCounterPrev;	 //!< licznik samochodow (poprzednia iteracja)


	vdl m_vdls[VDL_MAX_VDL_ON_LANE];					//!< kontener na poszczegolne VDL'e
	int m_tSpeedEstimation[VDL_MAX_VDL_ON_LANE];        //!< -||- zapamietywanie iinformacji o czasie
	int m_iCarCounter[VDL_MAX_VDL_ON_LANE];             //!< licznik samochowod



};


#endif
