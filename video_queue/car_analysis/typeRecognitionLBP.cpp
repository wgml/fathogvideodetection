#include "typeRecognitionLBP.h"
#include <fstream>
#include <iostream>


// ------------------------------------------------------------------------------------------------------------
// typeRecognitionLBP
// lbpImage - obraz w odpowiednim formacie LBP (de facto - okno detekcji)
// XX, YY - rozmiar okna detekcji
// bSize - rozmiar bloku (zakalada sie ze jest symetryczny)
// hSize - rozmiar histogramu (zalezy od typu LBP)
// Uwaga: tu optymistycznie zaklada sie, ze rozmiar XX,YY jest wielokrotnoscia XX i YY
// ------------------------------------------------------------------------------------------------------------
typeRecognitionLBP::typeRecognitionLBP()
{
    // Obliczenie rozmiaru na wektror cech
    m_iBlockSize = LBP_BSIZE;
    m_iHistSize  = LBP_HSIZE;

    m_iNoBlockX = LBP_XX/m_iBlockSize;
    m_iNoBlockY = LBP_YY/m_iBlockSize;

    m_iFeatureVectorSize = m_iNoBlockX * m_iNoBlockY * m_iHistSize;

    // Utworzenie wektora cech
    m_piFeatureVector = new int[m_iFeatureVectorSize];

    // Wczytanie SVM;ow z pliku
    readSVMFromFile();


}

// ------------------------------------------------------------------------------------------------------------
// ~typeRecognitionLBP
// ------------------------------------------------------------------------------------------------------------
typeRecognitionLBP::~typeRecognitionLBP()
{
    // Kasowanie tablicy na wektor cech
    delete [] m_piFeatureVector;

    // Kasowanie tablcy wag SVM i parametru b;
    for (int i=0; i < m_iNoClasses;i++)
        delete [] m_pfSVMWeights[i];

    delete [] m_pfSVMWeights;
    delete [] m_pfSVMB;
    delete [] m_pfSVMvalues;
}


// ------------------------------------------------------------------------------------------------------------
// recognizeType
// lbpImage - obraz w odpowiednim formacie LBP (de facto - okno detekcji)
// ------------------------------------------------------------------------------------------------------------
std::string typeRecognitionLBP::recognizeType(cv::Mat lbpImage) {

    createFeatureVectorLBP(lbpImage);           // utworzenie wektora cech
    int c = classifyFeatureVectorLBP_SVM();       // klasyfikacja liniowym SVM'em

    // Jesli cos znaleziono
    if ( c != -1 )
        return m_classNames[c];
    else
        return "No detection";

}

// ------------------------------------------------------------------------------------------------------------
// createFeatureVectorLBP
// lbpImage - obraz w odpowiednim formacie LBP (de facto - okno detekcji)
// ------------------------------------------------------------------------------------------------------------
void typeRecognitionLBP::createFeatureVectorLBP(cv::Mat lbpImage) {

    int bX,bY;          // wspolrzedne bloku
    uchar lbpValue;     // konkretna wartosc LBP
    int addr;           // konkretny adres w wektorze cech

    //debug
    //cv::imshow("LBP PATCH",lbpImage*4);

    //zerowanie histogramu
    memset(m_piFeatureVector,0,sizeof(int)*m_iFeatureVectorSize);

    // Petla po obrazie
    for (int jj = 1; jj < lbpImage.rows-1;jj++) {
		for (int ii = 1; ii < lbpImage.cols-1; ii++) {

            // Okreslenie do ktorego bloku nalezy dany piksel
            bY = jj / m_iBlockSize;
            bX = ii / m_iBlockSize;

            // Odczytanie wartosci lbp
            lbpValue = lbpImage.at<vec_uchar_1>(jj, ii)[0];

            // Okreslenie adresu w wektorze cech
            //std::cout << bY << " " << bX <<std::endl;
            addr = (bY*m_iNoBlockX*m_iHistSize)+(bX*m_iHistSize)+lbpValue;
            //if (addr > 1049){
            //    int x=0;
            //}
            //std::cout << addr <<std::endl;


            // Inkrementacja wektora cech
            m_piFeatureVector[addr]++;

		}
    }

}

// ------------------------------------------------------------------------------------------------------------
// classifyFeatureVectorLBP_SVM
// klasyfikacja na podstawie wczytanego wczesniej wektora cech
// TODO rozwazyc inne opcje...
// ------------------------------------------------------------------------------------------------------------
int typeRecognitionLBP::classifyFeatureVectorLBP_SVM(){

    // zerowanie bufora na odpowiedzi
    memset(m_pfSVMvalues,0,sizeof(float)*m_iNoClasses);


    //debug
    //for (int j=0; j < m_iFeatureVectorSize; j++)
    //    std::cout << m_piFeatureVector[j] << std::endl;

    // Obliczanie wszystkich odpowiedzi
    for (int j=0; j < m_iFeatureVectorSize; j++) {
        for (int i=0; i < m_iNoClasses; i++) {
            m_pfSVMvalues[i] += m_pfSVMWeights[i][j]*m_piFeatureVector[j];
        }
    }
    // Dodanie bias
    for (int i=0; i < m_iNoClasses; i++)
        m_pfSVMvalues[i] += m_pfSVMB[i];


    int vehicleClass = -1;
    float bestSVM = 0;
    // Wyszukiwanie maksimum
    for (int i=0; i < m_iNoClasses; i++) {
        // debug
        //std::cout << m_pfSVMvalues[i] << std::endl;
        if ( m_pfSVMvalues[i] > bestSVM ) {

            bestSVM = m_pfSVMvalues[i];
            vehicleClass = i;
        }
    }

    return vehicleClass;
}

// ------------------------------------------------------------------------------------------------------------
// readSVMFromFile
// wczytanie parametrow SVM z pliku
// Struktura pliku:
// Liczba klas
// Nazwy klas
// Liczba cech SVM'a (tylko zgodnosciowo)
// Kolejne wagi SVM'a + bias
// ------------------------------------------------------------------------------------------------------------
void typeRecognitionLBP::readSVMFromFile() {


    std::ifstream inFile;
	inFile.open(SVM_FILE, std::ifstream::in);  // otwarcie pliku z opisem kolorw
	if ( !inFile.is_open()) {
        std:: cout <<"Unable to open: "<<SVM_FILE<<std::endl;
	}
	inFile >> m_iNoClasses;                    // wczytanie liczby kolorow


	// Wczytanie nazw klas
	for (int i=0; i < m_iNoClasses; i++) {
        std::string name;
        inFile >> name;
        m_classNames.push_back(name);
	}

    // Wczytanie liczby cech
    int noFeatures;
    inFile >> noFeatures;

    if ( noFeatures != m_iFeatureVectorSize) {
        std::cout <<"SVM inconsitent with used window parameters" << std::endl;
        return;
    }

    // Utworzenie tablicy na paramtery SVM'a
    m_pfSVMWeights = new float*[m_iNoClasses];
    m_pfSVMB = new float[m_iNoClasses];
    m_pfSVMvalues = new float[m_iNoClasses];

    // Wczytanie poszczegolnych SVM'ow
    for (int i=0; i < m_iNoClasses; i++) {
        m_pfSVMWeights[i] = new float[m_iFeatureVectorSize];

        for (int j=0; j < m_iFeatureVectorSize; j++) {
            inFile >> m_pfSVMWeights[i][j];
        }
        inFile >> m_pfSVMB[i];
    }


    std::cout <<"SVM read correctly" << std::endl;

}
