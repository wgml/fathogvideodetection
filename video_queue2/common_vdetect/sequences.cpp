#include "sequences.h"

#include <iostream>
#include <fstream>

// ------------------------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------------------------
sequences::sequences(const std::string fileName){

	std::fstream file;

	file.open(fileName.c_str(),std::fstream::in);

	// 1. Read the number of items
	file >> m_iNumerOfSequences;
	// 2. Create the buffer
	m_seq = new sequence[m_iNumerOfSequences];


	for (int i=0; i < m_iNumerOfSequences; i++) {
		file >> m_seq[i].path;   // sciezka do folderu z sekwencjami
		file >> m_seq[i].iXX;    // rozdzielczosc pozioma obrazu
		file >> m_seq[i].iYY;    // rozdzielczosc pionowa obrazu
		file >> m_seq[i].iStart; // poczatek przetwarzania
		file >> m_seq[i].iEnd;   // koniec przetwarzania
		file >> m_seq[i].iLanes; // ile pasow na skrzyzowaniu

		m_seq[i].gtCounting = new int[m_seq[i].iLanes];
		m_seq[i].iVdls = new int[m_seq[i].iLanes];
		m_seq[i].lineStartX = new int *[m_seq[i].iLanes];
		m_seq[i].lineStartY = new int *[m_seq[i].iLanes];
		m_seq[i].lineEndX = new int *[m_seq[i].iLanes];
		m_seq[i].lineEndY = new int *[m_seq[i].iLanes];

		m_seq[i].iqueueROIs = new int [m_seq[i].iLanes];

		m_seq[i].queueROIs    = new queueROIPolygon*[m_seq[i].iLanes];

		m_seq[i].analysisROIs = new queueROIPolygon[m_seq[i].iLanes];

		m_seq[i].sName = new std::string[m_seq[i].iLanes];


        // Pasy
		for (int j = 0; j < m_seq[i].iLanes; j++) {

			file >> m_seq[i].sName[j];         // nazwa pasa
			file >> m_seq[i].gtCounting[j];    // ile samochodow na pasie
			file >> m_seq[i].iVdls[j];		   // ile detektorow


			// Wspolrzedne VDL

			m_seq[i].lineStartX[j] = new int[m_seq[i].iVdls[j]];
			m_seq[i].lineStartY[j] = new int[m_seq[i].iVdls[j]];
			m_seq[i].lineEndX[j] = new int[m_seq[i].iVdls[j]];
			m_seq[i].lineEndY[j] = new int[m_seq[i].iVdls[j]];


			for (int k=0; k < m_seq[i].iVdls[j]; k++) {

				int x1,y1,x2,y2;
				file >> x1 >> y1 >> x2 >> y2;
				m_seq[i].lineStartX[j][k] = x1;
				m_seq[i].lineStartY[j][k] = y1;
				m_seq[i].lineEndX[j][k] = x2;
				m_seq[i].lineEndY[j][k] = y2;

				//debug
				//std::cout << m_seq[i].lineStartX[j][k] <<" "<< m_seq[i].lineStartY[j][k] <<" "<< m_seq[i].lineEndX[j][k] <<" "<< m_seq[i].lineEndY[j][k] <<std::endl;

			}

			// Wspolrzednie dla kolejki
			file >> m_seq[i].iqueueROIs[j];		   // ile obszarow do analizy kolejki

			m_seq[i].queueROIs[j] = new queueROIPolygon[m_seq[i].iqueueROIs[j]];

			for (int k = 0; k < m_seq[i].iqueueROIs[j]; k++) {

				// Wspolrzedne
				int xTL, yTL, xBL, yBL, xTR, yTR, xBR, yBR;
				file >> xTL >> yTL >> xBL >> yBL >> xTR >> yTR >> xBR >> yBR ;

				// Progi
				int mTh, eTh;
				file >> mTh >> eTh;
				m_seq[i].queueROIs[j][k].BL.x = xBL;
				m_seq[i].queueROIs[j][k].BL.y = yBL;
				m_seq[i].queueROIs[j][k].TL.x = xTL;
				m_seq[i].queueROIs[j][k].TL.y = yTL;
				m_seq[i].queueROIs[j][k].BR.x = xBR;
				m_seq[i].queueROIs[j][k].BR.y = yBR;
				m_seq[i].queueROIs[j][k].TR.x = xTR;
				m_seq[i].queueROIs[j][k].TR.y = yTR;
				m_seq[i].queueROIs[j][k].movementTh = mTh;
				m_seq[i].queueROIs[j][k].edgeTh = eTh;

			}

			// Wspolrzedne dla analizy
            // Wspolrzedne
			int xTL, yTL, xBL, yBL, xTR, yTR, xBR, yBR;
			file >> xTL >> yTL >> xBL >> yBL >> xTR >> yTR >> xBR >> yBR ;

			// Progi
			int mTh, eTh;
			file >> mTh >> eTh;

			m_seq[i].analysisROIs[j].BL.x = xBL;
			m_seq[i].analysisROIs[j].BL.y = yBL;
			m_seq[i].analysisROIs[j].TL.x = xTL;
			m_seq[i].analysisROIs[j].TL.y = yTL;
			m_seq[i].analysisROIs[j].BR.x = xBR;
			m_seq[i].analysisROIs[j].BR.y = yBR;
			m_seq[i].analysisROIs[j].TR.x = xTR;
			m_seq[i].analysisROIs[j].TR.y = yTR;
			m_seq[i].analysisROIs[j].movementTh = mTh;
			m_seq[i].analysisROIs[j].edgeTh = eTh;


		}

	}

}

sequences::~sequences() {

	for (int i=0; i < m_iNumerOfSequences; i++) {

		delete [] m_seq[i].gtCounting;
		delete [] m_seq[i].iVdls;
		delete [] m_seq[i].sName;
		delete[]  m_seq[i].iqueueROIs;

		for (int j = 0; j < m_seq[i].iLanes; j++) {
			delete [] m_seq[i].lineStartX[j];
			delete [] m_seq[i].lineEndX[j];
			delete [] m_seq[i].lineStartY[j];
			delete [] m_seq[i].lineEndY[j];

			delete[] m_seq[i].queueROIs[j];
		}



		delete[] m_seq[i].queueROIs;


		delete [] m_seq[i].lineStartX;
		delete [] m_seq[i].lineEndX;
		delete [] m_seq[i].lineStartY;
		delete [] m_seq[i].lineEndY;

	}


}

using coord = std::pair<int, int>;
sequence::sequence(int width, int height, std::vector<coord> coords, int numLanes, int numRois,
				   int sepLanes, int sepRois, int movementTh, int edgeTh, cv::Mat image) {

	iXX = width;
	iYY = height;

	iLanes = numLanes;
	iqueueROIs = new int [iLanes];
	sName = new std::string[iLanes];
	for (int i = 0; i < numLanes; i++) {
		iqueueROIs[i] = numRois;
		sName[i] = std::string("xD");
	}
	queueROIs    = new queueROIPolygon*[iLanes];
	analysisROIs = new queueROIPolygon[iLanes];
	lineStartX = new int *[iLanes];
	lineStartY = new int *[iLanes];
	lineEndX = new int *[iLanes];
	lineEndY = new int *[iLanes];
	coord tl = coords[0];
	coord tr = coords[1];
	coord bl = coords[2];
	coord br = coords[3];
	const int dXTop = (tr.first - tl.first) / numLanes;
	const int dYTop = (tr.second - tl.second) / numLanes;
	const int dXBottom = (br.first - bl.first) / numLanes;
	const int dYBottom = (br.second - bl.second) / numLanes;

	for (int lane = 0; lane < numLanes; lane++) {
		queueROIs[lane] = new queueROIPolygon[iqueueROIs[lane]];
		lineStartX[lane] = new int[numRois];
		lineStartY[lane] = new int[numRois];
		lineEndX[lane] = new int[numRois];
		lineEndY[lane] = new int[numRois];
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

		const int dXLeft = (lane_bl.first - lane_tl.first) / numRois;
		const int dYLeft = (lane_bl.second - lane_tl.second) / numRois;
		const int dXRight = (lane_br.first - lane_tr.first) / numRois;
		const int dYRight = (lane_br.second - lane_tr.second) / numRois;

		for (int roi = 0; roi < numRois; roi++) {
			coord roi_tl = std::make_pair(
					lane_tl.first + roi * dXLeft,
					lane_tl.second + roi * dYLeft
			);
			coord roi_tr = std::make_pair(
					lane_tr.first + roi * dXRight,
					lane_tr.second + roi * dYRight
			);
			coord roi_bl = std::make_pair(
					lane_tl.first + (roi + 1) * dXLeft,
					lane_tl.second + (roi + 1) * dYLeft
			);
			coord roi_br = std::make_pair(
					lane_tr.first + (roi + 1) * dXRight,
					lane_tr.second + (roi + 1) * dYRight
			);

			queueROIs[lane][roi].TL.x = roi_tl.first;
			queueROIs[lane][roi].TL.y = roi_tl.second;
			queueROIs[lane][roi].TR.x = roi_tr.first;
			queueROIs[lane][roi].TR.y = roi_tr.second;
			queueROIs[lane][roi].BL.x = roi_bl.first;
			queueROIs[lane][roi].BL.y = roi_bl.second;
			queueROIs[lane][roi].BR.x = roi_br.first;
			queueROIs[lane][roi].BR.y = roi_br.second;
			queueROIs[lane][roi].movementTh = movementTh;
			queueROIs[lane][roi].edgeTh = edgeTh;
//			cv::circle(image, queueROIs[lane][roi].TL, 2, 0, 5);
//			cv::circle(image, queueROIs[lane][roi].TR, 2, 0, 5);
//			cv::circle(image, queueROIs[lane][roi].BL, 2, 0xFFFFFF, 10);
//			cv::circle(image, queueROIs[lane][roi].BR, 2, 0xFFFFFF, 10);

//			lineStartX[lane][roi] = std::min(tl.first, bl.first);
//			lineStartY[lane][roi] = std::min(tl.second, tr.second);
//			lineEndX[lane][roi] = std::max(br.first, tr.first);
//			lineEndY[lane][roi] = std::min(bl.second, br.second);
		}

		analysisROIs[lane].TL.x = lane_tl.first;
		analysisROIs[lane].TL.y = lane_tl.second;
		analysisROIs[lane].TR.x = lane_tr.first;
		analysisROIs[lane].TR.y = lane_tr.second;
		analysisROIs[lane].BL.x = lane_bl.first;
		analysisROIs[lane].BL.y = lane_bl.second;
		analysisROIs[lane].BR.x = lane_br.first;
		analysisROIs[lane].BR.y = lane_br.second;
		analysisROIs[lane].movementTh = movementTh;
		analysisROIs[lane].edgeTh = edgeTh;
		cv::circle(image, analysisROIs[lane].TL, 2, 0, 5);
		cv::circle(image, analysisROIs[lane].TR, 2, 0xffffff, 10);
		cv::circle(image, analysisROIs[lane].BL, 2, 0, 5);
		cv::circle(image, analysisROIs[lane].BR, 2, 0xffffff, 10);

	}
	//cv::imshow("a", image);
	//while(cv::waitKey(10) != 0);
}

