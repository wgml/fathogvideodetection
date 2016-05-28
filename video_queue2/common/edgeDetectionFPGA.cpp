// Metody detekcji krawedzie na FPGA
// Tomasz Kryjak
// Laboratroium Biocybernetyki, AGH
// 21.01.2014r.

#include "edgeDetectionFPGA.h"

#include <stdio.h>


#define SOBEL_MAX 63

//#define CREATE_DEBUG_DATA

typedef cv::Vec<uchar, 3> VT_3;

// Filtering with Sobel 3x3 edge detector
void sobelEdge(const cv::Mat input,  cv::Mat & sobelX, cv::Mat & sobelY, cv::Mat &sobelM) {



}

// --------------------------------------------------------------------------------------------------------------
// Filtering with Sobel 3x3 edge detector - magnitude only
// --------------------------------------------------------------------------------------------------------------
void sobelEdgeThresholdedMagintude(const cv::Mat & input, cv::Mat &sobelMB, int threshold) {

	sobelMB = cv::Mat::zeros(input.rows, input.cols, CV_8U);


	// Loop over image
	for (int i=0; i < input.rows; i++)
		for (int j=0; j< input.cols; j++) {

			if ( i >0 && i < (input.rows-1) && j >0 && (j < input.cols-1) ) {	// Context OK

				// Sobel horizontal
				int SX = input.at<uchar>(i-1,j-1)*(-1) + input.at<uchar>(i-1,j)*(-2) + input.at<uchar>(i-1,j+1)*(-1) +
							   input.at<uchar>(i+1,j-1)*(1)  + input.at<uchar>(i+1,j)*(2)  + input.at<uchar>(i+1,j+1)*(1);



				// Sobel veritcal
				int SY = input.at<uchar>(i-1,j-1)*(-1) + input.at<uchar>(i,j-1)*(-2) + input.at<uchar>(i+1,j-1)*(-1) +
							   input.at<uchar>(i-1,j+1)*(1)  + input.at<uchar>(i,j+1)*(2)  + input.at<uchar>(i+1,j+1)*(1);

				int SXA = abs(SX);
				int SYA = abs(SY);


				int summm = SXA + SYA;
				if (summm > 255 )
					summm = 255;

				if ( summm > threshold)
					sobelMB.at<uchar>(i,j) = 255;
				else
					sobelMB.at<uchar>(i,j) = 0;

			}
			else
				sobelMB.at<uchar>(i,j) = 255;
		}
}

// --------------------------------------------------------------------------------------------------------------
// Filtering with Sobel 3x3 edge detector - magnitude for RGB image
// --------------------------------------------------------------------------------------------------------------
void sobelEdgeThresholdedMagintude_RGB(const cv::Mat input, cv::Mat &sobelMB, int threshold) {

	sobelMB = cv::Mat::zeros(input.rows, input.cols, CV_8U);


	// Loop over image
	for (int i=0; i < input.rows; i++)
		for (int j=0; j< input.cols; j++) {

			if ( i >0 && i < (input.rows-1) && j >0 && (j < input.cols-1) ) {	// Context OK


				int summm[3] = {0,0,0};

				for (int z=0; z < 3; z++) {

				// Sobel horizontal
					int SX = input.at<VT_3>(i-1,j-1)[z]*(-1) + input.at<VT_3>(i-1,j)[z]*(-2) + input.at<VT_3>(i-1,j+1)[z]*(-1) +
								   input.at<VT_3>(i+1,j-1)[z]*(1)  + input.at<VT_3>(i+1,j)[z]*(2)  + input.at<VT_3>(i+1,j+1)[z]*(1);



					// Sobel veritcal
					int SY = input.at<VT_3>(i-1,j-1)[z]*(-1) + input.at<VT_3>(i,j-1)[z]*(-2) + input.at<VT_3>(i+1,j-1)[z]*(-1) +
								   input.at<VT_3>(i-1,j+1)[z]*(1)  + input.at<VT_3>(i,j+1)[z]*(2)  + input.at<VT_3>(i+1,j+1)[z]*(1);



					int SXA = abs(SX);
					int SYA = abs(SY);


					summm[z] = SXA + SYA;
					if (summm[z] > 255 )
						summm[z] = 255;

					if ( summm[z] > threshold)
						summm[z] = 255;
					else
						summm[z] = 0;

				}

				if ( summm[0] > 0 || summm[1] > 0 || summm[2] > 0 )
					sobelMB.at<uchar>(i,j) = 255;
				else
					sobelMB.at<uchar>(i,j) = 0;

			}
		}

}
// --------------------------------------------------------------------------------------------------------------
// Filtering with Sobel 3x3 edge detector - magnitude for RGB image
// --------------------------------------------------------------------------------------------------------------
void sobelEdge_RGB(const cv::Mat input, cv::Mat &sobelR, cv::Mat &sobelG, cv::Mat &sobelB){

	sobelR = cv::Mat::zeros(input.rows, input.cols, CV_32F);
	sobelG = cv::Mat::zeros(input.rows, input.cols, CV_32F);
	sobelB = cv::Mat::zeros(input.rows, input.cols, CV_32F);

	// Loop over image
	for (int i=0; i < input.rows; i++)
		for (int j=0; j< input.cols; j++) {

			if ( i >0 && i < (input.rows-1) && j >0 && (j < input.cols-1) ) {	// Context OK

				float S[3] = {0,0,0};

				for (int z=0; z < 3; z++) {

					// Sobel horizontal
					int SX = input.at<VT_3>(i-1,j-1)[z]*(-1) + input.at<VT_3>(i-1,j)[z]*(-2) + input.at<VT_3>(i-1,j+1)[z]*(-1) +
								   input.at<VT_3>(i+1,j-1)[z]*(1)  + input.at<VT_3>(i+1,j)[z]*(2)  + input.at<VT_3>(i+1,j+1)[z]*(1);



					// Sobel veritcal
					int SY = input.at<VT_3>(i-1,j-1)[z]*(-1) + input.at<VT_3>(i,j-1)[z]*(-2) + input.at<VT_3>(i+1,j-1)[z]*(-1) +
								   input.at<VT_3>(i-1,j+1)[z]*(1)  + input.at<VT_3>(i,j+1)[z]*(2)  + input.at<VT_3>(i+1,j+1)[z]*(1);



					int SXA = abs(SX);
					int SYA = abs(SY);

					S[z] = SXA + SYA;


				}

				sobelR.at<float>(i,j) = S[0];
				sobelG.at<float>(i,j) = S[1];
				sobelB.at<float>(i,j) = S[2];


			}
		}


}

// --------------------------------------------------------------------------------------------------------------
// advEdgeDetection
// --------------------------------------------------------------------------------------------------------------
void advEdgeDetection(const cv::Mat ER, const cv::Mat EG, const cv::Mat EB, int stdThreshold, int threshold, cv::Mat &edgeMask) {

	// PARAM !!!
	const int contextSize2 = 1;

	edgeMask = cv::Mat::zeros(ER.rows, ER.cols, CV_8U);

	int pixelCounter=0;

	int img_h = ER.rows;
	int img_w = ER.cols;

	for (int i=0; i <img_h; i++)
		for (int j=0; j < img_w; j++) {

			if ( i >= contextSize2 && i < img_h - contextSize2 &&  j >= contextSize2 && j < img_w - contextSize2 ) {

				int mean_R = 0;
				int mean_G = 0;
				int mean_B = 0;

				for (int ii = i-contextSize2; ii <= i+contextSize2; ii++)
					for (int jj = j-contextSize2; jj <= j+contextSize2; jj++) {
						mean_R += int(ER.at<float>(ii,jj));
						mean_G += int(EG.at<float>(ii,jj));
						mean_B += int(EB.at<float>(ii,jj));
					}

				mean_R /= 9;
				mean_G /= 9;
				mean_B /= 9;


				int std_R = 0;
				int std_G = 0;
				int std_B = 0;

				for (int ii = i-contextSize2; ii <= i+contextSize2; ii++)
					for (int jj = j-contextSize2; jj <= j+contextSize2; jj++) {
						std_R += fabs(ER.at<float>(ii,jj)-mean_R);
						std_G += fabs(EG.at<float>(ii,jj)-mean_G);
						std_B += fabs(EB.at<float>(ii,jj)-mean_B);
					}

				if ( ( (std_R > stdThreshold) && (ER.at<float>(i,j) > threshold) && (ER.at<float>(i,j)  > 1.00*mean_R) ) ||
					 (std_G > stdThreshold && EG.at<float>(i,j) > threshold && EG.at<float>(i,j)  > 1.00*mean_G ) ||
					 (std_B > stdThreshold && EB.at<float>(i,j) > threshold && EB.at<float>(i,j)  > 1.00*mean_B )
				){

						edgeMask.at<uchar>(i,j) = 255;
				}
			}
				else
					edgeMask.at<uchar>(i,j) = 255;
		}

}


// --------------------------------------------------------------------------------------------------------------
// Filtering with Prewitt 3x3 edge detector
// --------------------------------------------------------------------------------------------------------------
void prewittEdge(const cv::Mat input,  cv::Mat & sobelX, cv::Mat & sobelY, cv::Mat &sobelM) {


	sobelX = cv::Mat::zeros(input.rows, input.cols, CV_32S);
	sobelY = cv::Mat::zeros(input.rows, input.cols, CV_32S);
	sobelM = cv::Mat::zeros(input.rows, input.cols, CV_8U);

#ifdef CREATE_DEBUG_DATA
	// Open files
	//FILE *inputFile = fopen("sobelInput.txt","w");
	FILE *resultFile = fopen("sobelOuput.txt","w");
#endif

	// Loop over image
	for (int i=0; i < input.rows; i++)
		for (int j=0; j< input.cols; j++) {

#ifdef CREATE_DEBUG_DATA
//	fprintf(inputFile,"%02X %02X %02X\n",input.at<uchar>(i,j),i,j);
#endif
			if ( i >0 && i < (input.rows-1) && j >0 && (j < input.cols-1) ) {	// Context OK

				// Sobel horizontal
				int SX = input.at<uchar>(i-1,j-1)*(-1) + input.at<uchar>(i-1,j)*(-1) + input.at<uchar>(i-1,j+1)*(-1) +
							   input.at<uchar>(i+1,j-1)*(1)  + input.at<uchar>(i+1,j)*(1)  + input.at<uchar>(i+1,j+1)*(1);



				// Sobel veritcal
				int SY = input.at<uchar>(i-1,j-1)*(-1) + input.at<uchar>(i,j-1)*(-1) + input.at<uchar>(i+1,j-1)*(-1) +
							   input.at<uchar>(i-1,j+1)*(1)  + input.at<uchar>(i,j+1)*(1)  + input.at<uchar>(i+1,j+1)*(1);

				sobelX.at<int>(i,j) = SX;
				sobelY.at<int>(i,j) = SY;

				int SXA = abs(SX);


				// TRUNCATION
				//if ( SXA > SOBEL_MAX )
				//	SXA=SOBEL_MAX;

				int SYA = abs(SY);
				//if ( SYA > SOBEL_MAX )
				//	SYA=SOBEL_MAX;

				int summm = SXA + SYA;
				if (summm > 255 )
					summm = 255;
				sobelM.at<uchar>(i,j) = summm;
#ifdef CREATE_DEBUG_DATA
				/*if ( SX >= 0 )
					fprintf(resultFile,"%03X ",SX);
				else
					fprintf(resultFile,"%03X ",256+SX+512+256);

				if ( SY >= 0 )
					fprintf(resultFile,"%03X ",SY);
				else
					fprintf(resultFile,"%03X " ,256+SY+512+256);

				int SXY = abs(SX) + abs(SY);

				if ( SXY > SOBEL_MAX )
					SXY = SOBEL_MAX;*/

				//fprintf(resultFile,"%02X\n",SXY);
				//fprintf(resultFile,"%d\n ",summm);
				if ( SX >= 0 )
					fprintf(resultFile,"%d ",SX);
				else
					fprintf(resultFile,"%d ",256+SX+512+256);



				//fprintf(resultFile,"%03X %03X %02X\n",SX,SY,SXA+SYA);
				//fprintf(resultFile,"%03X %03X %02X\n",SX,SY,SXA+SYA);

				//fprintf(resultFile,"%03X\n",input.at<uchar>(i,j));


#endif
			}
			else // No Context - writing 0
			{
#ifdef CREATE_DEBUG_DATA
				//fprintf(resultFile,"%03X %03X %02X\n",0,0,0);
				//fprintf(resultFile,"%03X %03X %02X\n",0,0,0);
				fprintf(resultFile,"%d\n",0);
#endif
			}

		}

#ifdef CREATE_DEBUG_DATA
	// Close files
	//fclose(inputFile);
	fclose(resultFile);
#endif



}


// Scale L from 0 - 255 to 0 - 100
void scale_L_Component(const cv::Mat input,  cv::Mat output) {
	for (int i=0; i < input.rows; i++)
		for (int j=0; j< input.cols; j++) {
			output.at<uchar>(i,j) 	=(uchar) (( (double) input.at<uchar>(i,j))/255.0*100.0);
		}

}
