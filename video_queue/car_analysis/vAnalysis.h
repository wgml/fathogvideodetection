#pragma once

#include "../globals.h"
#include "colourRecognition.h"
#include "typeRecognitionLBP.h"
#include <iostream>

// Modul analizy pojazdow tj. detekcji typu i koloru (ew. w przyszloœci rejestracji, marki itp.)
// Taki modul jest jeden na pas ruchu - najblizej skrzyzowania


//TODO
// Maska obszaru wewnatrz linii wyliczana na starcie systemu

#define MEDIAN_1D_SIZE 11		// rozmiar filtru medianowego
#define RGB_TH 30               // prog przy segmentacji w RGB
#define GREY_TH 20              // prog przy segmentacji w odcienaich szarosci
#define RANGE_TH 15             // prog dla filtru range (max-min)




class vAnalysis {

public:
	// Konstruktor
	vAnalysis(colourRecognition *colourR, typeRecognitionLBP *typeR);

	// Destruktor
	~vAnalysis();

	// Inicjalizacja
	void init(queueROIPolygon analysisROI);

	// Krok analizy tj. detekcji kolotu i typu pojazdu
	void step(const cv::Mat & image_rgb,const cv::Mat & image_gray,const cv::Mat & image_lbp,const cv::Mat & movementMask,const cv::Mat & edgeMask);

	// Detekcja pojazdu
	bool detectVehiclePrecence(cv::Mat movementMask, cv::Mat edgeMask);


	// Aktualizacja modelu tla
	void bgModelUpdate(cv::Mat image_rgb, cv::Mat image_gray, cv::Mat movementMask, cv::Mat edgeMask);

	// Wizualizacja obszaru
	void draw(cv::Mat image_vis);

	// tymczasowo
	queueROIPolygon m_analysisROI;	  //!< wspolrzedne ROI

private:

	cv::Rect m_ROIBbox;             //!< prostokat otaczajacy dla ROI
	lineParams m_lineParams[4];     //!< parametry linii zaklada sie, ze ROI sklada sie z czterech linii (kolejnosc TL-BL, TR - BR, TL-TR, BL-BR)

	int m_edgeThreshold;            //!< prog krawedzi
	int m_movementThreshold;        //!< zwiazany z ruchem

	int m_edgeCounter;              //!< licznik krawedzi w danynm obszarze
	int m_movementCounter;          //!< licznik ruchu w danynm obszarze

	double m_dEdgeCoefficient;		//!< wspolczynnik opisujacy ile jest krawedzi w danej lokalizacji
	bool   m_bMovementDetcted;		//!< czy wykryto ruch
	bool   m_bEdgeDetcted;			//!< czy wykryto krawedzie

	int   m_bMovementDetctedCnt;    //!< czy wykryto ruch (licznik)
	int   m_bEdgeDetctedCnt;        //!< czy wykryto krawedzie (licznik)

	bool m_vehiclePresent;
	bool m_vehiclePresent_prev;



	cv::Mat m_bgModel;                //!< model tla
	cv::Mat m_currentROI;             //!< biezacy obraz ROI
	cv::Mat m_currentROIgrey;         //!< biezacy obraz ROI w odcieniach szarosci
	cv::Mat m_currentROIlbp;          //!< biezacy obraz ROI LBP


	cv::Mat m_ROIMask;                //!< maska obszaru ROI
	cv::Mat m_objectMask;             //!< maska obiekow pierwszoplanowych
	cv::Mat m_uniformAreaMask;        //!< maska jedorodnych obszarow


    colourRecognition *mp_colourR;          //! obiekt rozpoznawacza kolorow samochodow
    typeRecognitionLBP   *mp_typeR;            //! obiekt rozpoznawacza typu samochodow


	// Funkcje pomocnicze

	// Obliczenie parametow ROI
	void computeROIlineParams();

	// Obliczanie parametrow dla pojedynczej linii
	void singleLineParam(int idx, float x1, float y1, float x2, float y2, bool lor);

	// Filtracja medianowa 1D (binarna)
	bool medianbinary1DFilter(bool res, int & medianCounter);

	// Segmentacja obiektow na podstawie odejmowanie tla (model RGB)
	void diff_th_rgb(cv::Mat A, cv::Mat B, cv::Mat roiMask, cv::Mat  & mask);

	// Segmentacja obiektow na podstawie odejmowanie tla (model greyscale)
	void diff_th_grey(cv::Mat A, cv::Mat B, cv::Mat roiMask, cv::Mat  & mask);

	// Filtr range 3x3
	void rangeFilterTh(cv::Mat IG, cv::Mat  & mask, int threshold);

};


