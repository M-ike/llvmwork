#include "imageio_loop_19.h"
void readPGMFile_loop_1(int *i, char number[], int *c, FILE * *imageFile)
{
for((*i) = 1, number[0] = (*c); !isspace((*c) = getc((*imageFile))); ++(*i)) {
		number[(*i)] = (*c);
	}

}
void readPGMFile_loop_2(int *i, char number[], int *c, FILE * *imageFile)
{
for((*i) = 1, number[0] = (*c); !isspace((*c) = getc((*imageFile))); ++(*i)) {
		number[(*i)] = (*c);
	}

}
void readPGMFile_loop_3(int *i, char number[], int *c, FILE * *imageFile)
{
for((*i) = 1, number[0] = (*c); !isspace((*c) = getc((*imageFile))); ++(*i)) {
		number[(*i)] = (*c);
	}

}
void readPGMFile_loop_4(int *i, Image * *image, int *j, int *value, FILE * *imageFile)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i))
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			(*value) = getc((*imageFile));
			(*value) &=  0377;
			(*image)->pic[(*i)][(*j)] = (float) ((float) (*value) / 255.0f);
		}

}
void writePGMFile_loop_5(int *i, Image * *image, int *j, FILE * *output, float *maxival, int *re_arg_pa1_5)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			if((*image)->pic[(*i)][(*j)] < 0) {
				writePGMFileNegative((*image), (*output));
				fclose((*output));
				{ (*re_arg_pa1_5) = 0; return; }
			}
			if ((*image)->pic[(*i)][(*j)] > (*maxival))
				(*maxival) = (*image)->pic[(*i)][(*j)];
		}
	}

}
void writePGMFile_loop_6(int *i, Image * *image, int *j, float *value, float *maxival, FILE * *output)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i))
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			(*value) = (*image)->pic[(*i)][(*j)] / (*maxival);
			(*value) *= 255;
			fprintf((*output), "%c", (char) ((int) (*value)));
		}

}
void writePGMFileNegative_loop_7(int *i, Image * *image, int *j, float *maxival, float *minival)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			if((*image)->pic[(*i)][(*j)] > (*maxival))
				(*maxival) = (*image)->pic[(*i)][(*j)];
			if((*image)->pic[(*i)][(*j)] < (*minival))
				(*minival) = (*image)->pic[(*i)][(*j)];
		}
	}

}
void writePGMFileNegative_loop_8(int *i, Image * *image, int *j, float *value, float *maxval, FILE * *output)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i))
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			(*value) = (*image)->pic[(*i)][(*j)] / (*maxval) + 1.0;
			(*value) *= 127;
			fprintf((*output), "%c", (char) ((int) (*value)));
		}

}
void readPPMFile_loop_9(int *i, char number[], int *c, FILE * *imageFile)
{
for((*i) = 1, number[0] = (*c); !isspace((*c) = getc((*imageFile))); ++(*i)) {
		number[(*i)] = (*c);
	}

}
void readPPMFile_loop_10(int *i, char number[], int *c, FILE * *imageFile)
{
for((*i) = 1, number[0] = (*c); !isspace((*c) = getc((*imageFile))); ++(*i)) {
		number[(*i)] = (*c);
	}

}
void readPPMFile_loop_11(int *i, char number[], int *c, FILE * *imageFile)
{
for((*i) = 1, number[0] = (*c); !isspace((*c) = getc((*imageFile))); ++(*i)) {
		number[(*i)] = (*c);
	}

}
void readPPMFile_loop_12(int *i, ImageRGB * *image, int *j, int *value, FILE * *imageFile)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i))
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			(*value) = getc((*imageFile));
			(*value) &=  0377;
			(*image)->red->pic[(*i)][(*j)] = (float) ((float) (*value) / 255.0f);
			
			(*value) = getc((*imageFile));
			(*value) &=  0377;
			(*image)->green->pic[(*i)][(*j)] = (float) ((float) (*value) / 255.0f);
			
			(*value) = getc((*imageFile));
			(*value) &=  0377;
			(*image)->blue->pic[(*i)][(*j)] = (float) ((float) (*value) / 255.0f);
		}

}
void writePPMFile_loop_13(int *i, ImageRGB * *image, int *j, FILE * *output, float *maxival, int *re_arg_pa1_13)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			if((*image)->red->pic[(*i)][(*j)] < 0 ||
			   (*image)->green->pic[(*i)][(*j)] < 0 ||
			   (*image)->blue->pic[(*i)][(*j)] < 0) {
				writePPMFileNegative((*image), (*output));
				fclose((*output));
				{ (*re_arg_pa1_13) = 0; return; }
			}
			if((*image)->red->pic[(*i)][(*j)] > (*maxival))
				(*maxival) = (*image)->red->pic[(*i)][(*j)];
			if((*image)->green->pic[(*i)][(*j)] > (*maxival))
				(*maxival) = (*image)->green->pic[(*i)][(*j)];
			if((*image)->blue->pic[(*i)][(*j)] > (*maxival))
				(*maxival) = (*image)->blue->pic[(*i)][(*j)];
		}
	}

}
void writePPMFile_loop_14(int *i, ImageRGB * *image, int *j, float *value, float *maxival, FILE * *output)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i))
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			(*value) = (*image)->red->pic[(*i)][(*j)] / (*maxival);
			(*value) *= 255;
			fprintf((*output), "%c", (char) ((int) (*value)));
			
			(*value) = (*image)->green->pic[(*i)][(*j)] / (*maxival);
			(*value) *= 255;
			fprintf((*output), "%c", (char) ((int) (*value)));
			
			(*value) = (*image)->blue->pic[(*i)][(*j)] / (*maxival);
			(*value) *= 255;
			fprintf((*output), "%c", (char) ((int) (*value)));
		}

}
void writePPMFileNegative_loop_15(int *i, ImageRGB * *image, int *j, float *maxival, float *minival)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			if((*image)->red->pic[(*i)][(*j)] > (*maxival))
				(*maxival) = (*image)->red->pic[(*i)][(*j)];
			if((*image)->green->pic[(*i)][(*j)] > (*maxival))
				(*maxival) = (*image)->green->pic[(*i)][(*j)];
			if((*image)->blue->pic[(*i)][(*j)] > (*maxival))
				(*maxival) = (*image)->blue->pic[(*i)][(*j)];
			
			if((*image)->red->pic[(*i)][(*j)] < (*minival))
				(*minival) = (*image)->red->pic[(*i)][(*j)];
			if((*image)->green->pic[(*i)][(*j)] < (*minival))
				(*minival) = (*image)->green->pic[(*i)][(*j)];
			if((*image)->blue->pic[(*i)][(*j)] < (*minival))
				(*minival) = (*image)->blue->pic[(*i)][(*j)];
		}
	}

}
void writePPMFileNegative_loop_16(int *i, ImageRGB * *image, int *j, float *value, float *maxval, FILE * *output)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i))
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			(*value) = (*image)->red->pic[(*i)][(*j)] / (*maxval) + 1.0;
			(*value) *= 127;
			fprintf((*output), "%c", (char) ((int) (*value)));
			
			(*value) = (*image)->green->pic[(*i)][(*j)] / (*maxval) + 1.0;
			(*value) *= 127;
			fprintf((*output), "%c", (char) ((int) (*value)));
			
			(*value) = (*image)->blue->pic[(*i)][(*j)] / (*maxval) + 1.0;
			(*value) *= 127;
			fprintf((*output), "%c", (char) ((int) (*value)));
		}

}
void readDATFile_loop_17(int *i, Image * *image, int *j, FILE * *imageFile, int *value)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i))
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			fscanf((*imageFile),"%d",&(*value));
//			(*value) &=  0377;
			(*image)->pic[(*i)][(*j)] = (float) ((float) (*value) / 255.0f);
		}

}
void writeDATFile_loop_18(int *i, Image * *image, int *j, float *maxival)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
/*			if((*image)->pic[(*i)][(*j)] < 0) {
				writePGMFileNegative((*image), output);
				fclose(output);
				return;
			}*/
			if ((*image)->pic[(*i)][(*j)] > (*maxival))
				(*maxival) = (*image)->pic[(*i)][(*j)];
		}
	}

}
void writeDATFile_loop_19(int *i, Image * *image, int *j, float *value, float *maxival, FILE * *output)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i))
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			(*value) = (*image)->pic[(*i)][(*j)] / (*maxival);
			(*value) *= 255;
			fprintf((*output), "%d\n", (int)(*value));
		}

}
