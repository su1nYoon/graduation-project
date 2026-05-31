// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "image.h"
#include "bmp.h"
#include "ui.h"
#include "app.h"


static void restoreTerminal(void)    //터미널 화면 상태 복구
{
    printf("\x1b[?25h\x1b[?1049l");
    fflush(stdout);
}


int main(void) {

#ifndef _WIN32
    printf("\x1b[8;27;93t"); // 터미널 창 크기를 27x93으로 설정
    fflush(stdout);

    printf("\x1b[?1049h\x1b[?25l"); // 대체 화면 진입 + 커서 숨김
    fflush(stdout);

    atexit(restoreTerminal); // 종료 시 복구 함수 호출
#endif

    Screen scr = SCR_WELCOME;

    char filename[256] = {0};
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER ih;

    Image *original = NULL; 
    Image *work = NULL;    

    for (;;)
    {
        if (scr == SCR_WELCOME)
        {
            showWelcome();

            // 기존 이미지 정리 
            if (work)
            {
                freeImage(work);
                work = NULL;
            }
            if (original)
            {
                freeImage(original);
                original = NULL;
            }

            // 파일명 입력 실패
            if (scanf("%255s", filename) != 1)
            { 
                clearInput();
                clearScreen();
                showMenuBar(0);
                showMenuBar2();
                printf("┃                                                                                          ┃\n");
                printf("┃ 파일 이름 입력 오류                                                                      ┃\n");
                for (int i = 0; i < 19; i++)
                {
                    printf("┃                                                                                          ┃\n");
                }
                printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
                printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
                waitEnter();
                continue;
            }

            // 이미지 로드 실패
            original = loadBMP_Image(filename, &fh, &ih);
            if (!original)
            {
                clearScreen();
                showMenuBar(0);
                showMenuBar2();
                printf("┃                                                                                          ┃\n");
                printf("┃ 이미지 로드 실패: %s                                                                      ┃\n", filename);
                printf("┃                                                                                          ┃\n");
                printf("┃ 해당 파일이 현재 디렉토리에 없습니다.                                                    ┃\n");
                printf("┃ 이 프로그램은 bmp 파일만 지원합니다.                                                     ┃\n");
                printf("┃ 파일명을 .bmp까지 입력해주세요.                                                          ┃\n");
                for (int i = 0; i < 15; i++)
                {
                    printf("┃                                                                                          ┃\n");
                }
                printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
                printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
                waitEnter();
                continue;
            }

            // 작업본 생성 실패
            work = cloneImage(original);
            if (!work)
            {
                clearScreen();
                showMenuBar(3);
                printf("\n메모리 할당 실패\n");
                freeImage(original);
                original = NULL;
                waitEnter();
                continue;
            }
            scr = SCR_MAIN;
        }

        if (scr == SCR_MAIN)
            handleMainMenu(&scr, filename);

        else if (scr == SCR_FILE)
            handleFileMenu(&scr, filename, &original, &work, &fh, &ih);

        else if (scr == SCR_FILTER)
            handleFilterMenu(&scr, original, &work, &fh, &ih, filename);

        else if (scr == SCR_IMAGE)
            handleImageMenu(&scr, original, &work, &fh, &ih, filename);

        else if (scr == SCR_HELP)
            handleHelpMenu(&scr, filename);
    }

    // 자원 정리
    if (work)
        freeImage(work);
    if (original)
        freeImage(original);
        
    return 0;
}
