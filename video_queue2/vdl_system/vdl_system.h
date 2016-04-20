#ifndef VDL_SYSTEM_H
#define VDL_SYSTEM_H

#include "vdl_single_lane.h"

class vdlSystem {

public:
	vdlSystem(int iLanes, int *ivdls, int *gt, std::string *names, int **lineStartX, int **lineStartY, int **lineEndX, int **lineEndY);
	~vdlSystem();

	void step(const cv::Mat & image_grey, const cv::Mat & image_rgb, const cv::Mat & image_rgb_gauss, cv::Mat & image_rgb_vis, int iFrame);
	void summary();

private:

	int m_iNsingleLanes;                           //!< liczba linii
	int *m_piVdls;                                 //!< numery linii ??
	int *m_piCarCounters;                          //!< liczniki na poszczegolnych liniach
	int *m_piGroundTruths;                         //!< faktyczne zliczenia
	vdlSingleLane m_singleLanes[VDL_MAX_LANES];    //!< bufor na linie

};


#endif
