#include "vdl_system.h"


// ------------------------------------------------------------------------------------------------------------
// constructor
// ------------------------------------------------------------------------------------------------------------
vdlSystem::vdlSystem(int iLanes, int *ivdls, int *gt, std::string *names, int **lineStartX, int **lineStartY, int **lineEndX, int **lineEndY){

	m_iNsingleLanes = iLanes;

	m_iVdls = new int[m_iNsingleLanes];
	m_piCarCounters = new int[m_iNsingleLanes];
	m_piGroundTruths = new int [m_iNsingleLanes];

	m_singleLanes = new vdlSingleLane[m_iNsingleLanes];

	for (int i=0; i< m_iNsingleLanes; i++) {
	    std::cout <<"Inicjalizacja pasa VDL" << i << std::endl;
		m_iVdls[i] = ivdls[i];
		m_piGroundTruths[i] = gt[i];
		m_singleLanes[i].initSingleLane(names[i], ivdls[i], gt[i], lineStartX[i], lineStartY[i], lineEndX[i], lineEndY[i]);
	}
}

// ------------------------------------------------------------------------------------------------------------
// destructor
// ------------------------------------------------------------------------------------------------------------
vdlSystem::~vdlSystem() {
	delete [] m_iVdls;
	delete [] m_singleLanes;
	delete [] m_piCarCounters;
	delete [] m_piGroundTruths;
}

// ------------------------------------------------------------------------------------------------------------
// step
// - operacje na poziomie calej ramki
// ------------------------------------------------------------------------------------------------------------
void vdlSystem::step(const cv::Mat & image_grey,const  cv::Mat & image_rgb, const  cv::Mat & image_rgb_gauss, const  cv::Mat &movementMask,const  cv::Mat & edgeMask, cv::Mat & image_rgb_vis){

	// dla kazdego systemu wywolaj step ... chyba nic madrego na raize na tym poziomie ....

	for (int i = 0; i < m_iNsingleLanes; i++) {
	    // debug
        std::cout <<"Obliczenia dla pasa VDL" << i << std::endl;
        m_piCarCounters[i] = m_singleLanes[i].step(image_grey, image_rgb,image_rgb_gauss,movementMask,edgeMask);
    }
#ifdef DRAW_VDL
    for (int i = 0; i < m_iNsingleLanes; i++)
        m_singleLanes[i].drawVDL(image_rgb_vis);
#endif

}

// sumary
// ------------------------------------------------------------------------------------------------------------
void vdlSystem::summary() {
	for (int i=0; i < m_iNsingleLanes; i++) {
		printf("L%d | FAKTYCZNIE %d | ZLICZONO %d\n",i,m_piGroundTruths[i],m_piCarCounters[i]);
	}
}
