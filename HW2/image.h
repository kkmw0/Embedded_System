#include <stdio.h>
#include <stdlib.h>

typedef struct tagBITMAPFILEHEADER
{
    unsigned short  bfType;  //specifies the file type
    unsigned int bfSize;  //specifies the size in bytes of the bitmap file
    unsigned short bfReserved1;  //reserved; must be 0
    unsigned short bfReserved2;  //reserved; must be 0
    unsigned int bfOffBits;  //specifies the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    unsigned int biSize;  //specifies the number of bytes required by the struct
    int biWidth;  //specifies width in pixels
    int biHeight;  //specifies height in pixels
    unsigned short biPlanes;  //specifies the number of color planes, must be 1
    unsigned short biBitCount;  //specifies the number of bits per pixel
    unsigned int biCompression;  //specifies the type of compression
    unsigned int  biSizeImage;  //size of image in bytes
    unsigned int  biXPelsPerMeter;  //number of pixels per meter in x axis
    unsigned int  biYPelsPerMeter;  //number of pixels per meter in y axis
    unsigned int  biClrUsed;  //number of colors used by the bitmap
    unsigned int  biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER;

int LoadBitmapFile(char* filename, int* const width, int* const height, unsigned char* bitmapImage);
