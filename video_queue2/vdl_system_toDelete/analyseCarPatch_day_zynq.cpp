#include "analyseCarPatch_day_zynq.h"

#include <math.h>
#include <stdio.h>


int analyseCarPatch_day_zynq(unsigned char rgbImage[][MAX_VDL_WIDTH][4], int vdl_line_width, int car_patch_height){

	// Tablice
	static unsigned char rgbImageAdj[MAX_VDL_HEIGHT][MAX_VDL_WIDTH][3];           // obraz po rozciaganiu histogramu
	static unsigned char rgbImageFiltered[MAX_VDL_HEIGHT][MAX_VDL_WIDTH][3];      // obraz po filtracji Gaussa

	// Temporary
	static unsigned char edgeShadowImage[MAX_VDL_HEIGHT][MAX_VDL_WIDTH][3];



	int histR[256] = { 0 };   // wejsciowy histogram R
	int histG[256] = { 0 };   // wejsciowy histogram G
	int histB[256] = { 0 };   // wejsciowy histogram B

	int edgeShadowSumX[MAX_VDL_WIDTH] = { 0 };        // suma krawedzi i cienia poziomo
	int edgeShadowSumY[MAX_VDL_HEIGHT] = { 0 };       // suma krawedzi i cienia pionowo

	int histogramWEdgeShadow[64] = { 0 };               // histogram w obszarze krawedzi
	int histogramPozaEdgeShadow[64] = { 0 };            // histogram poza obszrem krawedzi



	// +++ Zmienne ogolnego zastosowania +++
	int i, j, k;
	int carCounter = 0;

	// +++ Zmienne do rozciagania histogramu +++
	// Wartosci maksymalne
	unsigned char maxR = 0;
	unsigned char maxG = 0;
	unsigned char maxB = 0;

	// Wartosci minialne
	unsigned char minR = 255;
	unsigned char minG = 255;
	unsigned char minB = 255;

	// Suma "od lewej"
	unsigned int sumRL = 0;
	unsigned int sumGL = 0;
	unsigned int sumBL = 0;

	// Suma "od prawej"
	unsigned int sumRR = 0;
	unsigned int sumGR = 0;
	unsigned int sumBR = 0;

	float scaleCoefR;         // wsp. skalujecy R
	float scaleCoefG;         // wsp. skalujecy G
	float scaleCoefB;         // wsp. skalujecy B

	int tempNewRGB = 0;       // pomocniczy bufro tymczasowy (ale potrzebny)

	// +++ Analiza krawedzi oraz obszarow zacienionych +++

	unsigned char shadowTh; // informacja o cieniu
	unsigned char sxTh;     // informacja o krawedziach poziomych

	int edgeXBegin = -1;
	int edgeYBegin = -1;
	int edgeXEnd = -1;
	int edgeYEnd = -1;

	char sumSobelSx;

	int edgeShadowArea = 0;
	unsigned int sumShadow = 0;
	unsigned int sumEdges = 0;

	int greyPixel = 0;                // pixel w odcieniach szarosci, obliczony zgodnie z OpenCV

	float histCoef = 0;               // wspolczynnik histogramowy

	int sredniaJasnosc = 0;

	bool patchZCieniem = false;       // flaga czy na danym patch'u jest cien zamiast autka


	// +++ Analiza duzych autek
	unsigned char segL[MAX_VDL_HEIGHT][MAX_VDL_WIDTH] = { 0 };
	unsigned char segL_D[MAX_VDL_HEIGHT][MAX_VDL_WIDTH] = { 0 };

	unsigned char segR[MAX_VDL_HEIGHT][MAX_VDL_WIDTH] = { 0 };
	unsigned char segR_D[MAX_VDL_HEIGHT][MAX_VDL_WIDTH] = { 0 };

	float s1 = 0;
	float s2 = 0;
	float s3 = 0;

	int sumR;
	int sumL;

	bool segRes = false;
	bool segRes_prev = false;
	int counter0 = 0;    // licznik '0'
	int carStart = 1;  // poczatek samochodu
	int sum = 0;

	float sumEdgesSX = 0;


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. Rozci¹ganie histogramu ...
	// 1.1 Obliczanie histogramu (dla kazdego kanalu osobno)
	//     Dodatkowo - analiza krawedzi

	for (j = 0; j < car_patch_height; j++) {
		for (i = 0; i < vdl_line_width; i++) {
			histR[rgbImage[j][i][0]]++;
			histG[rgbImage[j][i][1]]++;
			histB[rgbImage[j][i][2]]++;
		}
	}
	sredniaJasnosc = sredniaJasnosc / 3 / car_patch_height / vdl_line_width;

	// 1.2 Analiza histogramow
	// Od lewej
	for (i = 0; i < 256; i++){
		if (sumRL < 0.01*car_patch_height*vdl_line_width)
			sumRL += histR[i];
		else {
			minR = i;
			break;
		}
	}
	for (i = 0; i < 256; i++){
		if (sumGL < 0.01*car_patch_height*vdl_line_width)
			sumGL += histG[i];
		else {
			minG = i;
			break;
		}
	}
	for (i = 0; i < 256; i++){
		if (sumBL < 0.01*car_patch_height*vdl_line_width)
			sumBL += histB[i];
		else {
			minB = i;
			break;
		}
	}
	// Od prawej
	for (i = 255; i >= 0; i--){
		if (sumRR < 0.01*car_patch_height*vdl_line_width)
			sumRR += histR[i];
		else {
			maxR = i;
			break;
		}
	}
	for (i = 255; i >= 0; i--){
		if (sumGR < 0.01*car_patch_height*vdl_line_width)
			sumGR += histG[i];
		else {
			maxG = i;
			break;
		}
	}
	for (i = 255; i >= 0; i--){

		if (sumBR < 0.01*car_patch_height*vdl_line_width)
			sumBR += histB[i];
		else {
			maxB = i;
			break;
		}
	}
	// debug
	//printf("%d %d %d %d %d %d \n", minR, maxR, minG, maxG, minB, maxB);

	// 1.3 Skalowanie
	scaleCoefR = (float)(maxR - minR) / 255.0;
	scaleCoefG = (float)(maxG - minG) / 255.0;
	scaleCoefB = (float)(maxB - minB) / 255.0;

	for (j = 0; j < car_patch_height; j++) {
		for (i = 0; i < vdl_line_width; i++) {

			tempNewRGB = (int)((float)(rgbImage[j][i][0] - minR) / scaleCoefR);
			if (tempNewRGB > 255)	tempNewRGB = 255;
			if (tempNewRGB < 0)	tempNewRGB = 0;
			rgbImageAdj[j][i][0] = (unsigned char)tempNewRGB;

			tempNewRGB = (int)((float)(rgbImage[j][i][1] - minG) / scaleCoefG);
			if (tempNewRGB > 255)	tempNewRGB = 255;
			if (tempNewRGB < 0)	tempNewRGB = 0;
			rgbImageAdj[j][i][1] = (unsigned char)tempNewRGB;

			tempNewRGB = (int)((float)(rgbImage[j][i][2] - minB) / scaleCoefB);
			if (tempNewRGB > 255)	tempNewRGB = 255;
			if (tempNewRGB < 0)	tempNewRGB = 0;
			rgbImageAdj[j][i][2] = (unsigned char)tempNewRGB;

		}
	}


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. Filtracja,  Gaussa 5x5 ... (wspolczynniki z Matlab)
	// Uwaga - to mo¿e byæ opcjonalne, jakby by³ problem z wydajnoœci¹
	for (j = 0; j < car_patch_height; j++) {
		for (i = 0; i < vdl_line_width; i++) {
			for (k = 0; k < 3; k++) {
				if (j > 1 && j < car_patch_height - 2 && i > 1 && i < vdl_line_width - 2) {
					rgbImageFiltered[j][i][k] = (float)(rgbImageAdj[j - 2][i][k])*0.0002 +
						(float)(rgbImageAdj[j - 1][i - 1][k])*0.0113 + (float)(rgbImageAdj[j - 1][i][k])*0.0837 + (float)(rgbImageAdj[j - 1][i + 1][k])*0.0113 +
						(float)(rgbImageAdj[j][i - 2][k])*0.0002 + (float)(rgbImageAdj[j][i - 1][k])*0.0837 + (float)(rgbImageAdj[j][i][k])*0.6187 + (float)(rgbImageAdj[j][i + 1][k])*0.0837 + (float)(rgbImageAdj[j][i + 2][k])*0.0002 +
						(float)(rgbImageAdj[j + 1][i - 1][k])*0.0113 + (float)(rgbImageAdj[j + 1][i][k])*0.0837 + (float)(rgbImageAdj[j + 1][i + 1][k])*0.0113 +
						(float)(rgbImageAdj[j + 2][i][k])*0.0002;
				}
				else
					rgbImageFiltered[j][i][k] = rgbImageAdj[j][i][k];
			}
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2a. Filtracja,  Sobel 3x3 ..
	// Dodatkowo -> konwersja do skali szarosci
	for (j = 0; j < car_patch_height; j++) {
		for (i = 0; i < vdl_line_width; i++) {

			// Sobel poziomy
			sumSobelSx = 0;
			for (k = 0; k < 3; k++) {
				if (j > 0 && j < car_patch_height - 1 && i > 0 && i < vdl_line_width - 1) {
					if (fabs((float)(rgbImageFiltered[j - 1][i - 1][k]) * 1 + (float)(rgbImageFiltered[j - 1][i][k]) * 2 + (float)(rgbImageFiltered[j - 1][i + 1][k]) * 1
						- (float)(rgbImageFiltered[j + 1][i - 1][k]) * 1 - (float)(rgbImageFiltered[j + 1][i][k]) * 2 - (float)(rgbImageFiltered[j - +1][i + 1][k]) * 1) > EDGE_PATCH_TH)
						sumSobelSx++;
				}
			}
			if (sumSobelSx > 0)
				sxTh = 1;
			else
				sxTh = 0;

			sumEdgesSX += sxTh;

			// Konwersja do skali szarosci
			greyPixel = 0.299*(float)(rgbImageFiltered[j][i][0]) + 0.587*(float)(rgbImageFiltered[j][i][1]) + 0.114*(float)(rgbImageFiltered[j][i][2]);

			// Obliczanie sredniej jasnosci
			sredniaJasnosc += greyPixel;

			// Okreœlenie, czy cieñ
			shadowTh = greyPixel < SHADOW_TH;     // flaga zwiazana z cieniem

			// [!!!]
			// debug
			edgeShadowImage[j][i][2] = (char)(shadowTh)* 255;
			edgeShadowImage[j][i][1] = sxTh * 255;
			edgeShadowImage[j][i][0] = 0;

			// Obliczenia do okreslania granic obszaru
			if (/*shadowTh > 0 ||*/ sxTh > 0) {
				edgeShadowSumX[i]++;
				edgeShadowSumY[j]++;
			}

			// Tu jest myk z oszczednoscia pamieci...
			// Nowa wartosc w rgbImage[][][3]
			rgbImage[j][i][3] = greyPixel;
		}
	}

	sumEdgesSX = sumEdgesSX / car_patch_height/vdl_line_width;


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. Analiza krawedzi i obszarow zacienionych
	// 3.1 Wyszukiwanie granic obszaru
	// Uwaga -- tutaj celowo pomijany jest jeden piksel brzegowy
	for (i = 1; i < MAX_VDL_HEIGHT; i++) {
		if (edgeShadowSumY[i] > EDGE_SHADOW_NUM_TH) {
			edgeYBegin = i;
			break;
		}
	}

	for (i = MAX_VDL_HEIGHT - 2; i >= 0; i--) {
		if (edgeShadowSumY[i] > EDGE_SHADOW_NUM_TH) {
			edgeYEnd = i;
			break;
		}
	}

	for (i = 1; i < MAX_VDL_WIDTH; i++) {
		if (edgeShadowSumX[i] > EDGE_SHADOW_NUM_TH) {
			edgeXBegin = i;
			break;
		}
	}

	for (i = MAX_VDL_WIDTH - 2; i >= 0; i--) {
		if (edgeShadowSumX[i] > EDGE_SHADOW_NUM_TH) {
			edgeXEnd = i;
			break;
		}
	}

	// debug
	//printf("Obszar autka: %d %d %d %d", edgeXBegin, edgeXEnd, edgeYBegin, edgeYEnd);

	if (edgeXBegin != -1 && edgeXEnd != -1 && edgeYBegin != -1 && edgeYEnd != -1 && edgeYBegin != edgeYEnd) {

		// 3.2 Obliczanie pola obszaru z krawedziami i cieniem
		edgeShadowArea = (edgeXEnd - edgeXBegin)*(edgeYEnd - edgeYBegin);

		// 3.3 Analiza wewnatrz obszraru z krawedziami
		// Sumowanie pikseli uznanych za cien i krawedzie
		for (j = edgeYBegin; j < edgeYEnd; j++) {
			for (i = edgeXBegin; i < edgeXEnd; i++) {
				sumShadow += (edgeShadowImage[j][i][2] > 0);     // CIEN
				sumEdges += (edgeShadowImage[j][i][1] > 0);     // SX TH
			}
		}

		// 3.4 Histogram (zagregowany do 64) dla obu obszarow
		for (j = 0; j < car_patch_height; j++) {
			for (i = 0; i < vdl_line_width; i++) {

				greyPixel = rgbImage[j][i][3];

				if (j < edgeYBegin || j > edgeYEnd) { // Poza
					histogramPozaEdgeShadow[greyPixel / 4]++;
				}
				else { // W
					histogramWEdgeShadow[greyPixel / 4]++;
				}
			}
		}

		// 3.5 Analiza histogramow + obliczanie wspolczynnika histogramowego
		for (i = 0; i < 64; i++){

			if (histogramPozaEdgeShadow[i] > 5 || i < SHADOW_TH){ // zerujemy histogram - te obszary, ktore sa "poza" + "cienie"
				histogramWEdgeShadow[i] = 0;
			}
			histCoef += histogramWEdgeShadow[i];
		}
		//if (car_patch_height - (edgeYEnd - edgeYBegin) > 10)
			histCoef = histCoef / (float)(vdl_line_width) / (float)(edgeYEnd - edgeYBegin);
		//else
		//	histCoef = 0;

		// 3.6 Podsumowanie analizy histogramow, krawedzi i cieni
		// Opis warunkow:
		// - krawedzie + cienie wypelniaja wiekszosc obszaru (zwykle tak nie jest),
		// - stosunek krawedzi i cienie jest < 2
		// - histogramy jest rozny

		if (sredniaJasnosc > 100) // od tego zalezy prog na hist coef
			patchZCieniem = (float)(sumEdges + sumShadow) / (float)(edgeShadowArea) > 0.6 && (float)sumEdges / (float)sumShadow < 2 && histCoef < 0.25;
		else
			patchZCieniem = (float)(sumEdges + sumShadow) / (float)(edgeShadowArea) > 0.6 && (float)sumEdges / (float)sumShadow < 2 && histCoef < 0.1;

	}

	//debug
	printf("C: %f %f %f edges: %f\n", (float)(sumEdges + sumShadow) / (float)edgeShadowArea, (float)sumEdges / (float)sumShadow, histCoef, sumEdgesSX);


	//printf("HIST COEF %f, SREDNIA JASNOSC %d\n", histCoef, sredniaJasnosc);

	// 4. Decyzja co do dalszej analizy
	if (sumEdgesSX < NUM_EDGE_SX_TH) {
		printf("Brak autka\n");
	}
	else {

		if (car_patch_height < V_CAR_SIZE_TH) {
			if (patchZCieniem) {
				printf("Cien\n");
			}
			else {
				if (sumEdgesSX > NUM_EDGE_SX_TH) {
					printf("Autko\n");
					carCounter++;
				}
				else
				{

				}
			}
		}
		else { // Duze autko

			// 5. Szukanie wiekszej liczby pojazdow
			// Poprzez segmentacje przez rozrost z lewej i prawej strony

			// 5.1. Segmentacja z lewej i prawej
			for (j = 0; j < car_patch_height; j++) {

				// Od lewej
				segL[j][0] = 1; // brzeg ustawiony na 1
				segL[j][1] = 1; // brzeg ustawiony na 1

				// poczatkowe wartosci s -> pixele RGB
				s1 = (float)rgbImageFiltered[j][1][0];
				s2 = (float)rgbImageFiltered[j][1][1];
				s3 = (float)rgbImageFiltered[j][1][2];

				for (i = 2; i < vdl_line_width; i++) {
					s1 = 0.75*s1 + 0.25*(float)rgbImageFiltered[j][i][0];
					s2 = 0.75*s2 + 0.25*(float)rgbImageFiltered[j][i][1];
					s3 = 0.75*s3 + 0.25*(float)rgbImageFiltered[j][i][2];
					if (fabs(s1 - (float)rgbImageFiltered[j][i][0]) + fabs(s2 - (float)rgbImageFiltered[j][i][1]) + fabs(s3 - (float)rgbImageFiltered[j][i][2]) < SIM_TH)
						segL[j][i] = 1;
					else
						break;
				}
				// Od prawej
				segR[j][vdl_line_width - 1] = 1;
				segR[j][vdl_line_width - 2] = 1;

				s1 = (float)rgbImageFiltered[j][vdl_line_width - 2][0];
				s2 = (float)rgbImageFiltered[j][vdl_line_width - 2][1];
				s3 = (float)rgbImageFiltered[j][vdl_line_width - 2][2];


				for (i = vdl_line_width - 3; i >= 0; i--) {
					s1 = 0.75*s1 + 0.25*(float)rgbImageFiltered[j][i][0];
					s2 = 0.75*s2 + 0.25*(float)rgbImageFiltered[j][i][1];
					s3 = 0.75*s3 + 0.25*(float)rgbImageFiltered[j][i][2];
					if (fabs(s1 - (float)rgbImageFiltered[j][i][0]) + fabs(s2 - (float)rgbImageFiltered[j][i][1]) + fabs(s3 - (float)rgbImageFiltered[j][i][2]) < SIM_TH)
						segR[j][i] = 1;
					else
						break;
				}
			}
			// 4.2 Dylatajca obu obrazow

			for (j = 1; j < car_patch_height - 1; j++) {
				for (i = 1; i < vdl_line_width - 1; i++) {
					sumR = segR[j - 1][i - 1] + segR[j - 1][i] + segR[j - 1][i + 1] +
						segR[j][i - 1] + segR[j][i] + segR[j][i + 1] +
						segR[j + 1][i - 1] + segR[j + 1][i] + segR[j + 1][i + 1];

					sumL = segL[j - 1][i - 1] + segL[j - 1][i] + segL[j - 1][i + 1] +
						segL[j][i - 1] + segL[j][i] + segL[j][i + 1] +
						segL[j + 1][i - 1] + segL[j + 1][i] + segL[j + 1][i + 1];

					if (sumL > 0)
						segL_D[j][i] = 1;

					if (sumR > 0)
						segR_D[j][i] = 1;
				}
			}


			// 4.3. £aczenie obrazów -> wynik bedzie w L
			for (j = 0; j < car_patch_height; j++) {
				for (i = 0; i < vdl_line_width; i++) {
					segL[j][i] = segL_D[j][i] & segR_D[j][i];
				}
			}



			//// Dylatacja wyniku ...
			//   for (int j=1; j < height-1; j++) {
			//	for (int i=1; i < m_iLineWidth-1; i++) {
			//		int sumL = segL[j-1][i-1] + segL[j-1][i] + segL[j-1][i+1] +
			//			       segL[j][i-1] + segL[j][i] + segL[j][i+1] +
			//				   segL[j+1][i-1] + segL[j+1][i] + segL[j+1][i+1];
			//		if ( sumL > 0 )
			//			segL_D[j][i] = 1;
			//		else
			//			segL_D[j][i] = 0;
			//
			//		//printf("%d",segL_D[j][i]);
			//	}
			//	//printf("\n");
			//}
			////printf("=====================\n");

			// 4.4. Analiza otrzymanego wyniku



			for (j = 1; j < car_patch_height - 1; j++) { // pomijanie "ramki"
				// a. sumowanie w wierszu
				sum = 0;
				for (i = 1; i < vdl_line_width - 1; i++) {
					sum += (segL_D[j][i]);
				}
				if (sum > 0.85 * vdl_line_width /* && sumE < 0.1*m_iLineWidth*/)
					segRes = true;
				else
					segRes = false;

				//	//printf("%d",segRes);

				if (!segRes)
					counter0 += 1;
				//
				if (!segRes_prev && segRes || j == car_patch_height - 2) {
					// Car image !!!
					//if ( counter0 > 0 )
					//printf("CAR START:%d END:%d\n",carStart,carStart+counter0-1);

					if (counter0 > CAR_LENGTH_TH) { // dodac value
						counter0 = 0;
						carCounter++;
						printf("DAutko\n");
					}
				}

				if (segRes_prev && !segRes)
					carStart = j;

				segRes_prev = segRes;

			}

		} // duze autko
	}// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// X.1 Wyswietlanie debug
	// [!!!] Usunac Zynq
	cv::Mat image_bin(car_patch_height, vdl_line_width, CV_8UC3);
	cv::Mat image_rgb(car_patch_height, vdl_line_width, CV_8UC3);
	cv::Mat image(car_patch_height, vdl_line_width, CV_8UC3);

	cv::Vec<uchar, 3> v;
	for (j = 0; j < car_patch_height; j++) {
		for (i = 0; i < vdl_line_width; i++) {
			//v[0] = rgbImage[j][i][0];
			//v[1] = rgbImage[j][i][1];
			//v[2] = rgbImage[j][i][2];
			//v[0] = rgbImageAdj[j][i][0];
			//v[1] = rgbImageAdj[j][i][1];
			//v[2] = rgbImageAdj[j][i][2];

			v[0] = rgbImageFiltered[j][i][0];
			v[1] = rgbImageFiltered[j][i][1];
			v[2] = rgbImageFiltered[j][i][2];
			image_rgb.at<VT_3>(j, i) = v;

			v[0] = segL[j][i] * 255;
			v[1] = segL[j][i] * 255;
			v[2] = segL[j][i] * 255;



			image_bin.at<VT_3>(j, i) = v;


			v[0] = edgeShadowImage[j][i][0];
			v[1] = edgeShadowImage[j][i][1];
			v[2] = edgeShadowImage[j][i][2];
			image.at<VT_3>(j, i) = v;
			//v[0] = sxImage[j][i][0];
			//v[1] = sxImage[j][i][1];
			//v[2] = sxImage[j][i][2];



		}
	}

	// + linia
	cv::line(image, cv::Point(edgeXBegin, edgeYBegin), cv::Point(edgeXEnd, edgeYBegin), cv::Scalar(0, 0, 255));
	cv::line(image, cv::Point(edgeXBegin, edgeYEnd), cv::Point(edgeXEnd, edgeYEnd), cv::Scalar(0, 0, 255));
	cv::line(image, cv::Point(edgeXBegin, edgeYBegin), cv::Point(edgeXBegin, edgeYEnd), cv::Scalar(0, 0, 255));
	cv::line(image, cv::Point(edgeXEnd, edgeYBegin), cv::Point(edgeXEnd, edgeYEnd), cv::Scalar(0, 0, 255));


	cv::imshow("PATCH", image_rgb);
	cv::imshow("DEBUG", image);
	cv::imshow("BIN", image_bin);
	//cv::imwrite("debug.bmp", image_rgb);
	// cv::waitKey(0);








	return carCounter;
	//return 0;
}
