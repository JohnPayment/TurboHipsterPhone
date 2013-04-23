/*
     MAZE.C
     Includes all Wallinfos, moving of Doors and so on.

     Last changes: 21.01.01 / PM
     		   23.01.01 / PM
                   24.01.01 / PM
                   28.01.01 / PM: Stairs again as wall 			
*/

#include <mem.h>	/* memcpy*/
#include <stdio.h>	/* sprintf */
/* ----- Own headers ----- */
#include "maze.h"
#include "mazefile.h"

/******************************************************************************
* DEFINES								      *
******************************************************************************/

// Define Tileno
#define TILENO(x,y)  ((y * Maze.xsize) + x)

#define MAZE_MINWALLTYPE 25 	/* 0..2 reserved for Standard-Tiles	*/
				/* 3..22: Reserved for 2 Types of Door */
                                /*	  with and without knob		*/
                                /* 23: Stair up				*/
                                /* 24: Stair down			*/
#define MAZE_MAXWALLTYPE 127




#define MAZE_MAXDOORSTEPS   16

#define MAZE_MAXDOOR    16	/* Maximum of Door, that can be active the same time */
#define MAXANIMDEC 32	/* Maximum of "decorations" that can have an animation
			  cicle */

/******************************************************************************
* TYPEDEFS								      *
******************************************************************************/

typedef struct {
    char wallno;	 	/* Type of tile to display Wall, 0: Bottom   */
    				/* else number of Walltexture to use 	     */
    char dec;			/* Number of "Decoration 		     */
    char map;			/* Number of "Map"-picture		     */
    unsigned char wallflag;	/* Which kind of Wall, walk through is 	     */
    				/* possible and so on "WF_..."		     */
    unsigned char decev;	/* "Visible" Decoration-Event like "switch"
    				   of a Knob and so on "DEV_..."	     */
    unsigned char typebits;	/* Which kind of wall "WT_..."		     */
    unsigned int  evbits;	/* Event is triggered by "EV_..."	     */

    int      evnum;		/* Number of Event triggered 	             */
    unsigned char fill[2];	/* Fill up to 16 Bytes			     */
} WALLINFO;

typedef struct {
   int  tilenumber;             /* Which tile the door is at		     */
   char side;			/* On which side the door is orientated N,E  */
   char movepos;		/* Position of Door, between 0..MAZE_MAXDOORSTEPS */
      				/* 0: Closed, MAZE_MAXDOORSTEPS: Open	     */
   char moveadd; 		/* -1: down: Closing +1: Opening	     */
   				/*  0: Stucked or ended cicle		     */
   char doortype;	 	/* Type of door: ( 0..3 )		     */
   int  movemax; 		/* Number of cicles			     */	
} DOOR;

typedef struct {
    int tilenumber;		/* Which tile the animated decoration belongs to */
    int side;			/* On which side of the tile the decoration  */
    				/* is positioned.			     */
    int maxanim;		/* Maximum of animations in decoration	     */
    int aktanim;		/* Actual position in animation		     */
    int loop;			/* Has the animation to be repeated?	     */
    				/* True:  Example: Torch 		     */
                                /* False: Example: Moving handle	     */
    int animationnr;            /* Number of animation 			     */
} ANIMDEC;

/******************************************************************************
* DATA									      *
******************************************************************************/

WALLINFO WallInfo[MAZE_MAXWALLTYPE] = {
    { 0,  0, 0, 0x07, 0 },		   /* 0, Standard-Tile no wall */
    { 1,  0, 1, 0x00, 0 }, /* Standard-Tile wall       */
    { 2,  0, 1, 0x40, 0 },
    /* Door Type 1 */
    { 3, -1, 2, 0xA8, 1, WT_DOORCLOSED }, /* Closed Door     */
    { 3, -1, 2, 0x88, 1, WT_DOORSTUCK  }, /* Same door stuck */
    { 3, -1, 2, 0x88, 1, WT_DOORMOVES  }, /* Same door moves */
    { 3, -1, 2, 0x88, 1, WT_DOOROPEN   },
    { 3, -1, 2, 0x9F, 1, WT_DOOROPEN   }, /* Same Door open  */
                    /* Same without knob  */
    { 3,  0, 3, 0xA8, 6, WT_DOORCLOSED },
    { 3,  0, 3, 0x88, 0, WT_DOORSTUCK  },
    { 3,  0, 3, 0x88, 0, WT_DOORMOVES  },
    { 3,  0, 3, 0x88, 0, WT_DOOROPEN   },
    { 3,  0, 3, 0x9F, 0, WT_DOOROPEN   },
		    /* Door Type 2 */
    { 3, -1, 4, 0xA8, 1, WT_DOORCLOSED },
    { 3, -1, 4, 0x88, 1, WT_DOORSTUCK  },
    { 3, -1, 4, 0x88, 1, WT_DOORMOVES  },
    { 3, -1, 4, 0x88, 1, WT_DOORMOVES  },
    { 3, -1, 4, 0x9F, 1, WT_DOOROPEN   },
                   /* Same without knob2 */
    { 3,  0, 5, 0xA8, 6, WT_DOORCLOSED },
    { 3,  0, 5, 0x88, 0, WT_DOORSTUCK  },
    { 3,  0, 5, 0x88, 0, WT_DOORMOVES  },
    { 3,  0, 5, 0x88, 0, WT_DOORMOVES  },
    { 3,  0, 5, 0x9F, 0, WT_DOOROPEN   },   /* Same door stuck */
    /* Now the stairs...  */
    { 4,  0, 6, 0x03 },	/* 23 Stair up   	*/
    { 5,  0, 7, 0x03 },	/* 24 Stair down 	*/
    /* Now the "Standard-Walls" */
    { 0,  0, 0, 0x00 },		/* 25: Hole  in ceiling		*/
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },         /* 27: Hole in bottom		*/ 
    { 0,  0, 0, 0x00 },         /* 28: Pressure plate	    	*/
    { 1,  0, 0, 0x00 },		/* 29: Pidgeon hole in wall 	*/
    { 0,  0, 0, 0x00 },       	/* 30: Stuck door Type 1    	*/
    { 0,  0, 0, 0x00 },		/* 31: Suck door Type 2		*/
    { 1,  0, 0, 0x00 },		/* 32: Knob			*/
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 1,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 1,  0, 0, 0x00 },		/* 41: Writing on wall	*/
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 }, 	/* 44: Teleport 	*/
    { 0,  0, 0, 0x00 },
    { 1,  0, 0, 0x00 },		/* 46: "Mindrock"	*/
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 1,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 1,  0, 0, 0x00 },         /* 53: Keyhole		*/
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },         /* 55: Handle		*/
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 1,  0, 0, 0x00 },		/* 58: Door pole	*/
    { 0,  0, 0, 0x00 },
    { 1,  0, 0, 0x00 },		/* 60: Knob on wall	*/
    { 1,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0,  0, 0, 0x00 },
    { 0, 120, 4 },  /* Special "Animated" for demo purposes */
    { 0, 121, 5 },
    { 0, 122, 6 },

};



ANIMDEC  animdecs[MAXANIMDEC];		    /* Animated "Decorations" 	     */
DOOR     Door[MAZE_MAXDOOR];		    /* Doors	   		     */

struct {
    short int xsize,
    	      ysize;
    short int wallsize;
    unsigned char flags[8];		    /* 64 Flags to set / reset / ask */
    char          tiles[MAZE_MAXTILECOUNT][4];  /* The Maze itself 	     */
} Maze = {
     32, 32, 4,
     { 0, 0, 0, 0, 0, 0, 0, 0 },
     #include "level2.c";		    /* the test-data		     */
     };

/* Now the things that control update */
DISPLAYTILE *pActDisplayTiles = 0;	/* Pointer on the actuel used dis-    */
					/* play-tiles			      */
/******************************************************************************
* CODE									      *
******************************************************************************/

/*
 *  Name:
 *      Maze_OperateDoor
 *  Description:
 *	Operates a door in a the maze
 *  Input:
 *      postion:	Position in Maze, where to operate the door
 * 	op: 	Kind of operation
 *	  	DOOROP_OPEN     Opens a door, if closed
 *      	DOOROP_OPERATE  Switches the move-direction of a door
 * 		DOOROP_CLOSE    Closes a door, if open
 *
 */

int Maze_OperateDoor(short int pos, char op)
{
    WALLINFO *pwallinfo;
    DOOR     *door;
    int  i;
    int  tileno;
    char side, doortype;
    char *ptileside;	/* Points on the requested tile-side */


    tileno = pos;

    /* First take a look, if the door on this tile is operating yet */
    for (i = 0, door = Door; i < MAZE_MAXDOOR; i++, door++) {

        if (door -> tilenumber == tileno) {	/* The door is active */

            /* The only possibility here is: change the direction   */
            if (op == DOOROP_OPERATE) {
                door -> moveadd = -(door -> moveadd);
                return 1;
            }
	}
    }


    /* Otherwise We have to create a new moving door */
    for (i = 0, door = Door; i < MAZE_MAXDOOR; i++, door++) {
        if (door -> tilenumber == 0) {		/* We have found an empty slot    */
            side = 0;		 		/* First check north for doorinfo */
            ptileside = &Maze.tiles[tileno][0];	/* Point on the wall, 	  */
            					/* may have to be changed */
            pwallinfo = &WallInfo[*ptileside];

            if (! pwallinfo -> wallflag & WF_ISDOOR) {
                side++;
                ptileside++;
                pwallinfo = &WallInfo[*ptileside];

                /* if here is no door, we have a wrong input */
                if (! pwallinfo -> wallflag & WF_ISDOOR)
                    return 0;
            }


            switch (op) {
                case DOOROP_OPEN:
                    /* Door can only be opened, if it's closed or stuck      */
                    if (pwallinfo -> typebits & (WT_DOORCLOSED | WT_DOORSTUCK)) {
	                door -> movepos = 0; /* Set position closed */
                    }
                    break;
                case DOOROP_CLOSE:
                    /* Door can only be closed, if it's open or stuck    */
                    if (pwallinfo -> typebits & (WT_DOOROPEN | WT_DOORSTUCK)) {
	                door -> movepos = MAZE_MAXDOORSTEPS ;      /* Set position open    */
                        door -> movemax = MAZE_MAXDOORSTEPS ;
                    }
                    break;
                default:
                    /* Nothing happens at all */
                    return 1;
            } /* switch op */

            door -> movemax = MAZE_MAXDOORSTEPS ;

            /* Calc Base doortype */
            doortype = (char)(*ptileside - (char)(DOORTYPE1 / DOORTYPECOUNT));
            doortype = (char)(doortype * (char)(DOORTYPECOUNT + DOORTYPE1 + DOORSTATE_MOVES));

            /* Initialize the slot */
            Door[i].tilenumber = tileno;
            Door[i].side       = side;
            Door[i].moveadd    = op;  /* -1, +1 or 0 */
	    Door[i].doortype   = doortype ;
            /* Tell the maze that the door is moving */
            ptileside[0] = doortype;
            ptileside[2] = doortype;
        }  /* if tilenumber == 0 */
        else return 0;			/* No empty slot found, failed */
    }

    return 1;
}


/*
 *  Name:
 *      Maze_MoveDoors
 *  Description:
 *	Moves all Door in the "Door" list.
 *  Input:
 *	None:
 */
void Maze_MoveDoors(void)
{
    DOOR *door;
    int i;
    char *ptileside;
    char doortype;

    for (i = 0, door = Door; i < MAZE_MAXDOOR; i++, door++) {
    
        /* Only move active Door */
	if (door -> tilenumber > 0) {
            /* Get pointer on tilesides */
            ptileside = &Maze.tiles[door -> tilenumber][door -> side];

	    door -> movepos += door -> moveadd;   /* Add the movement  */
            /* Here is the Positon to play the sound ,that a door is moving */

            /* Check, if end of movement is reached */
	    if (Door[i].movepos >= Door[i].movemax) {
                /* The door is open */
                door -> tilenumber = 0;	      /* Delete moving door */

                /* Set it in the maze to open */
                doortype = (char)(door -> doortype + DOORSTATE_OPEN);
                ptileside[0] = doortype;
                ptileside[2] = doortype;
                /* Here is the Position to play a Sound "DoorClosed" */
            }
            else if (door -> movepos <= 0) {
            	/* The door is closed */
                door -> tilenumber = 0;	   /* Delete moving door */

                /* Set it in the maze to open */
                doortype     = Door[i].doortype + DOORSTATE_CLOSED;
                ptileside[0] = doortype;
                ptileside[2] = doortype;
                /* Here is the Position to play a Sound "DoorOpen */
            }
        } /* if door -> tilenumber > 0 */
    } /* for i < MAZE_MAXDOOR */
}

/*
 * Name:
 *     Maze_AddDoor
 * Description:
 *     Adds a door to the maze. If walls are on all 4 sides, then the passage
 *     leads to the north, else it's adjusted to full walls left an right.
 *     It is assumed that the position is not at the border of the maze.
 *     The door is initially closed, without knob.
 *  Input:
 *	pos:    Position where to put the door.
 *      typeno: Which type of door to add  0..3).
 */
int Maze_AddDoor(int  pos, char typeno)
{
    int  x, y;
    char wall1, wall2, doorno;

    typeno &= 3;				/* TO be on the safe side */
    doorno  = (char)((typeno * 8) + DOORTYPE1);	/* set Wall-Number	  */

    y = pos / Maze.xsize;
    x = pos % Maze.xsize;

    wall1 = Maze.tiles[TILENO(x-1,y)][1];
    wall2 = Maze.tiles[TILENO(x+1,y)][3];

    if ((wall1 != 0) && (wall2 != 0)) {
        /* The door is horizontally */
        Maze.tiles[pos][0] = doorno;
        Maze.tiles[pos][1] = 0;
        Maze.tiles[pos][2] = doorno;
        Maze.tiles[pos][3] = 0;
        return 1;
    }

    /* Check north/south */
    wall1 = Maze.tiles[TILENO(x,y-1)][2];
    wall2 = Maze.tiles[TILENO(x,y+1)][0];
    if ((wall1 != 0) && (wall2 != 0)) {
        /* The door is vertically */
        Maze.tiles[pos][0] = 0;
        Maze.tiles[pos][1] = doorno;
        Maze.tiles[pos][2] = 0;
        Maze.tiles[pos][3] = doorno;
        return 1;
    }

    return 0;		/* Failed */
}


/*
 * Name:
 *     Maze_Clear
 * Description:
 *     Inits the maze with Walls type 1 and 2, "checkered"
 */
void Maze_Clear(void)
{
    int  x, y;
    int  *pmaze;

    pmaze = (int *)&Maze.tiles[0][0];

    for (y = 0; y < MAZE_MAXSIZE; y++) {
        for (x = 0; x < MAZE_MAXSIZE; x++) {
            if ((x & 1) ^ ( y & 1))	/* A little tricky */
	        *pmaze = 0x01010202;
            else
	        *pmaze = 0x02020101;

            pmaze++;
        }
    }
}


/*
 * Name:
 *     Maze_Pos
 * Description:
 *
 * Input:
 *     x, y: Position in maze
 * Output:
 *     Number of the position.
 */
int Maze_Pos(int x, int y)
{
    return (y * Maze.xsize) + x;
}


/*
 * Name:
 *     Maze_SetWall
 * Description:
 *     Sets a solid/empty wall at tile "tileno"
 * Input:
 *     tileno: Number of tile to change
 *     solid:  0:  Empty tile
 */
void Maze_SetWall(int tileno, int solid)
{
    int *pmaze;

    pmaze = (int  *)&Maze.tiles[tileno][0];

    if (solid) {
        if ((tileno & 0x01) ^ ((tileno >> 6) & 1))
	    *pmaze = 0x01010202;	/* "Even" kind of Wall  */
        else
	    *pmaze = 0x02020101;   /* "Odd" kind of Wall */
    }
    else {
        *pmaze = 0;		/* All sides set to zero */
    }
}

/*
 *  Name:
 *	MazeSetWallSide
 *  Descritpion:
 *	Sets a walldesc-number at tile "tileno" at side side.
 *	if side = -1, then all sides are set.
 *  Input:
 *	tileno: Number of tile to change.
 *	side:	Side to change (NORTH..WEST)
 *      number: Number to set
 */

 void Maze_SetWallSide(int tileno, int side, char number)
 {
    int i;
    char *pmaze;

    pmaze = &Maze.tiles[tileno][0];

    if (side == -1) {
        for (i = 0; i < 4; i++)
            pmaze[i] = number;
    }
    else {
        pmaze[side] = number;
    }
 }


 /*
 *  Name:
 *	MazeChangeWallSide
 *  Descritpion:
 *	Changes a walldesc-number at tile "tileno" at side "side".
 *	if side = -1,   then all sides are set.
 *	if num1 = 0xFF, then the wall is set definitely
 *		        else, the tile-side is compared with the old number
 *		        and changed, if they are the same
 *	num2:	        Number to replace/switch the tile/tileside.
 *  Input:
 *	tileno:     Number of tile to change.
 *	side:	    Side to change (NORTH..WEST)
 *      num1, num2: Numbers to switch between
 */

void Maze_ChangeWallSide(int tileno, int side, char num1, char num2)
{
    int count;
    char *pmaze;
    char *pside;

    pmaze = &Maze.tiles[tileno][0];

    if (side == -1) {
        /* Change all sides */
        pside = &pmaze[0];
	count = 4;
    }
    else {
        /* Change one side */
        pside = &pmaze[side];
        count = 1;
    }

    if (num1 != -1) {
        /* We have to switch to switch the wallside */
        if (*pside != num1)
            num2 = num1;		/* switch */
        /* else its num2 anyway */
    }

    /* It allways num2 to set */
    while (count > 0) {
        *pside = num2;
        pside++;
        count--;
    }
}


/*
 * Name:
 *     Maze_SetDecoration
 * Description:
 *    Sets the number of a "Decoration" on tile.
 *    If (side = -1) then set all sides
 * Input:
 *    wallno: Number of wall where to put the "decoration"
 *    number: Number of decoration to set
 */

void Maze_SetDecoration(int wallno, char number)
{
    WallInfo[wallno].dec = number;
}

/*
 * Name:
 *     Maze_CopyWallDescription
 * Description:
 *    Creates a copy of a Wall-Description
 * Input:
 *    Src:   Number of Source Wall-Description
 *    Dst:   First Number of Destination Wall-Descriptor
 *    count: How many Wall-Descriptors to fill with copy
 */
void Maze_CopyWallDescription(char src, char dest, int count)
{
    WALLINFO *pwallinfo;

    pwallinfo = &WallInfo[dest];

    while (count > 0) {
        memcpy(pwallinfo,&WallInfo[src],sizeof(WALLINFO));

        pwallinfo++;
        count--;
    }
}

/*
 *  Name:
 *      Maze_GetWallSide
 *  Description:
 *      Get the Wallnumber at tileno ans side
 *  Input:
 *      tileno: Tile where to get the wall from
 *	side:	Which side the wallno to take from
 *  Output:
 *	Wallnumber
 */
char Maze_GetWallSide(int tileno, int side)
{
    return (Maze.tiles[tileno][side]);
}

/*
 *  Name:
 *      Maze_MovePos
 *  Description:
 *	Moves the position one tile in the given direction. The move
 *	don't happens, if the display list rectangle would leave the
 *	maze rectangle.
 *  Input:
 *      direction:   Direction to move Tile where to get the wall from
 *	xpos,  ypos: New position, if movement was possible.
 *  Output:
 *	1: Movement happened.
 *	0: Position stayed the same.
 */
int Maze_MovePos(int direction, int *xpos, int *ypos)
{
    int diradd,
    	newx, newy;

    direction &= 3;

    diradd = -1;
    if ((direction == 1) || (direction == 2)) {	/* 0 and 3: -1;	1 and 2: +1	*/
       diradd = +1;
    }

    if (direction & 1) {
  	/* x-direction */
        newx = *xpos + diradd;
        if ((newx < HALFVIEWMESH) || (newx > (Maze.xsize - HALFVIEWMESH - 1)))  {
            return 0;
        }
        else {
            *xpos = newx;
            return 1;
        }
    }
    else {
    	/* y-direction */
        newy = *ypos + diradd;
        if ((newy < HALFVIEWMESH) || (newy > (Maze.ysize - HALFVIEWMESH - 1)))  {
            return 0;
        }
        else {
            *ypos = newy;
            return 1;
        }
    }

}


/*
 * Name:
 *     Maze_SetDisplayList
 * Description:
 *     Fills all the infos on the visible rectangle of walls into the
 *     Display_TileList.
 *     If needed, the x-pos and y-pos are adjusted this way, that the visible
 *     regtangle never leaves the borders of the maze.
 * Input:
 *     tiles: Pointer on Display-Tile-List
 *     xpos, ypos: Pointer in Position in the middle on the visible mazerect
 */
void Maze_SetDisplayList(DISPLAYTILE *tiles, int *xpos, int *ypos)
{
    int x, y, i;
    int ystart, xstart;
    char walltype;
    DISPLAYTILE *pvisitiles;
    WALLINFO *pwallinfo;


    /* Set the pointer on the actual visble tiles */
    pActDisplayTiles = tiles;

    /* First get the numbers of the visible tiles. 	*/
    pvisitiles = tiles;	    /* Where to put the numbers	*/

    /* get the x - startpoint, adjust xpos if needed */
    xstart = *xpos - HALFVIEWMESH;

    if (xstart < 0) {
        *xpos -= xstart;        		/* Adjust x-pos  */
        xstart = 0;
    }

    if (xstart >= (MAZE_MAXSIZE-HALFVIEWMESH)) {
        xstart = (MAZE_MAXSIZE-HALFVIEWMESH-1);	/* Adjust x-pos  */
        *xpos  = xstart + HALFVIEWMESH;
    }

    /* get the y - startpoint	*/
    ystart = *ypos - HALFVIEWMESH;

    if (ystart < 0) {
        *ypos -= ystart;        		/* Adjust y-pos  */
        ystart = 0;
    }

    if (ystart >= (MAZE_MAXSIZE-HALFVIEWMESH)) {
        ystart = (MAZE_MAXSIZE-HALFVIEWMESH-1);		/* Adjust y-pos  */
        *ypos  = ystart + HALFVIEWMESH;
    }

    ystart = (ystart * MAZE_MAXSIZE) + xstart; 	/* get the xy - startpoint */

    for (y = 0; y < VIEWMESHSIZE; y++) {

        for (x = 0; x < VIEWMESHSIZE; x++) {

            pvisitiles -> number = ystart + x;
            pvisitiles -> flags &= ~TILE_WALLTYPES; /* Get rid of old wallflags */
            pvisitiles -> direction = -1; 	    /* Standard no direction*/
            pvisitiles++;
        }

        ystart += MAZE_MAXSIZE;
    }

    /* Now that we have the numbers of tiles to put into Display_TileList,  */
    pvisitiles = tiles;		         /* Begin at start of visible tiles */

    for (x = 0; x < VIEWMESHCOUNT; x++) {

        for (i = 0; i < 4; i++) {

            walltype = Maze.tiles[pvisitiles -> number][i];

	    pwallinfo = &WallInfo[walltype];

            pvisitiles -> side[i].walltype = walltype;
	    pvisitiles -> side[i].wall     = pwallinfo -> wallno;
            pvisitiles -> side[i].dec      = pwallinfo -> dec;

            switch(pwallinfo -> wallno) {
            	case 1:
                case 2:
                    pvisitiles -> flags	|= TILE_TOP;
                    break;
            	/* Doors and stairs have a direction */
            	case 3:
                     i += 2;

                     walltype = Maze.tiles[pvisitiles -> number][i];
                     pwallinfo = &WallInfo[walltype];

	             pvisitiles -> side[i].walltype = walltype;
		     pvisitiles -> side[i].wall     = pwallinfo -> wallno;
        	     pvisitiles -> side[i].dec      = pwallinfo -> dec;
                     pvisitiles -> doortype = 1;
                     pvisitiles -> doorpos = 10;
                     pvisitiles -> doormaxcicle = 16;
                     i -= 2;
                case 4:
                case 5:                    
                    pvisitiles -> direction = i;
                    i = 5;	/* Stop the loop */
                    break;
            }
        }

        pvisitiles++;
    }

    /* Check the visibility of the Walls around, clear these who haven't to  */
    /* be displayed							     */
    pvisitiles = tiles;		         /* Begin at start of visible tiles */
    y = 0;
    for (x = 0; x < VIEWMESHCOUNT; x++) {
        if (pvisitiles -> flags & TILE_TOP) {
            /* its a solid wall, may be hidden by the other two */
            if (pvisitiles[1].flags & TILE_TOP) {
                pvisitiles -> side[1].wall = -1;
                pvisitiles[1].side[3].wall = -1;
            }

            if (pvisitiles[VIEWMESHSIZE].flags & TILE_TOP) {
	        pvisitiles -> side[2].wall = -1;
                pvisitiles[VIEWMESHSIZE].side[0].wall = -1;
            }
        }

        pvisitiles++;		/* Next tile 	  */
        y++;
        if ((y % VIEWMESHSIZE) == (VIEWMESHSIZE-2)) {
            pvisitiles++;			/* Skip last tile */
        }

    }

}

/*
 * Name:
 *     Maze_SetMap
 * Description:
 *     Fills the given mapbuffer with the mapnumbers from the
 *     Maze-Walldescriptors
 * Input:
 *     left, top:     Left upper corner in maze
 *     width, height: Size of part of the maze to put in the mapbuffer
 *     map:	      Pointer on mapbuffer
 */
void Maze_SetMap(int x, int y, int width, int height, char *map)
{
    int xrun, ypos, i;
    char mappic;

    ypos = (y * Maze.xsize) + x;

    while (height > 0) {

        for (xrun = 0; xrun < width; xrun++) {

            mappic = WallInfo[Maze.tiles[ypos + xrun][0]].map;

            if (mappic == 0) { /* Handle that special for possible doors and stairs */

                for (i = 1; i < 4; i++) {
                    /* Set the map */
                    mappic = WallInfo[Maze.tiles[ypos + xrun][i]].map;
                    if (mappic > 0)
                    	break;
                }
            }

            *map = mappic;

            /* Next */
            map++;
        }

        ypos += MAZE_MAXSIZE;
        height--;
    }

}

/*
 * Name:
 *     Maze_IsTileVisible
 * Description:
 *     Checks, if the tile at position x, y is in the visible rectangle.
 *     if true, the appropriate field in "pActDisplayTiles" is updated.
 * Input:
 *     x, y: Position
 *     flag: Flag to set in the Display_TileList
 * Output:
 *     1: Visible  / 0: Not
 */
int Maze_IsTileVisible(int x, int y, int flag)
{
    int i, j;
    int tileno;
    char walltype;
    WALLINFO *pwallinfo;
    DISPLAYTILE *tiles;

    tiles = pActDisplayTiles;
    tileno = (y * MAZE_MAXSIZE) + x;

    for (i = 0; i < VIEWMESHCOUNT; i++) {

       if (tiles[i].number == tileno) {	 	/* The position is visible */
       	   /* Adjust the info in  DISPLAYTILE */
           tiles[i].flags |= flag;

           for (j = 0; j < 4; j++) {
               walltype = Maze.tiles[tileno][j];
               tiles[i].side[j].walltype = walltype;

               pwallinfo = &WallInfo[walltype];
	       tiles[i].side[j].wall = pwallinfo -> wallno;
               tiles[i].side[j].dec  = pwallinfo -> dec;
           }

           if (tiles -> side[0].wall != 0) {
               tiles -> flags |= TILE_TOP;
           }
           else {		/* maybe, a solid wall has changed to bottom */
               tiles -> flags &= ~TILE_TOP;         /* Make rid of this flag */
           }

           /* TODO: Check the visibility of the Walls around */

           return 1;
       }
    }

    return 0;
}

/*
 * Name:
 *     Maze_GetWallNumber
 * Description:
 *     Returns the number of the wall asked for
 * Input:
 *     x,y:  Position of wall
 *     side: Which side
 */
char Maze_GetWallNumber(int x, int y, int side)
{

   return Maze.tiles[TILENO(x,y)][side];
}

/*
 * Name:
 *     Maze_GetFreeWallNumber
 * Description:
 *      Returns the first number of following empty wall descriptor(s)
 *      The first number is always even, if more then one is asked for.
 *	The Walldescriptors are presumed empty if the whole WALLDESC is empty.
 *
 * Input:
 *     None
 * Output:
 *     0, if none found
 */

char Maze_GetFreeWallNumber(char count)
{
    int i;
    char wallno, firstwall, *pwinfo;

    firstwall  = 0;

    for (wallno = MAZE_MINWALLTYPE; wallno < MAZE_MAXWALLTYPE; wallno++) {
        /* Check if empty */
        pwinfo = (char *)&WallInfo[wallno];
        i = sizeof(WallInfo);

        while (*pwinfo == 0) {
            pwinfo++;
            i--;
        }

        if (i == 0) {  /* We found an emptyone */
            if (count == 1) {
                return wallno;
            }
            else {
                if (firstwall == 0 && (! (wallno & 1)) ) { /* Only get, if even */
   		    firstwall = wallno;
                }
                else {
                    if ((wallno - firstwall) == (count-1))
                        return firstwall;
                }
            }
        }
        else {
            if (count > 1) {
                firstwall = 0;
            }
        }
    }

    return 0;
}

/*
 * Name:
 *     Maze_HandleFlags
 * Description:
 *    Handles the flags in the Maze. It is created in a way, that Endianness
 *    doesn matter.
 * Input:
 *    num: Number of Flag
 *    how: How to handle the flag
 * Output:
 *    State of flag
 */

int Maze_HandleFlags(unsigned int num, int how)
{
    unsigned char *flag;
    unsigned char mask;

    /* first get pointer on flag */
    flag = &Maze.flags[(unsigned char)num  >> 3];	/* DIV 8     */
    mask = (unsigned char)(1 << (num & 0x07));		/* num mod 8 */

    switch(how) {
	case MAZEFLAG_SET:
	    *flag |= mask;
	    break;
	case MAZEFLAG_CLEAR:
            *flag &= (~mask);
	    break;
	case MAZEFLAG_FLIP:
            *flag ^= mask;
    }

   return (mask & (*flag));		/* MAZEFLAG_GET */
}


/*
 * Name:
 *     Maze_Load
 * Description:
 *     Loades the maze. In the file Filename
 * Input:
 *     Filename
 */

int Maze_Load(int levelno)
{
    char filename[128];

    sprintf(filename, "level%d", levelno);
    if (MazeFile_OpenRead(filename,"jmz")) {
    	MazeFile_Read(6, &Maze);
    	MazeFile_Read(MAZE_MAXTILECOUNT * 4, &Maze.tiles[0][0]);
    	MazeFile_Close();

        return 1;
    }

    return 0;
}

/*
 * Name:
 *     Maze_Save
 * Description:
 *     Saves the maze. In the file Filename
 * Input:
 *     Filename
 */
int Maze_Save(int levelno)
{
    char filename[128];

    sprintf(filename, "level%d", levelno);

    MazeFile_OpenWrite(filename,"jmz");
    MazeFile_Write(6, &Maze);
    MazeFile_Write(MAZE_MAXTILECOUNT * 4, &Maze.tiles[0][0]);
    MazeFile_Close();

    return 1;
}
