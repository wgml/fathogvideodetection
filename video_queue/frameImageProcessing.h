#pragma once

#include "globals.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#include "common/edgeDetectionFPGA.h"


// TYPY OBRAZU LBP
#define SLBP 0          // typowy LBP -> do 256
#define ULBP 1          // jednorodny LBP -> do
#define NRULB 2         // nie redundanty LBP


// EW. DODATKOWE PARAMTERY LBP
#define LBP_BIAS 10

class frameImageProcessing {

public:

	frameImageProcessing(int YY, int XX);
	~frameImageProcessing();

	//! Krok algorytmu
	void step(const cv::Mat & image_rgb, const cv::Mat & image_gray);

	//! Przekazanie wyniku odejmowania ramek
	void getMovementMask(cv::Mat &movementMask);

	//! Przekazanie wyniku detekcji krawedzi
	void getEdgeMask(cv::Mat &edgeMask);

    //! Przekazanie wyniku obliczania LBP
	void getLBP(cv::Mat &lbp);

	//! Przekazanie wyniku obliczania filtracji Gaussa
	void getGaussianImage(cv::Mat &gaussianImage);




private:

	int m_iXX;  //!< szerokosc ramki
	int m_iYY;  //!< wysokosc ramki

	int m_iframeDiffTh;		   //!< prog binaryzacji dla roznicy obrazow


	cv::Mat image_rgb_prev;     //!< poprzednia ramka w kolorze

	cv::Mat movementMask;      //!< zbinaryzowany wynik odejmowania dwoch kolejnych ramek
    cv::Mat edgeMask;          //!< zbinaryzowany wynik detekcji krawedzi
    cv::Mat lbpImage;          //!< obraz LBP
    cv::Mat m_gaussianImage;     //!< obraz po filtracji Gaussa

    uchar m_iNRULBP_LUT[128];  //!< tablica LUT przekodowania NR -> NRULBP

    // TOOL
	void rgbImageDifferenceTh(const cv::Mat & i1,const cv::Mat & i2, cv::Mat &mask, float threshold);		// obliczanie roznicy pomiedzy dwoma obrazami RGB (odleglosc modulowa)
	void edgeImageTh(const cv::Mat & i, cv::Mat & edgeMask, float threshold);							    // obliczanie maski krawedzi obiektow widocznych na scenie
	void lbp(const cv::Mat & i, int type);                                                                  // obliczanie deskryptora LBP
	void gaussianFiltering(const cv::Mat & i);                                                              // filtracja Gaussa


};
