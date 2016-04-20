#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>

#define DRAW             // czy ma cos byc rysowane (
#define DRAW_QUEUE       // rysowanie wynikow dla systemu kolejkek
//#define DRAW_ANALYSIS    // rysowanie wyników dla analizy
//#define DRAW_VDL       // rysowanie wynikow dla VDL
//#define VISUALIZE_VDL  // wizulizacja dzialania VDL
//#define FILE_VDL         // operacje zwiazane z logowaniem do plikow VDL

typedef cv::Vec<uchar, 3> vec_uchar_3;
typedef cv::Vec<uchar, 1> vec_uchar_1;
typedef cv::Vec<float, 1> vec_float_1;


// Opis pojedynczego ROI dla kolejki (detekcji obecnosci pojazdu)
// + zaklada sie, ze dol (botto) to ew. poczatek kolejki, a top to koniec kolejki

struct queueROIPolygon {

	cv::Point TL;	//!< top left corner
	cv::Point BL;   //!< bottom left corner
	cv::Point TR;   //!< top right corner
	cv::Point BR;   //!< bottom right corner

	int movementTh;
	int edgeTh;
};


// Strutkura na parametry prostej w postaci normalnej
struct lineParams {
	float sin_alfa;
	float cos_alfa;
	float p;
	bool gOrL;		//! Okreslanie, czy znaku nierownosci, ktory nalezy zastosowac
};

// PARAMETRY


// OPERACJE GLOBALNE
#define FRAMEOP_FRAME_DIFF_TH 20		// prog binaryzacji dla roznicy sasiednich ramek



// PARAMTERY KLASYFIKATORA LBP
#define LBP_BSIZE 32  // rozmiar bloku (kwadratowy)
#define LBP_HSIZE 30  // rozmiar histogramu
#define LBP_XX 160    // rozmiar okna detekcji
#define LBP_YY 224    // rozmiar okna detekcji


// =========================================================================================================================
// PARAMTERY VDL
// =========================================================================================================================
// Przelaczenie dzien/noc
#define NIGHTTIME 0


#define VDL_MAX_LANES 3             // maksymalna liczba pasow w systemie
#define VDL_MAX_VDL_ON_LANE 3       // maksymalna liczba VDL na jednym pasie


// TU SIE EW. USTAWIA PARAMETRY CZULOSCI LINII DETEKCJI !!!

// NOC [0-3] 0 i 1 61
// POLMROK [0] 0 - 66 1 - 56
// DESZCZ [0-5] 0 - 61 1 - 66
// SLONCE 3_1 - 56
//43 -dzien

#define VDL_LINE_HEIGHT 2          // polowa wyskosci linii detekcji (tj. analzowanego otoczenia)

#define VDL_LINE_WIDTH 56
#define VDL_SAD_TH 10
#define VDL_SX_TH 15
#define VDL_CENSUS_TH 0.3


// THRESHOLDS & PARAMS

// DO USUNIECIA
//#define SAD_TH 4*0.75     // prog dla SAD
//#define HE_TH 5*0.75      // prog dla poziomych krawedzi
//#define CENSUS_TH 0.03    // prog dla transformaty census
//#define VE_TH 10 // nie wiem czy nie 5 w nocy ... // w tym momencie pionowe krawedzie nie sa uzywane

#define VDL_LBP_BIAS 5


#define VDL_MEDIAN_FILTER_SIZE 11           // rozmiar okna filtra medianowego
#define VDL_MEDIAN_BOOL_FILTER_SIZE 11      // rozmiar okna binarnego filtra medianowego

#define VDL_IMAGE_LENGTH 800                // dlugosc obrazu VDL
#define VDL_IMAGE_INFO 40                   // dodatkowa informacja do obrazu VDL ...

//#define SHORT_WINDOW_SIZE 5

#define VDL_MINIMAL_PATCH_HEIGHT_TH 5        // minimalna wysokosc fragmentu obrazu, ktora bedzie analizowana


#define VDL_EDGE_TH 100                         // prog kawedzi do analizy patchow
#define VDL_NIGHT_TH 200                        // segmentacja swiatel
#define VDL_SHADOW_TH 50                        // detekcja cienia 50 ?



// analyse car patch zynq
#define VDL_MAX_VDL_WIDTH  200                  // maksymalna mozliwa szerokosci linii detekcji
#define VDL_MAX_VDL_HEIGHT 200                  // maksymalna mozliwa wysokosc pojedynczeg obrazka

//#define MINIMAL_PATCH_HEIGHT_TH 5


