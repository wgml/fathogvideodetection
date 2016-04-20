#ifndef QUEUE_SYSTEM_H
#define QUEUE_SYSTEM_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "car_analysis/colourRecognition.h"
#include "car_analysis/typeRecognitionLBP.h"

#include "queue_single_lane.h"
#include "globals.h"

// System do obslugi  okreslania dlugosi kolejki
// Obsluga calego obrazu

class queueSystem {

public:
	queueSystem(int iLanes, int *iqueueROIs, int *gtQueue, std::string *names, queueROIPolygon **queueROIs, queueROIPolygon *analysisROIs);
	~queueSystem();

	void step(cv::Mat & image_grey, cv::Mat & image_rgb, cv::Mat & image_lbp, cv::Mat & movementMask, cv::Mat & edgeMask, cv::Mat & image_vis);
	//void summary();



private:

	int m_iNsingleLanes;								  //!< liczba analizowanych pasow ruchu
	int *m_piQueueROIs;									  //!< liczba obszarow zainteresownia w ramach danej linii

	cv::Mat image_rgb_prev;								  //!< poprzednia ramka obrazu

    colourRecognition  m_colourR;                          //!< obiekt do rozpoznawnia kolorow
    typeRecognitionLBP m_typeR;                            //!< obiekt do rozpoznawnia typu pojadu


	std::vector<queue_single_lane> m_queueSingleLanes;    //!< obsluga pojedynczej linii



};


#endif
