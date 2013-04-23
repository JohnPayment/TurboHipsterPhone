/*
    EDITMAZE.H
    Header file for Main Edit Module

    Last change: 20.01.01 / MP
    		 May, 28  / PM
*/


#ifndef _EDITMAZE_H_
#define _EDITMAZE_H_

#include "jadinput.h"

/******************************************************************************
* DEFINES								      *
******************************************************************************/

/* Menutypes	*/
#define MENU_FONT8   0x01	// if not, FONT6  ( True / False )

/* ---- Numbers of Submenus */
#define EDITMAZE_MENUBASE   100
#define EDITMAZE_FILE	    101
#define EDITMAZE_WALLEDIT   102
#define EDITMAZE_DECORATION 103
#define EDITMAZE_DOORS	    104
#define EDITMAZE_ITEMS	    105
#define EDITMAZE_EVENTS	    106
#define EDITMAZE_WALL       107


#define EDITMAZE_MENUCOUNT    6

/* --- Maxmimum length of Inputstring --- */
#define EDITMAZE_MAXINPLEN   79
#define EDITMAZE_CLEARWALL  201
#define EDITMAZE_SETWALL    202 

/******************************************************************************
* CODE									      *
******************************************************************************/

void EditMaze_SetPos(int x, int y, int side);
void EditMaze_OpenMenu(int x, int y, int menu, INPUTVEC *menupoints);
int  EditMaze_HandleMenuInput(int menu, int choice);

#endif	/* _EDITMAZE_H_ */
