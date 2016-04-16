#include "globals.h"                   // ustawienia globalne

#include "common_vdetect/sequences.h"  // plik z opisem sekwencji
#include "frameImageProcessing.h"      // przetwarzanie dla pojedynczej ramki
#include "queue_system.h"              // system kolejek

#include "vdl_system/vdl_system.h"     // system lini VDL

#include <time.h>
#include <iostream>

int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        return -1;
    }
    sequences S("/home/vka/Programming/C/workspace/fathogvideodetection/video_queue/seq_720.txt");
    sequence seq = S.m_seq[0];

    // Obrazki
    cv::Mat inputImage_rgb;   // obraz wejsciowy
    cv::Mat inputImage_grey;  // obraz wejsciowy w odcieniach szarosci
    cv::Mat image_vis;        // obraz do wizualizacji

    cv::Mat movementMask;     // maska ruchu
    cv::Mat edgeMask;         // maska krawedziowa
    cv::Mat lbp;              // obraz LBP
    cv::Mat gaussianImage;    // obraz po filtracji Gaussa

    cap >> inputImage_rgb;

    frameImageProcessing *p_FIP=NULL;   // operacje globalne dla ramki
    queueSystem *p_QS=NULL;             // system kolejek

   
    p_FIP = new frameImageProcessing(seq.iYY, seq.iXX);                                                                                                        // operacje na poziomie ramki obrazu
    p_QS = new queueSystem(seq.iLanes, seq.iqueueROIs, NULL, seq.sName, seq.queueROIs, seq.analysisROIs);          // system kolejek + analiza


    while(1) {
        cap >> inputImage_rgb;

#ifdef DRAW
        inputImage_rgb.copyTo(image_vis);
#endif

        cv::cvtColor(inputImage_rgb, inputImage_grey, CV_RGB2GRAY);
        p_FIP->step(inputImage_rgb, inputImage_grey);
        p_FIP->getMovementMask(movementMask);               // wyznaczenie maski ruchu
        p_FIP->getEdgeMask(edgeMask);                       // wyznaczenie maski krawedzi
        p_FIP->getLBP(lbp);                                 // wyznaczenie deskryptora LBP
        p_FIP->getGaussianImage(gaussianImage);             // wyznaczenie rezultatu filtracji Gaussa
	
        p_QS->step(inputImage_grey, inputImage_rgb, lbp, movementMask, edgeMask, image_vis);

#ifdef DRAW
        cv::imshow("vis", image_vis);
//        cv::imshow("grb",  inputImage_rgb);
//        cv::imshow("gray",  inputImage_grey);
//        cv::imshow("movementMask",  movementMask);
//        cv::imshow("edgeMask",  edgeMask);
//        cv::imshow("lbp",  lbp);
//        cv::imshow("gaussianImage",  gaussianImage);
        if (cv::waitKey(30) >= 0) {
            break;
        }
        
#endif
    }
    return 0;
}
