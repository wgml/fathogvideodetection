#ifndef VDL_SINGLE_LANE
#define VDL_SINGLE_LANE

#include "../globals.h"
#include "vdl.h"

// THRESHOLDS & PARAMS
#define LINE_H 1          // Wyskosci linii

//#define SAD_TH 4*0.75     // prog dla SAD
#define HE_TH 5*0.75      // prog dla poziomych krawedzi
//#define CENSUS_TH 0.03    // prog dla transformaty census


#define VE_TH 10 // nie wiem czy nie 5 w nocy ... // w tym momencie pionowe krawedzie nie sa uzywane


class vdlSingleLane {
public:

	// Konstuktor
	vdlSingleLane();

	// Destruktor
	~vdlSingleLane();

	// Inicjalizcja jednej linii
	void initSingleLane(std::string laneName, int nVdls, int gt, int *laneStartX, int *laneStartY, int *laneEndX, int *laneEndY);

	// Krok algorytmu dla linii
	int step(const cv::Mat & image_grey,const  cv::Mat & image_rgb,const  cv::Mat & image_rgb_gauss,const  cv::Mat & movementMask,const  cv::Mat & edgeMask);

	// Wyrysowanie linii VDL
	void drawVDL(cv::Mat & image_rgb);


	int m_iNvdls;			//!< liczba wirtualnych linii detekcji
	int m_iGt;			    //!< ground truth -- liczba pojazdow, ktore faktycznie powinny zostac zliczone

	int m_iCarCounter;		 //!< licznik samochodow
	int m_iCarCounterPrev;	 //!< licznik samochodow (poprzednia iteracja)


	std::vector<vdl> m_vdls; //!< kontener na poszczegolne VDL'e
};


#endif
