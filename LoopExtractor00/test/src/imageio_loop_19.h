#ifndef imageio_loop_19_h_
#define imageio_loop_19_h_

#include "imageio.h"

void readPGMFile_loop_2(int *i, char number[], int *c, FILE * *imageFile);
void writeDATFile_loop_18(int *i, Image * *image, int *j, float *maxival);
void writePGMFile_loop_6(int *i, Image * *image, int *j, float *value, float *maxival, FILE * *output);
void readPGMFile_loop_1(int *i, char number[], int *c, FILE * *imageFile);
void writePPMFile_loop_13(int *i, ImageRGB * *image, int *j, FILE * *output, float *maxival, int *re_arg_pa1_13);
void writePPMFile_loop_14(int *i, ImageRGB * *image, int *j, float *value, float *maxival, FILE * *output);
void readPPMFile_loop_11(int *i, char number[], int *c, FILE * *imageFile);
void writePPMFileNegative_loop_16(int *i, ImageRGB * *image, int *j, float *value, float *maxval, FILE * *output);
void writePPMFileNegative_loop_15(int *i, ImageRGB * *image, int *j, float *maxival, float *minival);
void readPPMFile_loop_9(int *i, char number[], int *c, FILE * *imageFile);
void writePGMFile_loop_5(int *i, Image * *image, int *j, FILE * *output, float *maxival, int *re_arg_pa1_5);
void readPPMFile_loop_10(int *i, char number[], int *c, FILE * *imageFile);
void readDATFile_loop_17(int *i, Image * *image, int *j, FILE * *imageFile, int *value);
void writePGMFileNegative_loop_7(int *i, Image * *image, int *j, float *maxival, float *minival);
void writeDATFile_loop_19(int *i, Image * *image, int *j, float *value, float *maxival, FILE * *output);
void readPGMFile_loop_3(int *i, char number[], int *c, FILE * *imageFile);
void writePGMFileNegative_loop_8(int *i, Image * *image, int *j, float *value, float *maxval, FILE * *output);
void readPPMFile_loop_12(int *i, ImageRGB * *image, int *j, int *value, FILE * *imageFile);
void readPGMFile_loop_4(int *i, Image * *image, int *j, int *value, FILE * *imageFile);

#endif
