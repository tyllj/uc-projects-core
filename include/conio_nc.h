//
// Created by tyll on 06.02.22.
//

#ifndef UC_CORE_CONIO_H
#define UC_CORE_CONIO_H
// Copyright (C) 2005  by Piotr He³ka (piotr.helka@nd.e-wro.pl)
// Linux C++ (not full) implementation of Borland's conio.h
// v 1.01
// It uses Ncurses lib, so accept also its terms.



// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef __NCURSES_H
#include <ncurses.h>
#endif

#ifndef __CONIO_H
#define __CONIO_H
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#define MAX_OKIEN 256

#define BLACK       0
#define RED         1
#define GREEN       2
#define BROWN       3
#define BLUE        4
#define MAGENTA     5
#define CYAN        6
#define LIGHTGRAY   7
#define DARKGRAY    0
#define LIGHTRED    1
#define LIGHTGREEN  2
#define YELLOW      3
#define LIGHTBLUE   4
#define PINK        5
#define LIGHTCYAN   6
#define WHITE       7

#undef getch
#define getch CURSgetch

#undef getche
#define getche CURSgetche


void initConio();

class ConioWrapper
{	public:
    ConioWrapper(){ initConio(); }
    ~ConioWrapper(){ endwin(); }
};

inline ConioWrapper conioWrapper;

typedef struct
{
    int 	xup;
    int 	yup;
    int 	xdown;
    int 	ydown;
    WINDOW*	nc_window;
} Window;

bool	conioInitialized = FALSE; //czy juz po initscr() ?
int	wideCharacter = -1; //potrzebne do getch'a
int	n = 0; //liczba uzytych okienek

short	foregroundColor = COLOR_WHITE;
short	backgroundColor = COLOR_BLACK;
short	currentColorPair;

Window	windows[MAX_OKIEN];
WINDOW*	activeWindow = NULL;



// ----------------------------- koniec globalnych ------------

void initConio()
{
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    activeWindow = stdscr;
    conioInitialized = TRUE;

    short kolor = 1;
    for(short i=0; i<8; i++) {
        for(short j=0; j<8; j++, kolor++) {
            init_pair(kolor,i,j);
            if(i == COLOR_WHITE && j == COLOR_BLACK) {
                currentColorPair = kolor;
            }
        }
    }

    wrefresh(activeWindow);
}

int simple_strlen(char* str) {
    char* p;
    for(p = str; *p != 0; p++);
    return p-str;
}

void cputs(char* str) {
    waddstr(activeWindow, str);
    wrefresh(activeWindow);
}

char* cgets(char* str) {
    if(str == NULL || *str == 0)
    {
        *(str+1) = 0;
        return NULL;
    }

    int max = (int)(*str);

    echo();

    if(wgetnstr(activeWindow, (str + 2), max) == ERR)
    {
        *(str+1) = 0;
        return NULL;
    }

    noecho();

    *(str+1) = (char)simple_strlen(str+2);

    return str+2;
}

void clreol() {
    wclrtoeol(activeWindow);
    wrefresh(activeWindow);
}

void clrscr() {
    if(!conioInitialized) initConio();
    wbkgd(activeWindow, COLOR_PAIR(currentColorPair));
    //trzeba przesunac kursor? chyba nie...
    wclear(activeWindow);
}

int cprintf(char *fmt, ...) {
    if(!conioInitialized) initConio();

    va_list ap;
    va_start(ap, fmt);


    int i = vwprintw(activeWindow, fmt, ap);

    va_end(ap);

    wrefresh(activeWindow);

    return i;
}

int cscanf(char *fmt, ...) {
    if(!conioInitialized) initConio();

    echo();

    va_list ap;
    va_start(ap, fmt);

    int i = vwscanw(activeWindow, fmt, ap);

    va_end(ap);

    wrefresh(activeWindow);
    noecho();

    return i;
}

int CURSgetch() {
    if(!conioInitialized) initConio();

    int character;

    if(wideCharacter > 0) {
        character = wideCharacter;
        wideCharacter = -1;

        return character - 265 + 59;
    }

    character = wgetch(activeWindow);

    if(character > 255)
    {
        wideCharacter = character;
        return 0;
    }

    return character;
}

int CURSgetche() {
    echo();
    int character = getch();
    noecho();
    return character;
}

int gotoxy(int x, int y) {
    if(!conioInitialized) initConio();
    wmove(activeWindow, y - 1, x - 1);
    return 0;
}

int kbhit() {
    int character;
    wtimeout(activeWindow, 0);
    character = wgetch(activeWindow);
    nodelay(activeWindow, FALSE);
    if (character == ERR) return 0;
    ungetch(character);
    return 1;
}

int putch(int character) {
    return wechochar(activeWindow, character);
}

void textbackground(short color) {
    if(!conioInitialized) initConio();
    backgroundColor = color % 8;
    short k=1;
    for(short i=0; i<8; i++) {
        for(short j=0; j<8; j++, k++) {
            if(foregroundColor == i && backgroundColor == j) {
                currentColorPair = k;
                wbkgd(activeWindow, COLOR_PAIR(k));
            }
        }
    }

    wrefresh(activeWindow);
}

void textcolor(short color) {
    if(!conioInitialized) initConio();
    foregroundColor = color % 8;

    short k=1;
    for(short i=0; i<8; i++) {
        for(short j=0; j<8; j++, k++) {
            if(foregroundColor == i && backgroundColor == j)
            {
                currentColorPair = k;
                wcolor_set(activeWindow, k, NULL);
            }
        }
    }

    wrefresh(activeWindow);
}

int wherex(void) {
    if(!conioInitialized) initConio();
    int x, y;
    getyx(activeWindow, y, x);
    return x + 1;
}

int wherey(void) {
    if(!conioInitialized) initConio();
    int x, y;
    getyx(activeWindow, y, x);
    return y + 1;
}

void window(int xup, int yup, int xdown, int ydown) {
    if( xup<1 || yup<1 || xdown>COLS || ydown>LINES) {
        xdown = COLS - xup;
        ydown = LINES - yup;
    }

    bool exists = FALSE;

    if(!conioInitialized) initConio();

    for(int i=0; i<n && !exists; i++) {
        if(windows[i].xup == xup && windows[i].yup == yup
           && windows[i].xdown == xdown && windows[i].ydown == ydown) {
            activeWindow = windows[i].nc_window;
            exists = TRUE;
            clrscr();
        }
    }

    if(!exists && n < MAX_OKIEN) {
        activeWindow = newwin(ydown - yup + 1, xdown - xup + 1, yup - 1, xup - 1);

        windows[n].nc_window = activeWindow;
        windows[n].xup = xup;
        windows[n].yup = yup;
        windows[n].xdown = xdown;
        windows[n].ydown = ydown;

        wcolor_set(activeWindow, currentColorPair, NULL);
        wbkgd(activeWindow, COLOR_PAIR(currentColorPair));

        cbreak();
        noecho();
        keypad(activeWindow, TRUE);
        scrollok(activeWindow, TRUE);

        n++;
    }

    wrefresh(activeWindow);

    return;
}

#pragma GCC diagnostic pop

#endif //UC_CORE_CONIO_H
