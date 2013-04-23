/*
     JADINPUT.H
     Headerfile for Input-Interface.

     Last change: 20.01.01 / MP
*/

#ifndef _JADINPUT_H_
#define _JADINPUT_H_

/******************************************************************************
* INCLUDES							 	      *
******************************************************************************/

#include "jadglob.h"
#include "jadkeys.h"

/******************************************************************************
* DEFINES							 	      *
******************************************************************************/

/* INPVECTOR-Drawflags for display */
#define JADINPUT_HIGHLIGHT  0x001   /* Highlight the text, if any		*/
#define JADINPUT_ISTEXT     0x002   /* Has a text				*/
#define JADINPUT_HASDARKBK  0x004   /* Draw a dark background	      	*/
#define JADINPUT_HASBACKGR  0x008   /* Has a background for a menu or text	*/
#define JADINPUT_BLACKRECT  0x010   /* Is a black rectangle			*/
#define JADINPUT_HPHORIZBAR 0x020   /* Draw a horizontal hitpoint-bar	*/
#define JADINPUT_HPVERTBAR  0x040   /* Draw a vertical "hitpoint"-bar	*/
#define JADINPUT_ISVISIBLE  0x080   /* Is visible				*/
#define JADINPUT_DRAWCURSOR 0x100   /* Draw a cursor				*/

/* Standard inputevent codes */
#define JADINPUT_EDITVEC    202	


/******************************************************************************
* TYPEDEFS							 	      *
******************************************************************************/

typedef struct {
    unsigned short buttons;   /* Mouse-Buttons, that activate this Input */
    unsigned short drawflags; /* And additional flags for display	 */
    unsigned short key;       /* Key to activate this Input		 */
    JADRECT	   rect;      /* Rectangle for mouse.			 */
    unsigned short maincode;  /* What to do for main choose (procno) */
    unsigned short subleft;   /* Code to return,if left mouse button or  	*/
    			      /* the appropriate key is pressed (if any) 	*/
    unsigned short subright;  /* Code to return, if right mouse button is pressed */
    char *text;		      /* Text in rectangle, if any			*/
    short int  textcol;       /* Number of textcolor.			    	*/
    short int  argval;	      /* A value to display or as an procedure argument */
    int        helpval;	      /* For different purposes				*/	 
} INPUTVEC;

typedef struct {
    int maincode;		/* Maincode from INPUTVEC, if in a rect	*/
    int subcode; 	 	/* 0: If no click appeared 		*/
    int relx,                   /* Relative position ( from upper     	*/
        rely;                   /* left ) into the INPUTVEC-rectangle 	*/
    INPUTVEC *inpvec;		/* The chosen inputvector		*/
} INPEVENT;

typedef int (* JADEVENTHANDLER)(INPEVENT *inpevent);

/******************************************************************************
* CODE									      *
******************************************************************************/

void JadInput_OpenInputWindow(INPUTVEC *inpvec, JADEVENTHANDLER eventhandler);
void JadInput_InputBox(char *text, JADEVENTHANDLER eventhandler);
void JadInput_YesNoMsg(char *text, JADEVENTHANDLER eventhandler);
void JadInput_MsgBox(char *text);
/* Change values... */
void JadInput_SetMazeValues(int side, int level, char wallno);

#endif  /* _JADINPUT_H_ */
