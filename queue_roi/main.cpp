#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <memory>

#include "queue_system.h"

#include "processing/frameImageProcessing.h"

#define DRAW

std::unique_ptr<queueSystem> init_queue_system();

using queue_sys_ptr = std::unique_ptr<queueSystem>;
using frame_img_proc_ptr = std::unique_ptr<frameImageProcessing>;

int main() {
    cv::VideoCapture cap(1);
    if (!cap.isOpened()) {
        return -1;
    }

    // Obrazki
    cv::Mat inputImage_rgb;   // obraz wejsciowy
    cv::Mat inputImage_grey;  // obraz wejsciowy w odcieniach szarosci
    cv::Mat image_vis;        // obraz do wizualizacji

    cv::Mat movementMask;     // maska ruchu
    cv::Mat edgeMask;         // maska krawedziowa
    cv::Mat lbp;              // obraz LBP
    cv::Mat gaussianImage;    // obraz po filtracji Gaussa

    cap >> inputImage_rgb;    // todo-wojtek test frame, could somehow get those params from `cap`

    frame_img_proc_ptr p_FIP = std::make_unique<frameImageProcessing>(inputImage_rgb.rows, inputImage_rgb.cols);

    queue_sys_ptr p_QS = init_queue_system();

    while(1) {
        cap >> inputImage_rgb;

#ifdef DRAW
//        inputImage_rgb.copyTo(image_vis);
#endif

        cv::cvtColor(inputImage_rgb, inputImage_grey, CV_RGB2GRAY);

        p_FIP->step(inputImage_rgb, inputImage_grey);
        p_FIP->getMovementMask(movementMask);               // wyznaczenie maski ruchu
        p_FIP->getEdgeMask(edgeMask);                       // wyznaczenie maski krawedzi
        p_FIP->getLBP(lbp);                                 // wyznaczenie deskryptora LBP
        p_FIP->getGaussianImage(gaussianImage);             // wyznaczenie rezultatu filtracji Gaussa

#ifdef IS_IT_WORKING_YET
        p_QS->step(inputImage_grey, inputImage_rgb, lbp, movementMask, edgeMask, image_vis);
#endif
#ifdef DRAW
        cv::imshow("grb",  inputImage_rgb);
        cv::imshow("gray",  inputImage_grey);
        cv::imshow("movementMask",  movementMask);
        cv::imshow("edgeMask",  edgeMask);
        cv::imshow("lbp",  lbp);
        cv::imshow("gaussianImage",  gaussianImage);
        if (cv::waitKey(30) >= 0) {
            break;
        }
#endif
    }
    return 0;
}

std::unique_ptr<queueSystem> init_queue_system() {
#ifdef IS_IT_WORKING_YET
    int lanes = 2;
    int *rois = nullptr;
    std::string *names = nullptr;
    queueROIPolygon **queueRois = nullptr;
    return std::make_unique<queueSystem>(lanes,
                                         rois,
                                         nullptr,
                                         names,
                                         queueRois);
#else
    return nullptr;
#endif
}
