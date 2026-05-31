// image.h
#ifndef IMAGE_H
#define IMAGE_H

// === 데이터 구조 ===
typedef struct {
    unsigned char b, g, r;  // BMP는 b, g, r 순서
} Pixel;

typedef struct {
    int width;
    int height;
    Pixel **data;   // 2차원: data[y][x]
} Image;

// === 메모리/복제 ===
Image* allocImage(int width, int height);
void freeImage(Image* img);
Image* cloneImage(Image* src);
void copyImage(Image* src, Image* dst);

// === 기초 이미지 연산 ===
Image* imgRotate90(Image* src);   // 시계방향 90도 회전
void imgFlipH(Image* img);    // 좌우 반전
void imgFlipV(Image* img);    // 상하 반전
Image* imgResizeNearest(Image* src, int newW, int newH);  // 최근접 보간 리사이즈

// === 필터 ===
void convGrayscale(Image* img);   // 0.299R + 0.587G + 0.114B
void convInvert(Image* img);     // 255 - old
void convThreshold(Image* img, unsigned char t);    // gray>=t ? 255 : 0

#endif // IMAGE_H
