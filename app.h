#ifndef APP_H
#define APP_H

#include "image.h"
#include "bmp.h"


void ChangeMode(void);
int ReadyMode(Image *original, Image **pwork);

int getCommand(void);
void clearInput(void);

void FileOpen(char *filename, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, Image **poriginal, Image **pwork);
void FileSave(char *filename, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, Image *work);
void FileSaveAs(char *filename, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, Image *work);


// --상태 및 메인
typedef enum {
    SCR_WELCOME = 0, // 파일 입력
    SCR_MAIN,        // 메인 화면
    SCR_FILTER,      // 필터 하위 메뉴
    SCR_FILE,        // 파일 하위 메뉴
    SCR_IMAGE,       // 이미지 하위 메뉴
    SCR_HELP         // 도움말 하위 메뉴
} Screen;

// 메인 메뉴 처리
void handleMainMenu(Screen *scr, char *filename);

// 파일 메뉴 처리
void handleFileMenu(
    Screen *scr,
    char *filename,
    Image **original,
    Image **work,
    BITMAPFILEHEADER *fh,
    BITMAPINFOHEADER *ih
);

// 필터 메뉴 처리
void handleFilterMenu(
    Screen *scr,
    Image *original,
    Image **work,
    BITMAPFILEHEADER *fh,
    BITMAPINFOHEADER *ih,
    char *filename
);

// 이미지 메뉴 처리
void handleImageMenu(
    Screen *scr,
    Image *original,
    Image **work,
    BITMAPFILEHEADER *fh,
    BITMAPINFOHEADER *ih,
    char *filename
);

// 도움말 메뉴 처리
void handleHelpMenu(Screen *scr, char *filename);


#endif
