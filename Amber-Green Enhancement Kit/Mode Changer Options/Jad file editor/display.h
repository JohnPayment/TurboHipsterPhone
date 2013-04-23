/*
     DISPLAY.H
     Is it assumed, that the caller always starts with "SetCameraView".
     The calls to "Begin2DMode" and "End3DMode" are saving the settings
     of the active 3D-View.
     The 2D-View is set up in a way, that MouseCoordinates == ScreenCoordinates.

     Paul Mueller, January 2001
     Last change: 28.01.01 / MP
*/

#ifndef DISPLAY_H
#define DISPLAY_H


/* Make sure that _MACOS is defined in MetroWerks compilers */
#if defined( __MWERKS__ ) && defined( macintosh )
	#define	_MACOS
#endif


/* Headers required for MacOS compilation */
#ifdef _MACOS
	#include <string.h>	/* this provides access to memcpy, etc. */
#endif

#include "jadglob.h"

/******************************************************************************
* DEFINES								      *
******************************************************************************/

/* The standard numbers of textures */
#define DISPLAY_TEXWALLS   0
#define DISPLAY_TEXSTDDEC  1
#define DISPLAY_TEXICON1   2
#define DISPLAY_TEXICON2   3
#define DISPLAY_TEXITEMS   4
#define DISPLAY_TEXITEML   5
#define DISPLAY_TEXDOORS   6
#define DISPLAY_TEXFACES   7
#define DISPLAY_TEXWALLDEC 8



/* For the kinds of "MoveCamera" */
#define CAMERA_UP 	  1
#define CAMERA_DOWN       2
#define CAMERA_LEFT	  3
#define CAMERA_RIGHT	  4
#define CAMERA_ZOOMIN     5
#define CAMERA_ZOOMOUT    6
#define CAMERA_SWITCHMODE 7

/* The viewmesh */
#define VIEWMESHSIZE  9		// The visible Width/Height
#define HALFVIEWMESH  4		// Halfsize for visible rectangle

#define VIEWMESHCOUNT (VIEWMESHSIZE * VIEWMESHSIZE)

/* -- Tile-Flags --- */
#define TILE_TOP        0x01	   // 1: The top of the wall has to be drawn
				   //    even if there is no visible wallside
				   // 0: Draw bottom tile
#define TILE_BOTTOM     0x02	   // Is a bottom tile
#define TILE_DIRECTION	0x04	   // Is a tile with direction 
#define TILE_HASMONSTER 0x100	   // There's a monster on the tile
#define TILE_HASTHROWN	0x200	   // There's a thrown item/spell on the tile
#define TILE_HASITEM	0x400      // There's are items on the tile
#define TILE_ISEDIT	0x80000000 // This tile owes the edit-rectangle

#define TILE_WALLTYPES	0xFF	   // Is a kind of solid wall

/* --- Arrows --- */
#define ARROW_UP     0
#define ARROW_RIGHT  1
#define ARROW_DOWN   2
#define ARROW_LEFT   3

/* ------ Standard - Colors ----- */
#define DISPLAY_BLACK 	 0
#define DISPLAY_BLUE	 1
#define DISPLAY_GREEN 	 2
#define DISPLAY_CYAN 	 3
#define DISPLAY_RED 	 4
#define DISPLAY_MAGENTA       5
#define DISPLAY_BROWN 	      6
#define DISPLAY_LIGHTGREY     7
#define DISPLAY_DARKGRAY      8
#define DISPLAY_LIGHTBLUE     9
#define DISPLAY_LIGHTGREEN   10
#define DISPLAY_LIGHTCYAN    11
#define DISPLAY_LIGHTRED     12
#define DISPLAY_LIGHTMAGENTA 13
#define DISPLAY_YELLOW 	     14
#define DISPLAY_WHITE 	     15

/* Wall-Texture descriptor flags */

#define TEXFILE_CMDCOUNT      0xFF
#define TEXFILE_TRIANGLESTRIP 0xFE
#define TEXFILE_TRIANGLEFAN   0xFD
#define TEXFILE_TRIANGLES     0xFC
#define TEXFILE_QUADS	      0xFB
#define TEXFILE_QUADSTRIP     0xFA
#define TEXFILE_LINESTRIP     0xF9
#define TEXFILE_LINELOOP      0xF8

/******************************************************************************
* TYPEDEFS								      *
******************************************************************************/

typedef struct {
    float x,
          y,
          z;
} JADVERTEX;

typedef struct {
    float x, y, z;		/* The vertice itself  	     	 */
    float u, v;                 /* The texture coordinates   	 */
} JADDISPVERTEX;                /* The displayvertex for display */

typedef struct {
    JADVERTEX pos;  		// Position of the Camera
    JADVERTEX track; 		// Where the Camera looks at
    				// Up is always Y
    float    angle;		// Between 0 and 360 degrees round "LookAt"
    float    dist;		// Distance between eye an "LookAt"
    float    zoom;		// 1.0 sets a normal zoom window
    /* Additional: Viewport for split-screen	*/
    float    viewleft, viewtop,
             viewwidth, viewheight;
    int      mode;		/* Mode: 0: Turn camera around lookat */
    				/* Mode: 1: Turn lookat around camera */ 
} CAMERA;

typedef struct {
    unsigned char walltype;	/* Type of tile 0: Bottom, > 0 , Wall  	     */
    				/* 1 .. 2:  Flat Walls with texture of this  */
                                /*          number.			     */
				/* 3 .. 6:   Textures for Doors Typ 1 - 4    */
                                /* 7 .. 10:  Reserved for special doors      */
                                /* 12 .. 16: Flat Walls with texture of this */
                                /*           number.			     */
                                /* 17 .. 63:  Index in List of Wall- 	     */
                                /*            Descriptors for 3 Dimenions,   */
                                /*            using Flat - Wall Textures     */
                                /* 64 .. 119: Index in List of Wall- 	     */
                                /*            Descriptors for 3 Dimensions,  */
                                /*            using "Decorative" Wall	     */
                                /* 	      Textures     		     */
    char wall;                  /* Number of wall to display		     */	 
    char dec;		        /* Number of "Decoration" 		     */
    				/* If wall 3 .. 6: Type of Door (1 .. 4)     */
    				/* 1 .. 16: Flat Bottom with texture of this */
                                /*          number.			     */
    				/* 17 .. 63:  Index in List of Bottom-	     */
                                /* 	      Descriptors for 3 Dimensions,  */
                                /*	      using Flat - Bottom Textures.  */
                                /* 64 .. 119: Index in List of Wall- 	     */
                                /*            Descriptors for 3 Dimensions,  */
                                /*            using "Decorative" Wall	     */
                                /* 	      Textures     		     */
    				/* 120..127: Specials			     */
                                /* 120: "Teleport" / 121: "Magic"	     */
                                /* 126: Selected rectangle in view	     */
     char subtype;		/* Type of door 0: 1 without knob and so on  */ 
} DISPLAYTILESIDE;

typedef struct {
    int flags;			/* Flags: Info about what to display 	     */
    int number;			/* Number of tile, for visibility tests	     */
    /* Tile-Description */
    DISPLAYTILESIDE side[4];    /* North, East, South, West 		     */
    /* Edit-Info */
    int direction;		/* 0..3 or -1 for none if TILE_ISEDIT	     */
    int doortype,		/* Display-Type of door, if any.	     */	
        doorpos,		/* Position-Index of door, if any.	     */
        doormaxcicle;		/* The maximum cicle of the door	     */	
    /* Thrown things */
} DISPLAYTILE;		    	/* Makes up the Display-List		     */

				/* Texture belonging to display 	     */

/******************************************************************************
* DATA									      *
******************************************************************************/

extern DISPLAYTILE Display_TileList[VIEWMESHCOUNT];

/******************************************************************************
* CODE									      *
******************************************************************************/

void Display_Init(CAMERA *cam);
void Display_Close(void);
void Display_LoadTexture(unsigned char *data, int textureno);
void Display_Draw3DAxes(void);
void Display_SetCameraView(CAMERA *cam);
void Display_MoveCamera(CAMERA *cam, int kind, int units);
void Display_3DView(DISPLAYTILE *tiles, CAMERA *cam);
void Display_Begin2DMode(void);
void Display_End2DMode(void);

/* 2D-Drawing Standard-Routines */
void Display_DrawShadowedRect(JADRECT *rect);
void Display_ShadowedPrompt(JADRECT *rect);
void Display_Arrow(int x, int y, int dir);
void Display_Rectangle(JADRECT *rect, int colorno, int filled);

#endif /* DISPLAY_H */
