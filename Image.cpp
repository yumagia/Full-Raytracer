#include "Image.h"

Image::Image(int w, int h) {
	pixels = new Color[w * h];
}

Image::~Image() {
	delete[] pixels;
}

void Image::SetPixel(int i, int j, Color c) {
	pixels[i + j * width] = c;
}

Color& Image::GetPixel(int i, int j) {
	return pixels[i + j * width];
}

uint8_t* Image::ToBytes() {
	uint8_t *rawPixels = new uint8_t[width * height * 4];
	for(int i = 0; i < width; i++) {
		for(int j = 0; j < height; j++) {
			Color c = GetPixel(i, j);
			rawPixels[4 * (i + j * width) + 0] = uint8_t(fmin(c.r, 1) * 255);
			rawPixels[4 * (i + j * width) + 1] = uint8_t(fmin(c.g, 1) * 255);
			rawPixels[4 * (i + j * width) + 2] = uint8_t(fmin(c.b, 1) * 255);
			rawPixels[4 * (i + j * width) + 3] = 255; // Alpha
		}
	}

	return rawPixels;
}

void Image::Write(const char* fileName) {
	uint8_t *rawBytes = ToBytes();

	int lastc = strlen(fileName);

	switch(fileName[lastc - 1]) {
		case 'g': // Either jpeg (or jpg) or png
			if(fileName[lastc - 2] == 'p' || fileName[lastc - 2] == 'e') {	// jpeg or jpg
				stbi_write_jpg(fileName, width, height, 4, rawBytes, 95);	// 95% jpeg quality
			} 
			else {	//png
				stbi_write_png(fileName, width, height, 4, rawBytes, width * 4);
			}
		case 'a':	// tga (targa)
			stbi_write_tga(fileName, width, height, 4, rawBytes);
		case 'p':	// bmp
		default:
			stbi_write_bmp(fileName, width, height, 4, rawBytes);
	}

	delete[] rawBytes;
};