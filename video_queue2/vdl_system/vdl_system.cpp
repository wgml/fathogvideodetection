#include "vdl_system.h"

// ------------------------------------------------------------------------------------------------------------
// constructor
// ------------------------------------------------------------------------------------------------------------
vdlSystem::vdlSystem(int iLanes, int *ivdls, int *gt, std::string *names, int **lineStartX, int **lineStartY, int **lineEndX, int **lineEndY){

	m_iNsingleLanes = iLanes;                       // ile pasow

	m_piVdls = new int[m_iNsingleLanes];            // liczba linii detekcji na pasach
	m_piCarCounters = new int[m_iNsingleLanes];     // licznik samochdow na danym pasie
	m_piGroundTruths = new int [m_iNsingleLanes];   // zapamietanie GT (dla testow)

    // Utworzenie poszczegolnych pasow
	for (int i=0; i< m_iNsingleLanes; i++) {
	    m_piVdls[i] = ivdls[i];
		m_piGroundTruths[i] = gt[i];

        //std::cout <<"No vld "<< ivdls[i] << " " << gt[i]  << std::endl;
		m_singleLanes[i].initSingleLane(names[i], ivdls[i], gt[i], lineStartX[i], lineStartY[i], lineEndX[i], lineEndY[i]);
	}

	//for (int i=0; i< m_iNsingleLanes; i++)
    //    std::cout <<"No vld "<< names[i] <<" " <<m_piVdls[i] << " " << m_piGroundTruths[i]  << std::endl;

    std::cout <<"vdlSystem()"<< std::endl;

}

// ------------------------------------------------------------------------------------------------------------
// destructor
// ------------------------------------------------------------------------------------------------------------
vdlSystem::~vdlSystem() {
	std::cout <<"~vdlSystem" << std::endl;
	if (m_piVdls) 	delete [] m_piVdls;
	if (m_piCarCounters) delete [] m_piCarCounters;
	if (m_piGroundTruths) delete [] m_piGroundTruths;

}

// ------------------------------------------------------------------------------------------------------------
// step
// - operacje na poziomie calej ramki
// ------------------------------------------------------------------------------------------------------------
void vdlSystem::step(const cv::Mat & image_grey, const cv::Mat & image_rgb, const cv::Mat & image_rgb_gauss, cv::Mat & image_rgb_vis, int iFrame){


    // dla kazdego systemu wywolaj step ... chyba nic madrego na raize na tym poziomie ....
	for (int i = 0; i < m_iNsingleLanes; i++)
       m_piCarCounters[i] = m_singleLanes[i].step(image_grey, image_rgb,image_rgb_gauss,iFrame);

    // wizualizacja
#ifdef DRAW_VDL
    for (int i = 0; i < m_iNsingleLanes; i++)
        m_singleLanes[i].drawVDL(image_rgb_vis);
#endif

}

// ------------------------------------------------------------------------------------------------------------
// sumary
// wyswietlanie podsumowania dla systemu
// ------------------------------------------------------------------------------------------------------------
void vdlSystem::summary() {
	for (int i=0; i < m_iNsingleLanes; i++) {
		printf("L%d | FAKTYCZNIE %d | ZLICZONO %d\n",i,m_piGroundTruths[i],m_piCarCounters[i]);
	}
}
