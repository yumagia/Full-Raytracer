#ifndef IMAGE_INCLUDED
#define IMAGE_INCLUDED

#include "Math.h"

#include <math.h>
#include <cstring> // For memcpy
#include <stdint.h> // For uint8_t

class Image {
public:
    Image(int w, int h);
    ~Image();
    void SetPixel(int i, int j, Color c);
    void Write(const char* fileName);
private:
    uint8_t *ToBytes();
    Color &GetPixel(int i, int j);
private:
    int width, height;
    Color *pixels;
};

#endif