#ifndef TEXTURE_SYNTHESIS_INCLUDED
#define TEXTURE_SYNTHESIS_INCLUDED
#include "image.h"

/** A struct storing information about a to-be-synthesized pixel*/
typedef struct
{
	/** The index of the pixel to be synthesized*/
	PixelIndex idx;
	
	/** The number of neighboring pixels in a set state*/
	unsigned int neighborCount;
	
	/** A member for storing a value used to resolve ties*/
	unsigned int r;
} TBSPixel;

double * compute_gaussian(int r);

/**This is a function that sets the TBS Pixels**/

Image * set_TBS_Pixels(int TBSPixel_arr_size, TBSPixel * tbs_pixels, int r, Image * img, Image * exemp, double * gaussian);

/**This is a function that finds the minimum difference in values for future pixel setting**/
PixelDiff find_minimum_difference(PixelDiff * diff_array, int exemp_width, int exemp_height);

/**This is a function that compares the two windows we generated**/
PixelDiff * compare_windows(Pixel * tbs_pixel_window, Image * img,  Image * exemp, int r, double * gaussian);

/**This function finds the squared diffirence between the pixel values of the two windows**/
double find_difference(Pixel * tbs_pixel_window, Pixel * exemp_pixel_window, int r, double * gaussian);

/**This is a function that creates a window for TBS Pixels**/
Pixel * create_TBS_pixel_window(int r, TBSPixel TBSPixel, Pixel * pixels, int width, int height);

/**This is a function that creates a window for Exemplar Pixels**/
Pixel * create_exemplar_window(int r, int index, int width, int height, Pixel * pixels);


/**This is a function that determines the number of set neighbors for each unset pixels**/
TBSPixel * count_neighbors(Image * new_image, int * num_neighbor_counts);

/**This is a function that copies the examplar image and places it in top-left corner of a new image with dimensions that the user specifies */
Image * place_image(unsigned int width, unsigned int height,  Image * image);

/** A function that compares two TBSPixels and returns a negative number if the first should come earlier in the sort order and a positive number if it should come later*/
int CompareTBSPixels( const void *v1 , const void *v2 );

/** A function that sorts an array of TBSPixels*/
int SortTBSPixels( TBSPixel *tbsPixels , unsigned int sz );

/** A function that extends the exemplar into an image with the specified dimensions, using the prescribed window radius -- the verbose argument is passed in to enable logging to the command prompt, if desired*/
Image *SynthesizeFromExemplar(  Image *exemplar , unsigned int outWidth , unsigned int outHeight , unsigned int windowRadius); // bool verbose );

/** Counter functions for different pixels **/
int determine_position(unsigned int i, unsigned int width, unsigned int height);
void count_for_top(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width);
void count_for_bottom_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width);
void count_for_bottom_left(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width);
void count_for_bottom(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width);
void count_for_top_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width);
void count_for_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width);
void count_for_left(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width);
void count_for_other(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width);

#endif // TEXTURE_SYNTHESIS_INCLUDED
