#ifndef VDL_SYSTEM_H
#define VDL_SYSTEM_H
#include "../globals.h"
#include "vdl_single_lane.h"

class vdlSystem {

public:
	vdlSystem(int iLanes, int *ivdls, int *gt, std::string *names, int **lineStartX, int **lineStartY, int **lineEndX, int **lineEndY);
	~vdlSystem();

	void step(const cv::Mat & image_grey,const  cv::Mat & image_rgb, const  cv::Mat & image_rgb_gauss, const  cv::Mat &movementMask,const  cv::Mat & edgeMask, cv::Mat & image_rgb_vis);
	void summary();

private:

	int m_iNsingleLanes;          //!< liczba liniI
	int *m_iVdls;                 //!< numery linii ??
	int *m_piCarCounters;         //!< liczniki na poszczególnych liniach
	int *m_piGroundTruths;        //!< faktyczne zliczenia
	vdlSingleLane *m_singleLanes;    //!< bufor na linie
};


#endif
