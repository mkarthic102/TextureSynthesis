#ifndef IMAGE_INCLUDED
#define IMAGE_INCLUDED

#include <stdbool.h>

/** A struct for storing pixel information*/
typedef struct
{
	/** The red channel of the image*/
	unsigned char r;

	/** The green channel of the image*/
	unsigned char g;

	/** The blue channel of the image*/
	unsigned char b;

	/** The alpha channel of the image*/
	unsigned char a;
} Pixel;

/** A struct for storing an image*/
typedef struct
{
	/** The width of the image*/
	unsigned int width;
	
	/** The height of the image*/
	unsigned int height;
	
	/** The pixels in the image, laid out row-by-row and, within each row, column-by-column*/
	Pixel *pixels;
} Image;

typedef struct
{
	Pixel pixel;

	double diff;
} PixelDiff;

/** A struct for indexing a pixel*/
typedef struct
{
	/** The horizontal position (column) of the pixel*/
	unsigned int x;

	/** The vertical position (row) of the pixel*/
	unsigned int y;
} PixelIndex;

/** A function that returns the sum-of-squared differences of the pixels' red, green, and blue color values (the alpha values are not used)*/
double PixelSquaredDifference(Pixel p1 , Pixel p2 );

/** A function that checks that a pixel index is within the bounds of the image*/
bool InBounds( const Image *image , PixelIndex idx );

/** A function returning a pointer to the pixel described by the index (or the NULL pointer if the index is out of bounds)*/
Pixel *GetPixel( Image *image , PixelIndex idx );


/** A function returning a const pointer to the pixel described by the index (or the NULL pointer if the index is out of bounds)*/
const Pixel *GetConstPixel( const Image *image , PixelIndex idx );


/** A function returning a new image object with prescribed width and height -- both the pixels member of the image and the image itself are dynamically allocated (the function returns NULL if it failed to allocate the image or its pixels)*/
Image *AllocateImage( unsigned int width , unsigned int height );

/** A function deallocating the memory associated to an image and sets the pointer to the image to NULL (this deallocates both the pixels member of the image and the image itself*/
void FreeImage( Image **img );


#endif // IMAGE_INCLUDED