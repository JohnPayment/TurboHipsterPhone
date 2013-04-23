/*
     MAZEINP.C
     Every input, that has to be handled. Including display of menus,
     input- and messageboxes.

     Last changes: 20.01.01 / PM
     		   21.01.01 / PM
     		   23.01.01 / PM
                   28.01.01 / PM
                   30.05.01 / PM
*/

#include <stdio.h>
#include <string.h>	/* strcat */

#include "display.h"
#include "textout.h"
#include "maze.h"
#include "jadinput.h"
#include "jadedit.h"
#include "editmaze.h"
#include "animated.h"
#include "mazefile.h"		/* Load game data from files */

/******************************************************************************
* DEFINES		      						      *
******************************************************************************/

#define JADINPUT_CAPTION "Jad Editor V0.12"

#define HALFMAZESIZE	     (MAZESIZE / 2)

#define CAMERA_INPUT   	     100
#define PROMPT_INPUT	     101
#define MENU_INPUT           200
#define MENUMAP_INPUT	     201	/* Do something with the maze-map */

#define MENU_ESC               1
#define MENU_WHOLE             2
#define MENU_MAZE     	      14
#define MENU_MOVEMAZE         15

/* Combined flags */
#define INP_KEY (INP_ACTIVE | INP_KEYBOARD)

/* Menuvec-Start */
#define MENU_MAZEVEC	 13
#define MENU_MAINMENUVEC 14
#define MENU_MENUPOINTS  23     /* From this point, Menupoints, if any */

/******************************************************************************
* FORWARD DECLARATION							      *
******************************************************************************/

static int JadInput_EventHandler(INPEVENT *inpev);

/******************************************************************************
* TYPEDEFS								      *
******************************************************************************/

/******************************************************************************
* DATA									      *
******************************************************************************/
CAMERA MazeInpCamera = {
    { 448.0, 115.95, 256.0 },	/* Position of viewer			*/
    { 0.0, 0.0, 69.0 },		/* "LookAt"			        */
    15.5,	       		/* Rotation Angle round "Lookat"-Point  */
    462.76,			/* Distance on plane Eye -> LookAt      */
    1.0,			/* Zoom-Factor			        */
    /* The viewport, in case of splitscreen and for init		*/
    0, 0, 640, 480
};

	/* All data needed to know which part of the Mazemap to */
	/* display. Also which part to display in the 3D-View.  */
static struct {
    int left,
        top; 	 /* 0 .. ((MAZESIZE / 2) - 1). Position in Map-Window    */
    int posx,
        posy,	 /* At which position the actual edit takes place 	 */
        side,	 /* At which side of a tile the actual edit takes place. */
        	 /* -1, if all four sides of the tile are to change	 */
        level,	 /* Which level is to be edited: 0: None		 */
        wallno; /* Number of wall to set, initial empty.		 */
    JADRECT rect;

    /* The border of the visible area in the 3D-View 	*/
    char map[MAZE_MAXTILECOUNT];
} MazeMap = { 0, 0, 4, 4, -1, 0, 0, { 0, 0, 72, 72 } };


/* ------- Info - Variables ----- */
static char StatusText[256];
static int seconds = 0;		/* --> For clocktest... */
static int secondclock = 0;	/* Handle for the secondclock */

static int mposx = 0,
	   mposy = 0;

/* --------- The Inputvecs ---------- */
static INPUTVEC InputArray[60] = {
    /* First the ARROW_KEY's, no Mouse input */
    { INP_KEY, 0, JAD_CAMERA_LEFT,  { 0, 0, 0, 0 },  CAMERA_INPUT, CAMERA_LEFT },
    { INP_KEY, 0, JAD_CAMERA_RIGHT, { 0, 0, 0, 0 }, CAMERA_INPUT, CAMERA_RIGHT },
    { INP_KEY, 0, JAD_CAMERA_IN,    { 0, 0, 0, 0 }, CAMERA_INPUT, CAMERA_ZOOMIN },
    { INP_KEY, 0, JAD_CAMERA_OUT,   { 0, 0, 0, 0 }, CAMERA_INPUT, CAMERA_ZOOMOUT },
    { INP_KEY, 0, JAD_CAMERA_UP,    { 0, 0, 0, 0 }, CAMERA_INPUT, CAMERA_UP },
    { INP_KEY, 0, JAD_CAMERA_DOWN,  { 0, 0, 0, 0 }, CAMERA_INPUT, CAMERA_DOWN },
    { INP_KEY, 0, JAD_CAMMODE,      { 0, 0, 0, 0 }, CAMERA_INPUT, CAMERA_SWITCHMODE },
    /* ------- Now the movement keys, no map scroll  ------------------ */
    { INP_KEY, 0, JAD_FORWARD,   { 0, 0, 0, 0 },  MENU_MOVEMAZE, JAD_FORWARD },
    { INP_KEY, 0, JAD_BACKWARD,  { 0, 0, 0, 0 },  MENU_MOVEMAZE, JAD_BACKWARD },
    { INP_KEY, 0, JAD_STEPLEFT,  { 0, 0, 0, 0 },  MENU_MOVEMAZE, JAD_STEPLEFT },
    { INP_KEY, 0, JAD_STEPRIGHT, { 0, 0, 0, 0 },  MENU_MOVEMAZE, JAD_STEPRIGHT },
    { INP_KEY, 0, JAD_TURNLEFT,  { 0, 0, 0, 0 },  MENU_MOVEMAZE, JAD_TURNLEFT },
    { INP_KEY, 0, JAD_TURNRIGHT, { 0, 0, 0, 0 },  MENU_MOVEMAZE, JAD_TURNRIGHT },
    /* Here is the "maze" window */
    { INP_ACTIVE | JAD_MOULEFT | JAD_MOURIGHT, JADINPUT_HASDARKBK, 0, { 380, 16, 260, 260 }, EDITMAZE_WALL, EDITMAZE_CLEARWALL, EDITMAZE_SETWALL },
    /* Measure of Main-Menu, draw background... */
    { INP_DRAWONLY, JADINPUT_HASBACKGR, 0, { 0, 0, 640, 16 } },
    /* Here comes the main menu */
    { INP_ACTIVE | JAD_MOULEFT, JADINPUT_ISTEXT, 0,  { 4, 4, 36, 8 }, MENU_INPUT, EDITMAZE_FILE, MENU_ESC, "File" },
    { JAD_MOULEFT, JADINPUT_ISTEXT, 0, { 40,  4, 48, 8 }, MENU_INPUT, EDITMAZE_WALLEDIT, 0, "Walls" },
    { JAD_MOULEFT, JADINPUT_ISTEXT, 0, { 88,  4, 96, 8 }, MENU_INPUT, EDITMAZE_DECORATION, 0, "Decorations" },
    { JAD_MOULEFT, JADINPUT_ISTEXT, 0, { 184, 4, 48, 8 }, MENU_INPUT, EDITMAZE_DOORS, 0, "Doors" },
    { JAD_MOULEFT, JADINPUT_ISTEXT, 0, { 232, 4, 48, 8 }, MENU_INPUT, EDITMAZE_ITEMS, 0, "Items" },
    { JAD_MOULEFT, JADINPUT_ISTEXT, 0, { 280, 4, 56, 8 }, MENU_INPUT, EDITMAZE_EVENTS, 0, "Events" },
    /* Status-window */
    { INP_DRAWONLY, JADINPUT_HASBACKGR, 0, { 380, 276, 260, 64 }}, /* Status-Window */
    { INP_DRAWONLY, JADINPUT_ISTEXT, 0, { 384, 280, 0, 0  }, 0, 0, 0, StatusText }, /* Status-Window */
    /* From this point, Menupoints, if any */
    { 0 },
    { 0 }
};


/* The internal working struct */
struct {
    INPUTVEC *InputVecs;
    JADEVENTHANDLER EventHandler;
} JadInputMenuStack[32] = {
    { &InputArray[0],	   	/* First is the standard to be drawn... */
      JadInput_EventHandler }	/* Standard eventhandler    		*/
};

static int JadInputMenuStackIdx = 0;
static INPUTVEC *ActInputVec = 0;	 /* Pointer on actual highlighted Menu */
					 /* simple input boxes		       */

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

static int posinrect(INPUTVEC *inpvec, INPEVENT *inpev)
{
    /* First check horizontally */
    inpev -> relx -= inpvec -> rect.x;
    if (inpev -> relx >= 0) {				/* Is right of X */
        if (inpev -> relx > inpvec -> rect.width)
            return 0;			  /* Right side out of Rectangle */

        /* Now check vertically */
    	inpev -> rely -= inpvec -> rect.y;
	if (inpev -> rely >= 0) {		    /* Is Lower of Y-Pos */
            if (inpev -> rely <= inpvec -> rect.height)
            	return 1;			    /* Is in Rectangle   */
	}
    }
    return 0;
}

/*
 * Name:
 *     JadInput_UpdateView
 * Description:
 *     Calls all the needed commands to refresh the view.
 * Input:
 */
static void JadInput_UpdateView(void)
{
    int editpos; 	/* Edit position in the visible area	*/

    /* Get the mazemap */
    Maze_SetMap(MazeMap.left, MazeMap.top, 32, 32, MazeMap.map);

    /* Set up the display */
    Maze_SetDisplayList(Display_TileList, &MazeMap.posx, &MazeMap.posy);

    /* Set the edit-rectangle */
    editpos = ((MazeMap.posy - MazeMap.top) * VIEWMESHSIZE)
               + (MazeMap.posx - MazeMap.left) ;
    Display_TileList[editpos].flags  |= TILE_ISEDIT;
    Display_TileList[editpos].direction = MazeMap.side;	/* Which side to put */
}


/*
 * Name:
 *     JadInput_GetTextInput
 * Input:
 *    prompt: Pointer on descriptor of prompt
 *    maxlen: Maximum lenght of Input
 *    input:  The char to add to the input, if 0: Start
 * Output:
 *    1: Input Done
 *    0: Still editing
 */
static int JadInput_GetTextInput(char *edittext, int maxlen, int *curpos, char inpchar)
{
    char *input;
    int  cursorpos;

    input = edittext;

    /* ------------ */
    if (input == 0) {
        input[0] = '_';			/* Cursor		    */
        input[1] = 0;			/* Start with empty string  */
        *curpos = 0;	 		/* Position of cursor	    */
        return 0;
    }

    cursorpos = *curpos;

    /* ------------ */
    inpchar &= 0x7F;	/* Maximum 0 .. 127 */
    switch (inpchar) {
    	case 8:		/* Backspace */
            if (cursorpos > 0) {
            	cursorpos--;
                input[cursorpos] = '_';
                input[cursorpos + 1] = 0;
            }

            *curpos = cursorpos;
            return 0;			     /*  Still editing */
        case 27:	/* ESC */
            cursorpos = 0;
        case 13:	/* Enter */
            input[cursorpos] = 0; /* Clear Cursorchar */
            *curpos = 0;
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
                 *curpos = cursorpos;
             	 return 1;	/* Ignore, still editing */
             }
    }

    /* Check for length */
    input[cursorpos] = inpchar;
    if (cursorpos
    	< (maxlen - 1)) { /* Only move cursor, if possible */
        cursorpos++;
    }

    input[cursorpos] = '_';	/* Set Cursorchar */
    input[cursorpos + 1] = 0; /* End of String  */
    *curpos = cursorpos;
    return 1;		/* Still Editing */
}

/*
 * Name:
 *     JadInput_OpenInputWindow
 * Description:
 *     Sets the actual valid inputvectors and the eventhandler belonging to
 *     it. The inputvectors are set to the top of the inputstack. All input
 *     goes to this window.
 * Input:
 *     inpvec:       Pointer on Array with inputvectors.
 *     eventhandler: Callback function, called by the inputhandler.
 *
 */
void JadInput_OpenInputWindow(INPUTVEC *inpvec, JADEVENTHANDLER eventhandler)
{
    /* Push the next input descriptors on the stack */
    if (JadInputMenuStackIdx < 31) {	/* Just to be save */
	JadInputMenuStackIdx++;
    	JadInputMenuStack[JadInputMenuStackIdx].InputVecs = inpvec;
    	JadInputMenuStack[JadInputMenuStackIdx].EventHandler = eventhandler;
    }
}

/*
 * Name:
 *     JadInput_CloseInputWindow
 * Description:
 *     Reduces the index of the menustack by one.
 * Input:
 *     None
 */
static void JadInput_CloseInputWindow(void)
{
    if (JadInputMenuStackIdx > 0) {
    	JadInputMenuStackIdx--;
    }

    JadInput_UpdateView();	/* For the case ... */
}



/*
 *  Name:
 *      JadEdit_HandleInput
 *  Description:
 *	Hands the Mouse-Position and buttons to the Main-Eventhandler.
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

int JadEdit_HandleInput(int x, int y, int button, int key)
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
    button &= JAD_MOUBOTH;		/* Only Mousebutton-Flags */

    inpvec = JadInputMenuStack[JadInputMenuStackIdx].InputVecs;

    /* Now check the position, if there is an Event-Array */
    if (inpvec) {				/* There is any to check... */


        while (inpvec -> buttons != 0) { 	    /* While theres something to check  */

            inpvec -> drawflags &= ~JADINPUT_HIGHLIGHT;  /* Remove possible highlight - flag */

            if (inpvec -> buttons & INP_ACTIVE)  {

	        if (key == 0) {
                    /* Then check the mouse, if asked 	*/
                    inpev.relx = x;		/* move the mousepos to the Input-Event */
                    inpev.rely = y;

                    if (posinrect(inpvec, &inpev)) {

            	    	/* Mouse is in this rectangle*/
                    	inpev.maincode = inpvec -> maincode;   /* Mouse on rectangle */
                    	inpvec -> drawflags |= JADINPUT_HIGHLIGHT;  /* Set new highlight  */

                    	/* TODO: Change the position of the Menu-Box, if the */
                    	/*	     mouse moves to a new mainmenu point     */

	                if ((inpvec -> buttons & button) != 0) {
                    	    /* If this mouseclick is asked for */
                            switch(button) {
        	            	case JAD_MOULEFT:
                                    if (inpvec -> buttons & JAD_MOULEFT)
                                        inpev.subcode = inpvec -> subleft;
                                    break;
	            	        case JAD_MOURIGHT:
                            	    if (inpvec -> buttons & JAD_MOURIGHT)
                                        inpev.subcode = inpvec -> subright;
                            	    break;
            	    	    } /* switch button */

                            ActInputVec = inpvec;
                            /* Now call the event Handler */
                            done = JadInputMenuStack[JadInputMenuStackIdx].EventHandler(&inpev);
                    	    /* return JadEventHandler */
                            if (done == JAD_CLOSEINPBOX) {
                                JadInput_CloseInputWindow();
                            }
                            else {
			    	if (JadInputMenuStackIdx == 0) {
                                    /* InputArray[MENU_MENUPOINTS].buttons = 0; */
                                    return done;	/* Only, if menu active */
                                }
                            	else
                                    return 0;
                            }
                    	} /* if Button*/

                        /* Stop checking vectors and keys*/
                        return 0;
                    }  /* if posinrect */


            	} /* if key == 0 */
            	else {
                    /* check the key */
                    if (key == inpvec -> key) {
                    	inpev.maincode = inpvec -> maincode;
                        inpev.subcode  = inpvec -> subleft;

                        /* Now call the actual event Handler */
                        done = JadInputMenuStack[JadInputMenuStackIdx].EventHandler(&inpev);
                        ActInputVec = inpvec;
                        if (done == JAD_CLOSEINPBOX) {
                            JadInput_CloseInputWindow();
                        }
                        else {
			    if (JadInputMenuStackIdx == 0) {
                             	/* InputArray[MENU_MENUPOINTS].buttons = 0; */
                                return done;	/* Only, if menu active */
                            }
                            else
                                return 0;
                        }
		    }
                    else {	/* May be an edit window */

        	        if (inpvec -> buttons & INP_EDIT) {
                    	    inpev.maincode = JADINPUT_EDITVEC;
                            inpev.subcode = JadInput_GetTextInput(inpvec -> text, inpvec -> argval,
                        		 		          &inpvec -> helpval, (char)key);
                            /* maybe done editing */
                            if (JadInputMenuStack[JadInputMenuStackIdx].EventHandler(&inpev) == JAD_CLOSEINPBOX) {
                                JadInput_CloseInputWindow();
                             }
                             return 0;
                         }  /* else */
                    }
                }

            } /* if (inpvec -> buttons & INP_ACTIVE) */

            inpvec++;
        } /* while (inpvec -> buttons!= 0) */

        /* None of the input positions, delete a possible menu */
	/* InputArray[MENU_MENUPOINTS].buttons = 0; */  
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
void JadInput_SetMouseCursor(int number)
{

}


/*
 * Name:
 *     JadInput_EventHandler
 * Description:
 *     The standard handlrr for the event input.
 * Input:
 *     inpev:  Pointer on Inputevent
 * Output:
 *     1: Stop inputevents
 *     0: Get next.
 */

static int JadInput_EventHandler(INPEVENT *inpev)
{
    int units,		/* For change of Camera 		*/
        kind;           /* subcode				*/
    int mposx, mposy;	/* A new position in the map 		*/
    int direction,	/* Direction of movement		*/
        retval;		/* REturn value from "EditMaze"		*/


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
                        break;
                    case CAMERA_SWITCHMODE:
                    	units = 0;
                } /* switch */

	        Display_MoveCamera(&MazeInpCamera, kind, units);
                return 0;
            }  /* case CAMERA_INPUT */

        case MENU_MOVEMAZE: {
            direction = 0;

            switch (kind) {
                case JAD_FORWARD:
                    direction = 0;
                    break;

                case JAD_BACKWARD:
                    direction = 2;
                    break;

                case JAD_STEPLEFT:
                    direction = 3;
                    break;

                case JAD_STEPRIGHT:
                    direction = 1;
                    break;
                }

	        if (Maze_MovePos(direction, &MazeMap.posx, &MazeMap.posy)) {

                    /* Get the mazemap */
                    Maze_SetMap(MazeMap.left, MazeMap.top, 32, 32, MazeMap.map);

	            /* Set up the display */
	            Maze_SetDisplayList(Display_TileList, &MazeMap.posx, &MazeMap.posy);
                }

            }
            return 0;

        case EDITMAZE_WALL:
            mposx = (inpev -> relx / 8) + MazeMap.left;
            mposy = (inpev -> rely / 8) + MazeMap.top;

            switch (kind) {
	        case EDITMAZE_CLEARWALL:
                    Maze_SetWall(Maze_Pos(mposx, mposy), 0);
                    break;

                case EDITMAZE_SETWALL:
                    Maze_SetWall(Maze_Pos(mposx, mposy), 1);
                    break;
            }

            JadInput_UpdateView();
            return 0;

        case MENU_INPUT:
            EditMaze_OpenMenu(ActInputVec -> rect.x, ActInputVec -> rect.y + 16,
                              kind,
                              &InputArray[MENU_MENUPOINTS]);
            break;

        /* Submenus */
        case EDITMAZE_DOORS:
        case EDITMAZE_FILE:
        case EDITMAZE_WALLEDIT:
        case EDITMAZE_DECORATION:
        case EDITMAZE_ITEMS:
        case EDITMAZE_EVENTS:
            retval = EditMaze_HandleMenuInput(inpev -> maincode, kind);
            switch(retval) {
            	case 1:
                    return 1;		      /* Done */
                case 2:
                    JadInput_UpdateView();
                    break;
            }

	    break;

    }  /*  switch (inpev -> maincode) */

    return 0;
}

/******************************************************************************
* MENU DRAWING				  				      *
******************************************************************************/

/*
 * Name:
 *     JadInput_DrawMazemap
 * Description:
 *     Draws the Mazemap in the Menu-Mazewindow. Uses "MazeMap".
 *     The maze-map has to be filled by the caller.
 * Input:
 *     None
 *
 */
void JadInput_DrawMazemap(void)
{
    static JADRECT rect = { 0, 0, 8, 8};
    JADRECT drawrect;
    char *pmap;
    int x, y, xstart;




    /* Now draw it */
    pmap = MazeMap.map;
    xstart   = InputArray[MENU_MAZEVEC].rect.x + 2;
    rect.y   = InputArray[MENU_MAZEVEC].rect.y + 2;

    for (y = 0; y < 32; y++) {

       rect.x = xstart;

       for (x = 0; x < 32; x++) {
           switch (*pmap) {
               case 0:		/* No wall */
               	   Display_Rectangle(&rect, DISPLAY_BLUE, 1);
                   break;
               case 1:		/* Full wallblock */
                   Display_Rectangle(&rect, DISPLAY_BLACK, 1);
                   break;
	       case 2:  	/* Door horizontally / vertically */
               case 3:
               case 4:		/* 120 */
               case 5:
                    Display_Rectangle(&rect, DISPLAY_LIGHTMAGENTA, 1);
               	    break;
               case 6:
                     /* Stair up */
                    Display_Rectangle(&rect, DISPLAY_LIGHTGREY, 1);
               	    break;
               case 7:		/* Stair down */
                    Display_Rectangle(&rect, DISPLAY_DARKGRAY, 1);
               	    break;
           } /* switch */

           rect.x += 8;
           pmap++;
       }  /* for x */

       rect.y += 8;
    }  /* for y */

    /* Now draw the rectangle, what's visible */
    drawrect.x = InputArray[MENU_MAZEVEC].rect.x + 2 + ((MazeMap.posx - MazeMap.left - HALFVIEWMESH)*8);
    drawrect.y = InputArray[MENU_MAZEVEC].rect.y + 2 + ((MazeMap.posy - MazeMap.top - HALFVIEWMESH)*8);
    drawrect.width  = MazeMap.rect.width;
    drawrect.height = MazeMap.rect.height;

    Display_Rectangle(&drawrect, DISPLAY_WHITE, 0);

    /* Now draw the actual edit-rectangle */
    rect.x = drawrect.x + (HALFVIEWMESH * 8);
    rect.y = drawrect.y + (HALFVIEWMESH * 8);

    Display_Rectangle(&rect, DISPLAY_YELLOW, 0);
}

/*
 * Name:
 *     JadInput_DrawVecs
 * Description:
 *    Draw inputvectors.
 * Input:
 *   pinputvec: Inputvectors to draw
 */

void JadInput_DrawVecs(INPUTVEC *pinputvec)
{
    JADRECT currect = { 0, 0, 8, 8 };
    int colornum;

    /* run through all InputVectors */
    while (pinputvec -> buttons != 0) {
        if (pinputvec -> drawflags & JADINPUT_HASBACKGR) {
            /* Draw a Background */
	    Display_DrawShadowedRect(&pinputvec -> rect);
        }

        if (pinputvec -> drawflags & JADINPUT_HASDARKBK) {
            /* Background of the Mazemap or an Text-Inputfeld */
    	    Display_ShadowedPrompt(&pinputvec -> rect);
        }

        if (pinputvec -> drawflags & JADINPUT_ISTEXT) {
            currect.x = pinputvec -> rect.x;
            currect.y = pinputvec -> rect.y;

            /* Standard is font white */
            colornum = FONT_WHITE;

            /* Highlight only, if theres a button to press on the text */
            if ( ((pinputvec -> drawflags & (INP_KEYBOARD | JAD_MOUBOTH))) &&
	          (pinputvec -> drawflags & JADINPUT_HIGHLIGHT)) {
	        colornum = FONT_RED;
            }

            if (pinputvec -> drawflags & JADINPUT_DRAWCURSOR) {
                currect.x += (pinputvec -> helpval * 8);	/* Cursor... */
    	        Display_Rectangle(&currect, DISPLAY_DARKGRAY,1);
            }

            OutGLString(currect.x, currect.y, pinputvec -> text, colornum, FONT8);
        }

	pinputvec++;
    }  /* While InputVectors */

}


/*
 * Name:
 *     JadInput_DrawMainmenu
 * Description:
 *     Draw the main menu
 */

void JadInput_DrawMainmenu(int fps)
{
    char strbuf[80];
    int  veccounter;

    /* Make up the text for the status - window  */
    sprintf(StatusText,"Status:\r"
    		       "Mouse X: %03d Y: %03d\x06\x02\r" /* Draw it green */
                       "Camera Angle: %f\x06\x0F\r" 	 /* Again white   */
                       "Edit Position X: %02d Y: %02d\r"
                       "Level: %d / Wallno: %d",
    		       mposx, mposy, MazeInpCamera.angle,
                       MazeMap.posx, MazeMap.posy, MazeMap.level, MazeMap.wallno);

    /* Display actual menupoint, if any */
    if (ActInputVec) {
        sprintf(strbuf,"\rMenupoint: %u\r", ActInputVec -> subleft);
        strcat(StatusText, strbuf);
    }


    /* Draw the main menu vecs */
    /* ------- Draw the Main-Menu  ------ */
    JadInput_DrawVecs(&InputArray[MENU_MAZEVEC]);

    /* Draw the fps - text and the second timer */
    sprintf(strbuf,"FPS: %d\r\x06\x02Second - Timer: %d", fps, seconds);
    OutGLString(24, 600, strbuf, FONT_WHITE, FONT8);

    /* Add the second timer */
    if (JadEdit_ClockFunction(secondclock, 1000, JAD_CLOCKGET) == 0) {
    	seconds++;
    }

    /* Draw additional vecs, if any, from back to front */
    veccounter = 1;
    while (veccounter <= JadInputMenuStackIdx) {
    	JadInput_DrawVecs(JadInputMenuStack[JadInputMenuStackIdx].InputVecs);
        veccounter++;
    }
}


/******************************************************************************
* THE STANDARD-INPUTBOXES		  				      *
******************************************************************************/
/* TODO: Make the Inputboxes more flexible ( no fixed lines ), dynamic CR,
         dynamic size, automatic center and soon.			     */

/*
 * Name:
 *     JadInput_InputBox
 * Description:
 *     Creates the InputBox and set the InputVectors on it.
 *     No other input is possible until "OK" or "Cancel" is signalled.
 *     with JadInput_Reset.
 * Input:
 *	text:         The text to display in the box.
 *	eventhandler: For handling of the eventcodes of the inputbox
 */
void JadInput_InputBox(char *text, JADEVENTHANDLER eventhandler)
{
    static char EditBuffer[252];
    static INPUTVEC InputBox[] = {
        { INP_DRAWONLY, JADINPUT_HASBACKGR, 0, { 188, 206, 264, 56 } },
        { INP_ACTIVE | JAD_MOULEFT,  JADINPUT_ISTEXT | JADINPUT_HASBACKGR, 0, { 192, 242, 24, 16 }, PROMPT_INPUT, 1, 0, "Ok" },
        { INP_ACTIVE | JAD_MOULEFT,  JADINPUT_ISTEXT | JADINPUT_HASBACKGR, 0, { 334, 242, 56, 16 }, PROMPT_INPUT, 2, 0, "Cancel" },
        { INP_DRAWONLY, JADINPUT_HASDARKBK, JADINPUT_ISTEXT | JADINPUT_DRAWCURSOR, { 192, 222, 248, 16 }, JADINPUT_EDITVEC, 0, 0 },
        { 0 }
    };

    InputBox[0].text	= text;
    EditBuffer[0] 	= 0;
    InputBox[3].text    = EditBuffer;
    InputBox[3].argval  = 248;
    InputBox[3].helpval = 0;		/* Cursor */

    JadInput_OpenInputWindow(InputBox, eventhandler);
}



static int JadInput_MsgBoxHandler(INPEVENT *inpev)
{
    if (inpev -> maincode == PROMPT_INPUT) {
            	return inpev -> subcode;
    }  /* Promptinput */

    return 0;
}

/*
 * Name:
 *     JadInput_YesNoMsg
 * Description:
 *     Creates the YesNoMsgBox and set the InputVectors on it.
 *     No other input is possible until the user responded to his box.
 * Input:
 *	text: Text to display in the Box. There has to be a line-break
 *	      latest at a maximum of 32 chars, maximum 5 lines in height.
 */
void JadInput_YesNoMsg(char *text, JADEVENTHANDLER eventhandler)
{
     static INPUTVEC YesNoMsg[] = {
    	{ INP_DRAWONLY, JADINPUT_HASBACKGR, 0, { 188, 206, 264, 68 } },  /* Text an background rectangle  */
    	{ INP_ACTIVE | JAD_MOULEFT,  JADINPUT_ISTEXT | JADINPUT_HASBACKGR, 0, { 192, 254, 32, 16 }, PROMPT_INPUT, 1, 0, "Yes" },
    	{ INP_ACTIVE | JAD_MOULEFT,  JADINPUT_ISTEXT | JADINPUT_HASBACKGR, 0, { 424, 254, 24, 16 }, PROMPT_INPUT, 2, 0, "No" },
        { 0 }
    };

    YesNoMsg[0].text = text;
    JadInput_OpenInputWindow(YesNoMsg, eventhandler);
}



/*
 * Name:
 *     JadInput_MsgBox
 * Description:
 *     Creates MsgBox and set the InputVectors on it. No other input is
 *     possible until the user responded to his box.
 * Input:
 *	text:  Text to display in the Box. There has to be a line-break
 *	       latest at a maximum of 32 chars, maximum 5 lines in height.
 */
void  JadInput_MsgBox(char *text)
{
    static INPUTVEC MsgBox[] = {
    	{ INP_DRAWONLY, JADINPUT_HASBACKGR, 0, { 188, 206, 264, 68 } },  /* Text an background rectangle  */
    	{ INP_ACTIVE | JAD_MOULEFT,  JADINPUT_ISTEXT | JADINPUT_HASBACKGR, 0, { 308, 254, 24, 16 }, PROMPT_INPUT, JAD_CLOSEINPBOX, 0, "Ok" },
        { 0 }
    };

    MsgBox[0].text = text;
    JadInput_OpenInputWindow(&MsgBox[0], JadInput_MsgBoxHandler);
}


/*
 * Name:
 *     JadInput_DisplayList
 * Description:
 *     Draws a text with background. The line breaks have to be set
 *     by the user.
 * Input:
 *     rect: The size of the background
 *     List: Pointer on List ( Text )
 */

void JadInput_DisplayList(JADRECT *rect, char *List)
{
    Display_DrawShadowedRect(rect);
    OutGLString(rect -> x + 4, rect -> y + 4, List, FONT_WHITE, FONT8);
}

/*
 * Name:
 *     JadInput_SetMazeValues
 * Description:
 *     Sets the values to display in the statuswindow.
 *     If a given value is -1, it is ignored.
 *
 *
 */
void JadInput_SetMazeValues(int side, int level, char wallno)
{
    if (side  != -1)  MazeMap.side = side;
    if (level != -1)  MazeMap.level = level;
    if (wallno != -1) MazeMap.wallno = wallno;
}

/******************************************************************************
* THE MAIN ROUTINES, CALLED BY MAZEMAN  				      *
******************************************************************************/

#pragma argsused
char *JadEdit_Init(int val, void *data)
{
    int editpos;

    Display_Init(&MazeInpCamera);

    MazeFile_LoadInitialData();		/* Load the initial graphics data  */

    Animated_Init();			/* Now init the animation stuff */
    
    /* Get the mazemap */
    Maze_SetMap(MazeMap.left, MazeMap.top, 32, 32, MazeMap.map);
    /* Set up the display */
    Maze_SetDisplayList(Display_TileList,&MazeMap.posx, &MazeMap.posy);

    /* And at last the Edit-Cursor */
    editpos = (MazeMap.posy * VIEWMESHSIZE) + MazeMap.posx;
    Display_TileList[editpos].flags  |= TILE_ISEDIT;
    Display_TileList[editpos].direction = MazeMap.side;	/* Which Side to put */

    /* Set the textclock */
    secondclock = JadEdit_ClockFunction(0, 1000, JAD_CLOCKNEW);

    return JADINPUT_CAPTION;
}

#pragma argsused
void JadEdit_Main(int fps, int tickspassed)
{
    /* ---------- First fo all the change of Data -----------------  */
    Animated_Clock(tickspassed);	/* Drive clocks for animation */

    /* ----------- Display anithing --------  */
    /* Display the 3D-Stuff */
    Display_3DView(Display_TileList, &MazeInpCamera);

    /* Display the 2D-Stuff */
    Display_Begin2DMode();
        JadInput_DrawMainmenu(fps);

        /* Draw the mazemap */
	JadInput_DrawMazemap();
    Display_End2DMode();
}

void JadEdit_Shutdown(void)
{
    Animated_Shutdown();
}
