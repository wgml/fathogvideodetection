#include "globals.h"                   // ustawienia globalne

#include "common_vdetect/sequences.h"  // plik z opisem sekwencji
#include "frameImageProcessing.h"      // przetwarzanie dla pojedynczej ramki
#include "queue_system.h"              // system kolejek

#include "vdl_system/vdl_system.h"     // system lini VDL

#include <memory>

// PARAMETRY SYGNALU Z KAMERY
#define XX 640
#define YY 480
std::vector<coord> coords{};

void mouseCallback(int event, int x, int y, int flags, void *data) {
    if (event != cv::EVENT_LBUTTONDOWN)
        return;
    coords.push_back(std::make_pair(x, y));
}

sequence *sequence_from_camera(cv::VideoCapture &cap) {
    cv::Mat image;
    cv::namedWindow("Wybierz punkty krancowe ukladu");
    cv::setMouseCallback("Wybierz punkty krancowe ukladu", mouseCallback, NULL);
    while (1) {
        cap >> image;
        cv::imshow("Wybierz punkty krancowe ukladu", image);
        if (cv::waitKey(10) > 0 || coords.size() >= 4) {
            cv::destroyAllWindows();
            break;
        }
    }
    if (coords.size() != 4) {
        throw "Nie podano czterech punktow startowych";
    }

    int movementTh = 200;
    int edgeTh = 2000;
    int lanes = 4;
    int roisPerLane = 3;
    int width = image.cols;
    int height = image.rows;
    int sepLanes = 5;
    int sepRois = 5;

    std::cout << "Liczba pasów: " << lanes << ", segmentów na pas: " << roisPerLane << std::endl;
    return new sequence(width, height, coords, lanes, roisPerLane, sepLanes, sepRois, movementTh,
                        edgeTh, image);

}

int main() {
    cv::VideoCapture cap(0);                // open the default camera
    if (!cap.isOpened())                 // check if we succeeded
        return -1;

    cv::Mat inputImage_rgb;   // obraz wejsciowy
    cv::Mat inputImage_grey;  // obraz wejsciowy w odcieniach szarosci TODO zamienic na YCbCr (tylko poszukac jak :))

#ifdef DRAW
    cv::Mat image_vis;        // obraz do wizualizacji
#endif

    cv::Mat movementMask;     // maska ruchu
    cv::Mat edgeMask;         // maska krawedziowa
    cv::Mat lbp;              // obraz LBP
    cv::Mat gaussianImage;    // obraz po filtracji Gaussa

    sequence *seq = sequence_from_camera(cap);

    sequence sq = *seq;

    std::unique_ptr<frameImageProcessing> p_FIP = std::make_unique<frameImageProcessing>(YY, XX);
    std::unique_ptr<queueSystem> p_QS = std::make_unique<queueSystem>(sq.iLanes, sq.iqueueROIs,
                                                                      nullptr, sq.sName,
                                                                      sq.queueROIs,
                                                                      sq.analysisROIs);

    while (true) {

        cap >> inputImage_rgb;
#ifdef DRAW
        inputImage_rgb.copyTo(image_vis);
#endif
        cv::cvtColor(inputImage_rgb, inputImage_grey, CV_RGB2GRAY);

        p_FIP->step(inputImage_rgb, inputImage_grey);
        p_FIP->getMovementMask(movementMask);               // wyznaczenie maski ruchu
        p_FIP->getEdgeMask(edgeMask);                       // wyznaczenie maski krawedzi
        p_FIP->getLBP(lbp);                                 // wyznaczenie deskryptora LBP
        p_FIP->getGaussianImage(
                gaussianImage);             // wyznaczenie rezultatu filtracji Gaussa

        p_QS->step(inputImage_grey, inputImage_rgb, lbp, movementMask, edgeMask, image_vis);

#ifdef DRAW
        // debug
        cv::imshow("Input image", inputImage_rgb);
        cv::imshow("Blured image", gaussianImage);
        cv::imshow("Movement mask", movementMask);
        cv::imshow("Edge mask", edgeMask);

        cv::imshow("Image vis", image_vis);
        if (cv::waitKey(10) > 0)
            break;
#endif
    }
    return 0;
}
