#ifndef TYPERECOGNITIONLBP_H
#define TYPERECOGNITIONLBP_H


#include "../globals.h"

#define SVM_FILE "svm_1050.txt"


// Klasa odpowiedziala za rozpoznawanie typu pojazdu
// Operuje na cechach LBP

class typeRecognitionLBP
{
    public:
        typeRecognitionLBP();                                                   //!< Konstuktor
        ~typeRecognitionLBP();                                                  //!< Destruktor

        std::string recognizeType(cv::Mat lbp);	                                //!< rozpoznawanie typu samochodu

    private:


        int m_iBlockSize;                       //!< rozmiar bloku (zaklada sie bloki kwadratowe)
        int m_iHistSize;                        //!< rozmiar histogramu - zalezy od uzytych cech LBP
        int m_iNoBlockX;                        //!< Liczba blokow w pionie
        int m_iNoBlockY;                        //!< Liczba blokow w poziomie

        int m_iFeatureVectorSize;           //!< Rozmiar wektora cech
        int *m_piFeatureVector;             //!< Tablica na wektor cech

        int m_iNoClasses;                    //!< Liczba klas
        float** m_pfSVMWeights;              //!< Tablica na wagi liniwoego SVM
        float*  m_pfSVMB;                    //!< Parametr B liniowego SVM
        float*  m_pfSVMvalues;               //!< Wartosci odpowiedzi SVM'a
        std::vector<std::string> m_classNames;      //!< Nazwy klas


        // TOOL
        void createFeatureVectorLBP(cv::Mat lbpImage);                         //!< tworzenie wektora cech LBP
        int classifyFeatureVectorLBP_SVM();                                    //!< sklasyfikowanie danego wektora cech
        void readSVMFromFile();                                                //!< wczytanie klasyfikatora SVM z pliku
};

#endif // TYPERECOGNITIONLBP_H
