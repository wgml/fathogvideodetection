#include "../globals.h"
#include <math.h>



#define MINIMAL_OBJECT_SIZE 3     // minimalny rozmiar pojedynczego obiektu
#define MAX_VERT_DIST 10          // odleglosc pionowa (jaka musi byc, aby autka byly uznane za oddzielne)
#define HORZ_DIST_COEF 0.3        // wspolczynnik  odleglosci poziomej

#define MAX_OBJECTS 100           // maksymalna liczba obeiktow



int analyseCarPatch_night_zynq(unsigned char rgbImage[][VDL_MAX_VDL_WIDTH][4], int vdl_line_width, int car_patch_height);     // analiza patcha - wersja "nocna" -- zynq
