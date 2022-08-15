#ifndef PPM_H_INCLUDED
#define PPM_H_INCLUDED

#include <stdio.h>
#include "image.h"

/** A function for reading in PPM files (assumes fp != NULL) */
Image *ReadPPM( FILE *fp );


/** A function for writing out PPM files (return -1 if any failure occurs, otherwise return the number of pixels written) */
int WritePPM( FILE* fp , const Image *img );


#endif // PPM_H_INCLUDED
