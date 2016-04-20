#include "vdl.h"

#include <stdio.h>
#include <math.h>


// ------------------------------------------------------------------------------------------------------------
// constructor
// ------------------------------------------------------------------------------------------------------------
vdl::vdl() {

    m_iLineWidth = 0;
    m_lStart.x = 0; m_lStart.y = 0;
    m_lEnd.x =0;    m_lEnd.y =0;

    m_piVdl_points = NULL;
    m_piVdl_window = NULL;
    m_piVdl_window_prev = NULL;
    m_piShortWindow = NULL;

    m_pdD_sad= NULL;
	m_pdD_sx= NULL;
	m_pdD_sy= NULL;
	m_pdD_census= NULL;
    m_pb_road= NULL;
	m_pb_sy= NULL;
    m_pcSingleLineCharBuffer= NULL;


    m_vdl_buffer_iterator = VDL_MAX_VDL_HEIGHT - 1;      // iterator po buforze vdl
	m_medianBoolRoadIterator = 0;                        // iterator po buforze medianowym
	m_sumRoad = 0;                                       // okreslanie czy na ulicy
	m_b_road_median_prev = false;                        // poprzednia wartosc flagi road

	m_carPatchStart = 0;      // poczatek fragmentu samochodu
	m_carPatchEnd = 0;        // koniec fragmentu samochodu
	m_carPatchCounter = 0;    // liczba wykrytuy



#ifdef FILE_VDL
    m_dataFile=NULL;
#endif

    std::cout << "vdl()" << std::endl;
}


// ------------------------------------------------------------------------------------------------------------
// initVdl
// ------------------------------------------------------------------------------------------------------------
void vdl::initVdl(cv::Point lStart, cv::Point lEnd, int lineContext, std::string name) {

	// Przypisanie punktow poczatkowego i koncowego
	m_lStart = lStart;
	m_lStart.x = m_lStart.x - 1;
	m_lEnd = lEnd;
	m_lEnd.x = m_lEnd.x - 1;


	m_iLineWidth = m_lEnd.x - m_lStart.x + 1 + 2; // okreslenie szerokosci linii // dodane dwa punkty z lewej i prawej
	m_iLineHeight = 2 * lineContext + 1;          // Wysokosc okna detekcji



	// stworzenie kontenera na wspolrzednie punktow linii detekcji
    m_piVdl_points = new int *[m_iLineWidth];
	for (int i = 0; i < m_iLineWidth; i++)
		m_piVdl_points[i] = new int[2];

	// Przeliczenie linii detekcji
	computeLine();


	// Tworzenie buforow na okno obecne i poprzednie
	m_piVdl_window = new int **[m_iLineHeight];
	m_piVdl_window_prev = new int  **[m_iLineHeight];

	for (int i = 0; i < m_iLineHeight; i++) {
		m_piVdl_window[i] = new int*[m_iLineWidth];
		m_piVdl_window_prev[i] = new int*[m_iLineWidth];
		for (int j = 0; j < m_iLineWidth; j++) {
			m_piVdl_window[i][j] = new int[5];       // RGB + Grey + SOBEL binaryzacja
			m_piVdl_window_prev[i][j] = new int[5];
		}
	}

	// Tworzenie i zerowanie buforow na filtry medianowe dla sygnalow
	m_pdD_sad = new double[VDL_MEDIAN_FILTER_SIZE];
	memset(m_pdD_sad, 0.0, VDL_MEDIAN_FILTER_SIZE*sizeof(double));
	m_pdD_sx = new double[VDL_MEDIAN_FILTER_SIZE];
	memset(m_pdD_sx, 0.0, VDL_MEDIAN_FILTER_SIZE*sizeof(double));
	m_pdD_census = new double[VDL_MEDIAN_FILTER_SIZE];
	memset(m_pdD_census, 0.0, VDL_MEDIAN_FILTER_SIZE*sizeof(double));
	m_iMedianBufferIterator = 0;

	m_pdD_sy = new double[VDL_MEDIAN_FILTER_SIZE];
	memset(m_pdD_sy, 0.0, VDL_MEDIAN_FILTER_SIZE*sizeof(double));
	m_iMedianBufferIterator_sy = 0;

	// Filtracja boolean ...
	m_pb_road = new int[VDL_MEDIAN_BOOL_FILTER_SIZE];
	memset(m_pb_road, 0, VDL_MEDIAN_BOOL_FILTER_SIZE*sizeof(int));
	m_iMedianBoolBufferIterator = 0;

	m_pb_sy = new int[VDL_MEDIAN_BOOL_FILTER_SIZE];
	memset(m_pb_sy, 0, VDL_MEDIAN_BOOL_FILTER_SIZE*sizeof(int));
	m_iMedianBoolBufferIterator_sy = 0;

	// Licznik iteracji algorytmu
	m_iIteration = 0;

	// Obraz VDL - wizualizacja
#ifdef VISUALIZE_VDL
	m_vdl_vis = cv::Mat::zeros(VDL_IMAGE_LENGTH, m_iLineWidth + VDL_IMAGE_INFO, CV_8UC3);
	m_iVdl_vis_iterator = VDL_IMAGE_LENGTH - 1;
#endif

	// Krotkie okno do pionowych krawedzi
	//m_piShortWindow = new int *[SHORT_WINDOW_SIZE];
	//for (int i = 0; i < SHORT_WINDOW_SIZE; i++)
	//	m_piShortWindow[i] = new int[m_iLineWidth];


	m_moduleName = name;  // przypisanie nazwy okna

	// Inicjalizacja zmiennych
	m_iCarPatchStart = 0;
	m_iCarPatchEnd = 0;
	m_iCarPatchCounter = 0;
	m_iCarCounter = 0;

	m_b_sad = false;
	m_b_sx = false;
	m_b_sy = false;
	m_b_census = false;
	m_b_road = false;
	m_b_road_prev = false;

#ifdef FILE_VDL
	// Inicjalizacja pliku
	m_dataFile = fopen("data.out", "wb");
#endif

	// Bufor do przekazywania pojedycznej linii
	m_pcSingleLineCharBuffer = new unsigned char[VDL_MAX_VDL_WIDTH*5];

    std::cout << "vdl::initVdl" << std::endl;

}

// ------------------------------------------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------------------------------------------
vdl::~vdl() {

	//std::cout <<"m_iLineWidth " << m_iLineWidth << std::endl;

	// Usuniecie kontenera
	if (m_piVdl_points ) {
        for (int i = 0; i < m_iLineWidth; i++) {
            if (m_piVdl_points[i])
                delete [] m_piVdl_points[i] ;
            // debug
            //std::cout << i << std::endl;
        }
        delete [] m_piVdl_points;
    }

	// Bufory na biezacy i poprzedni kontekst
	if (m_piVdl_window && m_piVdl_window_prev) {
        for (int i = 0; i < m_iLineHeight; i++) {
            for (int j = 0; j < m_iLineWidth; j++) {
                delete[] m_piVdl_window[i][j];
                delete[] m_piVdl_window_prev[i][j];
            }
            delete [] m_piVdl_window[i];
            delete [] m_piVdl_window_prev[i];
        }
        delete [] m_piVdl_window;
        delete [] m_piVdl_window_prev;

    }
	// Usuniecie krotkiego okna
	//if ( m_piShortWindow) {
    //    for (int i=0; i < SHORT_WINDOW_SIZE; i++)
    //        delete [] m_piShortWindow[i];
    //    delete [] m_piShortWindow;
    //}

	if (m_pdD_sad) delete [] m_pdD_sad;
	if (m_pdD_sx) delete [] m_pdD_sx;
	if (m_pdD_sy) delete [] m_pdD_sy;

	if (m_pdD_census) delete [] m_pdD_census;

	if (m_pb_road) delete [] m_pb_road;
	if (m_pb_sy) delete [] m_pb_sy;

	if (m_pcSingleLineCharBuffer) delete[] m_pcSingleLineCharBuffer;

#ifdef FILE_VDL
	if (m_dataFile) fclose(m_dataFile);
#endif
	std::cout << "~vdl()" << std::endl;
}



// ------------------------------------------------------------------------------------------------------------
// setThresholds - ustawienie progów
// ------------------------------------------------------------------------------------------------------------
void vdl::setThresholds(double sad_th, double sx_th, double census_th, double sy_th) {

    // UWaga tu jest to na sztywno przypisane
	m_d_sad_th    = VDL_SAD_TH*m_iLineHeight*m_iLineWidth;
	m_d_sx_th     = VDL_SX_TH*m_iLineHeight*m_iLineWidth;
	m_d_census_th = VDL_CENSUS_TH*m_iLineHeight*m_iLineWidth;
}

// ------------------------------------------------------------------------------------------------------------
// drawVDL - wyrysowanie linii detekcji na obrazie
// + wypiszanie zliczenia
// ------------------------------------------------------------------------------------------------------------
void vdl::drawVDL(cv::Mat &image){

	for (int i=0; i<m_iLineWidth; i++) {
		vec_uchar_3 v = image.at<vec_uchar_3>(m_piVdl_points[i][1],m_piVdl_points[i][0]);
		v[2] = 255; // na czerwnono
		image.at<vec_uchar_3>(m_piVdl_points[i][1],m_piVdl_points[i][0]) = v;
	}

	char num[100];
	sprintf(num,"%d",m_iCarCounter);
	cv::putText(image,num,m_lEnd, cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,255,0) );
}

// ------------------------------------------------------------------------------------------------------------
// step - iteracja algorytmu
// ------------------------------------------------------------------------------------------------------------
int vdl::step(const cv::Mat & image_grey,const  cv::Mat & image_rgb,const  cv::Mat & image_rgb_gauss, bool & bCarStart) {

	//printf(" ================= NEW FRAME NO %d ============================== \n",m_iIteration);

	// Przesuniecie krotkiego okna
	/*
	for (int j=0; j < SHORT_WINDOW_SIZE-1; j++)
		for (int i=0; i < m_iLineWidth; i++)
			m_piShortWindow[j][i] = m_piShortWindow[j+1][i];
    */


	// 1. Pobranie otoczenia
	std::cout << m_iLineHeight << " " << m_iLineWidth << std::endl;
	for (int j=0; j < m_iLineHeight; j++) {
		for (int i=0; i < m_iLineWidth; i++){
			//std::cout << j <<" " << i << std::endl;
			int jj = m_piVdl_points[i][1]- floor(double(m_iLineHeight)/2)+j;
			int ii = m_piVdl_points[i][0];
			//std::cout << jj <<" " << ii << std::endl;

			vec_uchar_3 v;
			v= image_rgb_gauss.at<vec_uchar_3>(jj, ii);     // uwaga tu jest przekazywana wersja po filtacji !!!
			vec_uchar_1 v1;
			v1= image_grey.at<vec_uchar_1>(jj, ii);


			m_piVdl_window[j][i][0] = v[0];	                        // zapis do bufora R
			m_piVdl_window[j][i][1] = v[1];	                        // zapis do bufora G
			m_piVdl_window[j][i][2] = v[2];	                        // zapis do bufora B
			m_piVdl_window[j][i][3] = v1[0];                        // zapis do bufora grey
			m_piVdl_window[j][i][4] = 0;                            // zapis do sobel


			//m_piShortWindow[SHORT_WINDOW_SIZE-1][i] = v[0];       // zapis do krotkiego kontekstu
			//printf("%d ",m_piVdl_window[j][i] );
		}
		//printf("\n");
	}
	//printf("O");
	// Porówananie obszarów

	// 2. Kolejne operacje
	if ( m_iIteration > 0 ) {
		comparePatches();               // porownanie dwoch fragmentow obrazkow
		//printf("C");
		medianFilter1D();
		//printf("M");
		//analysis();                   // ta analiza jest uruchomiona w "sofcie"
		//printf("A");
		//edgesInShortWindow();

		// Detekcja samochodow
		// [!!!] To jest do ew. poprawy
		// Detekcja tez dziala w "sofcie"
/*
		if ( m_b_road_prev  && !m_b_road) { // Poczatek samochodu
			m_iCarPatchStart = m_iVdl_vis_iterator;
			m_iCarPatchCounter++;
		}

		if ( !m_b_road_prev  && m_b_road && m_iCarPatchCounter >0 ) { // Poczatek drogi
			m_iCarPatchEnd = m_iVdl_vis_iterator;
			//m_iCarCounter += analyseCarPatch();
			// temp solution
			m_iCarCounter++;
			printf("AUT: %d \n",m_iCarCounter);
			// [!!!]
			//cv::waitKey(0);

			//printf("DROGA\n");
		}
		// debug
		printf("D %d %d\n",m_b_road_prev,m_b_road);
*/
		//printf("%f %f %f %f\n",m_d_sx, m_d_sad,m_d_census,m_d_sy);
	}

	// ###############################################################################################
	// Zapis linii detekcji do pliku (dla potrzeb aplikacji Zynq)
	// ###############################################################################################

	int bufferCnt = 0;
	for (int i = 1; i < m_iLineWidth-1; i++){  // tylko to co jest de-facto na linii
		// Progi ...
		unsigned char pp0 = (m_piVdl_window[2][i][3] > VDL_NIGHT_TH )* 255;      // progowany obraz szary - obszary b. jasne do analizy w nocy
		unsigned char pp1 = (m_piVdl_window[2][i][3] < VDL_SHADOW_TH) * 255;     // progowany obraz szary - obszary ciemne do analizy w nocy
		unsigned char pp2 = (m_piVdl_window[2][i][4]) * 255;

		//if (pp1 > 0)
		//	int kkk = 0;
		unsigned char xxx = 0;
		xxx = (((unsigned char)(1) << 0) & pp0) | (((unsigned char)(1) << 1) & pp1) | (((unsigned char)(1) << 2) & pp2);

		m_pcSingleLineCharBuffer[bufferCnt] =     (unsigned char)(xxx);
		m_pcSingleLineCharBuffer[bufferCnt + 1] = (unsigned char)(m_piVdl_window[2][i][2]); // R
		m_pcSingleLineCharBuffer[bufferCnt + 2] = (unsigned char)(m_piVdl_window[2][i][1]); // G
		m_pcSingleLineCharBuffer[bufferCnt + 3] = (unsigned char)(m_piVdl_window[2][i][0]); // B

		//fprintf(m_dataFile, "%c", (unsigned char)(xxx));                     // other data ([00000,EDGE,SHADOW,NIGHT])
		//fprintf(m_dataFile, "%c", (unsigned char)(m_piVdl_window[2][i][2])); // red
		//fprintf(m_dataFile, "%c", (unsigned char)(m_piVdl_window[2][i][1])); // green
		//fprintf(m_dataFile, "%c", (unsigned char)(m_piVdl_window[2][i][0])); // blue

		bufferCnt += 4;
	}
	// Zapis median SAD, SX, Census
	//m_d_sad
	unsigned int i_sad    = (unsigned int)(m_d_sad);
	unsigned int i_sx     = (unsigned int)(m_d_sx);
	unsigned int i_census = (unsigned int)(m_d_census);
	unsigned char b0, b1, b2, b3;

	b0 = (((unsigned int)(255) << 0) & i_sad);
	b1 = (((unsigned int)(255) << 8) & i_sad)>>8;
	b2 = (((unsigned int)(255) << 16) & i_sad)>>16;
	b3 = (((unsigned int)(255) << 24) & i_sad)>>24;
	//fprintf(m_dataFile, "%c%x%c%c",b3, b2, b1, b0);
	//printf("%d %d %d %d\n", b3, b2, b1, b0);

	m_pcSingleLineCharBuffer[bufferCnt]   = b3;
	m_pcSingleLineCharBuffer[bufferCnt+1] = b2;
	m_pcSingleLineCharBuffer[bufferCnt+2] = b1;
	m_pcSingleLineCharBuffer[bufferCnt+3] = b0;
	bufferCnt += 4;


	b0 = (((unsigned int)(255) << 0) & i_sx);
	b1 = (((unsigned int)(255) << 8) & i_sx) >> 8;
	b2 = (((unsigned int)(255) << 16) & i_sx) >> 16;
	b3 = (((unsigned int)(255) << 24) & i_sx) >> 24;
	//fprintf(m_dataFile, "%c%x%c%c", b3, b2, b1, b0);

	m_pcSingleLineCharBuffer[bufferCnt] = b3;
	m_pcSingleLineCharBuffer[bufferCnt + 1] = b2;
	m_pcSingleLineCharBuffer[bufferCnt + 2] = b1;
	m_pcSingleLineCharBuffer[bufferCnt + 3] = b0;
	bufferCnt += 4;

	b0 = (((unsigned int)(255) << 0) & i_census);
	b1 = (((unsigned int)(255) << 8) & i_census) >> 8;
	b2 = (((unsigned int)(255) << 16) & i_census) >> 16;
	b3 = (((unsigned int)(255) << 24) & i_census) >> 24;
	//fprintf(m_dataFile, "%c%x%c%c", b3, b2, b1, b0);
	m_pcSingleLineCharBuffer[bufferCnt] = b3;
	m_pcSingleLineCharBuffer[bufferCnt + 1] = b2;
	m_pcSingleLineCharBuffer[bufferCnt + 2] = b1;
	m_pcSingleLineCharBuffer[bufferCnt + 3] = b0;
	bufferCnt += 4;

	//fprintf(m_dataFile, "\n");

	//for (int z = 0; z < MAX_VDL_WIDTH; z++)
	//	printf("%c", m_pcSingleLineCharBuffer[z]);
	//printf("\n");

	// ###############################################################################################
	// Uruchomienie analizy
	// ###############################################################################################
	// Test obrazka
	//cv::Mat patch_image(100, 100, CV_8UC3);
	//cv::Vec<uchar, 3> v;

	unsigned char I[VDL_MAX_VDL_HEIGHT * VDL_MAX_VDL_WIDTH * 3];

	m_iCarCounter += analyseCarPatch_zynq(m_pcSingleLineCharBuffer, I);
	/*
	if (zzz > 0) {
		for (int ii = 0; ii < 100; ii++)
			for (int jj = 0; jj < 100; jj++) {
				//for (int kk = 0; kk < 3; kk++)
				//	printf("%d", I[100 * ii + 3 * jj + kk]);
				v[0] = I[3*100 * ii + 3 * jj + 0];
				v[1] = I[3*100 * ii + 3 * jj + 1];
				v[2] = I[3*100 * ii + 3 * jj + 2];
				patch_image.at<VT_3>(ii, jj) = v;

			}
		cv::imshow("II", patch_image);
		cv::waitKey(0);
	}
	*/




	// Wizualizacja obrazu VDL
	// Odejmowanie - na razie nie uzywane
	// [!!!] Tu jeszcze byla opcjonalna mediana - ale juz moze nie....
	// ----------------------------------------------------------------------------------------------

//	for (int i=0; i < m_iLineWidth; i++){
//		cv::Vec<uchar, 3> v;
//		v = image_rgb.at<VT_3>(m_piVdl_points[i][1],m_piVdl_points[i][0]);
//		m_vdl_vis.at<VT_3>(m_iVdl_vis_iterator,i) = v;
		// Aktualiza tylko w przypadku gdy jestesmy na asflacie tj
		/*
		if ( m_b_road && !m_b_sy ) {
			if ( m_iIteration > 0 ) {
				m_piSimpleBgModel[i][0] = m_piSimpleBgModel[i][0]*0.95 + 0.05*v[0];
				m_piSimpleBgModel[i][1] = m_piSimpleBgModel[i][1]*0.95 + 0.05*v[1];
				m_piSimpleBgModel[i][2] = m_piSimpleBgModel[i][2]*0.95 + 0.05*v[2];
			} else {
				m_piSimpleBgModel[i][0] = v[0];
				m_piSimpleBgModel[i][1] = v[1];
				m_piSimpleBgModel[i][2] = v[2];
			}
		}
		// Odejmowanie tylko gdy nie droga i krawedzie pionowe
		if ( !m_b_road && m_b_sy ) {
			double d = abs(m_piSimpleBgModel[i][0] - double(v[0])) +  abs(m_piSimpleBgModel[i][1] - double(v[1])) + abs(m_piSimpleBgModel[i][2] - double(v[2]));
			// [!!!] Staly prog na razie
			if ( d > 60 )
				m_vdl_vis_binary.at<VT_1>(m_iVdl_vis_iterator,i) = 255;
			else
				m_vdl_vis_binary.at<VT_1>(m_iVdl_vis_iterator,i) = 0;
		}
		*/
/*
	}

	//printf("V");

	cv::Vec<uchar, 3> vg(255,0,0);
	cv::Vec<uchar, 3> vb(0,255,0);
	cv::Vec<uchar, 3> vr(0,0,255);


	// Tu jest dodawnie kolorkow z prawej strony obazu

	for (int i=m_iLineWidth; i < m_iLineWidth+5; i++)
		m_vdl_vis.at<VT_3>(m_iVdl_vis_iterator,i) = m_b_sad ? vb : vg;
	for (int i=m_iLineWidth+5; i < m_iLineWidth+10; i++)
		m_vdl_vis.at<VT_3>(m_iVdl_vis_iterator,i) = m_b_sx ? vb : vg;
	for (int i=m_iLineWidth+10; i < m_iLineWidth+15; i++)
		m_vdl_vis.at<VT_3>(m_iVdl_vis_iterator,i) = m_b_census ? vb : vg;
	for (int i=m_iLineWidth+15; i < m_iLineWidth+20; i++)
		m_vdl_vis.at<VT_3>(m_iVdl_vis_iterator,i) = m_b_road ? vr : vg;


	for (int i = 0; i < m_iLineWidth; i++) {
		if (m_b_road) {
			cv::Vec<uchar, 3> v;
			v = m_vdl_vis.at<VT_3>(m_iVdl_vis_iterator, i);
			v[2] = 0.5*v[2] + 127;
			m_vdl_vis.at<VT_3>(m_iVdl_vis_iterator, i) = v;
		}
	}

	m_iVdl_vis_iterator--;
	if ( m_iVdl_vis_iterator < 0 )
		m_iVdl_vis_iterator = VDL_IMAGE_LENGTH-1;

	cv::imshow(m_moduleName,m_vdl_vis);
	//printf("V");

    // Kopiowanie jako poprzednia iteracja
	// [!!!] Optymalizcja ???
*/
    for (int j=0; j < m_iLineHeight; j++)
		for (int i=0; i < m_iLineWidth; i++)
			for (int k = 0; k < 5; k++) // RGB + grey + Sobel bin
			m_piVdl_window_prev[j][i][k] = m_piVdl_window[j][i][k];

	// Wykrywanie poczatku samochodu
	bCarStart = m_b_road_prev  && !m_b_road;

	// Przypisania koncowe
	m_b_road_prev = m_b_road;

	m_iIteration++;
	//printf("F\n");


	return m_iCarCounter;
}


// ------------------------------------------------------------------------------------------------------------
// Compute line
// ------------------------------------------------------------------------------------------------------------
void vdl::computeLine(){

	// TODO - poprawa ew. na pionowe proste

	double a = (m_lEnd.y - m_lStart.y)/(m_lEnd.x - m_lStart.x);
	double b =  m_lStart.y - a*m_lStart.x;

	// Przypisanie wspolrzednych ...
	for (int i=0; i<m_iLineWidth; i++) {
		m_piVdl_points[i][0] = i+m_lStart.x;
		m_piVdl_points[i][1] = floor(a*i+b);
	}

}

// ------------------------------------------------------------------------------------------------------------
// compare pathes
// ------------------------------------------------------------------------------------------------------------
void vdl::comparePatches() {

	double diff_sx=0;
	double diff_sad=0;
	double diff_census=0;

	// Operacja kontekstowa 3x3 na linii
	for (int j = 0; j < m_iLineHeight; j++) {
		for (int i = 0; i < m_iLineWidth; i++){
			if (j > 0 && j < m_iLineHeight - 1 && i > 0 && i < m_iLineWidth - 1) { // context 3x3

				int sobel_bin_cnt = 0;

				// dla poszczegolnych skladowych
				for (int k = 0; k < 3; k++) {

					diff_sad += abs((m_piVdl_window[j][i][k]) - (m_piVdl_window_prev[j][i][k]));

					// Sobel horizontal
					int sx_a = 1 * m_piVdl_window[j - 1][i - 1][k] + 2 * m_piVdl_window[j - 1][i][k] + 1 * m_piVdl_window[j - 1][i + 1][k] -
						1 * m_piVdl_window[j + 1][i - 1][k] - 2 * m_piVdl_window[j + 1][i][k] - 1 * m_piVdl_window[j + 1][i + 1][k];

					int sx_b = 1 * m_piVdl_window_prev[j - 1][i - 1][k] + 2 * m_piVdl_window_prev[j - 1][i][k] + 1 * m_piVdl_window_prev[j - 1][i + 1][k] -
						1 * m_piVdl_window_prev[j + 1][i - 1][k] - 2 * m_piVdl_window_prev[j + 1][i][k] - 1 * m_piVdl_window_prev[j + 1][i + 1][k];
					diff_sx += abs(sx_a - sx_b);

					if (abs(sx_a) > VDL_EDGE_TH)
						sobel_bin_cnt++;

				}

                // Census
				int th_a = m_piVdl_window[j][i][3] + VDL_LBP_BIAS;      // + fixed bias !!!
				int th_b = m_piVdl_window_prev[j][i][3] + VDL_LBP_BIAS; // + fixed bias !!!

				double census_local = 0;
				for (int jj = -1; jj <= 1; jj++)
					for (int ii = -1; ii <= 1; ii++) {
						census_local += int((m_piVdl_window[j + jj][i + ii][3] > th_a) ^ (m_piVdl_window_prev[j + jj][i + ii][3] > th_b));
					}
				diff_census += census_local;

				if (sobel_bin_cnt > 0)
					m_piVdl_window[j][i][4] = 1;
				else
					m_piVdl_window[j][i][4] = 0;
				//printf("%d", m_piVdl_window[j][i][4]);
			}
		}
		//printf("\n");
	}
    // TODO - do odkomentowania

    // normalizacja wzgledem rozmiaru okna
	// +++ wylaczona, bo dla FPGA nie ma ulamkow
	//diff_sx /= (m_iLineHeight*m_iLineWidth);
	//diff_sad /= (m_iLineHeight*m_iLineWidth);
	//diff_census /= (m_iLineHeight*m_iLineWidth);

    // wsadzenie na bufor do filtracji medianowej
	m_pdD_sx[m_iMedianBufferIterator] = diff_sx;
	m_pdD_sad[m_iMedianBufferIterator] = diff_sad;
	m_pdD_census[m_iMedianBufferIterator] = diff_census;

	m_iMedianBufferIterator++;
	if ( m_iMedianBufferIterator == VDL_MEDIAN_FILTER_SIZE)
		m_iMedianBufferIterator = 0;

	//printf("\n");
	//printf("\n%f %f %f \n",diff_sad, diff_sx,diff_census);
}


// ------------------------------------------------------------------------------------------------------------
// compareQSort - potrzebne do Q-Sorta
// ------------------------------------------------------------------------------------------------------------
int compareQSort(const void * a, const void * b) {
  return ( *(double*)a - *(double*)b );
}

// ------------------------------------------------------------------------------------------------------------
// medianFilter1D
// obliczanie mediany poprzez zastosowanie sortowania QSORT
// ------------------------------------------------------------------------------------------------------------
void vdl::medianFilter1D(){
	double tempB[VDL_MEDIAN_FILTER_SIZE];
	// SX
	memcpy(tempB,m_pdD_sx,VDL_MEDIAN_FILTER_SIZE*sizeof(double));
	qsort (tempB, VDL_MEDIAN_FILTER_SIZE, sizeof(double), compareQSort);
	m_d_sx = tempB[VDL_MEDIAN_FILTER_SIZE/2];
	// SAD
	memcpy(tempB,m_pdD_sad,VDL_MEDIAN_FILTER_SIZE*sizeof(double));
	qsort (tempB, VDL_MEDIAN_FILTER_SIZE, sizeof(double), compareQSort);
	m_d_sad = tempB[VDL_MEDIAN_FILTER_SIZE/2];
	// CENSUS
	memcpy(tempB,m_pdD_census,VDL_MEDIAN_FILTER_SIZE*sizeof(double));
	qsort (tempB, VDL_MEDIAN_FILTER_SIZE, sizeof(double), compareQSort);
	m_d_census = tempB[VDL_MEDIAN_FILTER_SIZE/2];


}

// ------------------------------------------------------------------------------------------------------------
// analiysis
// ------------------------------------------------------------------------------------------------------------
void vdl::analysis() {


    // Progowanie trzech wskaznikow
	m_b_sad = m_d_sad > m_d_sad_th ? true : false;
	m_b_sad = !m_b_sad;
	m_b_sx = m_d_sx > m_d_sx_th ? true : false;
	m_b_sx = !m_b_sx;
	m_b_census = m_d_census > m_d_census_th ? true : false;
	m_b_census = !m_b_census;


    // Filtracja medianowa
	m_pb_road[m_iMedianBoolBufferIterator] = m_b_sad && m_b_sx && m_b_census;
	m_iMedianBoolBufferIterator++;

	if ( m_iMedianBoolBufferIterator == VDL_MEDIAN_BOOL_FILTER_SIZE)
		m_iMedianBoolBufferIterator = 0;

	// Boolean median filtering ...
	int sum_road = 0;
	for (int i=0; i< VDL_MEDIAN_BOOL_FILTER_SIZE; i++)
		sum_road += int(m_pb_road[i]);

	if ( sum_road > VDL_MEDIAN_BOOL_FILTER_SIZE/2 )
		m_b_road = true;
	else
		m_b_road = false;

    // debug
    //std::cout << m_moduleName;
	//printf(": %f | %f || %f| %f || %f | %f  \n",m_d_sad, m_d_sad_th,m_d_sx, m_d_sx_th, m_d_census,m_d_census_th); // after median
	//printf(" detekcja: %d %d \n", m_b_sad && m_b_sx && m_b_census,m_b_road);
	//cv::waitKey(0);
}

// edgesInShortWindow
// ------------------------------------------------------------------------------------------------------------
void vdl::edgesInShortWindow(){
/*
	int sumEdge = 0;
	for (int j=1; j < SHORT_WINDOW_SIZE-1; j++)
		for (int i=1; i < m_iLineWidth-1; i++){
			// oblicz krawedzie pionowe
			int sy = 1*m_piShortWindow[j-1][i-1] - 1*m_piShortWindow[j-1][i+1]
			        +2*m_piShortWindow[j][i-1] -   2*m_piShortWindow[j][i+1]
					+1*m_piShortWindow[j+1][i-1] -   1*m_piShortWindow[j+1][i+1];
			sumEdge += abs(sy);
		}

	m_pdD_sy[m_iMedianBufferIterator_sy++] = double(sumEdge)/(SHORT_WINDOW_SIZE*m_iLineWidth);

	if ( m_iMedianBufferIterator_sy == MEDIAN_FILTER_SIZE)
		m_iMedianBufferIterator_sy = 0;

	// Median filtering of the input values
	double tempB[MEDIAN_FILTER_SIZE];
	memcpy(tempB,m_pdD_sy,MEDIAN_FILTER_SIZE*sizeof(double));
	qsort (tempB, MEDIAN_FILTER_SIZE, sizeof(double), compareQSort);
	m_d_sy= tempB[MEDIAN_FILTER_SIZE/2];


	if ( m_d_sy > m_d_sy_th)
		m_pb_sy[m_iMedianBoolBufferIterator_sy++] = 1;
	else
		m_pb_sy[m_iMedianBoolBufferIterator_sy++] = 0;

	if ( m_iMedianBoolBufferIterator_sy == MEDIAN_BOOL_FILTER_SIZE)
		m_iMedianBoolBufferIterator_sy = 0;


	// Boolean median filtering ...
	int sum_sy = 0;
	for (int i=0; i< MEDIAN_BOOL_FILTER_SIZE; i++)
		sum_sy += int(m_pb_sy[i]);

	if ( sum_sy > MEDIAN_BOOL_FILTER_SIZE/2 )
		m_b_sy = true;
	else
		m_b_sy = false;
*/
	//printf("%f %d\n",m_d_sy,m_b_sy);
}

// analyseCarPatch
// ------------------------------------------------------------------------------------------------------------
int vdl::analyseCarPatch() {

/*
	// Okreœlenie rozmiarow - tj. dlugosci okna
	int height;

	// Zwiekszenie patch'a (ew. tryb nocny tylko ??) (zeby bylo wiecej autka)
	m_iCarPatchStart +=10;

	if ( m_iCarPatchStart >= VDL_IMAGE_LENGTH)
		m_iCarPatchStart = m_iCarPatchStart - VDL_IMAGE_LENGTH;

	m_iCarPatchEnd -=0;

 	if ( m_iCarPatchStart >= m_iCarPatchEnd )
		height = m_iCarPatchStart-m_iCarPatchEnd;
	else // Odwrocny
		height = m_iCarPatchStart + VDL_IMAGE_LENGTH-m_iCarPatchEnd; // chyba .....

	if ( height < VDL_MINIMAL_PATCH_HEIGHT_TH )
		return 0;

	// Storzenie obrazka na patch
	cv::Mat patch_image(height,m_iLineWidth,CV_8UC3);


	// Wypelnienie patcha
	// Przypadek (normalny)
	int cars=0; // licznik samochodow
	printf("S=%dE=%d",m_iCarPatchStart,m_iCarPatchEnd);
	if ( m_iCarPatchStart >= m_iCarPatchEnd ) {

		int iPatch = height-1;
		for (int j=m_iCarPatchStart; j > m_iCarPatchEnd; j--) {
			for (int i=0; i < m_iLineWidth; i++){
				cv::Vec<uchar, 3> v = m_vdl_vis.at<VT_3>(j,i);
				patch_image.at<VT_3>(iPatch,i) = v;
			}
			iPatch--;
		}

	} else {
		// To tu jest do sprawdzenia jak dziala !!!
		int iPatch = height-1;
		// Gorna czesc
		for (int j=m_iCarPatchStart; j >= 0; j--) {
			for (int i=0; i < m_iLineWidth; i++){
				cv::Vec<uchar, 3> v = m_vdl_vis.at<VT_3>(j,i);
				patch_image.at<VT_3>(iPatch,i) = v;
			}
			iPatch--;
		}
		// Dolna czesc
		for (int j=VDL_IMAGE_LENGTH-1; j > m_iCarPatchEnd; j--) {
			for (int i=0; i < m_iLineWidth; i++){
				cv::Vec<uchar, 3> v = m_vdl_vis.at<VT_3>(j,i);
				patch_image.at<VT_3>(iPatch,i) = v;
			}
			iPatch--;
		}
	}


	// ANALIZA JEST WYLACZONA

	//if (NIGHTTIME)
	//	cars = analyseCarPatch_night(patch_image,height);
	//else
		//cars = analyseCarPatch_day(patch_image,height);

	//cars = analyseCarPatch2(patch_image,height);
	//cv::imshow("PATCH",patch_image);


	// Testowy zapis do pliku
	char buffer[100];
	sprintf(buffer,"p%05d.png",m_iCarPatchCounter);
	cv::imwrite(buffer,patch_image);
    return cars;
*///
	return 0;
}

int vdl::analyseCarPatch_zynq(unsigned char *buffer, unsigned char *imageBuffer){


	// Stale
	const int  vdl_line_length = m_iLineWidth - 2;                                 // [!!!] to tu trzeba ustawic recznie w Zynq

	// Zmienne
	int carPatchHeight = 0;
	int carCounter = 0;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Odczytanie z bufora wartosci PIKSELA i MASEK
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	int j = 0;
	for (int i = 0; i < 4 * vdl_line_length; i += 4){
		m_vdl_buffer[m_vdl_buffer_iterator][j][3] = buffer[i];   // maski
		m_vdl_buffer[m_vdl_buffer_iterator][j][0] = buffer[i + 1]; // R
		m_vdl_buffer[m_vdl_buffer_iterator][j][1] = buffer[i + 2]; // G
		m_vdl_buffer[m_vdl_buffer_iterator][j][2] = buffer[i + 3]; // B
		j++;
	}
	m_vdl_buffer_iterator--;

	if (m_vdl_buffer_iterator < 0)
		m_vdl_buffer_iterator = VDL_MAX_VDL_HEIGHT - 1;


	// Odczytanie z bufora wartoœci SAD, SX, CENSUS
	unsigned int b0, b1, b2, b3;
	unsigned int sad_v = 0;
	unsigned int sx_v = 0;
	unsigned int census_v = 0;

	// TODO - do zmiany kolejnosc przed ARM !!!
	// Rekontrukcja wartosci SAD, SX, CENSUS
	b3 = buffer[4 * vdl_line_length];
	b2 = buffer[4 * vdl_line_length + 1];
	b1 = buffer[4 * vdl_line_length + 2];
	b0 = buffer[4 * vdl_line_length + 3];
	sad_v = (b3 << 24) | (b2 << 16) | (b1 << 8) | (b0);

	b3 = buffer[4 * vdl_line_length + 4];
	b2 = buffer[4 * vdl_line_length + 5];
	b1 = buffer[4 * vdl_line_length + 6];
	b0 = buffer[4 * vdl_line_length + 7];
	sx_v = (b3 << 24) | (b2 << 16) | (b1 << 8) | (b0);

	b3 = buffer[4 * vdl_line_length + 8];
	b2 = buffer[4 * vdl_line_length + 9];
	b1 = buffer[4 * vdl_line_length + 10];
	b0 = buffer[4 * vdl_line_length + 11];
	census_v = (b3 << 24) | (b2 << 16) | (b1 << 8) | (b0);



	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Progowanie SAD, SX, CENSUS, filtracja medianowa
	bool b_sad = sad_v    > int(m_d_sad_th) ? false : true;
	bool b_sx = sx_v     > int(m_d_sx_th) ? false : true;
	bool b_census = census_v > int(m_d_census_th) ? false : true;

	bool b_road = b_sad && b_sx && b_census;
	bool b_road_median = false;

	//debug
	//printf("%d %d | %d %d | %d %d \n", sad_v, int(m_d_sad_th), sx_v, int(m_d_sx_th), census_v, int(m_d_census_th));
	//cv::waitKey(0);


	m_sumRoad += (int)(b_road)-(int)(m_medianBoolRoad[m_medianBoolRoadIterator]);

	m_medianBoolRoad[m_medianBoolRoadIterator] = b_road;
	m_medianBoolRoadIterator++;

	if (m_medianBoolRoadIterator == VDL_MEDIAN_BOOL_FILTER_SIZE)
		m_medianBoolRoadIterator = 0;

	// Filtracja medianowa
	if (m_sumRoad > VDL_MEDIAN_BOOL_FILTER_SIZE / 2)
		b_road_median = true;
	else
		b_road_median = false;
	//debug
	//printf("%d %d %d %d %d \n", b_sad, b_sx, b_census, b_road, b_road_median);
	//printf("ROAD: %d\n", b_road_median);

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Okreslanie poczatka i konca samochodu ...

	if (m_b_road_median_prev  && !b_road_median) { // Poczatek samochodu
		m_carPatchStart = m_vdl_buffer_iterator;
		m_carPatchStart += 20;
		if (m_carPatchStart >= VDL_MAX_VDL_HEIGHT)
			m_carPatchStart = m_carPatchStart - VDL_MAX_VDL_HEIGHT; // to trzeba jeszcze sprawdzic
	}

	if (!m_b_road_median_prev  && b_road_median) { // Koniec samochodu, poczatek drogi
		m_carPatchEnd = m_vdl_buffer_iterator;
		m_carPatchCounter++;
		printf("Car start:%d end %d\n", m_carPatchStart, m_carPatchEnd);


		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Okreslanie wysokosci autka
		if (m_carPatchStart >= m_carPatchEnd)
			carPatchHeight = m_carPatchStart - m_carPatchEnd;
		else // Odwrocny
			carPatchHeight = m_carPatchStart + VDL_MAX_VDL_HEIGHT - m_carPatchEnd; // chyba .....

		if (carPatchHeight < VDL_MINIMAL_PATCH_HEIGHT_TH)
			return 0;

		// Stowrzenie obrazka OpenCV -> tylko debug
		//cv::Mat patch_image(carPatchHeight, vdl_line_length, CV_8UC3);
		//cv::Mat night_image(carPatchHeight, vdl_line_length, CV_8UC1);
		//cv::Vec<uchar, 3> v;

		// Stworzenie patcha na podstawie buforu
		if (m_carPatchStart >= m_carPatchEnd) {

			int iPatch = carPatchHeight - 1;
			for (int j = m_carPatchStart; j > m_carPatchEnd; j--) {
				for (int i = 0; i < vdl_line_length; i += 1){

					//v[0] = vdl_buffer[j][i][2];
					//v[1] = vdl_buffer[j][i][1];
					//v[2] = vdl_buffer[j][i][0];

					// new
					//imageThNightTime[iPatch][i] = vdl_buffer[j][i][3] & (((unsigned char)(1) << 0));
					//night_image.at<vec_uchar_1>(iPatch, i) = 255 * imageThNightTime[iPatch][i];


					m_imageRGBBuffer[iPatch][i][3] = m_vdl_buffer[j][i][3];
					m_imageRGBBuffer[iPatch][i][0] = m_vdl_buffer[j][i][2];
					m_imageRGBBuffer[iPatch][i][1] = m_vdl_buffer[j][i][1];
					m_imageRGBBuffer[iPatch][i][2] = m_vdl_buffer[j][i][0];

					//patch_image.at<VT_3>(iPatch, i) = v;
					//printf("%d", v[0]);
				}
				iPatch--;
				//printf("\n");
			}
		}
		else {
			// To tu jest do sprawdzenia jak dziala !!!
			int iPatch = carPatchHeight - 1;
			// Gorna czesc
			for (int j = m_carPatchStart; j >= 0; j--) {
				for (int i = 0; i < vdl_line_length; i += 1){
					//v[0] = vdl_buffer[j][i][2];
					//v[1] = vdl_buffer[j][i][1];
					//v[2] = vdl_buffer[j][i][0];

					m_imageRGBBuffer[iPatch][i][3] = m_vdl_buffer[j][i][3];
					m_imageRGBBuffer[iPatch][i][0] = m_vdl_buffer[j][i][2];
					m_imageRGBBuffer[iPatch][i][1] = m_vdl_buffer[j][i][1];
					m_imageRGBBuffer[iPatch][i][2] = m_vdl_buffer[j][i][0];

					// new
					//imageThNightTime[iPatch][i] = vdl_buffer[j][i][3] & (((unsigned char)(1) << 0));
					//night_image.at<vec_uchar_1>(iPatch, i) = 255 * imageThNightTime[iPatch][i];

					//patch_image.at<vec_uchar_3>(iPatch, i) = v;
				}
				iPatch--;
			}
			// Dolna czesc
			for (int j = VDL_MAX_VDL_HEIGHT - 1; j > m_carPatchEnd; j--) {
				for (int i = 0; i < vdl_line_length; i += 1){
					//v[0] = vdl_buffer[j][i][2];
					//v[1] = vdl_buffer[j][i][1];
					//v[2] = vdl_buffer[j][i][0];

					// new
					//imageThNightTime[iPatch][i] = vdl_buffer[j][i][3] & (((unsigned char)(1) << 0));
					//night_image.at<vec_uchar_1>(iPatch, i) = 255 * imageThNightTime[iPatch][i];

					m_imageRGBBuffer[iPatch][i][3] = m_vdl_buffer[j][i][3];
					m_imageRGBBuffer[iPatch][i][0] = m_vdl_buffer[j][i][2];
					m_imageRGBBuffer[iPatch][i][1] = m_vdl_buffer[j][i][1];
					m_imageRGBBuffer[iPatch][i][2] = m_vdl_buffer[j][i][0];

					//patch_image.at<vec_uchar_3>(iPatch, i) = v;
				}
				iPatch--;
			}
		}



		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Przekopiowanie obrazka do pamiêci zewnêtrznej
		/*
		for (int j = 0; j < VDL_MAX_VDL_HEIGHT; j++) {
			for (int i = 0; i < VDL_MAX_VDL_WIDTH; i++){
				imageBuffer[3 * VDL_MAX_VDL_WIDTH * j + 3 * i + 0] = m_imageRGBBuffer[j][i][0];
				imageBuffer[3 * VDL_MAX_VDL_WIDTH * j + 3 * i + 1] = m_imageRGBBuffer[j][i][1];
				imageBuffer[3 * VDL_MAX_VDL_WIDTH * j + 3 * i + 2] = m_imageRGBBuffer[j][i][2];
			}
		}
		//memcpy(imageBuffer, imageRGBBuffer, MAX_VDL_HEIGHT*MAX_VDL_WIDTH * 3 * sizeof(char));
        */

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Uruchomienie analizy - wariant nocny lub dzienny
		if (NIGHTTIME == 1)
			carCounter += analyseCarPatch_night_zynq(m_imageRGBBuffer, vdl_line_length, carPatchHeight);
		else
			carCounter += analyseCarPatch_day_zynq(m_imageRGBBuffer, vdl_line_length, carPatchHeight);

		// debug
		/*
		for (int j = 0; j < MAX_VDL_HEIGHT; j++) {
			for (int i = 0; i < MAX_VDL_WIDTH; i++){
				printf("%d ", imageThNightTime[j][i]);
			}
		}
		*/




		//cv::imshow("III", patch_image);
		//cv::imshow("IV", night_image);

        //+++ Zapis patch'a
		//char buffer[100];
		//sprintf(buffer, "p%05d.png", carPatchCounter);
		//cv::imwrite(buffer, patch_image);


		//cv::waitKey(0);

		m_b_road_median_prev = b_road_median;
		printf("Autek %d", carCounter);

		return carCounter;


	}


	// Przypisanie z iteracji na iteracje
	m_b_road_median_prev = b_road_median;

	return 0;
}


// analyseCarPatch - wersja nocna
// ------------------------------------------------------------------------------------------------------------
//int vdl::analyseCarPatch_night_zynq(unsigned char thImage[][MAX_VDL_WIDTH], int vdl_line_width, int car_patch_height) {
//}

// analyseCarPatch2 - to put i Zynq ....
// ------------------------------------------------------------------------------------------------------------
//int vdl::analyseCarPatch_day_zynq(unsigned char rgbImage[][MAX_VDL_WIDTH][4], int vdl_line_width, int car_patch_height){
//}



