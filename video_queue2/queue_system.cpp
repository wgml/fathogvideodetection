#include "queue_system.h"

// ------------------------------------------------------------------------------------------------------------
//! constructor
// ------------------------------------------------------------------------------------------------------------
queueSystem::queueSystem(int iLanes, int *iqueueROIs, int *gtQueue, std::string *names, queueROIPolygon **queueROIs, queueROIPolygon *analysisROIs) {

	m_iNsingleLanes = iLanes;						// ile pasow ruchu
	m_piQueueROIs = new int[m_iNsingleLanes];       // ile ROI na danym pasie


	for (int i = 0; i< m_iNsingleLanes; i++) {
		m_piQueueROIs[i] = iqueueROIs[i];

		queue_single_lane qsl;
		qsl.initSingleLane(names[i], iqueueROIs[i], queueROIs[i],analysisROIs[i],&m_colourR, &m_typeR);       // inicjalizacja pojedynczego pasa
		m_queueSingleLanes.push_back(qsl);

		//m_piGroundTruths[i] = gt[i];
		//m_singleLines[i].initSingleLine(names[i], ivdls[i], gt[i], lineStartX[i], lineStartY[i], lineEndX[i], lineEndY[i]);
	}

}


// ------------------------------------------------------------------------------------------------------------
//! destructor
// ------------------------------------------------------------------------------------------------------------
queueSystem::~queueSystem() {
	delete[] m_piQueueROIs;
	m_queueSingleLanes.clear();

	//delete[] m_singleLines;
	//delete[] m_piCarCounters;
	//delete[] m_piGroundTruths;
}

// ------------------------------------------------------------------------------------------------------------
//! step
// Tutaj bedzie ew. integracja informacji dla calego systemu linii detekcji
// Uwaga. Koncepcja wizualizacja -> kazda linia "sama siebie rysuje".
// ------------------------------------------------------------------------------------------------------------
void queueSystem::step(cv::Mat & image_grey, cv::Mat & image_rgb, cv::Mat & image_lbp, cv::Mat & movementMask, cv::Mat & edgeMask, cv::Mat & image_vis){



	cv::Mat darkAreaMask;

    // Wywolanie obslugi dla kazdego pasa osobno
	for (int i = 0; i < m_iNsingleLanes; i++) {
	    m_queueSingleLanes[i].step(image_grey, image_rgb, image_lbp, movementMask, edgeMask, darkAreaMask, image_vis);
    }



	// debug
	//cv::imshow("CFD Mask", mask);
	//cv::waitKey(1);



    // Kopiowanie poprzedniego
	image_rgb.copyTo(image_rgb_prev);

	// dla kazdego systemu wywolaj step ... chyba nic madrego na raize na tym poziomie ....
	//for (int i = 0; i < m_iNsingleLanes; i++)
		//m_piCarCounters[i] = m_singleLines[i].step(image_grey, image_rgb);

	//for (int i = 0; i < m_iNsingleLanes; i++)
		//m_singleLines[i].drawVDL(image_rgb);


}

void queueSystem::readjustSystem(coord tl, coord tr, coord bl, coord br) {
	const int numLanes = m_iNsingleLanes;

	const int dXTop = (tr.first - tl.first) / numLanes;
	const int dYTop = (tr.second - tl.second) / numLanes;
	const int dXBottom = (br.first - bl.first) / numLanes;
	const int dYBottom = (br.second - bl.second) / numLanes;

	for (int lane = 0; lane < numLanes; lane++) {
		coord lane_tl = std::make_pair(
				tl.first + lane * dXTop,
				tl.second + lane * dYTop
		);
		coord lane_tr = std::make_pair(
				lane_tl.first + dXTop,
				lane_tl.second + dYTop
		);
		coord lane_bl = std::make_pair(
				bl.first + lane * dXBottom,
				bl.second + lane * dYBottom
		);
		coord lane_br = std::make_pair(
				lane_bl.first + dXBottom,
				lane_bl.second + dYBottom
		);


		std::vector<queue_roi>& lane_rois = m_queueSingleLanes[lane].getRois();
		const int numRois = m_piQueueROIs[lane];

		const int dXLeft = (lane_bl.first - lane_tl.first) / numRois;
		const int dYLeft = (lane_bl.second - lane_tl.second) / numRois;
		const int dXRight = (lane_br.first - lane_tr.first) / numRois;
		const int dYRight = (lane_br.second - lane_tr.second) / numRois;

		for (int roi = 0; roi < numRois; roi++) {
			queue_roi qr = lane_rois[roi];
			coord roi_tl = std::make_pair(
					lane_tl.first + roi * dXLeft,
					lane_tl.second + roi * dYLeft
			);
			coord roi_tr = std::make_pair(
					roi_tl.first + dXLeft,
					roi_tl.second + dYLeft
			);
			coord roi_bl = std::make_pair(
					lane_bl.first + roi * dXRight,
					lane_bl.second + roi * dYRight
			);
			coord roi_br = std::make_pair(
					roi_bl.first + dXRight,
					roi_bl.second + dYRight
			);
			qr.readjust(roi_tl, roi_tr, roi_bl, roi_br);
		}
	}
}


// sumary
// ------------------------------------------------------------------------------------------------------------
/*
void queueSystem::summary() {
	for (int i = 0; i < m_iNsingleLines; i++) {
		printf("L%d | FAKTYCZNIE %d | ZLICZONO %d\n", i, m_piGroundTruths[i], m_piCarCounters[i]);


	}

}
*/




