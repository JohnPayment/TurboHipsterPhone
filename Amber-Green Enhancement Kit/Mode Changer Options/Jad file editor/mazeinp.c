/*
     MAZEINP.C
     Every input, that has to be handled. Including display of menus,
     input- and messageboxes.

     Last changes: 20.01.01 / MP
     		   21.01.01 / MP
     		   23.01.01 / PM
                   28.01.01 / MP	
*/

#include <stdio.h>
#include <string.h>	/* strcat */

#include "mazeinp.h"
#include "display.h"
#include "textout.h"
#include "maze.h"
#include "editmaze.h"
#include "animated.h"

/******************************************************************************
* DEFINES		      						      *
******************************************************************************/

#define HALFMAZESIZE	     (MAZESIZE / 2)

#define CAMERA_INPUT   	     100
#define PROMPT_INPUT	     101	
#define MENU_INPUT           200
#define MENUMAP_INPUT	     201	/* Do something with the maze-map */
#define MENU_ESC               1
#define MENU_WHOLE             2
#define MENU_SCROLLMAZEUP     10
#define MENU_SCROLLMAZEDOWN   11
#define MENU_SCROLLMAZELEFT   12
#define MENU_SCROLLMAZERIGHT  13
#define MENU_MAZE     	      14
#define MENU_MOVEMAZE         15

/* Menuvec-Start */
#define MENU_MAINMENUVEC 13
#define MENU_SCROLLVEC    6
#define MENU_MAZEVEC	 12
#define MENU_MENUPOINTS  21     /* From this point, Menupoints, if any */

/******************************************************************************
* TYPEDEFS								      *
******************************************************************************/

typedef struct {
    char *title;		/* Title of the prompt box	*/
    char cursorpos;		/* Position of the inputcursor	*/
    int  maxlen;		/* Maximum length of input	*/
    INPUTVEC box;       	/* The whole box		*/
    INPUTVEC leftbutton;	/* OK / Yes...			*/
    INPUTVEC rightbutton;	/* Cancel / No			*/
    INPUTVEC prompt;		/* Holds also a pointer on the  */
    				/* userinput buffer		*/
    INPUTVEC stop;		/* Just to have an empty field to stop it */
} PROMPTBOX;

/******************************************************************************
* DATA									      *
******************************************************************************/
CAMERA MazeInpCamera = {
    { 448.0, 115.95, 256.0 },	/* Position of viewer			*/
    { 0.0, 0.0, 0.0 },		/* "LookAt"			        */
    15.5,	       		/* Rotation Angle round "Lookat"-Point  */
    462.76,			/* Distance on plane Eye -> LookAt      */
    1.0				/* Zoom-Factor			        */
};

	/* All data needed to know which part of the Mazemap to */
	/* display. Also which part to display in the 3D-View.  */
static struct {
    int left,
        top; 	/* 0 .. ((MAZESIZE / 2) - 1). Position in Map-Window   */
    int posx,
        posy,	/* At which position the actual edit takes place 	*/
        side;	/* At which side of a tile the actual edit takes place. */
        	/* -1, if all four sides of the tile are to change	*/
    int rectx,
        recty,
        rectw,
        recth;  /* The border of the visible area in the 3D-View 	*/
    unsigned char map[MAZESIZE * MAZESIZE];
} MazeMap = { 0, 0, 4, 4, -1, 0, 0, 72, 72,  };


/* ------- Info - Variables ----- */
char StatusText[256];

static int mposx = 0,
	   mposy = 0;

/* ------- MENU ------ */
// int	   menustack[20];
// int        pmenustack = 0;  /* Pointer in "menustack" */
/* -------------- */
char       CameraAngle[30];	/* For display Camera-Text */
INPUTVEC   *InputVecs = 0;
INPUTVEC   *ActInputVec = 0;	/* Pointer on actual highlighted Menu */
INPUTVEC   *DisplayVecs = 0;    /* Additional vecs to display	      */

/* --------- The Inputvecs ---------- */
INPUTVEC InputArray[60] = {
    /* First the ARROW_KEY's, no Mouse input */
    { INP_NOBUTTON, KEY_CAMERA_LEFT, 0, 0, 0, 0,  CAMERA_INPUT, CAMERA_LEFT },
    { INP_NOBUTTON, KEY_CAMERA_RIGHT, 0, 0, 0, 0, CAMERA_INPUT, CAMERA_RIGHT },
    { INP_NOBUTTON, KEY_CAMERA_IN, 0, 0, 0, 0, CAMERA_INPUT, CAMERA_ZOOMIN },
    { INP_NOBUTTON, KEY_CAMERA_OUT, 0, 0, 0, 0, CAMERA_INPUT, CAMERA_ZOOMOUT },
    { INP_NOBUTTON, 'u', 0, 0, 0, 0, CAMERA_INPUT, CAMERA_UP },
    { INP_NOBUTTON, 'd', 0, 0, 0, 0, CAMERA_INPUT, CAMERA_DOWN },
    /* ----- The four scroll - arrows of maze map ---- */
    { INP_BUTTONLEFT | INP_HASBACKGR, 0, 364,  16, 16, 16, MENU_INPUT, MENU_SCROLLMAZEUP },
    { INP_BUTTONLEFT | INP_HASBACKGR, 0, 624, 276, 16, 16, MENU_INPUT, MENU_SCROLLMAZERIGHT },
    { INP_BUTTONLEFT | INP_HASBACKGR, 0, 364, 260, 16, 16, MENU_INPUT, MENU_SCROLLMAZEDOWN },
    { INP_BUTTONLEFT | INP_HASBACKGR, 0, 380, 276, 16, 16, MENU_INPUT, MENU_SCROLLMAZELEFT },
    /* --- Scrollbars for maze map --- */
    { INP_NOBUTTON | INP_HASBACKGR, 0, 364,  32,  16, 228 },	/* Vertical   */
    { INP_NOBUTTON | INP_HASBACKGR, 0, 396, 276, 228,  16 },	/* Horizontal */
    /* Here is the "maze" window */
    { INP_BUTTONLEFT | INP_BUTTONRIGHT | INP_HASDARKBK, 0, 380, 16, 260, 260,
    				      	 MENU_INPUT, MENU_MAZE, MENU_MOVEMAZE },
    /* Measure of Main-Menu */
    { INP_NOBUTTON  | INP_HASBACKGR, 0, 0, 0, 640, 16 },
    /* Here comes the main menu */
    { INP_BUTTONLEFT | INP_ISTEXT, 0,   4, 4, 36, 8, MENU_INPUT, EDITMAZE_FILE, MENU_ESC, "File" },
    { INP_NOBUTTON | INP_BUTTONLEFT | INP_ISTEXT, 0,  40, 4, 48, 8, MENU_INPUT, EDITMAZE_WALLEDIT, MENU_ESC, "Walls" },
    { INP_NOBUTTON | INP_BUTTONLEFT | INP_ISTEXT, 0,  88, 4, 96, 8, MENU_INPUT, EDITMAZE_DECORATION, MENU_ESC, "Decorations" },
    { INP_NOBUTTON | INP_BUTTONLEFT | INP_ISTEXT, 0, 184, 4, 48, 8, MENU_INPUT, EDITMAZE_DOORS, MENU_ESC, "Doors" },
    { INP_NOBUTTON | INP_BUTTONLEFT | INP_ISTEXT, 0, 232, 4, 48, 8, MENU_INPUT, EDITMAZE_ITEMS, MENU_ESC, "Items" },
    { INP_NOBUTTON | INP_BUTTONLEFT | INP_ISTEXT, 0, 280, 4, 56, 8, MENU_INPUT, EDITMAZE_EVENTS, MENU_ESC, "Events" },
    /* Status-window */
    { INP_NOBUTTON | INP_HASBACKGR | INP_ISTEXT, 0, 380, 292, 260, 56, 0, 0, 0, StatusText }, /* Status-Window */
    /* From this point, Menupoints, if any */
    { 0 },
    { 0 }
};

	/* ---------- Standard - InputBoxes --------- */

PROMPTBOX PromptInputString = {
    0, 0, 0,
    { INP_NOBUTTON | INP_HASBACKGR, 0, 188, 206, 264, 56 },
    { INP_BUTTONLEFT | INP_ISTEXT | INP_HASBACKGR, 0, 192, 242, 24, 16, PROMPT_INPUT, 2, 0, "Ok" },
    { INP_BUTTONLEFT | INP_ISTEXT | INP_HASBACKGR, 0, 334, 242, 56, 16, PROMPT_INPUT, 1, 0, "Cancel" },
    { INP_NOBUTTON | INP_HASDARKBK, 0, 192, 222, 248, 16 },
    { 0 }
};

PROMPTBOX PromptYesNoMsg = {
    0, 0, 0,
    { INP_NOBUTTON | INP_HASBACKGR, 0, 188, 206, 264, 68 },
    { INP_BUTTONLEFT | INP_ISTEXT | INP_HASBACKGR, 0, 192, 254, 32, 16, PROMPT_INPUT, 2, 0, "Yes" },
    { INP_BUTTONLEFT | INP_ISTEXT | INP_HASBACKGR, 0, 424, 254, 24, 16, PROMPT_INPUT, 1, 0, "No" },
    { 0 }
};

PROMPTBOX PromptMsgBox = {
    0, 0, 0,
    { INP_NOBUTTON | INP_HASBACKGR, 0, 188, 206, 264, 68 },
    { INP_BUTTONLEFT | INP_ISTEXT | INP_HASBACKGR, 0, 308, 254, 24, 16, PROMPT_INPUT, 2, 0, "Ok" },
    { 0 }
};


/******************************************************************************
* CODE									      *
******************************************************************************/

/*
 * Name:
 *     posinrect
 * Description:
 *     Checks if the given x,y position is in the given mouvec-rect
 * Input:
 *     inpev:  Pointer on event with position
 *     inpvec: Pointer on inputvec which holds the rectangle to check
 * Output:
 *     1: if Position in rect
 *     0: if not
 *     inpevent: the xrel + yrel are set to the relative position to the
 *		 checked rect.
 */

int posinrect(INPUTVEC *inpvec, INPEVENT *inpev)
{
    /* First check horizontally */
    inpev -> relx -= inpvec -> x;
    if (inpev -> relx >= 0) {				/* Is right of X */
        if (inpev -> relx > inpvec -> width)
            return 0;			  /* Right side out of Rectangle */

        /* Now check vertically */
    	inpev -> rely -= inpvec -> y;
	if (inpev -> rely >= 0) {		    /* Is Lower of Y-Pos */
            if (inpev -> rely <= inpvec -> height)
            	return 1;			    /* Is in Rectangle   */
	}
    }
    return 0;
}

/*
 *  Name:
 *      InputEvent
 *  Description:
 *	Hands the Mouse-Position and buttons to the Mouse-Eventhandler.
 *	- Event from inputvecs, if any.
 *	- Number of field the mouse is on, if any.
 *	- Position of the mouse relative to the event-rectangle, if on any.
 *  Input:
 *	x, y:   Mouse-Position
 *	button: Which buttons are clicked.
 *		INP_LEFT / INP_RIGHT / INP_NONE
 *	key:	if 0, then check mouse, else keys
 * Output:
 *	1:      Stop event loop
 *	0:	Get next event
 */

int MazeInp_InputEvent(int x, int y, int button, int key)
{
    static   INPEVENT inpev;		/* Static buffer for the event-result */
    INPUTVEC *inpvec;
    int done;

    memset(&inpev,0,sizeof(INPEVENT));	/* Assume nothing happened	*/

    if (key == 0) {
    	 mposx = x; 			/* Set the internal mouseposition */
	 mposy = y;
    }


    done = 0;
    ActInputVec = 0;
    button &= INP_BUTTONBOTH;		/* Only Mousebutton-Flags */

    /* Now check the position, if there is an Event-Array */
    if (InputVecs ) {				/* There is any to check... */
    	inpvec = InputVecs;

        while (inpvec -> buttons != 0) { 	  /* While theres something to check  */
            inpvec -> buttons &= ~INP_HIGHLIGHT;  /* Remove possible highlight - flag */

            if (key == 0) {
                /* Then check the mouse, if asked 	*/
            	inpev.relx = x;		/* move the mousepos to the Input-Event */
            	inpev.rely = y;

            	if (posinrect(inpvec, &inpev) ) {
            	    /* Mouse is in this rectangle*/
                    inpev.maincode = inpvec ->maincode;	/* Mouse on rectangle */
                    inpvec -> buttons |= INP_HIGHLIGHT; /* Set new highlight  */

                    /* TODO: Change the position of the Menu-Box, if the */
                    /*	     mouse moves to a new main menupoint	 */


                    if ((inpvec -> buttons & button) != 0) {
                    	/* If this mouseclick is asked for */
                    	switch(button) {
        	            case INP_BUTTONLEFT:
			    	if (inpvec -> buttons & INP_BUTTONLEFT)
                                    inpev.subcode = inpvec -> subleft;
                            	break;
	            	    case INP_BUTTONRIGHT:
                            	if (inpvec -> buttons & INP_BUTTONRIGHT)
                                    inpev.subcode = inpvec -> subright;
                            	break;
            	    	} /* switch button */

                    	/* Now call the event Handler */
                    	return MazeInp_EventHandler(&inpev);
                    } /* if Button*/

                    ActInputVec = inpvec;
                }  /* if posinrect */

            } /* if key == 0 */
            else {
                /* check the key */
                if (key == inpvec -> key) {
                    inpev.maincode = inpvec -> maincode;
                    inpev.subcode = inpvec -> subleft;
                    /* Now call the event Handler */
                    done = MazeInp_EventHandler(&inpev);
                    ActInputVec = inpvec;
                    return done ;
                }
            }

            inpvec++;
        } /* while (inpvec -> buttons!= 0) */
        
    }   /* if InputVecs */

    return 0;
}


/*
 * Name:
 *     MouseSetCursor
 * Description:
 *    Sets the cursor-Pic of the mouse.
 * Input:
 *    unsigned char: Number of Cursor-Pic
 *
 */

#pragma argsused
void MazeInp_SetMouseCursor(int number)
{

}

/*
 * Name:
 *     InputSetEvents
 * Description:
 *    Hands to the Input the Vectors with the rectangles, the mouse
 *    has to check as well with the key, that belong to the vector.
 *
 * Input:
 *    inpvec: Pointer on array of INPUTVECS , if NULL, the vectors are reset
 *	      to none. The array has to be ended by a 0 in the "Buttons" - field.
 */

void MazeInp_SetEvents(INPUTVEC *inpvec)
{
    InputVecs = inpvec;
}

/*
 * Name:
 *     InputEventHandler
 * Description:
 *     Do's all the Input-stuff
 * Input:
 *     inpev:  Pointer on Inputevent
 * Output:
 *     1: Stop inputevents
 *     0: Get next.
 */

int MazeInp_EventHandler(INPEVENT *inpev)
{
    int units,		/* For change of Camera 		*/
        kind;           /* subcode				*/
    int mposx, mposy;	/* A new position in the map 		*/
    int editpos;	/* Edit position in the visible area	*/


    kind = inpev -> subcode;

    switch (inpev -> maincode) {
    	case CAMERA_INPUT: {
                switch (kind) {
            	    case CAMERA_UP:
                    case CAMERA_DOWN:
                    	units = 8;
                    	break;
                    case CAMERA_LEFT:
                    case CAMERA_RIGHT:
                        units = 5;
                        break;
                    case CAMERA_ZOOMIN:
                    case CAMERA_ZOOMOUT:
                        units = 16;
                } /* switch */

	        Display_MoveCamera(&MazeInpCamera, kind, units);
                return 0;
            }  /* case CAMERA_INPUT */

        case MENU_INPUT: {
            	switch (kind) {
            	    case MENU_MAZE: 	/* Change a tile in maze */
                    	mposx = (inpev -> relx/8) + MazeMap.left;
                        mposy = (inpev -> rely/8) + MazeMap.top;

                    	/*  Clear a tile in the map */
			Maze_SetWall(TILENO(mposx, mposy),0);
                        break;

                    case MENU_MOVEMAZE:
                        mposx = (inpev -> relx / 8) + MazeMap.left;
                        mposy = (inpev -> rely / 8) + MazeMap.top;

                        /* Adjust position, if needed  */
                        if (mposx < (HALFMAZESIZE - 5))
                            mposx = (HALFMAZESIZE - 5);
			if (mposx < 4) mposx = 4;
                        if (mposy < (HALFMAZESIZE - 5))
                            mposy = (HALFMAZESIZE - 5);
			if (mposy < 4) mposy = 4;

                    	/* Set a solid tile in the map */
                        Maze_SetWall(TILENO(mposx,mposy) ,1);
                        /* Move the middle of Viewing rectangle */
                        MazeMap.posx  = mposx;
                        MazeMap.posy  = mposy;
                        MazeMap.rectx = (mposx - MazeMap.left - 4) * 8;
                        MazeMap.recty = (mposy - MazeMap.top - 4) * 8;

                        /* MazeInpCamera: "LookAt" is moved to the tile, that */
                        /* has the Edit-Focus.        			      */
                        /*
                        Display_SetCameraLookAt(&MazeInpCamera,
                        			mposx - MazeMap.left,
                                                mposy + MazeMap.top);
                        */
                        break;
                    case  MENU_SCROLLMAZEUP:
                        if (MazeMap.top > 0) {
                            MazeMap.top--; 	/* Top of the map */
                        }
                        else {
                            /* Just move the border of the visible area */
                            if ( MazeMap.posy > 5) {
                            	MazeMap.posy--;     /* Middle of visible area */
                            	MazeMap.recty -= 8; /* The border of the visible area  */
                            }
                        }
                    	break;
		    case MENU_SCROLLMAZEDOWN:
                    	if (MazeMap.top < HALFMAZESIZE) {
                            MazeMap.top++; 	/* Top of the map */
                        }
                        else {
                            /* Just move the displayrect */
                            if ( MazeMap.posy < (HALFMAZESIZE - 5)) {
                            	MazeMap.posy++;     /* Middle of visible area */
                            	MazeMap.recty += 8; /* The border of the visible area  */
                            }
                        }
                    	break;
                    case MENU_SCROLLMAZELEFT:
                        if (MazeMap.left > 0) {
                            MazeMap.left--; 	/* Top of the map */
                        }
                        else {
                            if ( MazeMap.posx > 5) {
                            MazeMap.posx--;     /* Middle of visible area */
			    MazeMap.rectx -= 8; /* The border of the visible area  */
                            }
                        }
                    	break;
                    case MENU_SCROLLMAZERIGHT:
                        if (MazeMap.left < HALFMAZESIZE) {
                            MazeMap.left++; 	/* Top of the map */
                        }
                        else {
                            if (MazeMap.posx < (HALFMAZESIZE - 5)) {

                            	MazeMap.posx++;     /* Middle of visible area */
			    	MazeMap.rectx += 8; /* The border of the visible area  */
                            }
                        }
                        break;
                    default:
                        /* Hand it to "Editmaze" */
 			EditMaze_OpenMenu(ActInputVec -> x, ActInputVec -> y + 16,
                        		  kind,
                        		  &InputArray[MENU_MENUPOINTS]);
                        return 0;
                }

                /* Get the mazemap */
                Maze_SetMap(MazeMap.left, MazeMap.top, 32, 32, MazeMap.map);

	        /* Set up the display */
	        Maze_SetDisplayList(Display_TileList,&MazeMap.posx, &MazeMap.posy);

                /* Set the edit-rectangle */
		editpos = ((MazeMap.posy - MazeMap.top) * VIEWMESHSIZE)
                	   + (MazeMap.posx - MazeMap.left) ;
		Display_TileList[editpos].flags  |= TILE_ISEDIT;
		Display_TileList[editpos].direction = MazeMap.side;	/* Which side to put */
            } /* case MENU_INPUT */
            break;

        /* Submenu */
        case EDITMAZE_FILE: {		// 101
        	return EditMaze_HandleMenuInput(EDITMAZE_FILE, kind,
			      		        MazeMap.posx, MazeMap.posx,
                                                MazeMap.side);
            } /* case EDITMAZE_FILE */

        case EDITMAZE_WALLEDIT: {	// 102
            } /* EDITMAZE_WALLEDIT */
            break;

        case EDITMAZE_DECORATION: {	// 103
            } /* EDITMAZE_DECORATION */
            break;

        case EDITMAZE_DOORS: {		// 104
            } /* EDITMAZE_DOORS */
            break;

        case EDITMAZE_ITEMS: {		// 105
            } /* EDITMAZE_ITEMS */
            break;

        case EDITMAZE_EVENTS: {	    	// 106
            } /* EDITMAZE_EVENTS */
            break;
    }  /*  switch (inpev -> maincode) */

    /* Otherwise clear a possible visible menu */
    InputArray[MENU_MENUPOINTS].buttons = 0;

    return 0;
}

/******************************************************************************
* MENU DRAWING				  				      *
******************************************************************************/

/*
 * Name:
 *     MazeInp_DrawMazemap
 * Description:
 *     Draws the Mazemap in the Menu-Mazewindow. Uses "MazeMap".
 *     The maze-map has to be filled by the caller.
 * Input:
 *     None
 *
 */
void MazeInp_DrawMazemap(void)
{
    unsigned char *pmap;
    int x, y, posx, posy, xstart;


    /* Now draw it */
    pmap = MazeMap.map;
    xstart = InputArray[MENU_MAZEVEC].x + 2;
    posy   = InputArray[MENU_MAZEVEC].y + 2;

    for (y = 0; y < 32; y++) {
       posx = xstart;

       for (x = 0; x < 32; x++) {
           switch (*pmap) {
               case 0:		/* No wall */
               	   Display_Rectangle(posx, posy, 8, 8, 0x00FF0000, 1);
                   break;
               case 1:		/* Full wallblock */
                   Display_Rectangle(posx, posy, 8, 8, 0x00000000, 1);
                   break;
	       case 2:  	/* Door horizontally / vertically */
               case 3:
               	   break;
               case 4:		/* 120 */
                    Display_Rectangle(posx, posy, 8, 8, 0x00A82CC4, 1);
               	    break;
               case 5:          /* 121 */
                    Display_Rectangle(posx, posy, 8, 8, 0x00B2B2B2, 1);
               	    break;
               case 6:		/* 122 */
               	    break;
           } /* switch */

           posx += 8;
           pmap++;
       }  /* for x */

       posy += 8;
    }  /* for y */

    /* Now draw the rectangle, what's visible */
    Display_Rectangle(InputArray[MENU_MAZEVEC].x + 2 + MazeMap.rectx,
     		      InputArray[MENU_MAZEVEC].y + 2 + MazeMap.recty,
    		      MazeMap.rectw, MazeMap.recth, 0x00FFFFFF, 0);

    /* Now draw the actual edit-rectangle */
    Display_Rectangle(InputArray[MENU_MAZEVEC].x + 2 + ((MazeMap.posx - MazeMap.left)*8),
     		      InputArray[MENU_MAZEVEC].y + 2 + ((MazeMap.posy - MazeMap.top)*8),
    		      8, 8, 0x0000FFFF, 0);
}

/*
 * Name:
 *     MazeInp_DrawVecs
 * Description:
 *    Draw inputvectors.
 * Input:
 *   pinputvec: Inputvectors to draw
 */

void MazeInp_DrawVecs(INPUTVEC *pinputvec)
{
    int colornum,
    	   textx,
           texty;

    /* run through all InputVectors */
    while (pinputvec -> buttons != 0) {
        if ((pinputvec -> buttons & INP_HASBACKGR) != 0) {
            /* Draw a Background */
	    Display_DrawShadowedRect(pinputvec -> x, pinputvec -> y,
    			     	     pinputvec -> width, pinputvec -> height);
        }

        if ((pinputvec -> buttons & INP_HASDARKBK) != 0) {
            /* Background of the Mazemap or an Text-Inputfeld */
    	    Display_ShadowedPrompt(pinputvec -> x, pinputvec -> y,
    			           pinputvec -> width, pinputvec -> height);
        }

        if ((pinputvec -> buttons & INP_ISTEXT) != 0) {
            textx = pinputvec -> x;
            texty = pinputvec -> y + 8;

            /* If there is a background, the text is moved +4, +4 from the */
            /* top left corner of InputVec - rectangle.       		   */
            if ((pinputvec -> buttons & INP_HASBACKGR) != 0) {
                textx += 4;
                texty += 4;
            }

            /* Standard is font white */
            colornum = FONT_WHITE;

            /* Highlight only, if theres a button to press on the text */
            if ( ((pinputvec -> buttons & INP_NOBUTTON) == 0) &&
	         (pinputvec -> buttons & INP_HIGHLIGHT)) {
	        colornum = FONT_RED;
            }

            OutGLString(textx, texty, pinputvec -> text, colornum, FONT8);
        }

	pinputvec++;
    }  /* While InputVectors */

}


/*
 * Name:
 *     MazeInp_DrawMainmenu
 * Description:
 *     Draw the main menu
 */

void MazeInp_DrawMainmenu(void)
{
    char strbuf[80];
    int  i, dir;

    /* Make up the text for the status - window  */
    sprintf(StatusText,"Status:\n"
    		       "Mouse X: %03d Y: %03d\x06\x02\n" /* Draw it green */
                       "Camera Angle: %f\x06\x0F\n" 	 /* Again white   */
                       "Edit Position X: %02d Y: %02d\n",
    		       mposx, mposy, MazeInpCamera.angle,
                       MazeMap.posx, MazeMap.posy);
    /* Display actual menupoint, if any */
    if (ActInputVec) {
        sprintf(strbuf,"Menupoint: %u\n", ActInputVec -> subleft);
        strcat(StatusText, strbuf);
    }

    /* ------- Draw the Main-Menu  ------ */
    MazeInp_DrawVecs(&InputArray[MENU_SCROLLVEC]);

    /* ScrollArrows for the Mazemap */
    for (i = MENU_SCROLLVEC, dir = 0; i < MENU_SCROLLVEC+4; i++, dir++) {
        Display_Arrow(InputArray[i].x + 2, InputArray[i].y + 2, dir);
    }

    /* Draw additional vecs, if any */
    if (DisplayVecs) {
    	MazeInp_DrawVecs(DisplayVecs);
    }
}

/******************************************************************************
* THE STANDARD-INPUTBOXES		  				      *
******************************************************************************/
/* TODO: Make the Inputboxes more flexible ( no fixed lines ), dynamic CR,
         dynamic size, automatic center and soon.			     */

/*
 * Name:
 *     MazeInp_InputText
 * Description:
 *     Creates the YesNoMsgBox and set the InputVectors on it.
 *     No other input is possible until the user responded to his box.
 * Input:
 *	title:  The title to display in the box.
 *	text:   Pointer on inputbuffer.
 *	curpos: Initial position of the cursor
 */
void MazeInp_InputText(char *title, char *text, int maxlen, char curpos)
{
    PromptInputString.title = title;
    PromptInputString.prompt.text = text;
    PromptInputString.cursorpos = curpos;
    PromptInputString.maxlen = maxlen;

    InputVecs = &PromptInputString.leftbutton;
    DisplayVecs = &PromptInputString.box;
}

/*
 * Name:
 *     MazeInp_YesNoMsg
 * Description:
 *     Creates the YesNoMsgBox and set the InputVectors on it.
 *     No other input is possible until the user responded to his box.
 * Input:
 *	text: Text to display in the Box. There has to be a line-break
 *	      latest at a maximum of 32 chars, maximum 5 lines in height.
 */
void  MazeInp_YesNoMsg(char *text)
{
    PromptYesNoMsg.title = text;
    InputVecs = &PromptYesNoMsg.leftbutton;
    DisplayVecs = &PromptYesNoMsg.box;
}

/*
 * Name:
 *     MazeInp_MsgBox
 * Description:
 *     Creates MsgBox and set the InputVectors on it. No other input is
 *     possible until the user responded to his box.
 * Input:
 *	text: Text to display in the Box. There has to be a line-break
 *	      latest at a maximum of 32 chars, maximum 5 lines in height.
 */
void  MazeInp_MsgBox(char *text)
{
    PromptYesNoMsg.title = text;
    InputVecs = &PromptMsgBox.leftbutton;
    DisplayVecs = &PromptMsgBox.box;
}



/*
 * Name:
 *     MazeInp_DisplayList
 * Description:
 *     Draws a text with background. The line breaks have to be set
 *     by the user.	
 * Input:
 *     x, y, width, height: The size of the background
 *     List: Pointer on List ( Text )
 */

void MazeInp_DisplayList(int x,int y,int width,int height, char *List)
{
    Display_DrawShadowedRect(x, y, width, height);
    OutGLString(x+4, y+4, List, FONT_WHITE, FONT8);
}

/*
 * Name:
 *     EditMaze_GetTextInput
 * Input:
 *    prompt: Pointer on descriptor of prompt
 *    maxlen: Maximum lenght of Input
 *    input:  The char to add to the input, if 0: Start
 * Output:
 *    1: Input Done
 *    0: Still editing
 */
int MazeInp_GetTextInput(unsigned char inpchar)
{
    char *input;

    input = PromptInputString.prompt.text;
    /* ------------ */
    if (input == 0) {
        input[0] = '_';			 /* Cursor		    */
        input[1] = 0;			 /* Start with empty string */
        PromptInputString.cursorpos = 0; /* Position of cursor	    */
        return 0;
    }

    /* ------------ */
    inpchar &= 0x7F;	/* Maximum 0 .. 127 */
    switch (inpchar) {
    	case 8:		/* Backspace */
            if (PromptInputString.cursorpos > 0) {
            	PromptInputString.cursorpos--;
                input[PromptInputString.cursorpos] = '_';
                input[PromptInputString.cursorpos + 1] = 0;
            }
            return 0;			     /*  Still editing */
        case 27:	/* ESC */
            PromptInputString.cursorpos = 0;
        case 13:	/* Enter */
            input[PromptInputString.cursorpos] = 0; /* Clear Cursorchar */
            return 1;			    /* Input done 	*/
	case 4:		/* Ae */
        case 20:        /* Oe */
        case 1:		/* Ue */
            break;	/* check for length */
        default:
             if (inpchar >= ' ' || inpchar <= 'z') {
             	 break;	/* check for length */
             }
             else {
             	 return 1;	/* Ignore, still editing */
             }
    }

    /* Check for length */
    input[PromptInputString.cursorpos] = inpchar;
    if (PromptInputString.cursorpos
    	< (PromptInputString.maxlen - 1)) { /* Only move cursor, if possible */
        PromptInputString.cursorpos++;
    }

    input[PromptInputString.cursorpos] = '_';	/* Set Cursorchar */
    input[PromptInputString.cursorpos + 1] = 0; /* End of String  */

    return 1;		/* Still Editing */
}


/******************************************************************************
* THE MAIN ROUTINES, CALLED BY MAZEMAN  				      *
******************************************************************************/
void JadEdit_Init(void)
{
    static int nowalls[] = { 65, 66, 67, 130, 194, 258, 259, 260, 324,
    			     388, 452, 516, 0};
    static DISPLAYTEX greywall = {
    	"grey.bmp",	/* Name of texture file 		     	     */
        8,		/* Number of wall texrect ( Four coordinates each )  */
        32,		/* Number of bottom texrect ( Four coord. each )     */
        {   0,  63,  63,  63,  63, 17, 	 0, 17,		/* Wall 1 */
           64,  63, 127,  63, 127, 17,  64, 17,       	/* Wall 2 */
          128,  63, 191,  63, 191, 17, 128, 17,         /* Wall 3 */
          192,  63, 255,  63, 255, 17, 192, 17,         /* Wall 4 */
            0, 127,  63, 127,  63, 81,   0, 81, 	/* Wall 5 */
           64, 127, 127, 127, 127, 81,  64, 81,       	/* Wall 6 */
          128, 127, 191, 127, 191, 81, 128, 81,         /* Wall 7 */
          192, 127, 255, 127, 255, 81, 192, 81 },       /* Wall 8 */
        /* Now Bottom 1 .. 32 					  */
        {   0, 159,  31, 159,  31, 128,   0, 128,	/* Bottom 1 .. 8  */
           32, 159,  63, 159,  63, 128,  32, 128,
           64, 159,  95, 159,  95, 128,  64, 128,
           96, 159, 127, 159, 127, 128,  96, 128,
          128, 159, 159, 159, 159, 128, 128, 128,
          160, 159, 191, 159, 191, 128, 160, 128,
          192, 159, 223, 159, 223, 223, 192, 128,
          224, 159, 255, 159, 255, 128, 224, 128,

            0, 191,  31, 191,  31, 160,   0, 160,	/* Bottom 9 .. 16 */
           32, 191,  63, 191,  63, 160,  32, 160,
           64, 191,  95, 191,  95, 160,  64, 160,
           96, 191, 127, 191, 127, 160,  96, 160,
          128, 191, 159, 191, 159, 160, 128, 160,
          160, 191, 191, 191, 191, 160, 160, 160,
          192, 191, 223, 191, 223, 160, 192, 160,
          224, 191, 255, 191, 255, 160, 224, 160,

            0, 223,  31, 223,  31, 192,   0, 192,	/* Bottom 17 .. 24 */
           32, 223,  63, 223,  63, 192,  32, 192,
           64, 223,  95, 223,  95, 192,  64, 192,
           96, 223, 127, 223, 127, 192,  96, 192,
          128, 223, 159, 223, 159, 192, 128, 192,
          160, 223, 191, 223, 191, 192, 160, 192,
          192, 223, 223, 223, 223, 192, 192, 192,
          224, 223, 255, 223, 255, 192, 224, 192,

            0, 255,  31, 255,  31, 224,   0, 224,       /* Bottom 25 .. 32 */
           32, 255,  63, 255,  63, 224,  32, 224,
           64, 255,  95, 255,  95, 224,  64, 224,
           96, 255, 127, 255, 127, 224,  96, 224,
          128, 255, 159, 255, 159, 224, 128, 224,
          160, 255, 191, 255, 191, 224, 160, 224,
          192, 255, 223, 255, 223, 224, 192, 224,
          224, 255, 255, 255, 255, 224, 224, 224,
        }
    };

    int *pnowalls;
    int editpos;

    Display_Init(640, 480, &MazeInpCamera);
    Display_LoadTextures(&greywall, 1);
    Animated_Init();			/* Now init the animation stuff */

    /* For test purposes, a little bit maze... */
    Maze_Init();

    pnowalls = nowalls;

    while (*pnowalls != 0) {
        Maze_SetWall(*pnowalls, 0);
        pnowalls++;
    }

    /* Test "Decorations"    36 = 120 / 37 = 121 / 38 = 122 */
    Maze_SetWallSide( 66, -1, 40);
    Maze_SetWallSide(130, -1, 41);
    Maze_SetWallSide(194, -1, 42);
    // Maze_SetWall(67, 0);		/* First make rid of the wall sides */
    Maze_SetWallSide(65, 3, 36);	/* Stair Down */
    // Maze_SetWall(65, 0);		/* First make rid of the wall sides */
    Maze_SetWallSide(67, 1, 35);	/* Stair Up */
    // Test the door
    Maze_SetWall(324, 0);		/* First make rid of the wall sides */
    Maze_SetWallSide(324, 0, 3);  	/* Door Type 1			    */
    Maze_SetWallSide(324, 2, 3);	/* On both sides		    */

    /* Get the mazemap */
    Maze_SetMap(MazeMap.left, MazeMap.top, 32, 32, MazeMap.map);
    /* Set up the display */
    Maze_SetDisplayList(Display_TileList,&MazeMap.posx, &MazeMap.posy);

    /* And at last the Edit-Cursor */
    editpos = (MazeMap.posy * VIEWMESHSIZE) + MazeMap.posx;
    Display_TileList[editpos].flags  |= TILE_ISEDIT;
    Display_TileList[editpos].direction = MazeMap.side;	/* Which Side to put */

    /* Set the input-vectors */
    InputVecs = &InputArray[0];
}

void JadEdit_Main(void)
{
    /* ---------- First fo all the change of Data */
    Animated_Clock();		/* Drive clocks for animation */

    /* ----------- Display anithing --------  */
    /* Display the 3D-Stuff */
    Display_3DView(Display_TileList, &MazeInpCamera);
    Display_Draw3DAxes();
    /* Display the 2D-Stuff */
    Display_Begin2DMode();
        MazeInp_DrawMainmenu();
        /* Draw the mazemap */
	MazeInp_DrawMazemap();
    Display_End2DMode();
}

void JadEdit_Shutdown(void)
{
    Animated_Shutdown();
}
