// bmp.h : 24bit BMP 읽기/저장
#ifndef BMP_H
#define BMP_H

#include "image.h"

// BMP 파일 헤더 (14바이트)
#pragma pack(push, 1)
typedef struct {
    unsigned short bfType;      // 'BM' (0x4D42)
    unsigned int bfSize;      // 파일 전체 크기
    unsigned short bfReserved1; // 0
    unsigned short bfReserved2; // 0
    unsigned int bfOffBits;   // 픽셀 데이터 시작 위치
} BITMAPFILEHEADER;

// BMP 정보 헤더 (BITMAPINFOHEADER, 40바이트)
typedef struct {
    unsigned int biSize;          // 40
    int biWidth;         // 가로
    int biHeight;        // 세로 (양수: bottom-up, 음수: top-down)
    unsigned short biPlanes;        // 1
    unsigned short biBitCount;      // 24
    unsigned int biCompression;   // 0 (BI_RGB)
    unsigned int biSizeImage;     // 이미지 데이터 크기 (row padding 포함)
    int biXPelsPerMeter; // 해상도
    int biYPelsPerMeter; // 해상도
    unsigned int biClrUsed;       // 0
    unsigned int biClrImportant;  // 0
} BITMAPINFOHEADER;
#pragma pack(pop)

// 입출력 API
Image* loadBMP_Image(char* filename, BITMAPFILEHEADER* fh, BITMAPINFOHEADER* ih);
int saveBMP_Image(char* filename, BITMAPFILEHEADER* fh, BITMAPINFOHEADER* ih, Image* img);

#endif // BMP_H
