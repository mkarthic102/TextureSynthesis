#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <float.h>
#include "texture_synthesis.h"
#include "image.h"

/*
* Determines the Gaussian of each pixel in a window 
*/
double * compute_gaussian(int r) {
	double * gaussian = (double *) malloc(sizeof(double) * (2*r +1) * (2*r+1));

	if (gaussian == NULL) {
		fprintf(stderr, "Error: Unable to allocate memory for gaussian\n");
		return NULL;
	}

	double sigma = (2 * r + 1) / 6.4;

	// starting index of gaussian array
	int counter = 0;

	for (int i = -r; i <= r; i++) {
    	for (int j = -r; j <= r; j++) {
    		gaussian[counter] = exp( -(i * i + j * j) / (2 * sigma * sigma));
      		counter++;
    	}
  	}
  	return gaussian;
}
 
/*
* Creates a window centered at the TBS pixel window
*/
Pixel * create_TBS_pixel_window(int r, TBSPixel TBSPixel, Pixel * pixels, int width, int height) {
	
	// determine the width and height of the TBS pixel window
	int window_width = 2 * r + 1;
	int window_height = 2 * r + 1;

	// allocate memory for the window and determine x and y of TBS pixel
	Pixel * pixel_window = (Pixel *) malloc(sizeof(Pixel) * window_width * window_height);

	if (pixel_window == NULL) {
		fprintf(stderr, "Error: Unable to allocate memory for window centered at TBS pixel\n");
		return NULL;
	}
	
	int x = TBSPixel.idx.x;
	int y = TBSPixel.idx.y;

	// starting index of pixel_window array
	int counter = 0;

	// x and y of top left corner of window
	int x_top_left = x - r;
	int y_top_left = y - r;

	for(int i = x_top_left; i <= (x + r); i++) {
		for(int j = y_top_left; j <= (y + r); j++) {

			// pixel is out-of-bounds cases
	    	if (i < 0) {
	      		Pixel pixel_out_bounds = {0, 0, 0, 0};
	      		pixel_window[counter] = pixel_out_bounds;
	      		counter++;
	    	}
	    	else if (i >= width) {
	      		Pixel pixel_out_bounds = {0, 0, 0, 0};
              	pixel_window[counter] = pixel_out_bounds;
              	counter++;
	    	}
	    	else if (j < 0) {
	      		Pixel pixel_out_bounds = {0, 0, 0, 0};
              	pixel_window[counter] = pixel_out_bounds;
              	counter++;
	    	}
	    	else if ( j >= height) {
	      		Pixel pixel_out_bounds = {0, 0, 0, 0};
              	pixel_window[counter] = pixel_out_bounds;
              	counter++;
	    	}

			// pixel is in-bounds
	    	else {
	      		int pos = (width * j) + i;
	      		pixel_window[counter] = pixels[pos];
              	counter++;
	    	}
	  	}
	}
	      
	return pixel_window;

}

/*
* Creates a window centered at an exemplar pixel
*/
Pixel * create_exemplar_window(int r, int index, int width, int height, Pixel * pixels) {
	
	// determine the width and height of the window
	int window_width = 2*r+1;
	int window_height = 2*r+1;

	// allocate memory for the window and determine x and y of exemplar pixel
    Pixel * pixel_window = (Pixel *) malloc(sizeof(Pixel) * window_width * window_height);

	if (pixel_window == NULL) {
		fprintf(stderr, "Error: Unable to allocate memory for window centered at exemplar pixel\n");
		return NULL;
	}
	
    int exemp_x  = index % width;
    int exemp_y  = index / width;

	// starting index of pixel_window array
    int counter = 0;

	// x and y of top left corner of window
    int x_top_left = exemp_x - r;
    int y_top_left = exemp_y - r;

    for(int i = x_top_left; i <= (exemp_x + r); i++) {
        for(int j = y_top_left; j <= (exemp_y + r); j++) {

			// pixel is out-of-bounds cases
            if (i < 0) {
        		Pixel pixel_out_bounds = {0, 0, 0, 0};
              	pixel_window[counter] = pixel_out_bounds;
              	counter++;
            }
            else if ( i >= width) {
              	Pixel pixel_out_bounds = {0, 0, 0, 0};
              	pixel_window[counter] = pixel_out_bounds;
              	counter++;
            }
            else if (j < 0) {
              	Pixel pixel_out_bounds = {0, 0, 0, 0};
              	pixel_window[counter] = pixel_out_bounds;
              	counter++;
	    	}
            else if ( j >= height) {
            	Pixel pixel_out_bounds = {0, 0, 0, 0};
              	pixel_window[counter] = pixel_out_bounds;
              	counter++;
	    	}

			// pixel is in-bounds
            else {
              int pos = (width * j) + i;
              pixel_window[counter] = pixels[pos];
              counter++;
	    	}
		}
    }

    return pixel_window;
}

/*
* Finds the difference between each corresponding pixel
* in the TBS pixel window and the exemplar pixel window
*/
double find_difference(Pixel * tbs_pixel_window, Pixel * exemp_pixel_window, int r, double * gaussian) {

	// total difference
	double diff = 0;

	// squared difference
	double d = 0;

	// gaussian
	double s = 0;

	// number of differences computed
	int diff_counter = 0;

	// starting index of gaussian array
	int counter = 0;

	for (int i = -r; i <= r; i++) {
		for (int j = -r; j <= r; j++) {
			Pixel tbs_pixel = tbs_pixel_window[counter];
			Pixel exemp_pixel = exemp_pixel_window[counter];

			// computes difference if the TBS pixel and exemp pixel are set
			if (tbs_pixel.a == 255 && exemp_pixel.a == 255) {
			    d = PixelSquaredDifference(tbs_pixel, exemp_pixel);
				s = gaussian[counter]; 
				diff += d*s;
				diff_counter++;
			}
		       
			// if an exemplar pixel is out-of-bounds, then don't perform comparison
			// between TBS and exemplar pixel windows
			else if (exemp_pixel.a != 255) {
				return DBL_MAX;
			}
			counter++;
		}
	}

	// difference was not computed for any corresponding TBS and exemplar pixel
	if (diff_counter == 0) {
		return DBL_MAX;
	}

	return diff / diff_counter; 
}
	

/*
* Finds the difference between the TBS pixel window and each exemplar pixel window
*/
PixelDiff * compare_windows(Pixel * tbs_pixel_window, Image * img, Image * exemp, int r, double * gaussian) {
	
	Pixel * pixels_array = img->pixels;
  	int width = img->width;
  	int height = img->height;
  	int exemp_width = exemp->width;
  	int exemp_height = exemp->height;
	
	// starting index of diff_array
  	int diff_array_index = 0; 

  	// difference between a TBS window and exemplar window
  	double diff_of_windows = 0;

  	// array of differences between TBS window and all exemplar windows
  	PixelDiff * diff_array = (PixelDiff *) malloc(sizeof(PixelDiff) * exemp->width * exemp->height);
	
	if (diff_array == NULL) {
		fprintf(stderr, "Error: Unable to allocate memory for array of differences between exemplar pixels and TBS pixel\n");
		return NULL;
	}

  	Pixel * exemp_pixel_window = NULL;

  	for(int i = 0; i < width; i++) {
    	for(int j = 0; j < height; j++) {
      		int pos = (width * j) + i; 
			// condition that determines if pos is in the exemplar image
      		if (pos >= (j * width) && pos <= (j * width + exemp_width - 1) && !(j >= exemp_height)) {
				exemp_pixel_window = create_exemplar_window(r, pos, width, height, pixels_array);
				diff_of_windows = find_difference(tbs_pixel_window, exemp_pixel_window, r, gaussian);
				PixelDiff exemp_pixel_diff = {pixels_array[pos], diff_of_windows};
				diff_array[diff_array_index] = exemp_pixel_diff;
				diff_array_index++;
				free(exemp_pixel_window);
      		}
    	}
 	}
    return diff_array;
}

/*
* Finds the minimum difference between the TBS pixel window and an exemplar pixel window
*/
PixelDiff find_minimum_difference(PixelDiff * diff_array, int exemp_width, int exemp_height) {
	
	double min = diff_array[0].diff;
	int num_elements = exemp_width * exemp_height;

	// determines the minimum difference
	for (int i = 0; i < num_elements; i++) {
		if (diff_array[i].diff <= min) {
			min = diff_array[i].diff;
		}
	}

	double threshold = min * 1.1;
	int capacity = 1;
	PixelDiff * threshold_arr = malloc(sizeof(PixelDiff) * capacity);
	
	// starting index of threshold_arr
	int counter = 0;

	// determines which differences are below the threshold difference
	for (int i = 0; i < num_elements; i++) {

		if (diff_array[i].diff <= threshold) {
			threshold_arr[counter].pixel = diff_array[i].pixel; 
			threshold_arr[counter].diff = diff_array[i].diff;
			counter++; 
		}

		// allocates more memory for threshold_arr
		if (counter == capacity && i != num_elements - 1) {
			capacity++;
			threshold_arr = realloc(threshold_arr, sizeof(PixelDiff) * capacity);
		}
	}

	// random index between 0 and counter - 1
	int random_index = rand() % counter;
	
	free(diff_array);
	PixelDiff random_exemp = threshold_arr[random_index];
	free(threshold_arr);
	return random_exemp;

}

/*
* Sets the r, g, and b values of each TBS pixel in the new image
* based on comparisons to all of the exemplar pixels
*/
Image * set_TBS_Pixels(int TBSPixel_arr_size, TBSPixel * tbs_pixels, int r, Image * img, Image * exemp, double * gaussian){

	Pixel * pixels = img->pixels;
	
	for (int i = 0; i < TBSPixel_arr_size; i++) {
		Pixel * tbs_pixel_window = create_TBS_pixel_window(r, tbs_pixels[i], pixels, img->width, img->height);
		PixelDiff * pixel_diffs = compare_windows(tbs_pixel_window, img, exemp, r, gaussian);
		PixelDiff rand_exemp = find_minimum_difference(pixel_diffs, exemp->width, exemp->height);
		Pixel for_tbs_pixel = rand_exemp.pixel;

		// set the r, g, and b values of the TBS pixel
		int x = tbs_pixels[i].idx.x;
		int y = tbs_pixels[i].idx.y;
		int pos = (img->width * y) + x; 
		pixels[pos].r = for_tbs_pixel.r;
		pixels[pos].g = for_tbs_pixel.g;
		pixels[pos].b = for_tbs_pixel.b;
		pixels[pos].a = for_tbs_pixel.a;
		free(tbs_pixel_window); 
	}
	return img;

}

/*
* Determines which TBS pixels have a set neighbor
* and stores these TBS pixels in an array of TBS pixels
*/
TBSPixel * count_neighbors(Image * new_img,  int * num_neighbor_counts) {

	unsigned int total_new_img_pixels = new_img->width * new_img->height;

	// variables for Pixel array and TBSPixels array
	Pixel * pixels = new_img->pixels;
	int capacity = 1;
 	TBSPixel * tbs_pixels = (TBSPixel*) malloc(sizeof(TBSPixel) * capacity);
	
	if (tbs_pixels == NULL) {
		fprintf(stderr, "Error: Unable to allocate memory for array of TBS pixels\n");
		return NULL;
	}
	
	// other important variables
	int TBSPixel_index = 0;
	int pixel_col = 0;
	int pixel_row = 0;
	int position = 0;
	int neighbor_count = 0;

	// loop that will iterate through all of the pixels in new_img
	for (unsigned int i = 0; i < total_new_img_pixels; i++) {
		if (pixels[i].a == 0) {
			pixel_col = i % new_img->width;
			pixel_row = i / new_img->width;
			position = determine_position(i, new_img->width, new_img->height);

			// counts the number of set neighbors around the TBS pixel given its position
			switch (position) {
				case 0:
					count_for_top(i, pixels, &neighbor_count, new_img->width);
					break;
				case 1:
					count_for_bottom_right(i, pixels, &neighbor_count, new_img->width);
					break;
				case 2:
					count_for_bottom_left(i, pixels, &neighbor_count, new_img->width);
					break;
				case 3:
					count_for_bottom(i, pixels, &neighbor_count, new_img->width);
					break;
				case 4:
					count_for_top_right(i, pixels, &neighbor_count, new_img->width);
					break;
				case 5:
					count_for_right(i, pixels, &neighbor_count, new_img->width);
					break;
				case 6:
					count_for_left(i, pixels, &neighbor_count, new_img->width);
					break;
				case 7:
					count_for_other(i, pixels, &neighbor_count, new_img->width);
				    break;
				}
            
			// only includes TBS pixels with more than set neighbor to tbs_pixels array
			if (neighbor_count > 0){
			  	tbs_pixels[TBSPixel_index].idx.x = pixel_col;
                tbs_pixels[TBSPixel_index].idx.y = pixel_row;
                tbs_pixels[TBSPixel_index].neighborCount = neighbor_count;
			  	TBSPixel_index++;
			}

			// allocates for memory for tbs_pixels
			if (TBSPixel_index == capacity && i != total_new_img_pixels - 1) {
				capacity++;
				tbs_pixels = (TBSPixel *) realloc(tbs_pixels, sizeof(TBSPixel) * capacity);
			}

			neighbor_count = 0;

		}
	}

	*num_neighbor_counts = TBSPixel_index; 
	return tbs_pixels;
}

/*
* Determines the position of a TBS pixel
*/
int determine_position(unsigned int i, unsigned int width, unsigned int height) {

	if (i + width >= width * height) {
		// bottom right
		if (i + 1 == width * height) {
			return 1;
		}
		// bottom left
		else if (i == width * height - width) {
			return 2;
		}
		// bottom
		else {
			return 3;
		}
	}
	else if ((i + 1) % width == 0) {
		// top right
		if (i + 1 == width) {
			return 4;
		}
		// right
		else {
			return 5;
		}
	}
	else if (i < width) {
		// top
		return 0;
	}
	else if (i % width == 0) {
		// left
		return 6;
	}
	else {
		// other
		return 7;
	}
}

/*
* Counts the number of set neighbors around a TBS pixel at the top of the image
*/
void count_for_top(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	if (pixel[i - 1].a == 255) { // left pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + 1].a == 255) { // right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width].a == 255) { // bottom
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width - 1].a == 255) { // bottom left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width + 1].a == 255) { // bottom right
		(*tbs_neighbor_tracker)++;
	}
}

/*
* Counts the number of set neighbors around a TBS pixel at the bottom right of the image
*/
void count_for_bottom_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {

	if (pixel[i - width].a == 255) { // top pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width - 1].a == 255) { // top left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - 1].a == 255) { // left
		(*tbs_neighbor_tracker)++;
	}
}

/*
* Counts the number of set neighbors around a TBS pixel at the bottom left of the image
*/
void count_for_bottom_left(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {

	if (pixel[i - width].a == 255) { // top pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width + 1].a == 255) { // top right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + 1].a == 255) { // right
		(*tbs_neighbor_tracker)++;
	}
}

/*
* Counts the number of set neighbors around a TBS pixel at the bottom of the image
*/
void count_for_bottom(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {

	if (pixel[i - width].a == 255) { // top
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width + 1].a == 255) { // top right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width - 1].a == 255) { // top left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - 1].a == 255) { // left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + 1].a == 255) { // right
		(*tbs_neighbor_tracker)++;
	}
}

/*
* Counts the number of set neighbors around a TBS pixel at the top right of the image
*/
void count_for_top_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {

	if (pixel[i - 1].a == 255) { // left pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width].a == 255) { // bottom
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width - 1].a == 255) { // bottom left
		(*tbs_neighbor_tracker)++;
	}
}

/*
* Counts the number of set neighbors around a TBS pixel at the right of the image
*/
void count_for_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {

	if (pixel[i - width].a == 255) { // top pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width - 1].a == 255) { // top left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - 1].a == 255) { // left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width].a == 255) { // bottom
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width - 1].a == 255) { //bottom left
		(*tbs_neighbor_tracker)++;
	}
}

/*
* Counts the number of set neighbors around a TBS pixel at the left of the image
*/
void count_for_left(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {

	if (pixel[i - width].a == 255) { // top pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width + 1].a == 255) { // top right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + 1].a == 255) { // right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width].a == 255) { // bottom
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width + 1].a == 255) { // bottom right
		(*tbs_neighbor_tracker)++;
	}
}

/*
* Counts the number of set neighbors around a TBS pixel that are not found
* at the corners of edges of the image
*/
void count_for_other(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {

	if (pixel[i - width].a == 255) { // top pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width + 1].a == 255) { // top right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width - 1].a == 255) { //top left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - 1].a == 255) { // left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + 1].a == 255) { // right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width].a == 255) { // bottom
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width - 1].a == 255) { // bottom left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width + 1].a == 255) { // bottom right
		(*tbs_neighbor_tracker)++;
	}
}

/* 
* Creates a new image with the exemplar image in the top left area of the new image
*/
Image * place_image(unsigned int width, unsigned int height, Image * exemplar_image) {
	
	// width and height of new image are greater than width and the height of exemplar image
	if ((width < exemplar_image->width) || (height < exemplar_image->height)) {
    	fprintf(stderr, "Error: Width or height of exemplar image are greater than width or height of the new image.\n");
		return NULL;
  	}
  
	// allocating memory for and setting the width and the height of the new image
	Image * new_img = (Image *) malloc(sizeof(Image));

	if (new_img == NULL) {
		fprintf(stderr, "Error: Unable to allocate memory for new image\n");
		return NULL;
	}
	
	new_img->width = width;
	new_img->height = height;
	new_img->pixels = (Pixel *) malloc(sizeof(Pixel) * width * height);

	if (new_img->pixels == NULL) {
		fprintf(stderr, "Error: Unable to allocate memory for pixels of new image\n");
		FreeImage(&new_img);
		return NULL;
	}
	
	// creating pointers to Pixel arrays of the new image and exemplar image
	Pixel * new_pixels = new_img->pixels;
	Pixel * exemplar_pixels = exemplar_image->pixels;

	// variables that are helpful in calculating the position of the exemplar pixel in the new image
  	unsigned int num_rows = 0;
  	unsigned int num_pixels = 0;

	// "copies" the exemplar image into the new image, the remaining pixels will be black
	for(unsigned int i = 0; i < width * height; i++) {
	
		if (i >= (num_rows * width) && i <= (num_rows * width + exemplar_image->width - 1) && !(num_rows >= exemplar_image->height)) {
			int exemp_pos = i - num_rows * (((num_rows + 1) * (width) - 1) - ((num_rows) * (width) + exemplar_image->width - 1));
			(new_pixels[i]).r = (exemplar_pixels[exemp_pos]).r;
			(new_pixels[i]).g = (exemplar_pixels[exemp_pos]).g;
			(new_pixels[i]).b = (exemplar_pixels[exemp_pos]).b;
			(new_pixels[i]).a = (unsigned char) 255; 
	  	}
		else {
			(new_pixels[i]).r = (unsigned char) 0;
        	(new_pixels[i]).g = (unsigned char) 0;
        	(new_pixels[i]).b = (unsigned char) 0;
			(new_pixels[i]).a = (unsigned char) 0;
	  	}
		
		num_pixels++;

		if (num_pixels % width == 0) {
			num_rows++;
	  	}
  	}

  	return new_img;
  
}

// compares tbs pixels 
int CompareTBSPixels( const void *v1 , const void *v2 )
{

	const TBSPixel *tp1 = (const TBSPixel *)v1;
	const TBSPixel *tp2 = (const TBSPixel *)v2;
	int d = tp1->neighborCount - tp2->neighborCount;
	if( d ) return -d;
	d = tp1->idx.y - tp2->idx.y;
	if( d ) return d;
	d = tp1->idx.x - tp2->idx.x;
	if( d ) return d;
	return tp2->r - tp1->r;
}

// sorts tbs pixels, returns zero if succeeded
int SortTBSPixels( TBSPixel *tbsPixels , unsigned int sz )
{

  unsigned int * permutation = (unsigned int*)malloc( sizeof(unsigned int)*sz);
	if( !permutation )
	{
		fprintf( stderr , "[ERROR] Failed to allocate memory for permutation: %d\n" , sz );
		return 1;
		exit(1);
	}
	for( unsigned int i=0 ; i<sz ; i++ ) permutation[i] = i;
	for( unsigned int i=0 ; i<sz ; i++ )
	{
		unsigned int i1 = rand() % sz;
		unsigned int i2 = rand() % sz;
		unsigned int tmp = permutation[i1];
		permutation[i1] = permutation[i2];
		permutation[i2] = tmp;
	}

	for( unsigned int i=0 ; i<sz ; i++ ) tbsPixels[i].r = permutation[i];
	free( permutation );

	qsort( tbsPixels , sz , sizeof( TBSPixel ) , CompareTBSPixels );

	return 0;
}

/*
* Creates a new image with user inputted width and height based on the input (exemplar) image
*/
Image *SynthesizeFromExemplar(Image *exemplar , unsigned int outWidth , unsigned int outHeight , unsigned int windowRadius)
{

	Image * new_image = place_image(outWidth, outHeight, exemplar);
	
	if (new_image == NULL) {
		fprintf(stderr, "Error: Unable to set exemplar pixels in new image\n");
		return NULL;
	}
	
	int num_neighbor_counts = 0; // the number of TBS pixels with more than one set neighor
	TBSPixel * tbs_pixels  = count_neighbors(new_image, &num_neighbor_counts);
	double * gaussian = compute_gaussian(windowRadius);
	
	// iterates while there are still unset TBS pixels
	while (num_neighbor_counts > 0) {
		SortTBSPixels(tbs_pixels, num_neighbor_counts);
		new_image = set_TBS_Pixels(num_neighbor_counts, tbs_pixels, windowRadius, new_image, exemplar, gaussian);
		free(tbs_pixels);
		tbs_pixels  = count_neighbors(new_image, &num_neighbor_counts);
	}
    
	free(tbs_pixels);
	free(gaussian);
	FreeImage(&exemplar);
	return new_image;
}

