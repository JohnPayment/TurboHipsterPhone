/*
    TEXTOUT.H
*/

#ifndef TEXTOUT_H
#define TEXTOUT_H

/******************************************************************************
* DEFINES								      								  *
******************************************************************************/

#define FONT6   0			/* Font 6 Points width and 8 Points height */
#define FONT8	1			/* Font 8 Points width and 8 Points height */

// Fontcolors
#define FONT_BLACK 	0
#define FONT_BLUE	1
#define FONT_GREEN 	2
#define FONT_CYAN 	3
#define FONT_RED 	4
#define FONT_MAGENTA 	5
#define FONT_BROWN 	6
#define FONT_LIGHTGREY  7
#define FONT_DARKGRAY 	8
#define FONT_LIGHTBLUE  9
#define FONT_LIGHTGREEN 10
#define FONT_LIGHTCYAN  11
#define FONT_LIGHTRED   12
#define FONT_LIGHTMAGENTA 13
#define FONT_YELLOW 	  14
#define FONT_WHITE 	  15

/******************************************************************************
* ROUTINES								      								  *
******************************************************************************/

void OutGLString(int sx, int sy, char *text, int textcolor, int fontno);
void OutGLchar(int sx, int sy, char text, int textcolor, int fontno);
void SetFont(unsigned char * font, int fontw, int fonth, int fontno);
int  FontGetSize(int fontno);	/* Loword: Fontwidth, HiWord: Fontheight */

#endif /* TEXTOUT_H */