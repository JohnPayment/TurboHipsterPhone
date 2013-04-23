/*
    JADGLOB.H
*/

#ifndef _JADGLOB_H_
#define _JADGLOB_H_

/******************************************************************************
* DEFINES								      *
******************************************************************************/
// Tiles
#define TILE_MAXXY 32
#define TILE_MAX  (TILE_MAXXY * TILE_MAXXY)
#define TILE_NUMBER(tile) ((tile.y * TILE_MAXXY) + tile.x)

#define JAD_NORTH     0
#define JAD_NORTHEAST 1
#define JAD_EAST      2
#define JAD_SOUTHEAST 3
#define JAD_SOUTH     4
#define JAD_SOUTHWEST 5
#define JAD_WEST      6
#define JAD_NORTHWEST 7

#define JAD_TILESIZE     128
#define JAD_HALFTILESIZE 64

/* ---- Throw Distances --- */
#define THROW_MAXITEMFLY     12
#define THROW_MAXSPELLFLY   127


// Character Class - Flags
#define CLASS_FIGHTER   0x01

/******************************************************************************
* GLOBAL TYPEDEFS							      *
******************************************************************************/

typedef struct {
    char cnt,		/* How many times to roll the dice		*/
    	 value,		/* Roll a value between 1 and this value	*/
         base;		/* Base to add to the rolled value		*/
    char level;         /* Level for later: Multiply randomno by this 	*/
} DICE;

typedef struct {
    int   valid;		/* If the position is valid or not	    */
    int   posx,
    	  posy,			/* Tile					    */
          posz,			/* Height above bottom			    */	
          subposx,		/* X-position -63 .. +64 on tile 	    */
    	  subposy,		/* Y-position -63 .. +64 on tile 	    */
          facing;  		/* Facing 0..359 Degrees, counterclockwise  */
    float velx,                 /* Velocity, if any 0 .. 128 ( in tilesize )*/
    	  vely,
          velz;
    float radius;		/* Collision radius			    */
} JADPOS; 			/* General position type */

typedef struct {
    int  x, y,
    	 width, height;
} JADRECT;

/******************************************************************************
* GLOBAL DATA								      *
******************************************************************************/

/******************************************************************************
* CODE									      *
******************************************************************************/

char *Jad_ListStr(char *strlist, int num);
int  Jad_RollDice(DICE dice);
int  Jad_RandomNo(int number);
int  Jad_MovePos(JADPOS *pos, JADPOS *newpos);
int  Jad_Distance(JADPOS *pos, JADPOS *newpos);

#endif // _JADGLOB_H_