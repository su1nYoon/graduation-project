// ui.h
#ifndef UI_H
#define UI_H

// 화면 / 커서 / 입력 대기
void clearScreen(void);
void gotoxy(int row, int col);
void waitEnter(void);
void waitEnterKey(void);

// 상단 메뉴바 + 박스
void showMenuBar(int active);
void showMenuBar2(void);

// 화면들
void showWelcome(void);
void showMainScreen(char *filename);

// 하위 메뉴들
void showFileMenu(void);
void showFilterMenu(void);
void showFilterMenu2(char *msg1, char *msg2);
void showImageMenu(void);
void showHelpMenu(void);

// 도움말 상세 페이지들
void showHelpShortcuts(void);
void showHelpTips(void);
void showHelpAbout(void);


#endif // UI_H
