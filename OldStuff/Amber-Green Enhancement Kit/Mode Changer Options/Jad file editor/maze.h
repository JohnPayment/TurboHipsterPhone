/*
     MAZE.H
     Header for all the things, that belongs to the maze.
*/


#ifndef _MAZE_H_
#define _MAZE_H_

#include "display.h"

/******************************************************************************
* DEFINES								      *
******************************************************************************/

/* Maximum of Wall-Descriptors */
#define MAZE_MAXSIZE       32	/* Power of two				*/
#define MAZE_MAXTILECOUNT (MAZE_MAXSIZE * MAZE_MAXSIZE)

#define MAZE_NORTH    0		/* The indexes of the tile-sides	*/
#define MAZE_EAST     1
#define MAZE_SOUTH    2
#define MAZE_WEST     3

#define DOORTYPECOUNT 2
#define DOORTYPE1     3		/* Base - Wallnumber of Door	*/
#define DOORTYPE2     8
#define DOORTYPE3     13
#define DOORTYPE4     18

#define DOORSTATE_CLOSED 0  /* Door closed     */
#define DOORSTATE_STUCK  1  /* Same door stuck */
#define DOORSTATE_MOVES  2  /* Same door moves */
#define DOORSTATE_OPEN	 4  /* Same Door open  */

/* Door Operations */
#define DOOROP_OPERATE  0	/* Closes the door, if open  */
				/* Opens the door if closed		     */
                                /* Switches the move-direction if door moves */
#define DOOROP_OPEN    +1       /* Opens a door, if closed or stuck */
#define DOOROP_CLOSE   -1       /* Closes a door, if open or stuck  */

/* Flags for triggering Events */
#define EV_ENTER     0x10       /* Something enters the tile 		*/
#define EV_LEAVE     0x20	/* Something leaves the tile		*/
#define EV_HANDLE    0x40	/* A handle triggers a tile-event	*/
#define EV_TIMER     0x80	/* An event is triggered by a timer	*/
#define EV_PLAYER    0x01      	/* An event is triggered by a player	*/
#define EV_THROWN    0x02	/* A thrown item/spell 			*/
#define EV_ITEM      0x04	/* An item is dropped/picked on tile	*/
#define EV_MONSTER   0x08	/* A monsters 				*/

/* Flags, which type of wall / if wall can be passed */
#define WT_


/* Number of Standard-Events for Walls */
/* EQUATIONS: DECEVENTS */
#define DEV_DOORKNOB     1
#define DEV_TOUCHWALL    2
#define DEV_HANDLEON     3
#define DEV_HANDLEOFF    4
#define DEV_FORCEDOOR    5
#define DEV_PRYDOOR      6
#define DEV_EXTWALL      7
#define DEV_WALL         8
#define DEV_EXTWALL2     9
#define DEV_COMPARTMENT 10		/* A compartment in the wall  */


/* Wall-Types: Who can pass */
#define WF_PASSALL     0x07    /* Can be passed by all			 */
#define WF_PARTYPASS   0x01    /* Can be passed by the player, big item */
#define WF_SMALLPASS   0x02    /* Can be passed by a small item	 */
#define WF_MONSTERPASS 0x04    /* Can be passed by a monster		 */
#define WF_ISDOOR      0x08    /* Is a door				 */
#define WF_DOOROPEN    0x10    /* The door is open			 */
#define WF_DOORCLOSED  0x20    /* The door is closed			 */
#define WF_DOORKNOB    0x40    /* The door has a knob			 */
#define WF_ONLYDEC     0x80    /* No wall, only decoration, items visi	 */
#define WF_PASSNONE    0x00    /* Can be passed by no one		 */
/* Wall-Types: Kind */
#define WT_SOLID      0x01	/* Is a solid wall, draw top		 */
#define WT_SELFDRAW   0x02	/* Is a stair, for example, no bottom	 */
#define WT_DOORSTUCK  0x04	/* The door is stuck			 */
#define WT_DOORMOVES  0x08    /* The door is open			 */
#define WT_DOOROPEN   0x10    /* The door is open			 */
#define WT_DOORCLOSED 0x20    /* The door is closed			 */

/* Handling of flags */
#define MAZEFLAG_GET   1
#define MAZEFLAG_SET   2
#define MAZEFLAG_CLEAR 3
#define MAZEFLAG_FLIP  4

/******************************************************************************
* CODE									      *
******************************************************************************/

    /* Doors */
    int  Maze_OperateDoor(short int pos, char op);
    void Maze_MoveDoors(void);
    int  Maze_AddDoor(int pos, char typeno);
    
    /* Maze-Tiles and Tile-Sides */
    void Maze_Clear(void);
    int  Maze_Pos(int x, int y);
    void Maze_SetWall(int pos, int solid);
    void Maze_SetWallSide(int pos, int side, char number);
    void Maze_ChangeWallSide(int pos, int side, char num1, char num2);
    void Maze_SetDecoration(int wallno, char number);
    void Maze_CopyWallDescription(char src, char dest, int count);
    char Maze_GetWallSide(int tileno, int side);
    int  Maze_MovePos(int direction, int *xpos, int *ypos);


    /* Create the Maze-Wall-Stuff of the View */
    void Maze_SetDisplayList(DISPLAYTILE *tiles, int *xpos, int *ypos);
    void Maze_SetMap(int left, int top, int width, int height, char *map);
    int  Maze_IsTileVisible(int x, int y, int flag);
    char Maze_GetWallNumber(int x, int y, int side);
    char Maze_GetFreeWallNumber(char count);
    int  Maze_HandleFlags(unsigned int num, int how);
    /* Now the File Read-/Write-Stuff */
    int Maze_Load(int levelno);
    int Maze_Save(int levelno);

#endif	/* _MAZE_H_ */


