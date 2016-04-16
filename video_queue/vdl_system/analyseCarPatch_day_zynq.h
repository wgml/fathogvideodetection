
// ! Zynq
#include "../globals.h"

typedef cv::Vec<uchar, 3> VT_3;



#define EDGE_PATCH_TH 100        // prog kawedzi do analizy patchow
#define SHADOW_TH 25             // prog do detekcji cieni
#define EDGE_SHADOW_NUM_TH 5     // ile sumrycznie piskeli w kolumnie i wireszu, aby wyznaczyc bbox krawedzi i cienia
#define V_CAR_SIZE_TH 50         // prog przelaczania analizy dla wiecej niz jednego autka
#define SIM_TH 60                // prog do segmentacji przez rozrost w liniach (RGB)
#define CAR_LENGTH_TH 10         // prog minialnej dlugosci pojedynczego samochodu (uzywane przy rozdzielaniu)

#define NUM_EDGE_SX_TH 0.1    // ile ma byc krawedzi poziomych aby patch zostal uznany za autko

int analyseCarPatch_day_zynq(unsigned char rgbImage[][VDL_MAX_VDL_WIDTH][4], int vdl_line_width, int car_patch_height);
