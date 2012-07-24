/*
 *  Image.h
 *  Projects
 *
 *  Created by Greg Hoffman on 2/23/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <vector>

class Image
{
public:
  Image();
  void Set(int width, int height, int mode);
  virtual ~Image();
  
  int GetWidth() { return (int)width; }
  int GetHeight() { return (int)height; }
  int GetType() { return colorMode; }
  unsigned char *GetData() { return imageData; }
  std::vector<double> GetHeightData();
  void SetRGB(int x, int y, double r, double g, double b);
  
  bool Save(const char *filename);
  
  // temp 
  static bool SaveImage(const char *filename, short int w, short int h, unsigned char *data){
    return SaveTGA(filename, w, h, data);
  }
  bool LoadImage(const char *filename){
    return LoadTGA(filename);
  }
  
protected:
  bool LoadTGA(const char *filename);
  static bool SaveTGA(const char *filename, short int w, short int h, unsigned char *data);
  
private:
  short int width;
  short int height;
  int colorMode; // 3 for rgb, 4 for rgba
  unsigned char bitCount;
  
  unsigned char *imageData;
};

#endif