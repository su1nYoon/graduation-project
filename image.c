// image.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"

// === 구현 ===
static void freeRows(Pixel **rows, int upto) {
    if (!rows) 
        return;
    for (int y = 0; y < upto; y++) 
        free(rows[y]);
    free(rows);
}

Image* allocImage(int width, int height) {
    if (width <= 0 || height <= 0) 
        return NULL;

    Image* img = (Image*)malloc(sizeof(Image));

    if (!img) 
        return NULL;

    img->width = width;
    img->height = height;

    img->data = (Pixel**)malloc((size_t)height * sizeof(Pixel*));
    if (!img->data) { 
        free(img); 
        return NULL; 
    }
    
    for (int y = 0; y < height; y++) {
        img->data[y] = (Pixel*)malloc((size_t)width * sizeof(Pixel));
        if (!img->data[y]) {
            freeRows(img->data, y); 
            free(img); 
            return NULL; 
        }
    }
    return img;
}

void freeImage(Image* img) {
    if (!img) 
        return;
    if (img->data) {
        for (int y = 0; y < img->height; y++) 
            free(img->data[y]);
        free(img->data);
    }
    free(img);
}

Image* cloneImage(Image* src) {
    if (!src || !src->data || src->width <= 0 || src->height <= 0) 
        return NULL;
    Image* dst = allocImage(src->width, src->height);
    if (!dst) 
        return NULL;
    for (int y = 0; y < src->height; y++)
        memcpy(dst->data[y], src->data[y], (size_t)src->width * sizeof(Pixel));
    return dst;
}

void copyImage(Image* src, Image* dst) {
    if (!src || !dst || !src->data || !dst->data) 
        return;
    if (src->width != dst->width || src->height != dst->height) 
        return;
    for (int y = 0; y < src->height; y++)
        memcpy(dst->data[y], src->data[y], (size_t)src->width * sizeof(Pixel));
}

// 회전: src(y,x) -> dst(x, H-1-y), dst 크기 = (H x W)
Image* imgRotate90(Image* src) {
    if (!src || !src->data || src->width <= 0 || src->height <= 0) 
        return NULL;
    int W = src->width, H = src->height;
    Image* dst = allocImage(H, W);
    if (!dst) 
        return NULL;
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            dst->data[x][H - 1 - y] = src->data[y][x];
    return dst;
}

void imgFlipH(Image* img) {
    if (!img || !img->data || img->width <= 0 || img->height <= 0) 
        return;
    int W = img->width, H = img->height;
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W / 2; x++) {
            Pixel tmp = img->data[y][x];
            img->data[y][x] = img->data[y][W - 1 - x];
            img->data[y][W - 1 - x] = tmp;
        }
    }
}

void imgFlipV(Image* img) {
    if (!img || !img->data || img->width <= 0 || img->height <= 0) 
        return;
    int H = img->height;
    for (int y = 0; y < H / 2; y++) {
        Pixel* tmp = img->data[y];
        img->data[y] = img->data[H - 1 - y];
        img->data[H - 1 - y] = tmp;
    }
}

Image* imgResizeNearest(Image* src, int newW, int newH) {
    if (!src || !src->data || src->width <= 0 || src->height <= 0) 
        return NULL;
    if (newW <= 0 || newH <= 0) 
        return NULL;
    int W = src->width, H = src->height;
    Image* dst = allocImage(newW, newH);
    if (!dst) 
        return NULL;
    double sx = (double)W / (double)newW;
    double sy = (double)H / (double)newH;
    for (int y = 0; y < newH; y++) {
        int syi = (int)(y * sy); if (syi >= H) syi = H - 1;
        for (int x = 0; x < newW; x++) {
            int sxi = (int)(x * sx); 
            if (sxi >= W) sxi = W - 1;
            dst->data[y][x] = src->data[syi][sxi];
        }
    }
    return dst;
}

// === 필터 ===
void convGrayscale(Image* img) {
    if (!img || !img->data) 
        return;
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            unsigned char r = img->data[y][x].r;
            unsigned char g = img->data[y][x].g;
            unsigned char b = img->data[y][x].b;
            unsigned char gray = (unsigned char)(0.299*r + 0.587*g + 0.114*b + 0.5);
            img->data[y][x].r = img->data[y][x].g = img->data[y][x].b = gray;
        }
    }
}

void convInvert(Image* img) {
    if (!img || !img->data) 
        return;
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].r = 255 - img->data[y][x].r;
            img->data[y][x].g = 255 - img->data[y][x].g;
            img->data[y][x].b = 255 - img->data[y][x].b;
        }
    }
}

void convThreshold(Image* img, unsigned char t) {
    if (!img || !img->data) 
        return;
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            unsigned char r = img->data[y][x].r;
            unsigned char g = img->data[y][x].g;
            unsigned char b = img->data[y][x].b;
            unsigned char gray = (unsigned char)(0.299*r + 0.587*g + 0.114*b + 0.5);
            unsigned char val = (gray >= t) ? 255 : 0;
            img->data[y][x].r = img->data[y][x].g = img->data[y][x].b = val;
        }
    }
}
