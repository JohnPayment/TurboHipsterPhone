/*
    JADGAME.H
*/

#ifndef _JADGAME_H_
#define _JADGAME_H_

/******************************************************************************
* DEFINES								      *
******************************************************************************/

/* Clock functions */
#define JAD_CLOCKNEW     1
#define JAD_CLOCKGET     2
#define JAD_CLOCKADD     3
#define JAD_CLOCKDEL     4	/* Delete a clock */
#define JAD_CLOCKCLEAR   5
#define JAD_CLOCKSTOP    6
#define JAD_CLOCKSTART   7

/* Standard clocks (always running), despite of CLOCKSTART / STOP  */
#define CLOCK_FRAMETIMER 0

/* Further standard - Clocks */
#define JADGAME_MAINCLOCK   	0	/* JADCLOCK_MAINTIMER */
#define JADGAME_EVENTCLOCK  	1
#define JADGAME_RANDSOUNDCLOCK  2

/******************************************************************************
* GLOBAL TYPEDEFS							      *
******************************************************************************/

/******************************************************************************
* GLOBAL DATA								      *
******************************************************************************/

/******************************************************************************
* GLOBAL CODE 								      *
******************************************************************************/

/* The platform dependent routines */
int  JadEdit_ClockFunction(int clockno, int amount, int func);

/* The main routines, to be delivered by exported by external gamecode */
char *JadEdit_Init(int val, void *data);
void JadEdit_Main(int jadfps, int tickspassed);
void JadEdit_Shutdown(void);
void JadEdit_ClearInput(void);
void JadEdit_Quit(void);

/* Called by the mouse & keyboard */
int  JadEdit_HandleInput(int x, int y, int button, int key);

#endif // _JADGAME_H_