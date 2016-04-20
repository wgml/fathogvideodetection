#include <math.h>


#define MAX_VDL_WIDTH  100       // maksymalna mozliwa szerokosci linii detekcji
#define MAX_VDL_HEIGHT 200       // maksymalna mozliwa wysokosc pojedynczeg obrazka 


#define NIGHT_TH 200              // segmentacja swiatel
#define MINIMAL_OBJECT_SIZE 3     // minimalny rozmiar pojedynczego obiektu
#define MAX_VERT_DIST 10          // odleglosc pionowa (jaka musi byc, aby autka byly uznane za oddzielne)
#define HORZ_DIST_COEF 0.3        // wspolczynnik  odleglosci poziomej

#define MAX_OBJECTS 100           // maksymalna liczba obeiktow



int analyseCarPatch_night_zynq(unsigned char rgbImage[][MAX_VDL_WIDTH][4], int vdl_line_width, int car_patch_height);     // analiza patcha - wersja "nocna" -- zynq