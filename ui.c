#include <stdio.h>
#include <stdlib.h>
#include "ui.h"

// 화면/입력
void clearScreen(void)
{
    system("clear");
    printf("\x1b[H\x1b[3J");  //커서를 맨 위로 옮기고, 화면과 스크롤 기록 전체를 지우기
}

// 커서 이동
void gotoxy(int row, int col)
{
    printf("\x1b[%d;%dH", row, col);
}

//엔터 입력
void waitEnterKey(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != '\r' && c != EOF)
    if (c == '\r') {
        int d = getchar();
        if (d != '\n' && d != EOF)
            ungetc(d, stdin);
    }
}

//엔터 입력 문구
void waitEnter(void)
{
    printf("\n ☛ 계속하려면 Enter를 누르세요...");
    fflush(stdout);
    waitEnterKey();
}


// --렌더링
// ANSI 색상 코드(파랑색)
#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"

// 상단 메뉴바
void showMenuBar(int active)
{
    printf(" ═══════════════════════════════════════════════════════════════════════════════════════════\n");
    printf(" 🖥️    ");

    if (active == 1)
        printf(BLUE "파일(F)" RESET "  ");
    else
        printf("파일(F)  ");

    if (active == 2)
        printf(BLUE "필터(T)" RESET "  ");
    else
        printf("필터(T)  ");

    if (active == 3)
        printf(BLUE "이미지(M)" RESET "  ");
    else
        printf("이미지(M)  ");

    if (active == 4)
        printf(BLUE "도움말(H)" RESET "\n");
    else
        printf("도움말(H)\n");
}

// 박스 상단
void showMenuBar2(void)
{
    printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
}

// 웰컴 화면
void showWelcome(void)
{
    clearScreen();
    showMenuBar(0);
    showMenuBar2();
    printf("┃                                                                                          ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃                             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━┓                                ┃\n");
    printf("┃                             ┃┏━━━━━━━━━━━━━━━━━━━━━━━━━┓┃                                ┃\n");
    printf("┃                             ┃┃                         ┃┃                                ┃\n");
    printf("┃                             ┃┃                         ┃┃                                ┃\n");
    printf("┃                             ┃┃        C언어 기반       ┃┃                                ┃\n");
    printf("┃                             ┃┃     영상처리 프로그램   ┃┃                                ┃\n");
    printf("┃                             ┃┃                         ┃┃                                ┃\n");
    printf("┃                             ┃┃                         ┃┃                                ┃\n");
    printf("┃                             ┃┗━━━━━━━━━━━━━━━━━━━━━━━━━┛┃                                ┃\n");
    printf("┃                             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━┛                                ┃\n");
    printf("┃                              파일 이름을 입력하세요 :                                    ┃\n");
    for (int i = 0; i < 7; i++)
    {
        printf("┃                                                                                          ┃\n");
    }
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    gotoxy(18, 57);
    fflush(stdout);
}

// 메인 화면
void showMainScreen(char *filename)
{
    clearScreen();
    showMenuBar(0);
    showMenuBar2();
    printf("┃                                                                                          ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃                                                                                          ┃\n");  // row 6
    printf("┃                                                                                          ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃ 단축키를 사용하세요.                                                                     ┃\n");
    for (int i = 0; i < 15; i++) {
        printf("┃                                                                                          ┃\n");
    }
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    gotoxy(6, 3);  // row=6, col=3 (┃ 다음 한 칸 뒤)
    if (filename && filename[0]) 
    {
        printf("불러온 파일: %s", filename);
    } 
    else 
    {
        printf("불러온 파일: (없음)");
    }
    fflush(stdout);
}

// 파일 하위 메뉴1
void showFileMenu(void)
{
    printf("┏━━━━┓        ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃    ┃ 새로 열기(O)          ┃                                                             ┃\n");
    printf("┃    ┃                       ┃                                                             ┃\n");
    printf("┃    ┃ 저장(S)               ┃                                                             ┃\n");
    printf("┃    ┃                       ┃                                                             ┃\n");
    printf("┃    ┃ 다른 이름으로 저장(A) ┃                                                             ┃\n");
    printf("┃    ┃                       ┃                                                             ┃\n");
    printf("┃    ┗━━━━━━━━━━━━━━━━━━━━━━━┛                                                             ┃\n");
    for (int i = 0; i < 14; i++)
    {
        printf("┃                                                                                          ┃\n");
    }
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    fflush(stdout);
}


// 필터 하위 메뉴
void showFilterMenu(void)
{
    printf("┏━━━━━━━━━━━━━┓        ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃             ┃ 그레이스케일(G)  ┃                                                         ┃\n");
    printf("┃             ┃                  ┃                                                         ┃\n");
    printf("┃             ┃ 색반전(I)        ┃                                                         ┃\n");
    printf("┃             ┃                  ┃                                                         ┃\n");
    printf("┃             ┃ 이진화(D)        ┃                                                         ┃\n");
    printf("┃             ┃                  ┃                                                         ┃\n");
    printf("┃             ┗━━━━━━━━━━━━━━━━━━┛                                                         ┃\n");
    for (int i = 0; i < 13; i++)
    {
        printf("┃                                                                                          ┃\n");
    }
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    fflush(stdout);
}

// 파일 하위 메뉴2(결과 포함)
void showFilterMenu2(char *msg1, char *msg2)
{
    printf("┏━━━━┓        ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃    ┃ 새로 열기(O)          ┃");
    if (msg1 && msg1[0])
        printf("   %s", msg1);
    printf("┃    ┃                       ┃");
    if (msg2 && msg2[0])
        printf("   %s", msg2);
    printf("                                ┃\n");
    printf("┃    ┃ 저장(S)               ┃                                                             ┃\n");
    printf("┃    ┃                       ┃                                                             ┃\n");
    printf("┃    ┃ 다른 이름으로 저장(A) ┃                                                             ┃\n");
    printf("┃    ┃                       ┃                                                             ┃\n");
    printf("┃    ┗━━━━━━━━━━━━━━━━━━━━━━━┛                                                             ┃\n");
    for (int i = 0; i < 14; i++)
    {
        printf("┃                                                                                          ┃\n");
    }
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    fflush(stdout);
}

// 이미지 하위 메뉴
void showImageMenu(void)
{
    printf("┏━━━━━━━━━━━━━━━━━━━━━━┓         ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃                      ┃ 90도 회전(R)     ┃                                                ┃\n");
    printf("┃                      ┃                  ┃                                                ┃\n");
    printf("┃                      ┃ 좌우 반전(H)     ┃                                                ┃\n");
    printf("┃                      ┃                  ┃                                                ┃\n");
    printf("┃                      ┃ 상하 반전(V)     ┃                                                ┃\n");
    printf("┃                      ┃                  ┃                                                ┃\n");
    printf("┃                      ┃ 크기 조절(Z)     ┃                                                ┃\n");
    printf("┃                      ┃                  ┃                                                ┃\n");
    printf("┃                      ┗━━━━━━━━━━━━━━━━━━┛                                                ┃\n");
    for (int i = 0; i < 11; i++)
    {
        printf("┃                                                                                          ┃\n");
    }
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    fflush(stdout);
}

// 도움말 하위 메뉴
void showHelpMenu(void)
{
    printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓          ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃                                 ┃ 단축키 보기(K)      ┃                                  ┃\n");
    printf("┃                                 ┃                     ┃                                  ┃\n");
    printf("┃                                 ┃ 사용법 요약(U)      ┃                                  ┃\n");
    printf("┃                                 ┃                     ┃                                  ┃\n");
    printf("┃                                 ┃ 프로그램 정보(A)    ┃                                  ┃\n");
    printf("┃                                 ┃                     ┃                                  ┃\n");
    printf("┃                                 ┗━━━━━━━━━━━━━━━━━━━━━┛                                  ┃\n");
    for (int i = 0; i < 14; i++)
    {
        printf("┃                                                                                          ┃\n");
    }
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    fflush(stdout);
}

// [도움말] 단축키 페이지
void showHelpShortcuts(void)
{
    clearScreen();
    showMenuBar(4);
    printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━          ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃                                                                                          ┃\n");
    printf("┃ [단축키 목록]                                                                            ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃   ─ 메인/공통 ────────────────────────────────────────────────────────                   ┃\n");
    printf("┃    F : 파일 메뉴   |  T : 필터 메뉴   |  M : 이미지 메뉴                                 ┃\n");
    printf("┃    H : 도움말 메뉴 |  B : 뒤로가기    |  Q : 종료                                        ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃   ─ 파일 ────────────────────────────────────────────────────────────                    ┃\n");
    printf("┃    O : 새로 열기  |  S : 저장  |  A : 다른 이름으로 저장                                 ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃   ─ 필터 ────────────────────────────────────────────────────────────                    ┃\n");
    printf("┃    G : 그레이스케일  |  I : 색반전  |  D : 이진화                                        ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃   ─ 이미지 ───────────────────────────────────────────────────────────                   ┃\n");
    printf("┃    R : 90도 회전  |  H : 좌우반전  |  V : 상하반전  |  Z : 크기 조절                     ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃   ─ 도움말 ───────────────────────────────────────────────────────────                   ┃\n");
    printf("┃    K : 단축키 보기  |  U : 사용법 요약  |  A : 프로그램 정보                             ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    waitEnter();
}

// [도움말] 사용 팁 페이지
void showHelpTips(void)
{
    clearScreen();
    showMenuBar(4);
    printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━          ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃                                                                                          ┃\n");
    printf("┃ [사용 팁]                                                                                ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃  1) 파일을 연 뒤, 필터/이미지 메뉴에서 변환을 적용하세요.                                ┃\n");
    printf("┃  2) 변환 후 파일 메뉴가 함께 떠서 바로 저장(S/A)할 수 있어요.                            ┃\n");
    printf("┃  3) 메인 화면에서도 F 를 눌러 파일 메뉴를 열 수 있습니다.                                ┃\n");
    printf("┃  4) 문제 발생 시 B 로 한 단계 뒤로 이동하세요.                                           ┃\n");
    for (int i = 0; i < 14; i++)
    {
        printf("┃                                                                                          ┃\n");
    }
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    waitEnter();
}

// [도움말] 프로그램 정보 페이지
void showHelpAbout(void)
{
    clearScreen();
    showMenuBar(4);
    printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━          ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃                                                                                          ┃\n");
    printf("┃ [프로그램 정보]                                                                          ┃\n");
    printf("┃                                                                                          ┃\n");
    printf("┃  - 이름 : C언어 영상처리 프로그램                                                        ┃\n");
    printf("┃  - 개발자 : 윤수인                                                                       ┃\n");
    printf("┃  - 버전 : 0.1.0                                                                          ┃\n");
    printf("┃  - 설명 : 텍스트 메뉴를 통해                                                             ┃\n");
    printf("┃          BMP 이미지를 손쉽게 편집하고 저장할 수 있는 프로그램                            ┃\n");
    printf("┃  - 미리보기 : preview.bmp 로 임시 저장 후 OS 기본 뷰어로 열기                            ┃\n");
    for (int i = 0; i < 12; i++)
    {
        printf("┃                                                                                          ┃\n");
    }
    printf("┃                                                                 뒤로가기(B)      종료(Q) ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    waitEnter();
}
