#include "image.h"

int LoadBitmapFile(char* filename, int* const width, int* const height, unsigned char* bitmapImage)
{
    FILE *filePtr;  //our file pointer
    BITMAPFILEHEADER *bitmapFileHeader = NULL;  //our bitmap file header
    BITMAPINFOHEADER *bitmapInfoHeader = NULL;  //our bitmap info header
    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open file in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return -1;
        
    bitmapFileHeader = (BITMAPFILEHEADER *) malloc(sizeof(BITMAPFILEHEADER));
    bitmapInfoHeader = (BITMAPINFOHEADER *) malloc(sizeof(BITMAPINFOHEADER));
 
    //read the bitmap file header
    fread(bitmapFileHeader, 14,1,filePtr);

    //verify that this is a .BMP file by checking bitmap id
    ////printf("sizeof(BITMAPFILEHEADER): %lu \n",sizeof(BITMAPFILEHEADER));
    ////printf("bitmapFileHeader.bfType: %X \n",bitmapFileHeader->bfType);
    ////printf("bitmapFileHeader.bfSize: %u \n",bitmapFileHeader->bfSize);
    if (bitmapFileHeader->bfType !=0x4D42)
    {
        fclose(filePtr);
        return -1;
    }

    //read the bitmap info header
    fread(bitmapInfoHeader, 40,1,filePtr); 

    //move file pointer to the beginning of bitmap data
    fseek(filePtr, bitmapFileHeader->bfOffBits, SEEK_SET);
    
    *width  = bitmapInfoHeader->biWidth;
    *height = bitmapInfoHeader->biHeight;
    
    ////
    //printf("biWidth of loaded image: %d \n",bitmapInfoHeader->biWidth);
    //printf("biHeight of loaded image: %d \n",bitmapInfoHeader->biHeight);
    

    //allocate enough memory for the bitmap image data
    ////bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

    //verify memory allocation
    /*if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return -1;
    }*/

    //read in the bitmap image data
    fread(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr);
    
    ////
    //printf("biSizeImage of loaded image: %d \n",bitmapInfoHeader->biSizeImage);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return -1;
    }

    //swap the R and B values to get RGB (bitmap is BGR)
    for (imageIdx = 0;imageIdx < bitmapInfoHeader->biSizeImage;imageIdx+=3)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //close file and return bitmap image data
    free(bitmapFileHeader);
    free(bitmapInfoHeader);
    fclose(filePtr);
    return 0;
}

