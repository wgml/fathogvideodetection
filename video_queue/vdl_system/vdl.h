#ifndef VDL_H
#define VDL_H
// Klasa Virtual Detection Line

#include "../globals.h"
#include <queue>
#include <iostream>


#include "analyseCarPatch_day_zynq.h"
#include "analyseCarPatch_night_zynq.h"





class vdl {

public:

	vdl();
	~vdl();

	void initVdl(cv::Point lStart, cv::Point lEnd, int lineContext, std::string name);
	void setThresholds(double sad_th, double sx_th, double census_th, double sy_th);
	int step(const cv::Mat & image_grey,const  cv::Mat & image_rgb,const  cv::Mat & image_rgb_gauss, bool & bCarStart);

	void drawVDL(cv::Mat &image);

	cv::Point m_lStart;
	cv::Point m_lEnd;

private:


	int **m_piVdl_points;           // punkty lezace na VDL

	int ***m_piVdl_window;          // okno biezace    (RGB)
	int ***m_piVdl_window_prev;     // okno poprzednie (RGB)

	int m_iIteration;               // numer iteracji algorytmu


	int m_iLineWidth;               // rozmiramy linii detekcji
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

#ifdef VISUALIZE_VDL
	// Wizualizacja okna
	cv::Mat m_vdl_vis;
	cv::Mat m_vdl_vis_binary;
	double m_iVdl_vis_iterator;
#endif

	// Parametry z okien
	double m_d_sad;
	double m_d_census;
	double m_d_sx;
	double m_d_sy;

	// Progi
	double m_d_sad_th;
	double m_d_sx_th;
	double m_d_sy_th;
	double m_d_census_th;

	// Wyniki progowania
	bool m_b_sad;
	bool m_b_sx;
	bool m_b_sy;
	bool m_b_census;
	bool m_b_road;          // znaleziono ulice
	bool m_b_road_prev;     // znaleziono poprzednio

	// Bufory na wyniki progowania
	int *m_pb_road;
	int *m_pb_sy;
	int m_iMedianBoolBufferIterator;
	int m_iMedianBoolBufferIterator_sy;


	unsigned char *m_pcSingleLineCharBuffer;    // bufor do przekazywanie pojedynczej linii obrazu do analizy ...


	// Fragment samochow
	int m_iCarPatchStart;
	int m_iCarPatchEnd;
	int m_iCarPatchCounter;

	// Licznik samochodow
	int m_iCarCounter;

	std::string m_moduleName;           // nazwa modulu


	// Zwiazane z analyseCarPatch_zynq

	unsigned char m_vdl_buffer[VDL_MAX_VDL_HEIGHT][VDL_MAX_VDL_WIDTH][4];        // bufor na linie vdl
	unsigned char m_imageRGBBuffer[VDL_MAX_VDL_HEIGHT][VDL_MAX_VDL_WIDTH][4];    // bufor na obraz RGB
	unsigned char m_imageThNightTime[VDL_MAX_VDL_HEIGHT][VDL_MAX_VDL_WIDTH];     // bufor na obraz po progowaniu do trybu nocnego

	bool m_medianBoolRoad[VDL_MEDIAN_BOOL_FILTER_SIZE];                          // bufor na mediane

	int  m_vdl_buffer_iterator;     // iterator po buforze vdl
	int  m_medianBoolRoadIterator;  // iterator po buforze medianowym
	int  m_sumRoad;                 // okreslanie czy na ulicy
	bool m_b_road_median_prev;      // poprzednia wartosc flagi road

	int m_carPatchStart;        // poczatek fragmentu samochodu
	int m_carPatchEnd;          // koniec fragmentu samochodu
	int m_carPatchCounter;      // liczba wykrytuy





#ifdef FILE_VDL
	FILE *m_dataFile;               // plik do zapisu danych (do testow)
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
