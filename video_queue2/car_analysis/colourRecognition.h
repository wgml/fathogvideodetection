#pragma once
#include "../globals.h"

// Plik w ktorym zapisane sa nazwy kolorow
#define COLOUR_NAMES_FILE "colourModel/colourNames.txt"
#include <iostream>


// Stuktura opsuja model koloru
struct colourModel {

	std::string name;              // nazwa
	cv::Mat meanVector;            // srednia
	cv::Mat covarianceMatrix1;     // kowariancja
	double scalingCoeff;           // wspolczynnik skalujacy (tj. czlon staly przed)

};

// Klasa odpowiedzialna za rozpoznawania koloru

class colourRecognition {
	public:
		colourRecognition();
		~colourRecognition();

		std::string recognizeColour(cv::Mat I, cv::Mat mask);	//!< rozpoznawanie kolorw na danej masce

	private:

		int mi_numberOfUsedColours;					            //!< liczba uzywanych kolorow
		std::vector<colourModel> m_colourModels;                //!< struktura z zapamietanymi kolorami

		// TOOL

		void initColourModelFromFile();                             //!< inicjalizacji modelu koloru na podstawie pliku
		void createGaussianModel(cv::Mat I, colourModel &model);    //!< stworzenie modelu Gaussowskiego

};
