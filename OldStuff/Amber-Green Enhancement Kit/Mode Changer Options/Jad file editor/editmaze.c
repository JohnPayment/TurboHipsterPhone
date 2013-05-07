/*
    EDITMAZE.C

    Main Edit Module
    All code for edit of level and a "LEVEL??.MAZ"-File

    Last change: 19.01.01 / PM
     		 20.01.01 / PM
		 10.06.01 / PM
*/

/******************************************************************************
* INCLUDES								      *
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>	/* atol */
/* ------ Own headers ------ */
#include "jadkeys.h"
#include "editmaze.h"
#include "textout.h"
#include "maze.h"
#include "jadinput.h"

/******************************************************************************
* DEFINES								      *
******************************************************************************/

/* Defines */
#define EDITMAZE_TESTLEVEL  0
#define EDITMAZE_CHOOSELEVEL 0x301

/******************************************************************************
* TYPEDEFS								      *
******************************************************************************/

typedef struct {
    unsigned int  type;		// Bits for type
    int		  count; 	// Count of Menupoints -> Excluding Abort / Ok
    char 	  *strings;	// Pointer on Menustrings, Endsign 0x80
    int		  width;	// Width of background-Rectangle
    				// X / Y / Width / Height
    int		  hadd;		// Distance between Menustrings vertically
    				// if < Height of Used font the set on height of
                                // used Font + 2.
    // Variables for internal use
    int  	  fontno;	// Number of Font used
    int  	  mcount; 	// Count of Menupoints that have to be checked
    int  	  oldmpoint;	// Old ( highlighted ) menupos
    int 	  actmpoint;	// Actual menupoint
    int		  fontw, fonth; //
} MENU;

/******************************************************************************
* DATA									      *
******************************************************************************/

MENU MainMenu[] = {
    {	/* FileMenu */
	MENU_FONT8, 5,
    	"Open...\0Save...\0Exit\0---------\0Clear Map\0\0x80",
    	72,
        12	/* Heightadd */
    },
    {	/* "Walls"-Menu */
    	MENU_FONT8, 4,
    	"Set Wall\0Stair Up\0Stair Down\0Number\0\x80",
    	80,
        12
    },
    {   /* Decorations */
	MENU_FONT8, 5,
    	"Decoration\0No Event\0With Event\0Number (Side)\0Switch Wall\0\x80",
    	104,
        12
    },
    {	/* Door-Menu */
	MENU_FONT8, 5,
    	"Door Type 1\0Door Type 2\0Knob Set/Clear\0"
        "Open Door\0Close Door\0\x80",
    	80,
        12
    },
    {	/* Item-Menu */
        MENU_FONT8, 3,
        "Get New\0Get One\0Clear\0\x80",
        56,
        12
    },
    {	/* Event - Menu */
        MENU_FONT8, 1,
        "New...\0Change...\0Clear...\0\x80",
        72,
        12
    }
};

MENU SetWallsMenu = {
    MENU_FONT8, 2,
    "No Wall\0Solid Wall\0\Doorx80",
    80,
    12
};

MENU FileNameMenu = {
    MENU_FONT8, 10,
    "Level 1\0Level 2\0Level 3\0Level 4\0Level 5\0Level 6\0"
		   "Level 7\0Level 8\0Level 9\0Level 10\0\x80",
    64,
    12
};

/* The menu editchoices	*/
static int EditLevel = 0;
static char NewWallNo = 0;

/* An array of vectors for submenupoints... */
static INPUTVEC SubMenuPoints[30];

/******************************************************************************
* CODE									      *
******************************************************************************/

/*
 * Name:
 *     EditMaze_CreateMenu
 * Function:
 *     Construction of a Submenu
 * Input:
 *     pmenu:      Pointer on a menu
 *     menupoints: Where to put the mouse-rectangles and strings
 */

void EditMaze_CreateMenu(int x, int y, MENU *pmenu, INPUTVEC *menupoints, unsigned short maincode)
{
    INPUTVEC *pmenup;
    int i;
    int fontsize, width;

    /* First set Fontsize */
    if (pmenu -> type & MENU_FONT8)
    	pmenu -> fontno = FONT8;
    else
        pmenu -> fontno = FONT6;

    fontsize = FontGetSize(pmenu -> fontno);  /* Loword: Fontwidth, HiWord: Fontheight */
    pmenu -> fontw = fontsize & 0xFFFF;
    pmenu -> fonth = fontsize >> 16;

    /* check hadd in MENU, adjust if needed */
    if (pmenu -> hadd < pmenu -> fonth)
    	pmenu -> hadd = pmenu -> fonth;

    /* Set rectangle-Widths and -heights */
    pmenup = menupoints;
    width = pmenu -> width + pmenu -> fontw;

    /* First create an inactive menupoint that holds the info for the size */
    /* of the background, telling the input to draw the background.	   */

    pmenup -> buttons   = INP_DRAWONLY;
    pmenup -> drawflags = JADINPUT_HASBACKGR;
    pmenup -> key         = 0;
    pmenup -> rect.x      = x - 4;
    pmenup -> rect.y      = y - 4;
    pmenup -> rect.width  = width;
    pmenup -> rect.height = (pmenu -> count + 1)  *  pmenu -> hadd;


    pmenup++;

    for (i = 0; i < pmenu -> count; i++) {
        pmenup -> rect.x      = x;
        pmenup -> rect.y      = y;
        pmenup -> rect.width  = width;
        pmenup -> rect.height = pmenu -> fonth;
        // Now add the string
        pmenup -> text      = Jad_ListStr(pmenu -> strings, i);
        pmenup -> buttons   = INP_ACTIVE |INP_KEYBOARD | JAD_MOULEFT;
        pmenup -> drawflags = JADINPUT_ISTEXT;
        pmenup -> key 	    = pmenup -> text[0];	/* Set first char for choice */
        pmenup -> maincode  = (unsigned short)maincode;
        pmenup -> subleft   = (unsigned short)(i + 1);
        pmenup -> subright  = 0;
        //
        y += pmenu -> hadd;
        pmenup++;
    }

    pmenup -> buttons = 0;			/* Now set the stop */
    pmenu -> oldmpoint = 0;
}


/****************** LIST FOR NEW WALLS **************/

/*
 * Name:
 *     EditMaze_GetFreeWallNumber
 * Description:
 *      Returns the first number of following empty wall descriptor(s)
 *     The first number is always even, if more then one is asked for.
 * Input:
 *     How many wall descriptors to get
 * Output:
 *     Number of first free wall-descriptor
 */
char EditMaze_GetFreeWallNumber(char count)
{
    NewWallNo = Maze_GetFreeWallNumber(count);

    return NewWallNo;
}

//***************** MENU-INPUTS **********************

/*
 * Name:
 *     EditMaze_EDITDecNoEV
 * Description:
 * 	Sets a Decoration without space for an EVENT.
 */


/*
 * Name:
 *
 * EditMaze_EditDecEV: Sets a Decoration, if possible.
 */


/***************** SUBROUTINES EDIT ****************************/

/********************* FILE - MENU ****************************/

/*
 * Name:
 *     EditMaze_FileOpenHandler
 * Description:
 *     Eventhandler for Submenu FileOpen
 * Input:
 *
 *
 */
static int EditMaze_FileOpenHandler(INPEVENT *inpevent)
{
    if ((inpevent -> subcode > 0) && (inpevent -> subcode < 11)){
        EditLevel = inpevent -> subcode;
        Maze_Load(inpevent -> subcode);
        JadInput_SetMazeValues(-1, EditLevel, -1);
        return JAD_CLOSEINPBOX;
    }

    return 0;
}

/*
 * Name:
 *     EditMaze_File
 * Input:
 *     choice: The menu-choice
 */
static int EditMaze_File(int choice)
{
    // "Open...\0Save...\0Exit\0--------\0Clear Map\0
    switch (choice) {
        case 1:		/* Open the Maze "level%d.jmz" 	*/
            /*
	    Maze_Load(EDITMAZE_TESTLEVEL);
            return 2;
            */
            /* Open the submenu... */
            EditMaze_CreateMenu(80, 24, &FileNameMenu, &SubMenuPoints[0], EDITMAZE_CHOOSELEVEL);
            JadInput_OpenInputWindow(&SubMenuPoints[0], EditMaze_FileOpenHandler);
            break;

        case 2:		/* Save the current work   	*/
            Maze_Save(EditLevel);
            break;
        case 3:		/* Exit the Editor 	   	*/
            return 1;
        case 4:   	/* MENUBREAK: Just do nothing 	*/
            break;
        case 5:		/* Clear the map, BUT ASK FIRST	*/
            /* JadInput_YesNoMsg("Clear the current map?", EditMaze_ClearHandler) */
            Maze_Clear();
            return 2;
    }

    return 0;
}



/*
 * Name:
 *     EditMaze_AskWall
 *
 *
 */



/*
 *  Name:
 *      EditMaze_AskDec
 *   Decription:
 *      Ask for a "Decoration" to attach
 *   Input:
 *     choice: MenuChoice
 */

static void EditMaze_AskDec(int choice)
{
    switch(choice) {
    	case 1:   // Decoration
            break;
        case 2:   // No Event
            break;
        case 3:   // With Event
            break;
        case 4:   // Number (Side)
            break;
        case 5:   // Switch Wall
            break;	
    }

}


/*
 * Name:
 *     MazeEdit_Item
 * Description:
 *     Make New item / Get special item / Delete item
 * Input:
 "     choice: MenuChoice
 */
static void MazeEdit_Item(int choice)
{
    switch(choice) {
    	case 1:		/* Set Item */
        case 2:
            /*  xchg	ax,si ; SI-> Save MenuNum
		;---
		mov	ax,OFFSET _ItemPrompt
		number = EditMaze_GetPromptNum  (1..124)
                break;
            */
            if (choice == 1) {
                // Item_AddToList(number);
            }
            else {
            	// ITTake(number);
            }
            break;
        case 3:		/* Delete actual item ( Mousepointer ) */
            // call	_ITConsume(0);
            break;
    }

}

/***************** MAIN_ROUTINE EDIT ***************************/

/*
 * Name:
 *     EditMaze_OpenMenu
 * Description:
 *     Opens the menu asked for by the number at position.
 * Input:
 *     x, y:       Position where to Open Menu ( Start of Background )
 *     menu:       Number of menu to open
 *     background: Pointer on Menuvec where to put the Background-Measure
 *     menupoints: Start of buffer where to put the info for the menupoints
 */
void EditMaze_OpenMenu(int x, int y, int menu, INPUTVEC *menupoints)
{
    /* Build the menu */
    EditMaze_CreateMenu(x,  y, &MainMenu[menu - EDITMAZE_MENUBASE - 1], menupoints, (unsigned short)menu);

    /* TODO: "Stacked"-Menu (Sub-Menu)*/
}


/*
 * Name:
 *     EditMaze_HandleMenuInput
 * Description:
 *     Opens the Dialog or whatever needed to get the input for the
 *     menupoint from the user.
 * Input:
 *     menu:   	   Number of menu to get input from
 *     choice: 	   Number of menupoint in menu
 *     posx, posy: Position of tile to be edited
 *     side:	   Side of the tile to be edited (if any, -1 means all )
 * Output:
 *    2: Refresh the display	
 *    1: Exit selected
 *    0: Move on
 */
int  EditMaze_HandleMenuInput(int menu, int choice)
{
    if (choice > 0) {
        switch(menu) {
            case EDITMAZE_FILE:
            	return EditMaze_File(choice);

	    case EDITMAZE_WALLEDIT:
            	// EditMaze_AskWall(choice);
            	break;

            case EDITMAZE_DECORATION:
            	EditMaze_AskDec(choice);
            	break;

            case EDITMAZE_DOORS:
            	/* EditMaze_AskDoor(choice); */
            	break;

            case EDITMAZE_ITEMS:
            	MazeEdit_Item(choice);
            	break;

	    case EDITMAZE_EVENTS:
	    	// MazeEdit_Events(choice);
                break;
        }
    }

    return 0;
}
