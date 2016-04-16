#include "analyseCarPatch_night_zynq.h"

#include <stdio.h>

int analyseCarPatch_night_zynq(unsigned char rgbImage[][VDL_MAX_VDL_WIDTH][4], int vdl_line_width, int car_patch_height) {

	int ileAut = 0;                          // lokalny licznik samochodow w danym patch'u

	int tabSklejen[MAX_OBJECTS] = { 0 };        // tablica sklejen do indeksacji
	float S[MAX_OBJECTS][3] = { 0, 0, 0 };        // parametry - pole oraz wspolrzedna x i y

	int L = 2;                       // indeksy
	int s[4] = { 0 };                // kontekst
	int i, j, z, zz, zzz;               // iteratory

	int minL, maxL;                // minimalna i maksymalna etykieta
	int p1, p2;                    // pole
	float x1, x2, y1, y2;          // wspolrzedne srodka ciezkosci

	bool odwiedzone[MAX_OBJECTS] = { false };  // tablica potrzebna przy analizie
	int carLevel[10] = { 0 };                  // maksymalnie 10 poziomow samochodow
	char carLevelI = 0;                        // iterator po poziomach autek

	static unsigned char thImage[VDL_MAX_VDL_HEIGHT][VDL_MAX_VDL_WIDTH];

	for (j = 1; j < car_patch_height; j++) {
		for (i = 1; i < vdl_line_width; i++) {
			if (0.299*(float)(rgbImage[j][i][0]) + 0.587*(float)(rgbImage[j][i][1]) + 0.114*(float)(rgbImage[j][i][2]) > VDL_NIGHT_TH)
				thImage[j][i] = 1;
			else
				thImage[j][i] = 0;

		}
	}

	// Indeksacja jednoprzegiegowa z wyznaczaniem pola i srodka ciezkosci
	for (j = 1; j < car_patch_height; j++) {
		for (i = 1; i < vdl_line_width; i++) {

			if (thImage[j][i] > 0) {

				s[0] = thImage[j - 1][i - 1];
				s[1] = thImage[j - 1][i];
				s[2] = thImage[j - 1][i + 1];
				s[3] = thImage[j][i - 1];

				if (s[0] + s[1] + s[2] + s[3] == 0) {
					thImage[j][i] = L;
					tabSklejen[L] = L;

					S[L][0] = 1; // pole
					S[L][1] = i; // wsp x
					S[L][2] = j; // wsp y
					L++;
				}
				else {
					minL = 100000000;
					maxL = -1;
					for (z = 0; z < 4; z++) {
						if (s[z] != 0) {
							if (s[z] < minL)
								minL = s[z];
							if (s[z] > maxL)
								maxL = s[z];
						}
					}

					S[minL][0] = S[minL][0] + 1;
					S[minL][1] = S[minL][1] / (S[minL][0] + 1)*S[minL][0] + i / (S[minL][0] + 1);
					S[minL][2] = S[minL][2] / (S[minL][0] + 1)*S[minL][0] + j / (S[minL][0] + 1);

					thImage[j][i] = minL;

					if (minL != maxL)
						tabSklejen[maxL] = minL;
				}

			}
		}
	}
	// Porzadkowanie tablicy sklejen
	for (z = 0; z < L; z++)
	if (tabSklejen[z] != 0)
		tabSklejen[z] = tabSklejen[tabSklejen[z]];

	// Laczenie elementow
	for (z = 0; z < L; z++) {
		if (tabSklejen[z] != z && tabSklejen[z] != 0) {

			p1 = S[tabSklejen[z]][0];
			p2 = S[z][0];
			S[tabSklejen[z]][0] = p1 + p2;

			x1 = S[tabSklejen[z]][1];
			x2 = S[z][1];
			y1 = S[tabSklejen[z]][2];
			y2 = S[z][2];

			S[tabSklejen[z]][1] = x1 / (p1 + p2)*p1 + x2 / (p1 + p2)*p2;
			S[tabSklejen[z]][2] = y1 / (p1 + p2)*p1 + y2 / (p1 + p2)*p2;

			S[z][0] = 0;
			S[z][1] = 0;
			S[z][2] = 0;
		}
	}

	// Tymczasowe wypisanie
	for (z = 0; z < L; z++)
	if (S[z][0] > 0)
		printf("O | P= %f, SX = %f,  SY = %f \n", S[z][0], S[z][1], S[z][2]);

	// Analiza
	for (z = 0; z < L; z++) {
		if (!odwiedzone[z] && S[z][0] > MINIMAL_OBJECT_SIZE) {
			odwiedzone[z] = true;
			for (zz = z + 1; zz < L; zz++) {
				if (!odwiedzone[zz] && S[zz][0] > MINIMAL_OBJECT_SIZE)
					if (fabs(S[z][2] - S[zz][2]) < MAX_VERT_DIST && fabs(S[z][1] - S[zz][1]) > vdl_line_width*HORZ_DIST_COEF) {
						odwiedzone[zz] = true;
						for (zzz = 0; zzz <= carLevelI; zzz++){
							if (fabs(carLevel[zzz] - ((S[z][2] + S[zz][2]) / 2)) > MAX_VERT_DIST) {
								carLevel[carLevelI++] = (S[z][2] + S[zz][2]) / 2;
								ileAut++;
								break;
							}
						}


				}
			}
		}

	}

	// Opcja
	printf("Autek %d \n", ileAut);
	return ileAut;
}
