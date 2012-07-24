/*
 *  Image.cpp
 *  Projects
 *
 *  Created by Greg Hoffman on 2/23/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Image.h"
#include <iostream>
#include <stdio.h>


Image::Image()
{
  imageData = 0;
  width = height = colorMode = 0;
  bitCount = 0;
}

void Image::Set(int width, int height, int mode)
{
  this->width = width;
  this->height = height;
  colorMode = mode;
  // allocate the memory
  long totalSize = width*height*mode;
  imageData = (unsigned char*)malloc(sizeof(unsigned char)*totalSize);
}

Image::~Image()
{
  // if image still here, remove it
  if ( imageData != 0 )
  {
    free(imageData);
  }
}

bool Image::Save(const char* filename)
{
  return SaveImage(filename, width, height, imageData);
}

std::vector<double> Image::GetHeightData()
{
  std::vector<double> temp;
  long length = width*height*colorMode;
  long id = 0;
  
  // loop through each color and average them to the new "height val"
  for (; id < length; id += colorMode)
  {
    double val = imageData[id]/255.0 
    + imageData[id+1]/255.0 
    + imageData[id+2]/255.0;
    val = val / ( 3.0 );
    temp.push_back(val);
  }
  return temp;
}

void Image::SetRGB(int x, int y, double r, double g, double b)
{
  int index = (y*width + x)*colorMode;
  imageData[index] = (unsigned char)(r*255);
  imageData[index+1] = (unsigned char)(g*255);
  imageData[index+2] = (unsigned char)(b*255);
}

bool Image::SaveTGA(const char *filename, short int w, short int h, unsigned char *data)
{
  unsigned char bskip;
  short int iskip;
  unsigned char imageType;
  unsigned char colorSwap;
  int cm;
  long imageIdx;
  unsigned char bitDepth;
  long imageSize;
  FILE *file;
  
  // create file for writing binary mode
  fopen_s(&file, filename, "wb");
  if ( !file )
  {
    fclose(file);
    return false;
  }
  
  imageType = 2; // RGB UNCOMPRESSED
  bitDepth = 24;
  cm = 3;
  
  bskip = 0;
  iskip = 0;
  
  // write blank data
  fwrite(&bskip, sizeof(unsigned char), 1, file);
  fwrite(&bskip, sizeof(unsigned char), 1, file);
  
  // image type
  fwrite(&imageType, sizeof(unsigned char), 1, file);
  
  // blank data
  fwrite(&iskip, sizeof(short int), 1, file);
  fwrite(&iskip, sizeof(short int), 1, file);
  fwrite(&bskip, sizeof(unsigned char), 1, file);
  fwrite(&iskip, sizeof(short int), 1, file);
  fwrite(&iskip, sizeof(short int), 1, file);
  
  // write dimensions
  fwrite(&w, sizeof(short int), 1, file);
  fwrite(&h, sizeof(short int), 1, file);
  fwrite(&bitDepth, sizeof(unsigned char), 1, file);
  
  // byte 'o' blank data
  fwrite(&bskip, sizeof(unsigned char), 1, file);
  
  imageSize = w*h*cm;
  
  // change to BGR from RGB
  for ( imageIdx = 0; imageIdx < imageSize; imageIdx += cm )
  {
    colorSwap = data[imageIdx];
    data[imageIdx] = data[imageIdx + 2];
    data[imageIdx + 2] = colorSwap;
  }
  
  // write out our data baby
  fwrite(data, sizeof(unsigned char), imageSize, file);
  
  // close the file and return success
  fclose(file);
  return true;
}

bool Image::LoadTGA(const char *filename)
{
  FILE *file;
  unsigned char gchar;
  short int gint;
  long imageSize;
  long imageIdx;
  unsigned char swap;
  unsigned char imageType;
  
  // open and error check
  fopen_s(&file, filename, "rb");
  if ( !file )
    return false;
  
  // start reading in header, discard garbage bytes
  fread(&gchar, sizeof(unsigned char), 1, file);
  fread(&gchar, sizeof(unsigned char), 1, file);
  
  // get type
  fread(&imageType, sizeof(unsigned char), 1, file);
  
  // check to make sure its a 2 or 3
  if ( (imageType != 2) && (imageType != 3) )
  {
    fclose(file);
    return false;
  }
  
  // read other data we don't need
  fread(&gint, sizeof(short int), 1, file);
  fread(&gint, sizeof(short int), 1, file);
  fread(&gchar, sizeof(unsigned char), 1, file);
  fread(&gint, sizeof(short int), 1, file);
  fread(&gint, sizeof(short int), 1, file);
  
  // get dimensions
  fread(&width, sizeof(short int), 1, file);
  fread(&height, sizeof(short int), 1, file);
  
  // bit depth
  fread(&bitCount, sizeof(unsigned char), 1, file);
  
  // more garbade
  fread(&gchar, sizeof(unsigned char), 1, file);
  
  // color mode 3 = BGR, 4 = BGRA
  colorMode = bitCount / 8;
  imageSize = width*height*colorMode;
  
  
  // allocate memory for image
  imageData = (unsigned char*)malloc(sizeof(unsigned char)*imageSize);
  
  // read in image data
  fread(imageData, sizeof(unsigned char), imageSize, file);
  
  // change to rgb for open gl
  for (imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
  {
    swap = imageData[imageIdx];
    imageData[imageIdx] = imageData[imageIdx + 2];
    imageData[imageIdx + 2] = swap;
  }
  
  // close the file and end
  fclose(file);
  return true;
}