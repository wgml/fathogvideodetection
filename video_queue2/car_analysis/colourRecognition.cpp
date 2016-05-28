#include "colourRecognition.h"
#include <fstream>
#include <iostream>

// ------------------------------------------------------------------------------------------------------------
// colourRecognition
// ------------------------------------------------------------------------------------------------------------
colourRecognition::colourRecognition() {

	// inicjalizacja modelu
	initColourModelFromFile();

}

// ------------------------------------------------------------------------------------------------------------
// ~colourRecognition
// ------------------------------------------------------------------------------------------------------------
colourRecognition::~colourRecognition(){

}


// ------------------------------------------------------------------------------------------------------------
// recognizeColour
// ------------------------------------------------------------------------------------------------------------
std::string colourRecognition::recognizeColour(cv::Mat I, cv::Mat mask){


	// Tablica do rozpoznawania kolorow
	double *pdColourSimilarities = new double[mi_numberOfUsedColours];
	memset(pdColourSimilarities, 0, mi_numberOfUsedColours*sizeof(double));


	//I = cv::imread("colourModel/red.png");
	//mask = cv::Mat::ones(100, 100, CV_8U);


	//debug
	//cv::imshow("I", I);
	//cv::imshow("mask", mask);
	//cv::waitKey(0);

	// debug ++
	//cv::imwrite("I.png", I);
	//cv::imwrite("mask.png", mask);



	cv::Mat x = cv::Mat::zeros(1, 3, CV_64F);           // Pojedynczy piksel obrazu
	cv::Mat ICS;                                        // Obraz w nowej przestrzeni barw (tu YCbCr)

	cv::cvtColor(I, ICS, cv::COLOR_BGR2YCrCb);          // Konwersja fragmentu obrazu do przestrzeni YCbCr

    int minC;

	// petla po obrazu
	for (int jj = 0; jj < ICS.rows; jj++) {
		for (int ii = 0; ii < ICS.cols; ii++) {

			// jeœli obszar nadaje sie do analizy ...
			if (mask.at<uchar>(jj, ii) > 0 ) {

				x.at <double>(0, 0) = double(ICS.at<vec_uchar_3>(jj, ii)[0]);
				x.at <double>(0, 1) = double(ICS.at<vec_uchar_3>(jj, ii)[1]);
				x.at <double>(0, 2) = double(ICS.at<vec_uchar_3>(jj, ii)[2]);

                minC = 0;
                double minV = INFINITY;
				for (int c = 0; c < mi_numberOfUsedColours; c++) {

					cv::Mat xm = m_colourModels[c].meanVector;  // pobranie wekrora średniego

					cv::Mat dx = x - xm;                        // obliczenie odlgelosci

					double dx1 = dx.at <double>(0, 0);
					double dx2 = dx.at <double>(0, 1);
					double dx3 = dx.at <double>(0, 2);

					//cv::Mat z = -0.5 * ((dx))*cov*(dx.t());

					double zz = fabs(dx1) + 2*(fabs(dx2) + fabs(dx3)); // obliczenie sumy modulow

                    if ( zz < minV) {
                        minC = c;
                        minV = zz;
                    }

					//std::cout << c << "|" << zz << std::endl;

				}
				pdColourSimilarities[minC] += 1 /*m_colourModels[c].scalingCoeff **/ ; // podobienstwo do tego koloru
			}

		}
	}

	// Znajdujemy minumum bez szkla.... (najbardziej podobny w sensie minimum)
	// ++ trudno szarych nie bedzie wykrywac

	int sumWithoutBlack =0;

	int maxC = 0;
	double maxV = 0;


	for (int c = 0; c < mi_numberOfUsedColours - 1; c++) {

	    // debug
	    std::cout << m_colourModels[c].name << " -> " << pdColourSimilarities[c] << std::endl;

        if ( c > 0 )
            sumWithoutBlack += pdColourSimilarities[c];

		if (pdColourSimilarities[c] > maxV &&  c != 0) {
			maxC = c;
			maxV = pdColourSimilarities[c];
		}
	}

	//jesli suma pozostalych kolorow jest x2 niz czarnych
	if ( sumWithoutBlack < 0.25* pdColourSimilarities[0])
        minC = 0;
    else
        minC = maxC;

    // debug
    std::cout << m_colourModels[minC].name << std::endl;
    //cv::waitKey(0);

    // Czyszenie tablicy
	delete[] pdColourSimilarities;

	return m_colourModels[minC].name;
}

// ------------------------------------------------------------------------------------------------------------
// initColourModelFromFile
// ------------------------------------------------------------------------------------------------------------
void colourRecognition::initColourModelFromFile() {


	std::ifstream inFile;
	inFile.open(COLOUR_NAMES_FILE, std::ifstream::in);  // otwarcie pliku z opisem kolorw
	if ( !inFile.is_open()) {
        std:: cout <<"Unable to open: "<<COLOUR_NAMES_FILE<<std::endl;
	}
	inFile >> mi_numberOfUsedColours;                   // wczytanie liczby kolorow

	std::string colourName;                             // bufor na ciag znakowy

    // wczytwanie poszczegolnych kolorow
	for (int i = 0; i < mi_numberOfUsedColours; i++) {

		// Wczytanie nazwy
		colourModel model;
		inFile >> colourName;
		model.name = colourName;

		// Wczytanie obrazka
		std::string filePath = "colourModel/";
		filePath += colourName;
		filePath += ".png";

		std::cout <<"Reading " << filePath <<std::endl;

		cv::Mat sample = cv::imread(filePath);

        std::cout << sample.rows <<std::endl;

		// obliczenie modelu na podstawie obrazka
		createGaussianModel(sample, model);

        // wsadzenie modelu na strukture
		m_colourModels.push_back(model);

		//debug
		//cv::imshow("sample", sample);
		//cv::waitKey(0);

	}

	inFile.close();
}

// ------------------------------------------------------------------------------------------------------------
// createGaussianModel
// ------------------------------------------------------------------------------------------------------------
void colourRecognition::createGaussianModel(cv::Mat I, colourModel &model) {

	cv::Mat mean;           // srednia
	cv::Mat covar;          // kowariancja
	cv::Mat ICS;            // obraz w docelowej przestrzeni barw


	cv::cvtColor(I, ICS, cv::COLOR_BGR2YCrCb);


	// troche pokraczna konwersja do wektora
	// +todo - trzeba sie temu przyjrzec
	cv::Mat IR = cv::Mat::zeros(ICS.rows*ICS.cols, 3, CV_8U);

	int k = 0;
	for (int j = 0; j < ICS.rows; j++) {
		for (int i = 0; i < ICS.cols; i++) {
			IR.at<uchar>(k, 0) = ICS.at<vec_uchar_3>(j, i)[0];
			IR.at<uchar>(k, 1) = ICS.at<vec_uchar_3>(j, i)[1];
			IR.at<uchar>(k, 2) = ICS.at<vec_uchar_3>(j, i)[2];
			k++;

		}
	}

	// obliczanie sredniej i kowariancji
	cv::calcCovarMatrix(IR, covar, mean, CV_COVAR_NORMAL | CV_COVAR_ROWS);

	cv::Mat covar1;
	cv::invert(covar, covar1);

	// Przypisania do modelu: stednia, kowariancja, wspolczynnik skalujacy
	model.covarianceMatrix1 = covar1;
	model.meanVector = mean;
	//TODO !!!
    model.scalingCoeff = 1 / (2 * CV_PI * sqrt(cv::determinant(covar)));
}
