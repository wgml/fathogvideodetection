#ifndef SEQUENCES_H
#define SEQUENCES_H

#include "../globals.h"

#include <string>
#include <opencv2/core/core.hpp>


// TODO
// + to trzeba jako� lepiej wymyslic ( bardziej kompatktowy sposob opisu)

class sequence {
public:
	sequence() {};
	sequence(int width, int height, std::vector<std::pair<int, int>> coords, int lanes, int numRois,
			 int sepLanes, int sepRois, int movementTh, int edgeTh, cv::Mat image);

	std:: string path;		//!< sciezka do folderu z sekwencja (w postci zbioru obrazkow)
	int iXX;                //!< rozdzielczosc pozioma sekwencji
	int iYY;                //!< rozdzielczosc pionowa sekwencji
	int iStart;             //!< poczatek sewkencji
	int iEnd;               //!< koniec sekwencji
	int iLanes;             //!< liczba pasow, ktore maja byc analizowane
	std::string *sName;     //!< nazwy pasow (umowne)
	int *gtCounting;        //!< wartosc referencyjna dla zliczania
	int *iVdls;             //!< liczba linii detekcji na danym pasie

	int **lineStartX;       //!< wspolrzedne poczatkowe linii detekcji
	int **lineStartY;
	int **lineEndX;         //!< wspolrzedne koncowe linii detekcji
	int **lineEndY;


	// Obszary analizy zajetosci ....
	int *iqueueROIs;                    //!< liczba obszarow analizy kolejke na danym pasie
	queueROIPolygon **queueROIs;        //!< obszary zainteresowan dla kolejek

    // Obszar analizy pojazdow (kolor, marka itp.) - jedna na pas ruchu
    queueROIPolygon *analysisROIs;      //!< obszary zainteresowania dla obiektow analizy

};


class sequences {

public:
	sequences(const std::string fileName);
	~sequences();
	int m_iNumerOfSequences;
	sequence *m_seq;
};








#endif
