#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "image.h"
#include "ppm.h"
#include "texture_synthesis.h"

int main( int argc , char *argv[] )
{
	// Check number of command line arguments
	if (argc != 6) {
		fprintf(stderr, "Error: Not enough arguments provided\n");
		return 1;
	}
	
	// Seed the random number generator so that the code produces the same results on the same input.
	srand(0);

	// Open the input file for reading
    FILE *fp = fopen(argv[1], "r");

	// Check for error opening the file
	if (fp == NULL) {
		fprintf(stderr, "Error: Unable to read file of the exemplar image\n");
		return 2;
	}
	
	// Setting fields in the Image struct representing the exemplar image
	Image * img = NULL;
	img = ReadPPM(fp);

	// Error creating the Image struct
	if (img == NULL) {
		fprintf(stderr, "Error: Unable to allocate memory for exemplar image\n");
		return 3;
	}

	// Closing the input file
	fclose(fp);
	
	// Storing the user inputted width, height, and window radius
	int width = atoi(argv[3]);
	int height = atoi(argv[4]);
	int window_radius = atoi(argv[5]);

	// Setting the fields of the struct representing the new image
	Image * new_image = NULL;

	//Get the time at the start of execution
	clock_t start_clock = clock();

	// Updating fields in Image struct
	new_image = SynthesizeFromExemplar(img, width, height, window_radius);

	// Get the time at the end of the execution
	clock_t clock_difference = clock() - start_clock;

	if (new_image == NULL) {
		fprintf(stderr, "Error: Unable to allocate memory for new image\n");
		return 1;
	}

	// Convert the time difference into seconds and print
	printf( "Synthesized texture in %.2f(s)\n" , (double)clock_difference/CLOCKS_PER_SEC );

	// Opening the output file for writing
	FILE *fp_write = fopen(argv[2], "w+");

	// Checking for error when opening file
	if (fp_write == NULL) {
		fprintf(stderr, "Error: Unable to successfully open output file\n");
		FreeImage(&new_image);
		return 4;
	}

	// Checking for errors when writing to file
	if ((unsigned int) WritePPM(fp_write, new_image) != (new_image->width * new_image->height)) {
	    fprintf(stderr, "Error: New image size does not match expected image size based on dimensions provided by user\n");
		FreeImage(&new_image);
		return 4;
	}

	FreeImage(&new_image);
	fclose(fp_write);
	return 0;
}

