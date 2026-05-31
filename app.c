#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "app.h"
#include "ui.h"
#include "image.h"
#include "bmp.h"

#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"

// 적용 모드(누적/원본)
int Mode = 0;

void ChangeMode(void)
{
    Mode = !Mode; 
}

int ReadyMode(Image *original, Image **pwork)
{
    if (Mode == 0)
        return 1; 
    if (!original) 
        return 0;
    Image *fresh = cloneImage(original);
    if (!fresh)
        return 0;
    if (*pwork)
        freeImage(*pwork);
    *pwork = fresh;
    return 1;
}

void clearInput(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        /* discard */
    }
}

// 키 입력 처리 함수
int getCommand(void)
{
    int ch = getchar();

    if (ch == '\r')       // Enter 입력 시 '\r'을 '\n'으로 통일
        ch = '\n';

    if (ch == EOF)
        return EOF;      // 입력 오류 발생 시 EOF 반환

    if (ch == '\n')
        return '\n';     // Enter만 눌렀을 경우 그대로 반환

    return toupper((unsigned char)ch);       // 대문자로 변환하여 반환
}

// 현재 모드 표시
void ShowMode(void)
{
    printf("\n 현재 모드 : %s   (X: 모드 전환)\n", (Mode == 0) ? "누적" : "원본");
}

void showModeBox(int activeMenu)
{
    clearScreen();
    showMenuBar(activeMenu);
    showMenuBar2();
    printf("┃                                                                                          ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃ %-88s           ┃\n", (Mode == 0) ? "현재 모드 : 누적   (X: 모드 전환)" : "현재 모드 : 원본   (X: 모드 전환)");
    printf("┃                                                                                          ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃ 단축키를 사용하세요.                                                                     ┃\n");
    for (int i = 0; i < 15; i++)
    {
        printf("┃                                                                                          ┃\n");
    }
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    fflush(stdout);
}


// 미리보기 저장+열기
int previewSilent(BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, Image *img)
{
    char pv[] = "preview.bmp";
    if (saveBMP_Image(pv, fh, ih, img))
    {
        system("open \"preview.bmp\"");
        return 1; // 성공
    }
    return 0; // 실패
}


// --파일 코드
// 파일 존재 여부
int FileExists(char *path)
{
    FILE *fp = fopen(path, "rb");
    if (fp)
    {
        fclose(fp);
        return 1;
    }
    return 0;
}

// 파일 덮어쓰기 여부 확인
int FileConfirm(char *path, int row, int col)
{
    if (!FileExists(path))
        return 1; 

    char line[32];

    for (;;) {
        gotoxy(row + 3, col);
        printf("이미 존재하는 파일입니다. 덮어쓸까요? (Y/N): ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            clearerr(stdin);    
            continue;           
        }

        int i = 0;
        while (line[i] == ' ' || line[i] == '\t') 
            i++;

        int ch = toupper((unsigned char)line[i]);
        if (ch == 'Y') 
            return 1; 
        if (ch == 'N') 
            return 0;

        gotoxy(row + 4, col);
        printf("Y 또는 N만 입력하세요.                              ");
        fflush(stdout);
    }
}

// 파일 열기
void FileOpen(char *filename, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, Image **poriginal, Image **pwork)
{
    char input[256];

    clearScreen();
    showMenuBar(1);
    showMenuBar2();
    for (int i = 0; i < 20; i++)
        printf("┃                                                                                          ┃\n");
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");

    // 제목
    gotoxy(4, 3);
    printf("[파일] 새로 열기(O)");

    // 프롬프트
    gotoxy(6, 3);
    printf("파일 이름(.bmp까지): ");
    fflush(stdout);

    // 입력
    clearInput();
    if (!fgets(input, sizeof(input), stdin)) {
        clearerr(stdin);
        gotoxy(7, 3);
        printf("입력 오류로 취소되었습니다.");
        return;
    }
    input[strcspn(input, "\r\n")] = '\0';

    if (input[0] == '\0') {
        gotoxy(7, 3);
        printf("입력이 비었습니다. 취소합니다.");
        gotoxy(8, 3);
        printf("☛ 계속하려면 Enter를 누르세요...");
        waitEnter();
        return;
    }

    // 기존 이미지 정리
    if (*poriginal) { 
        freeImage(*poriginal); 
        *poriginal = NULL; 
    }
    if (*pwork) {
        freeImage(*pwork);     
        *pwork = NULL; 
    }

    // 로드
    *poriginal = loadBMP_Image(input, fh, ih);
    if (!*poriginal) {
        gotoxy(8, 3);
        printf("파일 열기 실패! (bmp 파일인지 / 경로 확인)");
        gotoxy(9, 3);
        printf("☛ 계속하려면 Enter를 누르세요...");
        waitEnterKey();
        return;
    }

    *pwork = cloneImage(*poriginal);
    strncpy(filename, input, 255);
    filename[255] = '\0';

    // 성공 메시지
    gotoxy(8, 3);
    printf("열기 완료: %s (%d x %d)", filename, ih->biWidth, ih->biHeight);
    gotoxy(9, 3);
    printf("☛ 계속하려면 Enter를 누르세요...");
    waitEnterKey();
}

// 현재 파일명으로 저장
void FileSave(char *filename, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, Image *work)
{
    if (!work) {
        clearScreen();
        showMenuBar(0); 
        showMenuBar2();
        printf("┃                                                                                          ┃\n");
        printf("┃ [저장]                                                                                   ┃\n");
        printf("┃                                                                                          ┃\n");
        printf("┃ 저장할 이미지가 없습니다.                                                                ┃\n");
        for (int i = 0; i < 16; ++i) printf("┃                                                                                          ┃\n");
        printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
        printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
        waitEnter();
        return;
    }
    if (!filename || !filename[0]) {
        clearScreen();
        showMenuBar(0); 
        showMenuBar2();
        printf("┃                                                                                          ┃\n");
        printf("┃ [저장]                                                                                   ┃\n");
        printf("┃                                                                                          ┃\n");
        printf("┃ 현재 파일 이름이 없습니다. '다른 이름으로 저장(A)'를 사용하세요.                        ┃\n");
        for (int i = 0; i < 16; ++i) printf("┃                                                                                          ┃\n");
        printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
        printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
        waitEnter();
        return;
    }

    if (!FileConfirm(filename, 8, 3)) {
        clearScreen();
        showMenuBar(0); 
        showMenuBar2();
        printf("┃                                                                                          ┃\n");
        printf("┃ [저장]                                                                                   ┃\n");
        printf("┃                                                                                          ┃\n");
        printf("┃ 저장을 취소했습니다.                                                                     ┃\n");
        for (int k = 0; k < 17; ++k) 
            printf("┃                                                                                          ┃\n");
        printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
        printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
        waitEnter();
        return;
    }

    int ok = saveBMP_Image(filename, fh, ih, work);

    clearScreen();
    showMenuBar(0); 
    showMenuBar2();
    printf("┃                                                                                          ┃\n");
    printf("┃ [저장]                                                                                   ┃\n");
    printf("┃                                                                                          ┃\n");
    if (ok) {
        printf("┃ 저장 완료: %-76.76s ┃\n", filename);
    } else {
        printf("┃ 저장 실패: %-76.76s ┃\n", filename);
    }
    for (int i = 0; i < 12; ++i) printf("┃                                                                                          ┃\n");
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    waitEnter();
}

// 다른 이름으로 저장
void FileSaveAs(char *filename, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, Image *work)
{
    if (!work) {
        clearScreen();
        showMenuBar(0); 
        showMenuBar2();
        printf("┃                                                                                          ┃\n");
        printf("┃ [다른 이름으로 저장]                                                                      ┃\n");
        printf("┃                                                                                          ┃\n");
        printf("┃ 저장할 이미지가 없습니다.                                                                 ┃\n");
        for (int i = 0; i < 16; ++i) printf("┃                                                                                          ┃\n");
        printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
        printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
        waitEnter();
        return;
    }

    char label[] = "새 파일 이름 : ";
    char newname[256];

    for (;;) {
        clearScreen();
        showMenuBar(0); 
        showMenuBar2();
        printf("┃                                                                                          ┃\n"); // row 4
        printf("┃ [다른 이름으로 저장]                                                                     ┃\n"); // row 5
        printf("┃                                                                                          ┃\n"); // row 6
        printf("┃                                                                                          ┃\n"); // row 7
        for (int i = 0; i < 17; ++i) printf("┃                                                                                          ┃\n");
        printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
        printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
        fflush(stdout);

        gotoxy(7, 3);
        printf("%s", label);

        fflush(stdout);
        clearInput();
        if (!fgets(newname, sizeof(newname), stdin)) {
            clearerr(stdin);
            continue;
        }
        newname[strcspn(newname, "\r\n")] = '\0';
        if (newname[0] == '\0') 
            continue;

        if (!FileConfirm(newname, 8, 3)) {
            clearScreen();
            showMenuBar(0); 
            showMenuBar2();
            printf("┃                                                                                          ┃\n");
            printf("┃ [다른 이름으로 저장]                                                                     ┃\n");
            printf("┃                                                                                          ┃\n");
            printf("┃ 저장을 취소했습니다.                                                                       ┃\n");
            for (int i = 0; i < 16; ++i) printf("┃                                                                                          ┃\n");
            printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
            printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
            waitEnter();
            return;
        }

        // 저장
        int ok = saveBMP_Image(newname, fh, ih, work);
        if (ok) {
            strncpy(filename, newname, 255);
            filename[255] = '\0';
        }

        clearScreen();
        showMenuBar(0); 
        showMenuBar2();
        printf("┃                                                                                          ┃\n");
        printf("┃ [다른 이름으로 저장]                                                                     ┃\n");
        printf("┃                                                                                          ┃\n");
        if (ok) printf("┃ 저장 완료: %-76.76s ┃\n", newname);
        else     printf("┃ 저장 실패: %-76.76s ┃\n", newname);
        for (int i = 0; i < 16; ++i) printf("┃                                                                                          ┃\n");
        printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
        printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
        waitEnter();
        return;
    }
}


// --필터 코드
// 필터 적용 후 미리보기
void Filter_G(Image *work, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih)
{
    convGrayscale(work);
    int ok = previewSilent(fh, ih, work);
    char msg1[128], msg2[128];
    clearScreen();
    showMenuBar(1);

    snprintf(msg1, sizeof(msg1), "[필터] " BLUE "그레이스케일(G)" RESET " 적용 완료                          ┃\n");
    snprintf(msg2, sizeof(msg2), ok ? "미리보기 생성: preview.bmp" : "미리보기 저장 실패");
    showFilterMenu2(msg1, msg2);
}

void Filter_I(Image *work, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih)
{
    convInvert(work);
    int ok = previewSilent(fh, ih, work);
    char msg1[128], msg2[128];

    clearScreen();
    showMenuBar(1);

    snprintf(msg1, sizeof(msg1), "[필터] " BLUE "색 반전(I)" RESET " 적용 완료                               ┃\n");
    snprintf(msg2, sizeof(msg2), ok ? "미리보기 생성: preview.bmp" : "미리보기 저장 실패");
    showFilterMenu2(msg1, msg2);
}

void Filter_T(Image *work, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, int th)
{
    if (th < 0)
        th = 0;
    if (th > 255)
        th = 255;
    convThreshold(work, (unsigned char)th);
    int ok = previewSilent(fh, ih, work);
    char msg1[128], msg2[128];
    clearScreen();
    showMenuBar(1);

    snprintf(msg1, sizeof(msg1), "[필터] " BLUE "이진화(D)" RESET " 적용 완료 (Threshold=%d)                ┃\n", th);
    snprintf(msg2, sizeof(msg2), ok ? "미리보기 생성: preview.bmp" : "미리보기 저장 실패");
    showFilterMenu2(msg1, msg2);
}


// --이미지 코드
// 회전
void ImageRotate90(Image **pwork, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih)
{
    if (!pwork || !*pwork)
        return;
    Image *tmp = imgRotate90(*pwork);
    if (!tmp)
    {
        printf("\n[이미지] 회전 실패\n");
        waitEnter();
        return;
    }
    freeImage(*pwork);
    *pwork = tmp;

    int oldW = ih->biWidth, oldH = ih->biHeight;
    ih->biWidth = oldH;
    ih->biHeight = oldW;

    int ok = previewSilent(fh, ih, *pwork);
    char msg1[128], msg2[128];

    snprintf(msg1, sizeof(msg1), "[이미지] " BLUE "90도 회전(R)" RESET " 완료                                ┃\n");
    snprintf(msg2, sizeof(msg2), ok ? "미리보기 생성: preview.bmp" : "미리보기 저장 실패");
    showFilterMenu2(msg1, msg2);
}

// 좌우 반전
void ImageFlipH(Image *work, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih)
{
    if (!work)
        return;
    imgFlipH(work);
    int ok = previewSilent(fh, ih, work);
    char msg1[128], msg2[128];

    snprintf(msg1, sizeof(msg1), "[이미지] " BLUE "좌우 반전(H)" RESET " 완료                                ┃\n");
    snprintf(msg2, sizeof(msg2), ok ? "미리보기 생성: preview.bmp" : "미리보기 저장 실패");
    showFilterMenu2(msg1, msg2);
}

// 상하 반전
void ImageFlipV(Image *work, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih)
{
    if (!work)
        return;
    imgFlipV(work);
    int ok = previewSilent(fh, ih, work);
    char msg1[128], msg2[128];

    snprintf(msg1, sizeof(msg1), "[이미지] " BLUE "상하 반전(V)" RESET " 완료                                ┃\n");
    snprintf(msg2, sizeof(msg2), ok ? "미리보기 생성: preview.bmp" : "미리보기 저장 실패");
    showFilterMenu2(msg1, msg2);
}

// 크기 조절
void ImageResize(Image **pwork, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih)
{
    if (!pwork || !*pwork) 
        return;

    char label[] = "새 가로×세로(px)를 입력하세요 (예: 800 600): ";
    char line[128];
    int newW = 0, newH = 0;
    for (;;) {
        clearScreen();
        showMenuBar(3);
        showMenuBar2();
        printf("┃                                                                                          ┃\n"); // row 4
        printf("┃ [이미지] " BLUE "크기 조절(Z)" RESET "                                                                    ┃\n"); // row 5
        printf("┃                                                                                          ┃\n"); // row 6
        printf("┃                                                                                          ┃\n"); // row 7 prompt
        printf("┃                                                                                          ┃\n"); // row 8 warn
        for (int i = 0; i < 16; ++i) printf("┃                                                                                          ┃\n");
        printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
        printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
        fflush(stdout);

        gotoxy(7, 3);
        printf("%s", label);
        fflush(stdout);

        clearInput();
        if (!fgets(line, sizeof(line), stdin)) {
            gotoxy(8, 3);
            printf("입력 오류입니다. 다시 입력하세요.");
            waitEnter();
            continue;
        }

        if (sscanf(line, "%d %d", &newW, &newH) == 2 && newW > 0 && newH > 0) {
            break;
        }
        gotoxy(8, 3);
        printf("형식 오류 또는 범위 오류입니다. 예: 800 600 (양의 정수)");
        waitEnter();
    }

    Image *tmp = imgResizeNearest(*pwork, newW, newH);
    if (!tmp) {
        clearScreen();
        showMenuBar(3);
        showMenuBar2();
        printf("┃                                                                                          ┃\n");
        printf("┃ [이미지] " BLUE "크기 조절(Z)" RESET "                                                                    ┃\n");
        printf("┃                                                                                          ┃\n");
        printf("┃ 크기 조절 실패                                                                           ┃\n");
        for (int i = 0; i < 14; ++i) printf("┃                                                                                          ┃\n");
        printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
        printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
        waitEnter();
        return;
    }

    freeImage(*pwork);
    *pwork = tmp;
    ih->biWidth  = newW;
    ih->biHeight = newH;

    int ok = previewSilent(fh, ih, *pwork);

    char msg1[128], msg2[128];
    snprintf(msg1, sizeof(msg1), "[이미지] " BLUE "크기 조절(Z)" RESET "                                     ┃\n");
    snprintf(msg2, sizeof(msg2), ok ? "미리보기 생성: preview.bmp" : "미리보기 저장 실패");

    clearScreen();
    showMenuBar(1);
    showFilterMenu2(msg1, msg2);  
}


//메인 단축키
void handleMainMenu(Screen *scr, char *filename)
{   
    showMainScreen(filename);

    int ch = getCommand();

    if (ch == EOF)
        return;
    if (ch == '\n')
        return;

    switch (ch) 
    {
        case 'F':
            clearInput();
            clearScreen();
            showMenuBar(1);
            showFileMenu(); // 파일 하위 메뉴 표시
            *scr = SCR_FILE;
            break;
        
        case 'T':
            clearInput();
            clearScreen();
            showMenuBar(2);
            showFilterMenu(); // 필터 하위 메뉴 표시
            ShowMode();
            *scr = SCR_FILTER;
            break;

        case 'M':
            clearInput();
            clearScreen();
            showMenuBar(3);
            showImageMenu(); // 이미지 하위 메뉴 표시
            ShowMode();
            *scr = SCR_IMAGE;
            break;
        
        case 'H':
            clearInput();
            clearScreen();
            showMenuBar(4);
            showHelpMenu(); // 도움말 하위 메뉴 표시
            *scr = SCR_HELP;
            break;

        case 'B':
            clearInput();
            *scr = SCR_WELCOME;
            break;

        case 'Q':
            clearInput();
            clearScreen();
            printf("프로그램 종료\n");
            exit(0);

        default: 
            clearInput();
            clearScreen();
            showMenuBar(0);
            showMenuBar2();
            printf("┃                                                                                          ┃\n");
            printf("┃ 알 수 없는 입력: %c                                                                   ┃\n",ch);
            for (int i = 0; i < 17; i++)
            {
                printf("┃                                                                                          ┃\n");
            }
            printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
            printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
            waitEnter();
            break;
    }
}

//파일 단축키
void handleFileMenu(Screen *scr, char *filename, Image **original, Image **work, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih)
{
    int ch = getCommand();

    if (ch == EOF)
        return;
    if (ch == '\n')
        return;

    switch (ch)
    {
        case 'S':
            FileSave(filename, fh, ih, *work);
            showMainScreen(filename);
            *scr = SCR_MAIN;
            break;

        case 'A':
            FileSaveAs(filename, fh, ih, *work);
            showMainScreen(filename);
            *scr = SCR_MAIN;
            break;

        case 'O':
            clearScreen();
            showMenuBar(1);
            FileOpen(filename, fh, ih, original, work);
            showMainScreen(filename);
            *scr = SCR_MAIN;
            break;

        case 'B':
            showMainScreen(filename);
            *scr = SCR_MAIN;
            break;

        case 'Q':
            clearScreen();
            printf("프로그램 종료\n");
            exit(0);

        default:
            clearScreen();
            showMenuBar(0);
            showMenuBar2();                
            printf("┃                                                                                          ┃\n");
            printf("┃ 알 수 없는 입력: %c                                                                 ┃\n", ch);
            for (int i = 0; i < 17; i++)
            {
                printf("┃                                                                                          ┃\n");
            }
            printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
            printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
            waitEnter();
            showMainScreen(filename);
            *scr = SCR_MAIN;
            break;
    }
}

//필터 단축키
void handleFilterMenu(Screen *scr, Image *original, Image **work, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, char *filename)
{
    int ch = getCommand();

    if (ch == EOF)
        return;
    if (ch == '\n')
        return;

    switch (ch) 
    {
        case 'G':
            clearScreen();
            showMenuBar(2);

            if (!ReadyMode(original, work)) 
            {
                printf("메모리 할당 실패\n");
                waitEnter();
                return;   
            }

            Filter_G(*work, fh, ih); 
            *scr = SCR_FILE;
            return;

        case 'I':
            clearScreen();
            showMenuBar(2);
            if (!ReadyMode(original, work)) {
                printf("메모리 할당 실패\n");
                waitEnter();
                return;   
            }
            Filter_I(*work, fh, ih);
            *scr = SCR_FILE;
            return;
        
        case 'D':
        {
            clearScreen();
            showMenuBar(2);

            if (!ReadyMode(original, work)) 
            {
                printf("메모리 할당 실패\n");
                waitEnter();
                return;   
            }

            printf("┏━━━━━━━━━━━━━━        ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
            printf("┃                                                                                          ┃\n");
            printf("┃ [필터] 이진화(D)                                                                         ┃\n");
            printf("┃                                                                                          ┃\n");
            printf("┃ Threshold 값(0~255, 기본=128):                                                           ┃\n");
            printf("┃                                                                                          ┃\n");
            printf("┃                                                                                          ┃\n");
            printf("┃                                                                                          ┃\n");
            printf("┃                                                                                          ┃\n");
            printf("┃                                                                                          ┃\n");
            for (int i = 0; i < 12; i++)
            {
                printf("┃                                                                                          ┃\n");
            }
            printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
            printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
            
            gotoxy(7, 34);
            fflush(stdout);
            
            clearInput();

            char line[64];
            int th = 128;

            if (fgets(line, sizeof(line), stdin))
            {
                if (line[0] != '\n')
                {
                    int tmp;
                    if (sscanf(line, "%d", &tmp) == 1)
                        th = tmp;
                    else
                        printf("입력 형식 오류로 기본값 128 사용\n");
                }
            }
            else
            {
                printf("입력 오류로 기본값 128 사용\n");
            }
        
            if (th < 0)
                th = 0;
            if (th > 255)
                th = 255;
            
            Filter_T(*work, fh, ih, th);    // 내부에서 파일 메뉴 형태로 결과 표시
            
            *scr = SCR_FILE;
            return;
        }

        case 'B':
            showMainScreen(filename);
            *scr = SCR_MAIN;
            return;
        
        case 'Q':
            clearScreen();
            printf("프로그램 종료\n");
            exit(0);
        
        case 'X':
            ChangeMode();
            showModeBox(2);
            *scr = SCR_FILTER;
            return;
        
        default:
            clearScreen();
            showMenuBar(0);
            showMenuBar2();
            printf("┃                                                                                          ┃\n");
            printf("┃ 알 수 없는 입력: %c                                                                 ┃\n", ch);
            for (int i = 0; i < 17; i++)
            {
                printf("┃                                                                                          ┃\n");
            }
            printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
            printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
            
            waitEnter();
            
            *scr = SCR_MAIN;
            return;
    }
}

//이미지 단축키
void handleImageMenu(Screen *scr, Image *original, Image **work, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, char *filename)
{
    int ch = getCommand();

    if (ch == EOF)
        return;
    if (ch == '\n')
        return;
    
    switch (ch) 
    {
        case 'R':
            clearScreen();
            showMenuBar(3);

            if (!ReadyMode(original, work)) 
            {
                printf("메모리 할당 실패\n"); 
                waitEnter(); 
                return;
            }
            ImageRotate90(work, fh, ih);
            *scr = SCR_FILE;
            return;
    
        case 'H':
            clearScreen();
            showMenuBar(3);
            
            if (!ReadyMode(original, work)) 
            {
                printf("메모리 할당 실패\n"); 
                waitEnter(); 
                return;
            }
            ImageFlipH(*work, fh, ih);
            *scr = SCR_FILE;
            return;

        case 'V':
            clearScreen();
            showMenuBar(3);
            
            if (!ReadyMode(original, work)) 
            {
                printf("메모리 할당 실패\n"); 
                waitEnter(); 
                return;
            }
            ImageFlipV(*work, fh, ih);
            *scr = SCR_FILE;
            return;
        
        case 'Z':
            clearScreen();
            showMenuBar(3);
            
            if (!ReadyMode(original, work)) 
            {
                printf("메모리 할당 실패\n"); 
                waitEnter(); 
                return;
            }
            ImageResize(work, fh, ih);
            *scr = SCR_FILE;
            return;

        case 'X':
            ChangeMode();
            showModeBox(3);
            *scr = SCR_IMAGE;
            return;        

        case 'B':
            showMainScreen(filename);
            *scr = SCR_MAIN;
            return;

        case 'Q':
            clearScreen();
            printf("프로그램 종료\n");
            exit(0);
        
        default:
            clearScreen();
            showMenuBar(0);
            showMenuBar2();
            printf("┃                                                                                          ┃\n");
            printf("┃ 알 수 없는 입력: %c                                                                 ┃\n", ch);
            for (int i = 0; i < 17; i++)
            {
                printf("┃                                                                                          ┃\n");
            }
            printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
            printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
            
            waitEnter();
            
            showMainScreen(filename);
            *scr = SCR_MAIN;
            return;
    }
}

//도움말 단축키
void handleHelpMenu(Screen *scr, char *filename)
{
    int ch = getCommand();

    if (ch == EOF)
        return;
    if (ch == '\n')
        return;
    
    switch (ch) 
    {
        case 'K':
            clearInput();
            showHelpShortcuts();
            clearScreen();
            showMenuBar(4);
            showHelpMenu(); 
            *scr = SCR_HELP;
            return;
        
        case 'U':
            clearInput();
            showHelpTips(); 
            clearScreen();
            showMenuBar(4);
            showHelpMenu();
            *scr = SCR_HELP;
            return;
        
        case 'A':
            clearInput();
            showHelpAbout();
            clearScreen();
            showMenuBar(4);
            showHelpMenu();
            *scr = SCR_HELP;
            return;

        case 'B':
            showMainScreen(filename);
            *scr = SCR_MAIN;
            return;

        case 'Q':
            clearScreen();
            printf("프로그램 종료\n");
            exit(0);

        default:
            clearScreen();
            showMenuBar(0);
            showMenuBar2();

            printf("┃                                                                                          ┃\n");
            printf("┃ 알 수 없는 입력: %c                                                                 ┃\n", ch);
            for (int i = 0; i < 19; i++)
            {
                printf("┃                                                                                          ┃\n");
            }
            printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
            printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
            
            waitEnter();
            
            clearScreen();
            showMenuBar(4);
            showHelpMenu();
            return;
    }
}
