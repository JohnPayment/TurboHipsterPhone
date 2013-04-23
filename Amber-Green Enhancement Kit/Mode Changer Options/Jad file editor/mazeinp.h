/*
     MAZEINP.H
     Headerfile for Input-Interface.

     Last change: 20.01.01 / MP
*/

#ifndef MAZEINP_H
#define MAZEINP_H

#include "jadedit.h"	/* THe main routine */

/******************************************************************************
* DEFINES							 	      *
******************************************************************************/

#define KEY_JUMP            0
#define KEY_LEFT_USE        1
#define KEY_LEFT_GET        2
#define KEY_LEFT_PACK       3
#define KEY_RIGHT_USE       4
#define KEY_RIGHT_GET       5
#define KEY_RIGHT_PACK      6
#define KEY_MESSAGE         7
#define KEY_CAMERA_LEFT     8
#define KEY_CAMERA_RIGHT    9
#define KEY_CAMERA_IN       10
#define KEY_CAMERA_OUT      11
#define KEY_UP              12
#define KEY_DOWN            13
#define KEY_LEFT            14
#define KEY_RIGHT           15


#define INP_BUTTONNONE  0
#define INP_BUTTONLEFT  1
#define INP_BUTTONRIGHT 2
#define INP_BUTTONBOTH  3
#define INP_NOBUTTON	0x40	// No mouse button
#define INP_INACTIVE	0x80	// Not active

#define INP_ARROWUP    KEY_UP	/* Low char zero means special key */
#define INP_ARROWDOWN  KEY_DOWN
#define INP_ARROWLEFT  KEY_LEFT
#define INP_ARROWRIGHT KEY_RIGHT

/* INPVECTOR-Flags for display */
#define INP_HIGHLIGHT   0x100   /* Highlight the text, if any		*/
#define INP_ISTEXT      0x200	/* Has a text				*/
#define INP_HASDARKBK   0x400 	/* Draw a dark background	      	*/
#define INP_HASBACKGR   0x800	/* Has a background for a menu or text	*/
#define INP_ISVISIBLE  0x1000   /* Is visible				*/
#define INP_DISPFLAGS  0xFF00

/******************************************************************************
* TYPEDEFS							 	      *
******************************************************************************/

typedef struct {
    unsigned short int buttons;	/* Mouse-Buttons, that activate this Input */
    unsigned short int key; 	/* Key to activate this Input		   */
    int x,
        y,
        width,
        height;			/* Rectangle for mouse.			   */
    unsigned short int  maincode;
    unsigned char subleft;     /* Code to return,if left mouse button or  	   */
    			       /* the appropriate key is pressed (if any) 	   */
    unsigned char subright;    /* Code to return, if right mouse button is pressed */
    char *text;		       /* Text in rectangle, if any			   */
} INPUTVEC;

typedef struct {
    int code;
    int maincode;		/* Maincode from INPUTVEC, if in a rect	*/
    int subcode; 	 	/* 0: If no click appeared 		*/
    int relx,                   /* Realtive positon ( from upper     	*/
        rely;                   /* left ) into the INPUTVEC-rectangle 	*/
} INPEVENT;

/******************************************************************************
* CODE									      *
******************************************************************************/

int  MazeInp_InputEvent(int x, int y, int button, int key);  /* To be called by the mouse & keyboard    */

void MazeInp_SetEvents(INPUTVEC *inpvec);
int  MazeInp_EventHandler(INPEVENT *inpevent);	/* To be written by the user */

/* The standard inputboxes */
void MazeInp_InputText(char *title, char *text, int maxlen, char curpos);
void MazeInp_YesNoMsg(char *text);
void MazeInp_DisplayList(int x,int y,int width,int height, char *List);

#endif  /* MAZEINP_H */
