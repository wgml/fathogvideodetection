#include "frameImageProcessing.h"

// ------------------------------------------------------------------------------------------------------------
//! Konstruktor
// ------------------------------------------------------------------------------------------------------------
frameImageProcessing::frameImageProcessing(int YY, int XX) {

	m_iXX = XX;
	m_iYY = YY;

	// ustawienie parametrow (na razie z globalnej bazy)
	m_iframeDiffTh = FRAMEOP_FRAME_DIFF_TH;

	// stworzenie obrazk�w
	image_rgb_prev = cv::Mat::zeros(YY, XX, CV_8UC3);
	movementMask   = cv::Mat::zeros(YY, XX, CV_8U);
	edgeMask       = cv::Mat::zeros(YY, XX, CV_8U);
	lbpImage       = cv::Mat::zeros(YY, XX, CV_8U);


	// Definicja stalych do LBP
	m_iNRULBP_LUT[0] = 1; m_iNRULBP_LUT[1] = 2; m_iNRULBP_LUT[2] = 3; m_iNRULBP_LUT[3] = 4; m_iNRULBP_LUT[4] = 5; m_iNRULBP_LUT[5] = 0; m_iNRULBP_LUT[6] = 6; m_iNRULBP_LUT[7] = 7; m_iNRULBP_LUT[8] = 8; m_iNRULBP_LUT[9] = 0; m_iNRULBP_LUT[10] = 0; m_iNRULBP_LUT[11] = 0; m_iNRULBP_LUT[12] = 9; m_iNRULBP_LUT[13] = 0; m_iNRULBP_LUT[14] = 10; m_iNRULBP_LUT[15] = 11; m_iNRULBP_LUT[16] = 12; m_iNRULBP_LUT[17] = 0; m_iNRULBP_LUT[18] = 0; m_iNRULBP_LUT[19] = 0; m_iNRULBP_LUT[20] = 0; m_iNRULBP_LUT[21] = 0; m_iNRULBP_LUT[22] = 0; m_iNRULBP_LUT[23] = 0; m_iNRULBP_LUT[24] = 13; m_iNRULBP_LUT[25] = 0; m_iNRULBP_LUT[26] = 0; m_iNRULBP_LUT[27] = 0; m_iNRULBP_LUT[28] = 14; m_iNRULBP_LUT[29] = 0; m_iNRULBP_LUT[30] = 15; m_iNRULBP_LUT[31] = 16; m_iNRULBP_LUT[32] = 17; m_iNRULBP_LUT[33] = 0; m_iNRULBP_LUT[34] = 0; m_iNRULBP_LUT[35] = 0; m_iNRULBP_LUT[36] = 0; m_iNRULBP_LUT[37] = 0; m_iNRULBP_LUT[38] = 0; m_iNRULBP_LUT[39] = 0; m_iNRULBP_LUT[40] = 0; m_iNRULBP_LUT[41] = 0; m_iNRULBP_LUT[42] = 0; m_iNRULBP_LUT[43] = 0; m_iNRULBP_LUT[44] = 0; m_iNRULBP_LUT[45] = 0; m_iNRULBP_LUT[46] = 0; m_iNRULBP_LUT[47] = 0; m_iNRULBP_LUT[48] = 18; m_iNRULBP_LUT[49] = 0; m_iNRULBP_LUT[50] = 0; m_iNRULBP_LUT[51] = 0; m_iNRULBP_LUT[52] = 0; m_iNRULBP_LUT[53] = 0; m_iNRULBP_LUT[54] = 0; m_iNRULBP_LUT[55] = 0; m_iNRULBP_LUT[56] = 19; m_iNRULBP_LUT[57] = 0; m_iNRULBP_LUT[58] = 0; m_iNRULBP_LUT[59] = 0; m_iNRULBP_LUT[60] = 20; m_iNRULBP_LUT[61] = 0; m_iNRULBP_LUT[62] = 21; m_iNRULBP_LUT[63] = 22; m_iNRULBP_LUT[64] = 23;
	m_iNRULBP_LUT[65] = 0; m_iNRULBP_LUT[66] = 0; m_iNRULBP_LUT[67] = 0; m_iNRULBP_LUT[68] = 0; m_iNRULBP_LUT[69] = 0; m_iNRULBP_LUT[70] = 0; m_iNRULBP_LUT[71] = 0; m_iNRULBP_LUT[72] = 0; m_iNRULBP_LUT[73] = 0; m_iNRULBP_LUT[74] = 0; m_iNRULBP_LUT[75] = 0; m_iNRULBP_LUT[76] = 0; m_iNRULBP_LUT[77] = 0; m_iNRULBP_LUT[78] = 0; m_iNRULBP_LUT[79] = 0; m_iNRULBP_LUT[80] = 0; m_iNRULBP_LUT[81] = 0; m_iNRULBP_LUT[82] = 0; m_iNRULBP_LUT[83] = 0; m_iNRULBP_LUT[84] = 0; m_iNRULBP_LUT[85] = 0; m_iNRULBP_LUT[86] = 0; m_iNRULBP_LUT[87] = 0; m_iNRULBP_LUT[88] = 0; m_iNRULBP_LUT[89] = 0; m_iNRULBP_LUT[90] = 0; m_iNRULBP_LUT[91] = 0; m_iNRULBP_LUT[92] = 0; m_iNRULBP_LUT[93] = 0; m_iNRULBP_LUT[94] = 0; m_iNRULBP_LUT[95] = 0; m_iNRULBP_LUT[96] = 24; m_iNRULBP_LUT[97] = 0; m_iNRULBP_LUT[98] = 0; m_iNRULBP_LUT[99] = 0; m_iNRULBP_LUT[100] = 0; m_iNRULBP_LUT[101] = 0; m_iNRULBP_LUT[102] = 0; m_iNRULBP_LUT[103] = 0; m_iNRULBP_LUT[104] = 0; m_iNRULBP_LUT[105] = 0; m_iNRULBP_LUT[106] = 0; m_iNRULBP_LUT[107] = 0; m_iNRULBP_LUT[108] = 0; m_iNRULBP_LUT[109] = 0; m_iNRULBP_LUT[110] = 0; m_iNRULBP_LUT[111] = 0; m_iNRULBP_LUT[112] = 25; m_iNRULBP_LUT[113] = 0; m_iNRULBP_LUT[114] = 0; m_iNRULBP_LUT[115] = 0; m_iNRULBP_LUT[116] = 0; m_iNRULBP_LUT[117] = 0; m_iNRULBP_LUT[118] = 0; m_iNRULBP_LUT[119] = 0; m_iNRULBP_LUT[120] = 26; m_iNRULBP_LUT[121] = 0; m_iNRULBP_LUT[122] = 0; m_iNRULBP_LUT[123] = 0; m_iNRULBP_LUT[124] = 27; m_iNRULBP_LUT[125] = 0; m_iNRULBP_LUT[126] = 28; m_iNRULBP_LUT[127] = 29;



}

// ------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------
frameImageProcessing::~frameImageProcessing() {}

// ------------------------------------------------------------------------------------------------------------
//! Krok algorytmu
// ------------------------------------------------------------------------------------------------------------
void frameImageProcessing::step(const cv::Mat & image_rgb, const cv::Mat & image_gray) {

    if (image_rgb_prev.rows != 0) {

		// roznica sasiednich ramek / prog na stale
		rgbImageDifferenceTh(image_rgb, image_rgb_prev, movementMask, 50);

		// detekcja krawedzi / prog na stale
		edgeImageTh(image_gray,edgeMask,50);

		// obliczanie cech LBP
		lbp(image_gray,SLBP);

		// filtreacja gaussa
		gaussianFiltering(image_rgb)
;
    }

	// Obliczanie roznicy sasiednich ramek
	//cv::Mat diff;
	//cv::absdiff(image_gray, prevFrame_gray, diff);

	// Binaryzacja
	//cv::Mat diff_mask;
	//cv::threshold(diff, diff_mask, m_iframeDiffTh, 255, cv::THRESH_BINARY);

	// debug
	//cv::imshow("DIFF_MASK", diff_mask);
	//cv::imshow("LBP", lbpImage);
	//cv::waitKey(0);


	// przypisanie koncowe
	//image_gray.copyTo(prevFrame_gray);
	image_rgb.copyTo(image_rgb_prev);
}

// ------------------------------------------------------------------------------------------------------------
//! rgbImageDifferenceTh
// ------------------------------------------------------------------------------------------------------------
void frameImageProcessing::rgbImageDifferenceTh(const cv::Mat & i1,const  cv::Mat & i2, cv::Mat & mask, float threshold) {

	cv::Vec<uchar, 3> p1;
	cv::Vec<uchar, 3> p2;


	for (int jj = 0; jj < i1.rows;jj++) {
		for (int ii = 0; ii < i1.cols; ii++) {

			// pobranie pikseli
			p1 = i1.at<vec_uchar_3>(jj, ii);
			p2 = i2.at<vec_uchar_3>(jj, ii);

			// obliczenie roznych (modul)
			float diff = fabs(float(p1[0]) - float(p2[0])) + fabs(float(p1[1]) - float(p2[1])) + fabs(float(p1[2]) - float(p2[2]));

			// progowanie
			if (diff > threshold) {
				mask.at<uchar>(jj, ii) = 255;
			}
			else {
				mask.at<uchar>(jj, ii) = 0;
			}

		}
	}
}

// ------------------------------------------------------------------------------------------------------------
//! edgeImageTh
// metoda Sobel (taka jest w sprzecie)
// ------------------------------------------------------------------------------------------------------------
void frameImageProcessing::edgeImageTh(const cv::Mat & i, cv::Mat & edgeMask, float threshold){

    sobelEdgeThresholdedMagintude(i, edgeMask, threshold);



	//cv::Canny(i, edgeMask, 50, 100); // dobrac progi
	//cv::threshold(edges, edgeMask, threshold, 255, cv::THRESH_BINARY);

	//debug
	//imshow("EDGE MASK", edgeMask);
	//cv::waitKey(0);
}

// ------------------------------------------------------------------------------------------------------------
//! lbpImage
// TODO ew. inne warianty progowania ?
// ------------------------------------------------------------------------------------------------------------
void frameImageProcessing::lbp(const cv::Mat & iG, int type){

    // Konkteks
    uchar c11,c12,c13;
    uchar c21,c23;
    uchar c31,c32,c33;

    uchar th;  // prog binaryzjacji
    uchar lbp;


    // Petla po obrazie
    for (int jj = 1; jj < iG.rows-1;jj++) {
		for (int ii = 1; ii < iG.cols-1; ii++) {

            // Obliczanie cechy LBP
            th = iG.at<vec_uchar_1>(jj, ii)[0]+LBP_BIAS;

            c11 = iG.at<vec_uchar_1>(jj-1, ii-1)[0] > th ? 1:0;
            c12 = iG.at<vec_uchar_1>(jj-1, ii)[0] > th ? 1:0;
            c13 = iG.at<vec_uchar_1>(jj-1, ii+1)[0] > th ? 1:0;

            c21 = iG.at<vec_uchar_1>(jj, ii-1)[0] > th ? 1:0;
            c23 = iG.at<vec_uchar_1>(jj, ii+1)[0] > th ? 1:0;

            c31 = iG.at<vec_uchar_1>(jj+1, ii-1)[0] > th ? 1:0;
            c32 = iG.at<vec_uchar_1>(jj+1, ii)[0] > th ? 1:0;
            c33 = iG.at<vec_uchar_1>(jj+1, ii-1)[0] > th ? 1:0;


            lbp = c11*128 + c12*64 + c13*32 + c21*16 + c23*8 + c31*4 + c32*2 + c33*1;

            // Obliczenia NR
            if ( 255-lbp < lbp)
                lbp = 255-lbp;

            // Obliczenia uniform (na podstawie tablicy przekodowan)
            lbp = m_iNRULBP_LUT[lbp];

            lbpImage.at<vec_uchar_1>(jj, ii) = lbp;

		}
    }
}

// ------------------------------------------------------------------------------------------------------------
//! Filtracja Gaussa
// TODO - tu sa paraetry na sztywno
// ------------------------------------------------------------------------------------------------------------
void frameImageProcessing::gaussianFiltering(const cv::Mat & i){
	cv::GaussianBlur(i, m_gaussianImage, cv::Size(3, 3), 0.5, 0.5, cv::BORDER_REPLICATE);
}

// ------------------------------------------------------------------------------------------------------------
//! Przekazanie wyniku odejmowania ramek
// ------------------------------------------------------------------------------------------------------------
void frameImageProcessing::getMovementMask(cv::Mat &movementMask) {
	movementMask = this->movementMask;
}

// ------------------------------------------------------------------------------------------------------------
//! Przekazanie wyniku detekcji krawedzi ramek
// ------------------------------------------------------------------------------------------------------------
void frameImageProcessing::getEdgeMask(cv::Mat &edgeMask) {
	edgeMask = this->edgeMask;
}

// ------------------------------------------------------------------------------------------------------------
//! Przekazanie wyniku obliczania LBP
// ------------------------------------------------------------------------------------------------------------
void frameImageProcessing::getLBP(cv::Mat &lbp) {
    lbp = this->lbpImage;
}

// ------------------------------------------------------------------------------------------------------------
//! Przekazanie wyniku obliczania LBP
// ------------------------------------------------------------------------------------------------------------
void frameImageProcessing::getGaussianImage(cv::Mat &gaussianImage) {
    gaussianImage = this->m_gaussianImage;
}

