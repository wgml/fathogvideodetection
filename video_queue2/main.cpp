#include "globals.h"                   // ustawienia globalne

#include "common_vdetect/sequences.h"  // plik z opisem sekwencji
#include "frameImageProcessing.h"      // przetwarzanie dla pojedynczej ramki
#include "queue_system.h"              // system kolejek

#include "vdl_system/vdl_system.h"     // system lini VDL

#include <time.h>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <utility>
#include <vector>

//using namespace std;

// PARAMETRY SEKWENCJI
#define FRAME_START 0
#define FRAME_END   5000

// PARAMETRY SYGNALU Z KAMERY
#define XX 640
#define YY 480
std::vector<coord> coords{};
void mouseCallback(int event, int x, int y, int flags, void* data) {
    if (event != cv::EVENT_LBUTTONDOWN)
        return;
    coords.push_back(std::make_pair(x, y));
}

sequence* sequence_from_camera(cv::VideoCapture& cap) {
    cv::Mat image;
    cv::namedWindow("Wybierz punkty krancowe ukladu");
    cv::setMouseCallback("Wybierz punkty krancowe ukladu", mouseCallback, NULL);
    while(1) {
        cap >> image;
        cv::imshow("Wybierz punkty krancowe ukladu", image);
        if ( cv::waitKey(10) > 0 || coords.size()>=4) {
            cv::destroyAllWindows();
            break;
        }
    }

    if (coords.size() != 4) {
        throw "Nie podano czterech punktow startowych";
    }

    int movementTh = 100;
    int edgeTh = 100;
    int lanes = 4;
    int roisPerLane = 3;
    int width = image.cols;
    int height = image.rows;
    int sepLanes = 5;
    int sepRois = 5;
    return new sequence(width, height, coords, lanes, roisPerLane, sepLanes, sepRois, movementTh, edgeTh, image);

}

int main()
{

    // Sekwencja do analizy (konfiguracja w pliku)

    // Obikety
    frameImageProcessing *p_FIP=NULL;   // operacje globalne dla ramki
    queueSystem *p_QS=NULL;             // system kolejek
    //vdlSystem   *p_vdlS=NULL;         // system linii detekcji
    cv::VideoCapture cap(0);                // open the default camera

    if(!cap.isOpened())                 // check if we succeeded
        return -1;

    // Obrazki
    cv::Mat inputImage_rgb;   // obraz wejsciowy
    cv::Mat inputImage_grey;  // obraz wejsciowy w odcieniach szarosci TODO zamienic na YCbCr (tylko poszukac jak :))
    cv::Mat image_vis;        // obraz do wizualizacji

	cv::Mat movementMask;     // maska ruchu
	cv::Mat edgeMask;         // maska krawedziowa
	cv::Mat lbp;              // obraz LBP
    cv::Mat gaussianImage;    // obraz po filtracji Gaussa

    // Zmienne pomocnicze
    clock_t t1,t2;              // Obliczenia czasu trwania


    sequence* seq = sequence_from_camera(cap);

    sequence sq = *seq;

    // Obiekty globalne
    p_FIP = new frameImageProcessing(YY, XX);                                                                                                                                                // operacje na poziomie ramki obrazu
//    p_QS = new queueSystem(S.m_seq[iSequence].iLanes, S.m_seq[iSequence].iqueueROIs, NULL, S.m_seq[iSequence].sName, S.m_seq[iSequence].queueROIs,S.m_seq[iSequence].analysisROIs);          // system kolejek + analiza
    p_QS = new queueSystem(sq.iLanes, sq.iqueueROIs, nullptr, sq.sName, sq.queueROIs, sq.analysisROIs);

    t1=clock();
    for (;;) {

            // Wczytanie obrazka
            cap >> inputImage_rgb;

            // Kopiowanie obrazu do wizualizacji
            // +++ Wizualizacja
#ifdef DRAW
            inputImage_rgb.copyTo(image_vis);
#endif
            // Konwersja do odcieni szarosci
            // TODO to bedzie do zamiany na YCbCr
            cv::cvtColor(inputImage_rgb, inputImage_grey, CV_RGB2GRAY);

            // Globalne operacje dla ramki
            p_FIP->step(inputImage_rgb, inputImage_grey);
            p_FIP->getMovementMask(movementMask);               // wyznaczenie maski ruchu
            p_FIP->getEdgeMask(edgeMask);                       // wyznaczenie maski krawedzi
            p_FIP->getLBP(lbp);                                 // wyznaczenie deskryptora LBP
            p_FIP->getGaussianImage(gaussianImage);             // wyznaczenie rezultatu filtracji Gaussa

            // System kolejek
            p_QS->step(inputImage_grey, inputImage_rgb, lbp, movementMask,edgeMask,image_vis);

            // System VDL
            //p_vdlS->step(inputImage_grey, inputImage_rgb, gaussianImage, image_vis,iFrame);

            //cv::resize(inputImage_rgb,inputImage_rgb,cv::Size(720,576));

            // Zapis (zmiana rozdzielczosci) -- jednorazowe
            //sprintf(buffer, "seq_720/i%05d.png", iFrame); // normalne
            //cv::imwrite(buffer,inputImage_rgb);
#ifdef DRAW
            // debug
            cv::imshow("Input image",  inputImage_rgb);
            cv::imshow("Blured image",   gaussianImage);
            cv::imshow("Movement mask",  movementMask);
            cv::imshow("Edge mask",  edgeMask);

            cv::imshow("Image vis",  image_vis);
            if ( cv::waitKey(10) > 0 )
                break;
#endif
        }

        t2=clock();
        float diff ((float)t2-(float)t1);
        float seconds = diff/CLOCKS_PER_SEC;
        float noFrames = sq.iEnd - sq.iStart;
        std::cout<<"FRAMES = "<< noFrames         <<std::endl;
        std::cout<<"TIME = "  << seconds          <<std::endl;
        std::cout<<"FPS = "   << noFrames/seconds <<std::endl;

        // Kasowanie obiektow tymczasowych
        if (p_FIP) delete p_FIP;
		if (p_QS) delete p_QS;
    return 0;
}
