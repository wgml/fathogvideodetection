#include "globals.h"                   // ustawienia globalne

#include "common_vdetect/sequences.h"  // plik z opisem sekwencji
#include "frameImageProcessing.h"      // przetwarzanie dla pojedynczej ramki
#include "queue_system.h"              // system kolejek

#include "vdl_system/vdl_system.h"     // system lini VDL

#include <time.h>
#include <iostream>


//using namespace std;

// PARAMETRY SEKWENCJI
#define FRAME_START 0
#define FRAME_END   5000

// PARAMETRY SYGNALU Z KAMERY
#define XX 640
#define YY 480



// TODO !!!!!!!!!!!!
// - obliczanie kawerdzi nie metoda Canny
// - dodanie LBP do operacji globlanych
// - co to sa za filtry medianowa w module queue_roi
// - o co chodzi z tym darkGray (czy to nie dziala, czy co ?)

// DOBOR PROGOW
// Sobel/Canny
// Odejmowanie ramek

int main()
{

    // Sekwencja do analizy (konfiguracja w pliku)
    sequences S("/home/vka/Programming/C/workspace/fathogvideodetection/video_queue2/queue_demo.txt");

    // Obikety
    frameImageProcessing *p_FIP=NULL;   // operacje globalne dla ramki
    queueSystem *p_QS=NULL;             // system kolejek
    //vdlSystem   *p_vdlS=NULL;         // system linii detekcji
    cv::VideoCapture cap(2);                // open the default camera

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
    char buffer[100];
    clock_t t1,t2;              // Obliczenia czasu trwania


    int iSequence = 0;          // numer sekwencji - na sztywno 0 (kompatybilność)

    // Obiekty globalne
    p_FIP = new frameImageProcessing(YY, XX);                                                                                                                                                // operacje na poziomie ramki obrazu
    p_QS = new queueSystem(S.m_seq[iSequence].iLanes, S.m_seq[iSequence].iqueueROIs, NULL, S.m_seq[iSequence].sName, S.m_seq[iSequence].queueROIs,S.m_seq[iSequence].analysisROIs);          // system kolejek + analiza

    //p_vdlS = new vdlSystem(S.m_seq[iSequence].iLanes, S.m_seq[iSequence].iVdls, S.m_seq[iSequence].gtCounting, S.m_seq[iSequence].sName, S.m_seq[iSequence].lineStartX, S.m_seq[iSequence].lineStartY, S.m_seq[iSequence].lineEndX, S.m_seq[iSequence].lineEndY); // system linii detekcji

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
        float noFrames = S.m_seq[iSequence].iEnd - S.m_seq[iSequence].iStart;
        std::cout<<"FRAMES = "<< noFrames         <<std::endl;
        std::cout<<"TIME = "  << seconds          <<std::endl;
        std::cout<<"FPS = "   << noFrames/seconds <<std::endl;

        // Kasowanie obiektow tymczasowych
        if (p_FIP) delete p_FIP;
		if (p_QS) delete p_QS;
		//if (p_vdlS) delete p_vdlS;



    //cv::viz::Viz3d window = cv::viz::Viz3d("Viz demonstration");


    //cv::Point3d min(0.25, 0.0, 0.25);
    //cv::Point3d max(0.75, 0.5, 0.75);

    //cv::viz::WCube cube(min, max, true, cv::viz::Color::blue());
    //cube.setRenderingProperty(cv::viz::LINE_WIDTH, 4.0);

    //window.showWidget("Axis widget", cv::viz::WCoordinateSystem());
    //window.showWidget("Cube widget", cube);

    //while(!window.wasStopped()) {
    //    window.spinOnce(1, true);
    //}

    return 0;
}
