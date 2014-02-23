#include "main_loop_4.h"
void printTrashed(FILE *output, KeyPoint *trash, double scale, int octave);
void printPoints(FILE *output, KeyPoint *peaks, double scale, int octave);
void printFooter(FILE *output);

int main(int argc, char **argv) {
//void main() {

	FILE     *output; //keypoint output (text)
	Image    *sourceImage, *sourceImage2;
	Image    *doubledImage;
	Image    *currentImage;
	Octave   *octavesList;
	Octave   *temp, *current;
	KeyPoint *peaks;
	KeyPoint *counter, *prevPoint;
	KeyPoint *keypoints, *keyCounter;
	int      candidates = 0;
	double   scale;
	double   ***mags, ***directions;
	int      octaveCount;
	int      tempHeight, tempWidth, tempDepth;
	int      totalKeypoints = 0;
	double   imageSigma;
	char     fileName[32] = {'\0'};
	FILE     *candidateOutput;
	FILE     *trashOutput;
	FILE     *filteredOutput;
/*
	if(argc < 2) {
		printf("Usage: sift dat_file\n");
		return 0;
	}
*/
	/* read input */
	//sourceImage = readPGMFile(argv[1]);
	sourceImage = readDATFile("./image.dat");
	if(sourceImage == NULL) {
		printf("file was not read\n");
		return 1;
	}
	scale = START_SCALE;

	/* print constants used */
	printf("Constants:\n");
	printf("START_SIGMA=%f\n", START_SIGMA);
	printf("OCTAVE_LIMIT=%d\n", OCTAVE_LIMIT);
	printf("IMAGES_PER_OCTAVE=%d\n", IMAGES_PER_OCTAVE);
	printf("SKIP_OCTAVES=%d\n", SKIP_OCTAVES);
	printf("MIN_SIZE=%d\n", MIN_SIZE);
	printf("MAX_ADJ_STEPS=%d\n", MAX_ADJ_STEPS);
	printf("EDGE_RATIO=%f\n", EDGE_RATIO);
	printf("CONTRAST_THRESH=%f\n", CONTRAST_THRESH);
	printf("CAP=%f\n", CAP);

	/* double image to save data */
	doubledImage = doubleImage(sourceImage);
	if(doubledImage == NULL) {
		return 1;
	}
	scale /= 2.0;
	printf("\nImage doubled\n");

	/* preprocessing smoothing*/
	printf("Preprocessing smoothing\n");
	doubledImage = gaussianConvolution2DFast(doubledImage, 1.0);
//	doubledImage = gaussianConvolution2DFast(sourceImage, 1.0);

	/* build octaves of scalespace */
	printf("Octave pyramid construction\n");
	currentImage = doubledImage;
	octavesList = NULL;
	octaveCount = 0;
	while(octaveCount < OCTAVE_LIMIT && currentImage->width >= MIN_SIZE && currentImage->height >= MIN_SIZE) {
		if(currentImage == NULL) {
			printf("  ***Current image null. Exiting.\n");
			return 1;
		}

		/* create octave pointer wrapper */
		printf("  Octave for pic %d %d (%d)\n", currentImage->width, currentImage->height, octaveCount);
		temp = (Octave *) malloc(sizeof(Octave));
		if(temp == NULL) {
			printf("  ***Out of memory. Exiting.\n");
			return 1;
		}
		/* build scale space L(x,y,sigma) */
		printf("    Gaussian Scale Space\n");
		if(octaveCount == 0)
			imageSigma = 1.0; //blur of the first image is 1.0
		else
			imageSigma = START_SIGMA;
		temp->gaussianSpace = buildScaleSpaceOctave(currentImage, scale, imageSigma);

		/* build diff space D(x,y,sigma) */
		printf("    Diff Gaussian Space\n");
		temp->diffGaussianSpace = buildDifferences(temp->gaussianSpace);

		/* store and go on */
		octavesList = link(octavesList, temp);
		currentImage = halveImage(getLastImage(temp->gaussianSpace));
		scale *= 2.0;
		++octaveCount;
		printf("    Image halved\n");
	}
	--octaveCount; /* one extra */

	/* generate keypoints from each octave of scalespace */
	printf("Keypoint generation\n");
	candidateOutput = fopen("output_candidates.txt", "w"); //helpful output file
	trashOutput = fopen("output_trash.txt", "w"); //helpful output file
	filteredOutput = fopen("output_filtered.txt", "w"); //helpful output file
	
	histOutput = fopen("output_histogram.txt", "w"); //outputs histograms
	histOutput2 = fopen("output_histogram2.txt", "w"); //turn on/off in scalespace.c
	
	keypoints = NULL;
	sourceImage2 = cloneImage(sourceImage);
	for(current = octavesList; current != NULL && octaveCount >= SKIP_OCTAVES; current = current->next, --octaveCount) {
		printf("  Octave %d\n", octaveCount);
		fprintf(histOutput, "Octave %d\n", octaveCount);
		fprintf(histOutput, "---------\n");
		fprintf(histOutput2, "Octave %d\n", octaveCount);
		fprintf(histOutput2, "---------\n");

		/* find keypoint candidates */
		printf("    Looking for peaks\n");
		peaks = getPeaks(current);
		/* temporary output */
		scale = current->diffGaussianSpace->imageScale;
		printPoints(candidateOutput, peaks, scale, octaveCount);
		markImageSPoint(peaks, sourceImage2, scale); //output candidates
		if(peaks != NULL)
			printf("      found %d candidates\n", peaks->count);
		else
			printf("      found 0 candidates\n");

		/* filter peaks for contrast and edge-iness and localize them */
		printf("    Filtering and localizing\n");
		trash = NULL;
		peaks = filterAndLocalizePeaks(current->diffGaussianSpace, current->gaussianSpace, peaks);

		/* temporary output */
		printPoints(filteredOutput, peaks, scale, octaveCount);
		printTrashed(trashOutput, trash, scale, octaveCount);
		if(peaks != NULL)
			printf("      have %d candidates remaining\n", peaks->count);
		else
			printf("      have 0 candidates remaining\n");

		/* precompute magnitudes and orientations */
		printf("    Generating magnitudes and orientations\n");
		tempHeight = current->gaussianSpace->imgHeight;
		tempWidth = current->gaussianSpace->imgWidth;
		tempDepth = current->gaussianSpace->imageCount - 2;
		current->gaussianSpace->magnitudes = allocate3D(tempHeight, tempWidth, tempDepth);
		current->gaussianSpace->directions = allocate3D(tempHeight, tempWidth, tempDepth);
		getMagnitudesAndOrientations(current->gaussianSpace);

		/* assign orientation(s) */
		printf("    Assigning orientation(s)\n");
		generateKeypoints(current->gaussianSpace, peaks);

		/* generate descriptors*/
		printf("    Calculating descriptors\n");
		createDescriptors(current->gaussianSpace, peaks);

		/* multiply by imgScale to get real values */
		for(counter = peaks; counter != NULL; counter = counter->next) {
			counter->finalX *= counter->imgScale;
			counter->finalY *= counter->imgScale;
			//also multiply x,y?
			counter->scale *= counter->imgScale;
		}

		/* merge lists */
		if(peaks != NULL) {
			if(keypoints != NULL) {
				keypoints->listRear->next = peaks;
				keypoints->listRear = peaks->listRear;
			}
			else
				keypoints = peaks;
		}

		/* deallocate memory */
		deallocate3D(current->gaussianSpace->magnitudes, tempHeight, tempWidth);
		deallocate3D(current->gaussianSpace->directions, tempHeight, tempWidth);

		/* deallocate trashed points */
		counter = trash;
		prevPoint = NULL;
		while(counter != NULL) {
			prevPoint = counter;
			counter = counter->next;
			free(prevPoint);
		}
	}

	fclose(histOutput);
	fclose(histOutput2);

	/* write footer to each file */
	printFooter(candidateOutput);
	fclose(candidateOutput);
	printFooter(trashOutput);
	fclose(trashOutput);
	printFooter(filteredOutput);
	fclose(filteredOutput);

	/* output keypoint descriptors */
	printf("Output\n");
	output = fopen("output.txt", "w");
	
    main_loop_1(&keyCounter, &keypoints, &totalKeypoints);

	fprintf(output, "%d %d\n", totalKeypoints, 128);
	
    main_loop_2(&keyCounter, &keypoints, &output);

	fclose(output);

	/* output keypoints on image*/
	markImage(keypoints, sourceImage);
	writeDATFile(sourceImage, "./output/keypoints.dat");

	/* output initial candidates on image */
	writeDATFile(sourceImage2, "./output/keypoints_all.dat");
	printf("Done. %d keypoints found.\n", totalKeypoints);

	free(sourceImage);
	free(sourceImage2);
	free(doubledImage);
	//free octaves - should put it here
	return 0;
}

void printTrashed(FILE *output, KeyPoint *trash, double scale, int octave) {

	KeyPoint *counter;

	fprintf(output, "Octave %d\n", octave);
	fprintf(output, "--------\n");
	fprintf(output, "   y    x level  scale reason\n");
	
    printTrashed_loop_3(&counter, &trash, &output, &scale);

	fprintf(output, "\n");
}

void printPoints(FILE *output, KeyPoint *peaks, double scale, int octave) {

	KeyPoint *counter;

	fprintf(output, "Octave %d\n", octave);
	fprintf(output, "--------\n");
	fprintf(output, "   y    x level  scale note\n");
	
    printPoints_loop_4(&counter, &peaks, &output, &scale);

	fprintf(output, "\n");
}

void printFooter(FILE *output) {
	fprintf(output, "\nlevel - Difference of Gaussian level in the scalespace stack of images\n"
				"P - passed\n"
				"F - failed\n");
}
