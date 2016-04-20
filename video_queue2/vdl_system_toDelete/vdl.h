#ifndef VDL_H
#define VDL_H
// Klasa Virtual Detection Line

#include <queue>
#include <iostream>

#include "analyseCarPatch_day_zynq.h"
#include "analyseCarPatch_night_zynq.h"


#define MEDIAN_FILTER_SIZE 11
#define MEDIAN_BOOL_FILTER_SIZE 11
#define VDL_IMAGE_LENGTH 800
#define VDL_IMAGE_INFO 40     // dodatkowa informacja do obrazu VDL ...

#define SHORT_WINDOW_SIZE 5

#define MINIMAL_PATH_LENGTH_TH 5


#define EDGE_TH 100        // prog kawedzi do analizy patchow
#define NIGHT_TH 200       // segmentacja swiatel




// analyse car patch zynq
#define MAX_VDL_WIDTH  100 // maksymalna mozliwa szerokosci linii detekcji
#define MAX_VDL_HEIGHT 200 // maksymalna mozliwa wysokosc pojedynczeg obrazka

#define MINIMAL_PATCH_HEIGHT_TH 5




// TU SIE EW. USTAWIA PARAMETRY CZULOSCI LINII DETEKCJI !!!

// NOC [0-3] 0 i 1 61
// POLMROK [0] 0 - 66 1 - 56
// DESZCZ [0-5] 0 - 61 1 - 66
// SLONCE 3_1 - 56
//43 -dzien

#define VDL_LINE_WIDTH 56
#define SAD_TH 8*3*VDL_LINE_WIDTH
#define SX_TH 30*3*VDL_LINE_WIDTH
#define CENSUS_TH int(0.75*3*VDL_LINE_WIDTH)

// Prze³¹czenie dzieñ/noc
#define NIGHTTIME 0



typedef cv::Vec<uchar, 3> VT_3;
typedef cv::Vec<uchar, 1> VT_1;
typedef cv::Vec<float, 1> VF_1;


class vdl {

public:

	vdl();
	~vdl();

	void initVdl(cv::Point lStart, cv::Point lEnd, int lineContext, std::string name);
	void setThresholds(double sad_th, double sx_th, double census_th, double sy_th);
	int step(const cv::Mat & image_grey,const  cv::Mat & image_rgb,const  cv::Mat & image_rgb_gauss, bool & bCarStart);

	void drawVDL(cv::Mat &image);

private:
	cv::Point m_lStart;
	cv::Point m_lEnd;

	int ** m_piVdl_points;

	int ***m_piVdl_window;          // okno biezace (RGB
	int ***m_piVdl_window_prev;     // okno poprzednie (RGB)

	int m_iIteration;   // algorithm iteration number


	int m_iLineWidth;
	int m_iLineHeight;

	// Bufory na filtrowanie parametrow otrzymanych z analizy okien...
	double *m_pdD_sad;
	double *m_pdD_census;
	double *m_pdD_sx;
	double *m_pdD_sy;

	int m_iMedianBufferIterator;
	int m_iMedianBufferIterator_sy;




	// Krotkie okno do krawedzi pionowych ...
	int **m_piShortWindow;


	// Wizualizacja okna
	cv::Mat m_vdl_vis;
	cv::Mat m_vdl_vis_binary;

	double m_iVdl_vis_iterator;


	// Parametry z okien
	double m_d_sad;
	double m_d_census;
	double m_d_sx;
	double m_d_sy;

	// Progi
	double m_d_sad_th;          // Prog dla wskaznika SAD
	double m_d_sx_th;           // Prog dla analizy krawedzi poziomych
	double m_d_sy_th;           // Prog dla analizy krawedzi pionowych
	double m_d_census_th;       // Prog dla transformaty CENSUS

	// Wyniki progowania
	bool m_b_sad;
	bool m_b_sx;
	bool m_b_sy;
	bool m_b_census;
	bool m_b_road;              // Wyktyto ulice
	bool m_b_road_prev;         // Wykryto ulice w poprzedniej iteracji

	// Bufory na wyniki progowania
	int *m_pb_road;
	int *m_pb_sy;
	int m_iMedianBoolBufferIterator;
	int m_iMedianBoolBufferIterator_sy;


	unsigned char *m_pcSingleLineCharBuffer; // Bufor do przekazywanie pojedynczej linii obrazu do analizy ...


	// Fragment samochow
	int m_iCarPatchStart;
	int m_iCarPatchEnd;
	int m_iCarPatchCounter;

	// Licznik samochodow
	int m_iCarCounter;


	std::string m_moduleName;

#ifdef FILE_VDL
	FILE *m_dataFile;
#endif
	// Functions
	void computeLine();            // wyzacznie wspolrzednych punktow wchodzaych w skald linii detekcji
	void comparePatches();         // porownanie dwoch fragmentow obrazu (biezacego i poprzedniego)
	void medianFilter1D();         // filtracja mednianowa detektorow SAD, Census, Edfe
	void edgesInShortWindow();     // karwedzie w pinowym oknie (na raize nie uzywane)
	void analysis();               // analiza detekcji - tj. progwanie wartosci + filtacja


	int analyseCarPatch();         // wycinanie fragmentu obrazka


	int analyseCarPatch_zynq(unsigned char *buffer, unsigned char *imageBuffer);                                          // wycinanie -- wersja zynq
	//int analyseCarPatch_night_zynq(unsigned char thImage[][MAX_VDL_WIDTH], int vdl_line_width, int car_patch_height);      // analiza patcha - wersja "nocna" -- zynq
	// int analyseCarPatch_day_zynq(unsigned char rgbImage[][MAX_VDL_WIDTH][4], int vdl_line_width, int car_patch_height);    // analiza patcha - wersja "dzienna"


};


#endif
