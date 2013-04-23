/*
    JADGLOB.C
*/

/******************************************************************************
* INCLUDES								      *
******************************************************************************/

#include <stdlib.h>	/* RAND_MAX 	*/
#include <mem.h>	/* memcpy	*/
/* ---- Own headers ---- */
#include "jadglob.h"

/******************************************************************************
* DATA									      *
******************************************************************************/

static int gravity = 1.0;		/* 1 game measure unit	*/

/******************************************************************************
* CODE									      *
******************************************************************************/

/*
 * Name:
 *     Jad_ListStr
 * Function:
 *     Get a String from a Concat-List. The Concatlist has to be termianted
 *     with 0x80
 *  Input:
 *     strlist: Pointer on strlinglist
 *     strno:   Number of string
 *  Output:
 *     Pointer on String with this Index, may be an empty String.
 */

char *Jad_ListStr(char *strlist, int strno)
{
    while (strno > 0) {
        if ((unsigned char)strlist[1] == 0x80)
            break;		/* Endsign detected */
        while (*strlist != 0)
            strlist++;		/* Find the next String */
        strlist++;		/* Jump over the Zero   */
        strno--;		/* Decrement Counter    */
    }

    return strlist;
}

/*
 * Name:
 *     Jad_RollDice
 * Description:
 *     Generates a random number on base of the given DICE-Structure.
 *     Formula: "count" times * (1 <= randno <= value) + base;
 *  Input:
 *	dice: Dice-descriptor.
 *  Output:
 *     Generated random number, 0 in the worst case.
 */
int Jad_RollDice(DICE dice)
{
    int result;

    result = 0;

    while (dice.cnt > 0) {
    	  result += (int)(((long)rand()*(dice.value))/(RAND_MAX+1));
          dice.cnt--;
    }

    return (result + dice.base);
}

/*
 * Name:
 *     General_RandomNoCount
 * Description:
 *     Returns a random number between 0 and number - 1;
 *  Output:
 *     Generated random number, 0 in the worst case.
 */
int  Jad_Random(int number)
{
   return (int)(((long)rand()*(number))/(RAND_MAX+1));
}

/*
 * Name:
 *     Jad_MovePos
 * Description:
 *     Moves the given position and velocity. Returns the new position
 *     in newpos. If zvel > 0, gravity is addedto this moving.
 *     Middle of tile is always (0 / 0)
 * Input:
 *	pos: Position with collision radius to check for collision
 *	     with a wall.
 * Output:
 *	   If the collision radius left the tile...
 *	   0..7: Directon of collision tile... JAD_... --> Collision
 *	  -1: No collision
 *
 */
int Jad_MovePos(JADPOS *pos, JADPOS *newpos)
{
    static char dirlookup[] = {
                   -1,     JAD_NORTH,      JAD_EAST, JAD_NORTHEAST,
            JAD_SOUTH,            -1, JAD_SOUTHWEST, 	        -1,
             JAD_WEST, JAD_NORTHWEST,            -1,            -1,
        JAD_SOUTHWEST,            -1,            -1,            -1
    };

    int leaveidx,
        dirflag;
    int   xadd, yadd;
    float velx, vely, velz;


    memcpy(newpos, pos, sizeof(JADPOS));
    leaveidx = -1;


    velx = pos -> velx;
    vely = pos -> vely;
    velz = pos -> velz;

    dirflag = 0;

    /* depending on direction, xpos/ypos added or subtracted */
    if ((pos -> facing > 0) && (pos -> facing < 180)) {
        yadd = -1;
        dirflag |= 0x02;	/* Flag East 	*/
    }
    else {
        yadd = +1;
        dirflag |= 0x08;	/* Flag West 	*/
    }

    if ((pos -> facing <= 90) && (pos -> facing >= 270)) {
        xadd = -1;
	dirflag |= 0x01;	/* Flag north	*/
    }
    else {
        xadd = +1;
        dirflag |= 0x04;	/* Flag south	*/
    }

    newpos -> subposx += velx;
    newpos -> subposy += vely;

    /* Now check, if the subpos left the tile */
    if (abs(newpos -> subposx) > JAD_HALFTILESIZE) {
        newpos -> subposx -= (JAD_HALFTILESIZE * xadd);
        newpos -> posx += xadd;
    }

    if (abs(newpos -> subposy) > JAD_HALFTILESIZE) {
        newpos -> subposy -= (JAD_HALFTILESIZE * yadd);
        newpos -> posy += yadd;
    }

    /* Now check the collision radius */
    if (abs(newpos -> subposx + pos -> radius) > JAD_HALFTILESIZE) {
        leaveidx = dirflag;
    }

    if (abs(newpos -> subposy + pos -> radius) > JAD_HALFTILESIZE) {
        leaveidx = dirflag;
    }

    if (pos -> velz > 0) {
        newpos -> posz += velz;
        newpos -> velz += gravity;
    }

    if (leaveidx >= 0) {
        return dirlookup[leaveidx];
    }
    else {
        return -1;
    }
}