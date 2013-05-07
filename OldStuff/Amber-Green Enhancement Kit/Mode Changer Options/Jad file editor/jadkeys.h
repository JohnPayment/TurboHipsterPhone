/*
     JADKEYS.H
     The key definitions for JAD.
*/

#ifndef _JADKEYS_H_
#define _JADKEYS_H_

/* --- Mouse keys --- */

#define JAD_MOUNONE    0x00
#define JAD_MOULEFT    0x01
#define JAD_MOURIGHT   0x02
#define JAD_MOUMIDDLE  0x04
#define JAD_MOUBOTH    0x03
#define INP_DRAWONLY   0x10
#define INP_EDIT       0x20   /* Edit box input				*/
#define INP_KEYBOARD   0x40   /* Keyboard input				*/
#define INP_ACTIVE     0x80   /* Active		                        */



/* --- Character movement --- */
#define JAD_FORWARD   0x10	/* Low char zero means special key */
#define JAD_BACKWARD  0x11
#define JAD_STEPLEFT  0x12
#define JAD_STEPRIGHT 0x13
#define JAD_TURNLEFT  0x14
#define JAD_TURNRIGHT 0x15
#define JAD_STOP      0x16
#define JAD_CAMMODE   0x17	/* Switch between eyes of first character */
				/* in character array and third person 	  */
                                /* perspective.	     			  */

/* --- Edit commands --- */
#define JAD_ENTERKEY    0x21
#define JAD_ESCAPEKEY   0x22
#define JAD_BACKSPACE   0x23
#define JAD_TAB         0x24
#define JAD_ARROWUP     0x25
#define JAD_ARROWDOWN   0x26
#define JAD_ARROWLEFT   0x27
#define JAD_ARROWRIGHT  0x28
#define JAD_CLOSEINPBOX 0x29


/* --- Camera control --- */
#define JAD_CAMERA_LEFT     0x61
#define JAD_CAMERA_RIGHT    0x62
#define JAD_CAMERA_IN       0x63
#define JAD_CAMERA_OUT      0x64
#define JAD_CAMERA_UP       0x65
#define JAD_CAMERA_DOWN     0x66

#endif /* _JADKEYS_H_ */




