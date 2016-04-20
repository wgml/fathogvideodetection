#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "globals.h"
#include <iostream>


#define MEDIAN_1D_SIZE 11


// Klasa dla pojedycznego obszaru detekcji
class queue_roi {

public:
	queue_roi(queueROIPolygon queueROI);
	~queue_roi();

	bool step(cv::Mat movementMask, cv::Mat edgeMask, cv::Mat image_grey);
	void draw(cv::Mat image_vis, cv::Mat image_grey);

	// tymczasowo
	queueROIPolygon m_queueROI;	  //!< wspolrzedne ROI

private:


	cv::Rect m_ROIBbox;           //!< prostokat otaczajacy dla ROI
    cv::Mat  m_darkAreas;     //!< mapa ciemnych obszarow


	lineParams m_lineParams[4];   //!< parametry linii zaklada sie, ze ROI sklada sie z czterech linii (kolejnosc TL-BL, TR - BR, TL-TR, BL-BR)

	int m_edgeThreshold;            //!< prog krawedzi
	int m_movementThreshold;        //!< prog zwiazany z ruchem
	int m_darkAreasThreshold;       //!< prog zwiazany  z ciemnymi obszarami

	int m_edgeCounter;              //!< licznik krawedzi w danyn obszarze
	int m_movementCounter;          //!< licznik ruchu w danyn obszarze
	int m_darkAreasCounter;         //!< licznik ciemnych pikseli w danyn obszarze

	int m_meanGreyValue;             //!< srednia szarosc





	double m_dEdgeCoefficient;    //!< wspolczynnik opisujacy ile jest krawedzi w danej lokalizacji
	bool   m_bMovementDetcted;    //!< czy wykryto ruch
	bool   m_bEdgeDetcted;        //!< czy wykryto krawedzie
	bool   m_bDarkAreaDetcted;    //!< czy wykryto ciemny obszar


	int   m_bMovementDetctedCnt;    //!< czy wykryto ruch
	int   m_bEdgeDetctedCnt;        //!< czy wykryto krawedzie
	int   m_bDarkAreaDetctedCnt;    //!< czy wykryto ciemny obszar



	// tool function
	void computeROIlineParams();
	void singleLineParam(int idx, float x1, float y1, float x2, float y2, bool lor);

	bool median1DFilter(bool res, int & medianCounter);



};
