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

// ------------------------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------------------------
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
