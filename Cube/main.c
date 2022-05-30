#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:6031)
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <math.h>

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ESC 27

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;
void SetCursorType(CURSOR_TYPE c);
void SetTextsColor(int n);
void goToXY(int x, int y);
void setFrame(HDC hdc);
void setBoard(HDC hdc);
void drawRectangle(HDC hdc, RECT rect, int left, int top, int right, int bottom, HBRUSH inside, HBRUSH outside);
void setCube(HDC hdc, int controlLeft, int controlRight);
void rotate(int key);
void rotateShow(HDC hdc, int key);
int rotateCheck(int key);
void boardRandom();
void cubePosRandom(HDC hdc);
int ruleCheck();
void stickCheck();
void moveCount(int* count);
void roateLeft(HDC hdc);
void rotateRight(HDC hdc);
void rotateUp(HDC hdc);
void rotateDown(HDC hdc);

typedef struct {
    RECT rect;
    int Color;
}Board;

Board board[4][4] = {
    { {100, 100, 200, 200, 0}, {200, 100, 300, 200, 0}, {300, 100, 400, 200, 0}, {400, 100, 500, 200, 0} },
    { {100, 200, 200, 300, 0}, {200, 200, 300, 300, 0}, {300, 200, 400, 300, 0}, {400, 200, 500 ,300 ,0} },
    { {100, 300, 200, 400, 0}, {200, 300, 300, 400, 0}, {300, 300, 400, 400, 0}, {400, 300, 500, 400 ,0} },
    { {100, 400, 200, 500, 0}, {200, 400, 300, 500, 0}, {300, 400, 400, 500, 0}, {400, 400, 500, 500, 0} }
};

int Cube[] = { 0, 0, 0, 0, 0, 0 }; // Back, Top, Left, Right, Bottom, Front
POINT BackVertices[4];
POINT FrontVertices[4];
POINT LeftVertices[4];
POINT RightVertices[4];
POINT TopVertices[4];
POINT BottomVertices[4];

int main() {
    SetCursorType(NOCURSOR);
    srand(time(NULL));
    HWND myConsole = GetConsoleWindow();
    HDC hdc = GetDC(myConsole);

    HDC backMemDC = CreateCompatibleDC(hdc);
    HBITMAP MemBmp = CreateCompatibleBitmap(hdc, 560, 560);
    SelectObject(backMemDC, MemBmp);



    // -- Initialize -- //

    int count = 0;
    goToXY(70, 10);
    printf("Moves : %d", count);
    Sleep(50);
    setFrame(hdc);
    boardRandom();
    setBoard(hdc);
    cubePosRandom(hdc);
    
    // -- Game -- //

    while (!ruleCheck()) {
        int key;
        _getch();
        key = _getch();
        
        rotateShow(hdc, key);
        
        if (rotateCheck(key)) {
            moveCount(&count);
        }
        
        setFrame(backMemDC);
        
        rotate(key);
        
        stickCheck();
        
        setBoard(backMemDC);
        
        setCube(backMemDC, BackVertices[0].x, BackVertices[0].y);

        BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    }

    goToXY(70, 12);
    printf("COMPLETED! Press ESC to Exit");

    // -- After Completed -- //

    while (1) {
        int key;
        key = _getch();
        if (key == ESC) {
            break;
        }
        else {
            rotateShow(hdc, key);
            setFrame(backMemDC);
            rotate(key);
            setBoard(backMemDC);
            setCube(backMemDC, BackVertices[0].x, BackVertices[0].y);
            BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
        }
    }

    goToXY(70, 14);
    
    DeleteDC(backMemDC);
    DeleteObject(MemBmp);
    ReleaseDC(myConsole, hdc);

	return 0;
}

void SetCursorType(CURSOR_TYPE c) { //커서 표시를 바꿔주는 함수  
    CONSOLE_CURSOR_INFO CurInfo;

    switch (c) {
    case NOCURSOR:
        CurInfo.dwSize = 1;
        CurInfo.bVisible = FALSE;
        break;
    case SOLIDCURSOR:
        CurInfo.dwSize = 100;
        CurInfo.bVisible = TRUE;
        break;
    case NORMALCURSOR:
        CurInfo.dwSize = 20;
        CurInfo.bVisible = TRUE;
        break;
    }
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

void SetTextsColor(int n) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), n);
}

void goToXY(int x, int y) {
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void setFrame(HDC hdc) {
    HBRUSH white = CreateSolidBrush(RGB(242, 242, 242));
    HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
    RECT Frame = { 40, 40, 560, 560 };
    drawRectangle(hdc, Frame, Frame.left, Frame.top, Frame.right, Frame.bottom, white, black);
    DeleteObject(white);
    DeleteObject(black);
}

void setBoard(HDC hdc) {

    HBRUSH white = CreateSolidBrush(RGB(242, 242, 242));
    HBRUSH blue = CreateSolidBrush(RGB(0, 0, 255));
    HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j].Color == 0) {
                drawRectangle(hdc, board[i][j].rect, board[i][j].rect.left, board[i][j].rect.top, board[i][j].rect.right, board[i][j].rect.bottom, white, black);
            }
            else {
                drawRectangle(hdc, board[i][j].rect, board[i][j].rect.left, board[i][j].rect.top, board[i][j].rect.right, board[i][j].rect.bottom, blue, black);
            }
            
        }
    }

    DeleteObject(white);
    DeleteObject(blue);
    DeleteObject(black);

}

void drawRectangle(HDC hdc, RECT rect, int left, int top, int right, int bottom, HBRUSH inside, HBRUSH outside) {
    rect.left = left;
    rect.top = top;
    rect.right = right;
    rect.bottom = bottom;
    FillRect(hdc, &rect, inside);
    FrameRect(hdc, &rect, outside);
}

void setCube(HDC hdc, int controlLeft, int controlTop) {
    
    for (int i = 0; i < 4; i++) {
        switch (i)
        {
        case 0:
            BackVertices[i].x = controlLeft;
            BackVertices[i].y = controlTop;
            FrontVertices[i].x = controlLeft - 30;
            FrontVertices[i].y = controlTop - 30;
            LeftVertices[i].x = controlLeft - 30;
            LeftVertices[i].y = controlTop - 30;
            RightVertices[i].x = controlLeft + 70;
            RightVertices[i].y = controlTop - 30;
            TopVertices[i].x = controlLeft - 30;
            TopVertices[i].y = controlTop - 30;
            BottomVertices[i].x = controlLeft - 30;
            BottomVertices[i].y = controlTop + 70;
            break;
        case 1:
            BackVertices[i].x = controlLeft + 100;
            BackVertices[i].y = controlTop;
            FrontVertices[i].x = controlLeft + 70;
            FrontVertices[i].y = controlTop - 30;
            LeftVertices[i].x = controlLeft;
            LeftVertices[i].y = controlTop;
            RightVertices[i].x = controlLeft + 100;
            RightVertices[i].y = controlTop;
            TopVertices[i].x = controlLeft + 70;
            TopVertices[i].y = controlTop - 30;
            BottomVertices[i].x = controlLeft + 70;
            BottomVertices[i].y = controlTop + 70;
            break;
        case 2:
            BackVertices[i].x = controlLeft + 100;
            BackVertices[i].y = controlTop + 100;
            FrontVertices[i].x = controlLeft + 70;
            FrontVertices[i].y = controlTop + 70;
            LeftVertices[i].x = controlLeft;
            LeftVertices[i].y = controlTop + 100;
            RightVertices[i].x = controlLeft + 100;
            RightVertices[i].y = controlTop + 100;
            TopVertices[i].x = controlLeft + 100;
            TopVertices[i].y = controlTop;
            BottomVertices[i].x = controlLeft + 100;
            BottomVertices[i].y = controlTop + 100;
            break;
        case 3:
            BackVertices[i].x = controlLeft;
            BackVertices[i].y = controlTop + 100;
            FrontVertices[i].x = controlLeft - 30;
            FrontVertices[i].y = controlTop + 70;
            LeftVertices[i].x = controlLeft - 30;
            LeftVertices[i].y = controlTop + 70;
            RightVertices[i].x = controlLeft + 70;
            RightVertices[i].y = controlTop + 70;
            TopVertices[i].x = controlLeft;
            TopVertices[i].y = controlTop;
            BottomVertices[i].x = controlLeft;
            BottomVertices[i].y = controlTop + 100;
            break;
        }
    }
    
    for (int i = 0; i < 6; i++) {
        if (Cube[i] == 1) {
            HPEN bPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
            HPEN bOldPen = SelectPen(hdc, bPen);
            HBRUSH bBrush = CreateSolidBrush(RGB(0, 0, 255));
            HBRUSH bOldBrush = SelectBrush(hdc, bBrush);

            switch (i)
            {
            case 0:
                Polygon(hdc, BackVertices, 4);
                break;
            case 1:
                Polygon(hdc, TopVertices, 4);
                break;
            case 2:
                Polygon(hdc, LeftVertices, 4);
                break;
            case 3:
                Polygon(hdc, RightVertices, 4);
                break;
            case 4:
                Polygon(hdc, BottomVertices, 4);
                break;
            case 5:
                Polygon(hdc, FrontVertices, 4);
                break;
            }

            SelectBrush(hdc, bOldBrush);
            DeleteObject(bBrush);
        
            SelectPen(hdc, bOldPen);
            DeleteObject(bPen);
        }
        else {

            HPEN gPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
            HPEN gOldPen = SelectPen(hdc, gPen);
            HBRUSH gBrush = CreateSolidBrush(RGB(242, 242, 242));
            HBRUSH gOldBrush = SelectBrush(hdc, gBrush);

            switch (i)
            {
            case 0:
                Polygon(hdc, BackVertices, 4);
                break;
            case 1:
                Polygon(hdc, TopVertices, 4);
                break;
            case 2:
                Polygon(hdc, LeftVertices, 4);
                break;
            case 3:
                Polygon(hdc, RightVertices, 4);
                break;
            case 4:
                Polygon(hdc, BottomVertices, 4);
                break;
            case 5:
                Polygon(hdc, FrontVertices, 4);
                break;
            }

            SelectBrush(hdc, gOldBrush);
            DeleteObject(gBrush);

            SelectPen(hdc, gOldPen);
            DeleteObject(gPen);
        }
    }
}

void rotate(int key) {
    int temp;
    /*int key;
    _getch();
    key = _getch();*/
    switch (key) {
    case LEFT:
        if (rotateCheck(key)) {
            for (int i = 0; i < 4; i++) {
                BackVertices[i].x -= 100;
                TopVertices[i].x -= 100;
                LeftVertices[i].x -= 100;
                RightVertices[i].x -= 100;
                BottomVertices[i].x -= 100;
                FrontVertices[i].x -= 100;
            }
            temp = Cube[0]; // temp = Back
            Cube[0] = Cube[2]; // Back = Left
            Cube[2] = Cube[5]; // Left = Front
            Cube[5] = Cube[3]; // Front = Right
            Cube[3] = temp; // Right = Back (temp)
        }
        break;

    case RIGHT:
        if (rotateCheck(key)) {
            for (int i = 0; i < 4; i++) {
                BackVertices[i].x += 100;
                TopVertices[i].x += 100;
                LeftVertices[i].x += 100;
                RightVertices[i].x += 100;
                BottomVertices[i].x += 100;
                FrontVertices[i].x += 100;
            }
            temp = Cube[0]; // temp = Back
            Cube[0] = Cube[3]; // Back = Right
            Cube[3] = Cube[5]; // Right = Front
            Cube[5] = Cube[2]; // Front = Left
            Cube[2] = temp; // Left = Back (temp)

        }
        break;

    case UP:
        if (rotateCheck(key)) {
            for (int i = 0; i < 4; i++) {
                BackVertices[i].y -= 100;
                TopVertices[i].y -= 100;
                LeftVertices[i].y -= 100;
                RightVertices[i].y -= 100;
                BottomVertices[i].y -= 100;
                FrontVertices[i].y -= 100;
            }
            temp = Cube[0]; // temp = Back
            Cube[0] = Cube[1]; // Back = Top
            Cube[1] = Cube[5]; // Top = Front
            Cube[5] = Cube[4]; // Front = Bottom
            Cube[4] = temp; // Bottom = Back (temp);
        }
        break;

    case DOWN:
        if (rotateCheck(key)) {
            for (int i = 0; i < 4; i++) {
                BackVertices[i].y += 100;
                TopVertices[i].y += 100;
                LeftVertices[i].y += 100;
                RightVertices[i].y += 100;
                BottomVertices[i].y += 100;
                FrontVertices[i].y += 100;
            }
            temp = Cube[0]; // temp = Back
            Cube[0] = Cube[4]; // Back = Bottom
            Cube[4] = Cube[5]; // Bottom = Front
            Cube[5] = Cube[1]; // Front = Top
            Cube[1] = temp; // Top = Back
        }
    }
}

void rotateShow(HDC hdc, int key) { // Show Rotating

    switch (key) {
    case LEFT:
        if (rotateCheck(key)) {
            roateLeft(hdc);
        }
        break;


    case RIGHT:
        if (rotateCheck(key)) {
            rotateRight(hdc);
        }
        break;


    case UP:
        if (rotateCheck(key)) { 
            rotateUp(hdc);
        }
        break;


    case DOWN:
        if (rotateCheck(key)) {
            rotateDown(hdc);
        }
        break;
    
    }

}

int rotateCheck(int key) {
    switch (key)
    {
    case LEFT:
        if (BackVertices[0].x != 100) {
            return 1;
        }
        else {
            return 0;
        }
        break;
    case RIGHT:
        if (BackVertices[0].x != 400) {
            return 1;
        }
        else {
            return 0;
        }
        break;
    case UP:
        if (BackVertices[0].y != 100) {
            return 1;
        }
        else {
            return 0;
        }
        break;
    case DOWN:
        if (BackVertices[0].y != 400) {
            return 1;
        }
        else {
            return 0;
        }
        break;
    }

}

void boardRandom() {
    int* arr = (int*)malloc(sizeof(int) * 16);

    for (int i = 0; i < 16; i++) {
        arr[i] = rand() % 16;
        for (int sub_i = 0; sub_i < i; sub_i++) {
            if (arr[i] == arr[sub_i]) {
                i--;
                break;
            }
        }
    }

    for (int i = 0; i < 6; i++) {
        board[arr[i] % 4][arr[i] / 4].Color = 1;
    }

    free(arr);
}

void cubePosRandom(HDC hdc) {
    
    int pos;

    do {
        pos = rand() % 16;
    } while (board[pos % 4][pos / 4].Color == 1);
    
    setCube(hdc, board[pos % 4][pos / 4].rect.left, board[pos % 4][pos / 4].rect.top);
}

int ruleCheck() {
    for (int i = 0; i < 6; i++) {
        if (Cube[i] == 0) {
            return 0;
        }
    }

    return 1;
}

void stickCheck() {
    if (Cube[0] == 0 && board[(BackVertices[0].y / 100) - 1][(BackVertices[0].x / 100) - 1].Color == 1) {
        Cube[0] = 1;
        board[(BackVertices[0].y / 100) - 1][(BackVertices[0].x / 100) - 1].Color = 0;
    }
    else if (Cube[0] == 1 && board[(BackVertices[0].y / 100) - 1][(BackVertices[0].x / 100) - 1].Color == 0) {
        Cube[0] = 0;
        board[(BackVertices[0].y / 100) - 1][(BackVertices[0].x / 100) - 1].Color = 1;
    }
    else {
        return;
    }
}

void moveCount(int* count) {
    goToXY(70, 10);
    printf("Moves : %d", ++(*count));
}

void roateLeft(HDC hdc) {
    HDC backMemDC = CreateCompatibleDC(hdc);
    HBITMAP MemBmp = CreateCompatibleBitmap(hdc, 560, 560);
    SelectObject(backMemDC, MemBmp);

    HPEN Pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH gBrush = CreateSolidBrush(RGB(242, 242, 242));
    HBRUSH bBrush = CreateSolidBrush(RGB(0, 0, 255));

    // Cube Coordinates Copy
    POINT TempBackVertices[4] = { {BackVertices[0].x, BackVertices[0].y}, {BackVertices[1].x, BackVertices[1].y}, {BackVertices[2].x, BackVertices[2].y}, {BackVertices[3].x, BackVertices[3].y} };
    POINT TempFrontVertices[4] = { {FrontVertices[0].x, FrontVertices[0].y }, {FrontVertices[1].x, FrontVertices[1].y }, {FrontVertices[2].x, FrontVertices[2].y }, {FrontVertices[3].x, FrontVertices[3].y } };
    POINT TempLeftVertices[4] = { {LeftVertices[0].x, LeftVertices[0].y}, {LeftVertices[1].x, LeftVertices[1].y}, {LeftVertices[2].x, LeftVertices[2].y} ,{LeftVertices[3].x, LeftVertices[3].y} };
    POINT TempRightVertices[4] = { {RightVertices[0].x, RightVertices[0].y}, {RightVertices[1].x, RightVertices[1].y}, {RightVertices[2].x, RightVertices[2].y}, {RightVertices[3].x, RightVertices[3].y} };
    POINT TempTopVertices[4] = { {TopVertices[0].x, TopVertices[0].y}, {TopVertices[1].x, TopVertices[1].y}, {TopVertices[2].x, TopVertices[2].y}, {TopVertices[3].x, TopVertices[3].y} };
    POINT TempBottomVertices[4] = { {BottomVertices[0].x, BottomVertices[0].y}, {BottomVertices[1].x, BottomVertices[1].y}, {BottomVertices[2].x, BottomVertices[2].y}, {BottomVertices[3].x, BottomVertices[3].y} };

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_01
    TempFrontVertices[0].x -= 25;
    TempFrontVertices[0].y;

    TempFrontVertices[1].x -= 40;
    TempFrontVertices[1].y -= 10;

    TempFrontVertices[2].x -= 40;
    TempFrontVertices[2].y -= 10;

    TempFrontVertices[3].x -= 25;
    TempFrontVertices[3].y;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    

    //Right_01
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x -= 15;
    TempRightVertices[1].y -= 10;

    TempRightVertices[2].x -= 15;
    TempRightVertices[2].y -= 10;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }

    //Bottom_01
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[2].x = TempRightVertices[2].x;
    TempBottomVertices[2].y = TempRightVertices[2].y;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    Sleep(20);

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_02
    TempFrontVertices[0].x -= 25;
    TempFrontVertices[0].y += 5;

    TempFrontVertices[1].x -= 31;
    TempFrontVertices[1].y;

    TempFrontVertices[2].x -= 31;
    TempFrontVertices[2].y;

    TempFrontVertices[3].x -= 25;
    TempFrontVertices[3].y += 5;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Right_02
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x -= 15;
    TempRightVertices[1].y -= 5;

    TempRightVertices[2].x -= 15;
    TempRightVertices[2].y -= 5;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }

    //Bottom_02
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[2].x = TempRightVertices[2].x;
    TempBottomVertices[2].y = TempRightVertices[2].y;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    Sleep(20);

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_03
    TempFrontVertices[0].x -= 12;
    TempFrontVertices[0].y += 5;

    TempFrontVertices[1].x -= 35;
    TempFrontVertices[1].y -= 5;

    TempFrontVertices[2].x -= 35;
    TempFrontVertices[2].y -= 5;

    TempFrontVertices[3].x -= 12;
    TempFrontVertices[3].y += 5;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Right_03
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x -= 20;
    TempRightVertices[1].y -= 7;

    TempRightVertices[2].x -= 20;
    TempRightVertices[2].y -= 7;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }

    //Bottom_03
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[2].x = TempRightVertices[2].x;
    TempBottomVertices[2].y = TempRightVertices[2].y;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    Sleep(20);

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_04
    TempFrontVertices[0].x -= 8;
    TempFrontVertices[0].y += 5;

    TempFrontVertices[1].x -= 35;
    TempFrontVertices[1].y -= 5;

    TempFrontVertices[2].x -= 35;
    TempFrontVertices[2].y -= 5;

    TempFrontVertices[3].x -= 8;
    TempFrontVertices[3].y += 5;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Right_04
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x -= 20;
    TempRightVertices[1].y -= 7;

    TempRightVertices[2].x -= 20;
    TempRightVertices[2].y -= 7;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }

    //Bottom_04
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[2].x = TempRightVertices[2].x;
    TempBottomVertices[2].y = TempRightVertices[2].y;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    Sleep(20);

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_05
    TempFrontVertices[0].x;
    TempFrontVertices[0].y += 5;

    TempFrontVertices[1].x -= 35;
    TempFrontVertices[1].y += 5;

    TempFrontVertices[2].x -= 35;
    TempFrontVertices[2].y += 5;

    TempFrontVertices[3].x;
    TempFrontVertices[3].y += 5;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Right_05
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x -= 25;
    TempRightVertices[1].y -= 2;

    TempRightVertices[2].x -= 25;
    TempRightVertices[2].y -= 2;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }

    //Bottom_05
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[2].x = TempRightVertices[2].x;
    TempBottomVertices[2].y = TempRightVertices[2].y;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    Sleep(20);

    DeleteDC(backMemDC);
    DeleteObject(MemBmp);
    DeleteObject(Pen);
    DeleteObject(gBrush);
    DeleteObject(bBrush);
}

void rotateRight(HDC hdc) {
    HDC backMemDC = CreateCompatibleDC(hdc);
    HBITMAP MemBmp = CreateCompatibleBitmap(hdc, 560, 560);
    SelectObject(backMemDC, MemBmp);

    HPEN Pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH gBrush = CreateSolidBrush(RGB(242, 242, 242));
    HBRUSH bBrush = CreateSolidBrush(RGB(0, 0, 255));

    // Cube Coordinates Copy
    POINT TempBackVertices[4] = { {BackVertices[0].x, BackVertices[0].y}, {BackVertices[1].x, BackVertices[1].y}, {BackVertices[2].x, BackVertices[2].y}, {BackVertices[3].x, BackVertices[3].y} };
    POINT TempFrontVertices[4] = { {FrontVertices[0].x, FrontVertices[0].y }, {FrontVertices[1].x, FrontVertices[1].y }, {FrontVertices[2].x, FrontVertices[2].y }, {FrontVertices[3].x, FrontVertices[3].y } };
    POINT TempLeftVertices[4] = { {LeftVertices[0].x, LeftVertices[0].y}, {LeftVertices[1].x, LeftVertices[1].y}, {LeftVertices[2].x, LeftVertices[2].y} ,{LeftVertices[3].x, LeftVertices[3].y} };
    POINT TempRightVertices[4] = { {RightVertices[0].x, RightVertices[0].y}, {RightVertices[1].x, RightVertices[1].y}, {RightVertices[2].x, RightVertices[2].y}, {RightVertices[3].x, RightVertices[3].y} };
    POINT TempTopVertices[4] = { {TopVertices[0].x, TopVertices[0].y}, {TopVertices[1].x, TopVertices[1].y}, {TopVertices[2].x, TopVertices[2].y}, {TopVertices[3].x, TopVertices[3].y} };
    POINT TempBottomVertices[4] = { {BottomVertices[0].x, BottomVertices[0].y}, {BottomVertices[1].x, BottomVertices[1].y}, {BottomVertices[2].x, BottomVertices[2].y}, {BottomVertices[3].x, BottomVertices[3].y} };

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_01
    TempFrontVertices[0].x += 25;
    TempFrontVertices[0].y -= 5;

    TempFrontVertices[1].x += 35;
    TempFrontVertices[1].y += 5;

    TempFrontVertices[2].x += 35;
    TempFrontVertices[2].y += 5;

    TempFrontVertices[3].x += 25;
    TempFrontVertices[3].y -= 5;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Bottom_01
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[3].x;
    TempBottomVertices[3].y -= 10;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    Sleep(20);

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_02
    TempFrontVertices[0].x += 50;
    TempFrontVertices[0].y -= 8;

    TempFrontVertices[1].x += 35;
    TempFrontVertices[1].y;

    TempFrontVertices[2].x += 35;
    TempFrontVertices[2].y;

    TempFrontVertices[3].x += 50;
    TempFrontVertices[3].y -= 8;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Bottom_02
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[2].x;
    TempBottomVertices[2].y;

    TempBottomVertices[3].x;
    TempBottomVertices[3].y -= 10;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }

    //Left_02
    TempLeftVertices[0].x += 30;
    TempLeftVertices[0].y += 10;

    TempLeftVertices[1].x = TempFrontVertices[0].x;
    TempLeftVertices[1].y = TempFrontVertices[0].y;

    TempLeftVertices[2].x = TempFrontVertices[3].x;
    TempLeftVertices[2].y = TempFrontVertices[3].y;

    TempLeftVertices[3].x = TempBottomVertices[3].x;
    TempLeftVertices[3].y = TempBottomVertices[3].y;

    if (Cube[2] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempLeftVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempLeftVertices, 4);
    }
    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    Sleep(20);

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_03
    TempFrontVertices[0].x += 40;
    TempFrontVertices[0].y -= 5;

    TempFrontVertices[1].x += 20;
    TempFrontVertices[1].y;

    TempFrontVertices[2].x += 20;
    TempFrontVertices[2].y;

    TempFrontVertices[3].x += 40;
    TempFrontVertices[3].y -= 5;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Bottom_03
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[2].x;
    TempBottomVertices[2].y;

    TempBottomVertices[3].x += 20;
    TempBottomVertices[3].y -= 5;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }

    //Left_03
    TempLeftVertices[0].x += 20;
    TempLeftVertices[0].y -= 5;

    TempLeftVertices[1].x = TempFrontVertices[0].x;
    TempLeftVertices[1].y = TempFrontVertices[0].y;

    TempLeftVertices[2].x = TempFrontVertices[3].x;
    TempLeftVertices[2].y = TempFrontVertices[3].y;

    TempLeftVertices[3].x = TempBottomVertices[3].x;
    TempLeftVertices[3].y = TempBottomVertices[3].y;

    if (Cube[2] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempLeftVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempLeftVertices, 4);
    }
    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    Sleep(20);
    
    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_04
    TempFrontVertices[0].x += 35;
    TempFrontVertices[0].y += 5;

    TempFrontVertices[1].x += 20;
    TempFrontVertices[1].y += 15;

    TempFrontVertices[2].x += 20;
    TempFrontVertices[2].y += 15;

    TempFrontVertices[3].x += 35;
    TempFrontVertices[3].y += 5;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Bottom_04
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[2].x;
    TempBottomVertices[2].y;

    TempBottomVertices[3].x += 15;
    TempBottomVertices[3].y -= 5;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }

    //Left_04
    TempLeftVertices[0].x += 15;
    TempLeftVertices[0].y -= 5;

    TempLeftVertices[1].x = TempFrontVertices[0].x;
    TempLeftVertices[1].y = TempFrontVertices[0].y;

    TempLeftVertices[2].x = TempFrontVertices[3].x;
    TempLeftVertices[2].y = TempFrontVertices[3].y;

    TempLeftVertices[3].x = TempBottomVertices[3].x;
    TempLeftVertices[3].y = TempBottomVertices[3].y;

    if (Cube[2] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempLeftVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempLeftVertices, 4);
    }
    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    Sleep(20);

    DeleteDC(backMemDC);
    DeleteObject(MemBmp);
    DeleteObject(Pen);
    DeleteObject(gBrush);
    DeleteObject(bBrush);
}

void rotateUp(HDC hdc) {
    HDC backMemDC = CreateCompatibleDC(hdc);
    HBITMAP MemBmp = CreateCompatibleBitmap(hdc, 560, 560);
    SelectObject(backMemDC, MemBmp);

    HPEN Pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH gBrush = CreateSolidBrush(RGB(242, 242, 242));
    HBRUSH bBrush = CreateSolidBrush(RGB(0, 0, 255));

    // Cube Coordinates Copy
    POINT TempBackVertices[4] = { {BackVertices[0].x, BackVertices[0].y}, {BackVertices[1].x, BackVertices[1].y}, {BackVertices[2].x, BackVertices[2].y}, {BackVertices[3].x, BackVertices[3].y} };
    POINT TempFrontVertices[4] = { {FrontVertices[0].x, FrontVertices[0].y }, {FrontVertices[1].x, FrontVertices[1].y }, {FrontVertices[2].x, FrontVertices[2].y }, {FrontVertices[3].x, FrontVertices[3].y } };
    POINT TempLeftVertices[4] = { {LeftVertices[0].x, LeftVertices[0].y}, {LeftVertices[1].x, LeftVertices[1].y}, {LeftVertices[2].x, LeftVertices[2].y} ,{LeftVertices[3].x, LeftVertices[3].y} };
    POINT TempRightVertices[4] = { {RightVertices[0].x, RightVertices[0].y}, {RightVertices[1].x, RightVertices[1].y}, {RightVertices[2].x, RightVertices[2].y}, {RightVertices[3].x, RightVertices[3].y} };
    POINT TempTopVertices[4] = { {TopVertices[0].x, TopVertices[0].y}, {TopVertices[1].x, TopVertices[1].y}, {TopVertices[2].x, TopVertices[2].y}, {TopVertices[3].x, TopVertices[3].y} };
    POINT TempBottomVertices[4] = { {BottomVertices[0].x, BottomVertices[0].y}, {BottomVertices[1].x, BottomVertices[1].y}, {BottomVertices[2].x, BottomVertices[2].y}, {BottomVertices[3].x, BottomVertices[3].y} };

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_01
    TempFrontVertices[0].x += 7;
    TempFrontVertices[0].y -= 40;

    TempFrontVertices[1].x += 7;
    TempFrontVertices[1].y -= 40;

    TempFrontVertices[2].x -= 7;
    TempFrontVertices[2].y -= 70;

    TempFrontVertices[3].x -= 7;
    TempFrontVertices[3].y -= 70;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Bottom_01
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[2].x -= 15;
    TempBottomVertices[2].y -= 25;

    TempBottomVertices[3].x -= 15;
    TempBottomVertices[3].y -= 25;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }

    //Right_01
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x;
    TempRightVertices[1].y;

    TempRightVertices[2].x = TempBottomVertices[2].x;
    TempRightVertices[2].y = TempBottomVertices[2].y;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    Sleep(20);

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_02
    TempFrontVertices[0].x += 4;
    TempFrontVertices[0].y -= 15;

    TempFrontVertices[1].x += 4;
    TempFrontVertices[1].y -= 15;

    TempFrontVertices[2].x;
    TempFrontVertices[2].y -= 30;

    TempFrontVertices[3].x;
    TempFrontVertices[3].y -= 30;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Bottom_02
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[2].x -= 5;
    TempBottomVertices[2].y -= 15;

    TempBottomVertices[3].x -= 5;
    TempBottomVertices[3].y -= 15;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }

    //Right_02
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x;
    TempRightVertices[1].y;

    TempRightVertices[2].x = TempBottomVertices[2].x;
    TempRightVertices[2].y = TempBottomVertices[2].y;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);
    Sleep(20);

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_03
    TempFrontVertices[0].x += 4;
    TempFrontVertices[0].y -= 15;

    TempFrontVertices[1].x += 4;
    TempFrontVertices[1].y -= 15;

    TempFrontVertices[2].x -= 2;
    TempFrontVertices[2].y -= 45;

    TempFrontVertices[3].x -= 2;
    TempFrontVertices[3].y -= 45;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Bottom_03
    TempBottomVertices[0].x = TempFrontVertices[3].x;
    TempBottomVertices[0].y = TempFrontVertices[3].y;

    TempBottomVertices[1].x = TempFrontVertices[2].x;
    TempBottomVertices[1].y = TempFrontVertices[2].y;

    TempBottomVertices[2].x -= 5;
    TempBottomVertices[2].y -= 30;

    TempBottomVertices[3].x -= 5;
    TempBottomVertices[3].y -= 30;

    if (Cube[4] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempBottomVertices, 4);
    }

    //Right_03
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x;
    TempRightVertices[1].y;

    TempRightVertices[2].x = TempBottomVertices[2].x;
    TempRightVertices[2].y = TempBottomVertices[2].y;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }

    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);

    Sleep(20);

    DeleteDC(backMemDC);
    DeleteObject(MemBmp);
    DeleteObject(Pen);
    DeleteObject(gBrush);
    DeleteObject(bBrush);
}

void rotateDown(HDC hdc) {

    HDC backMemDC = CreateCompatibleDC(hdc);
    HBITMAP MemBmp = CreateCompatibleBitmap(hdc, 560, 560);
    SelectObject(backMemDC, MemBmp);

    HPEN Pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH gBrush = CreateSolidBrush(RGB(242, 242, 242));
    HBRUSH bBrush = CreateSolidBrush(RGB(0, 0, 255));

    // Cube Coordinates Copy
    POINT TempBackVertices[4] = { {BackVertices[0].x, BackVertices[0].y}, {BackVertices[1].x, BackVertices[1].y}, {BackVertices[2].x, BackVertices[2].y}, {BackVertices[3].x, BackVertices[3].y} };
    POINT TempFrontVertices[4] = { {FrontVertices[0].x, FrontVertices[0].y }, {FrontVertices[1].x, FrontVertices[1].y }, {FrontVertices[2].x, FrontVertices[2].y }, {FrontVertices[3].x, FrontVertices[3].y } };
    POINT TempLeftVertices[4] = { {LeftVertices[0].x, LeftVertices[0].y}, {LeftVertices[1].x, LeftVertices[1].y}, {LeftVertices[2].x, LeftVertices[2].y} ,{LeftVertices[3].x, LeftVertices[3].y} };
    POINT TempRightVertices[4] = { {RightVertices[0].x, RightVertices[0].y}, {RightVertices[1].x, RightVertices[1].y}, {RightVertices[2].x, RightVertices[2].y}, {RightVertices[3].x, RightVertices[3].y} };
    POINT TempTopVertices[4] = { {TopVertices[0].x, TopVertices[0].y}, {TopVertices[1].x, TopVertices[1].y}, {TopVertices[2].x, TopVertices[2].y}, {TopVertices[3].x, TopVertices[3].y} };
    POINT TempBottomVertices[4] = { {BottomVertices[0].x, BottomVertices[0].y}, {BottomVertices[1].x, BottomVertices[1].y}, {BottomVertices[2].x, BottomVertices[2].y}, {BottomVertices[3].x, BottomVertices[3].y} };

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_01
    TempFrontVertices[0].x -= 10;
    TempFrontVertices[0].y += 25;

    TempFrontVertices[1].x -= 10;
    TempFrontVertices[1].y += 25;

    TempFrontVertices[2].x -= 5;
    TempFrontVertices[2].y += 30;

    TempFrontVertices[3].x -= 5;
    TempFrontVertices[3].y += 30;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Right_01
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x -= 5;
    TempRightVertices[1].y -= 7;

    TempRightVertices[2].x;
    TempRightVertices[2].y;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }

    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);

    Sleep(20);

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_02
    TempFrontVertices[0].x -= 2;
    TempFrontVertices[0].y += 20;

    TempFrontVertices[1].x -= 2;
    TempFrontVertices[1].y += 20;

    TempFrontVertices[2].x += 2;
    TempFrontVertices[2].y += 25;

    TempFrontVertices[3].x += 2;
    TempFrontVertices[3].y += 25;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Right_02
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x -= 5;
    TempRightVertices[1].y += 3;

    TempRightVertices[2].x;
    TempRightVertices[2].y;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }

    //Top_02
    TempTopVertices[0].x += 23;
    TempTopVertices[0].y += 25;

    TempTopVertices[1].x = TempRightVertices[1].x;
    TempTopVertices[1].y = TempRightVertices[1].y;

    TempTopVertices[2].x = TempFrontVertices[1].x;
    TempTopVertices[2].y = TempFrontVertices[1].y;

    TempTopVertices[3].x = TempFrontVertices[0].x;
    TempTopVertices[3].y = TempFrontVertices[0].y;

    if (Cube[1] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempTopVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempTopVertices, 4);
    }

    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);

    Sleep(20);

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_03
    TempFrontVertices[0].x -= 2;
    TempFrontVertices[0].y += 70;

    TempFrontVertices[1].x -= 2;
    TempFrontVertices[1].y += 70;

    TempFrontVertices[2].x += 16;
    TempFrontVertices[2].y += 40;

    TempFrontVertices[3].x += 16;
    TempFrontVertices[3].y += 40;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Right_03
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x -= 13;
    TempRightVertices[1].y += 22;

    TempRightVertices[2].x;
    TempRightVertices[2].y;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }

    //Top_03
    TempTopVertices[0].x -= 15;
    TempTopVertices[0].y += 22;

    TempTopVertices[1].x = TempRightVertices[1].x;
    TempTopVertices[1].y = TempRightVertices[1].y;

    TempTopVertices[2].x = TempFrontVertices[1].x;
    TempTopVertices[2].y = TempFrontVertices[1].y;

    TempTopVertices[3].x = TempFrontVertices[0].x;
    TempTopVertices[3].y = TempFrontVertices[0].y;

    if (Cube[1] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempTopVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempTopVertices, 4);
    }

    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);

    Sleep(20);

    setFrame(backMemDC);
    setBoard(backMemDC);

    //Front_04
    TempFrontVertices[0].x += 3;
    TempFrontVertices[0].y += 55;

    TempFrontVertices[1].x += 3;
    TempFrontVertices[1].y += 55;

    TempFrontVertices[2].x += 8;
    TempFrontVertices[2].y += 25;

    TempFrontVertices[3].x += 8;
    TempFrontVertices[3].y += 25;

    if (Cube[5] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempFrontVertices, 4);
    }

    //Right_04
    TempRightVertices[0].x = TempFrontVertices[1].x;
    TempRightVertices[0].y = TempFrontVertices[1].y;

    TempRightVertices[1].x -= 8;
    TempRightVertices[1].y += 33;

    TempRightVertices[2].x;
    TempRightVertices[2].y;

    TempRightVertices[3].x = TempFrontVertices[2].x;
    TempRightVertices[3].y = TempFrontVertices[2].y;

    if (Cube[3] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempRightVertices, 4);
    }

    //Top_04
    TempTopVertices[0].x -= 8;
    TempTopVertices[0].y += 33;

    TempTopVertices[1].x = TempRightVertices[1].x;
    TempTopVertices[1].y = TempRightVertices[1].y;

    TempTopVertices[2].x = TempFrontVertices[1].x;
    TempTopVertices[2].y = TempFrontVertices[1].y;

    TempTopVertices[3].x = TempFrontVertices[0].x;
    TempTopVertices[3].y = TempFrontVertices[0].y;

    if (Cube[1] == 1) {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)bBrush);
        Polygon(backMemDC, TempTopVertices, 4);
    }
    else {
        SelectObject(backMemDC, (HGDIOBJ)Pen);
        SelectObject(backMemDC, (HGDIOBJ)gBrush);
        Polygon(backMemDC, TempTopVertices, 4);
    }

    BitBlt(hdc, 40, 40, 560, 560, backMemDC, 40, 40, SRCCOPY);

    Sleep(20);

    DeleteDC(backMemDC);
    DeleteObject(MemBmp);

    DeleteObject(Pen);
    DeleteObject(gBrush);
    DeleteObject(bBrush);
}