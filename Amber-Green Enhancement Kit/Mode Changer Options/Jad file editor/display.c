/*
     DISPLAY.C

     Display-Code for Maze-Edition.

     To save Memory, only the visible Mesh is calculated every frame.
     The Camera can be moved around the tile in the middle of the "View-Mesh".

     Taken from he players point of view:
        - Always from back to front.

     Size of a Tile is 128 X 128.

     Paul Mueller, January 2000

     Last changes: 23.01.01 / PM
     		   24.01.01 / PM: Stairs
                   28.01.01 / PM: Rotate the walls that have to be drawn
                   06.03.01 / PM: Added experimental doors
                   11.03.01 / PM: Added some example Wall-Textures
                   02.06.01 / PM: Added data format for textures. Additional
                   		  camera mode.
                   13.06.01 / PM: Fixed a bug in displaying textured data
*/


/******************************************************************************
* HEADER FILES							      *
******************************************************************************/
#include "glhead.h"
#include <stdio.h>
#include <math.h>

#include "display.h"
#include "textout.h"
#include "animated.h"
#include "gltexture.h"


/******************************************************************************
* DEFINES								      *
******************************************************************************/

/* Define M_PI for MacOS */
#ifdef _MACOS
	#define	M_PI		3.14159265
#endif

/* For the math */
#define TWO_PI	    2 * M_PI
#define ONE_DEGREE  ((2 * M_PI) / 360)

/* ------------- */
#define DISPLAY_WALLHEIGHT  92.0

/* Internal colors */
#define DISPLAY_MENUCOL1 16
#define DISPLAY_MENUCOL2 17
#define DISPLAY_MENUCOL3 18

/* Internal defines */

/* For the Display description */
#define DISPLAY_TILESIZE    128.0	// Can be divided by 8
#define DISPLAY_VIEWANGLE   59.0

/* For the display texture descriptors */
#define DISPLAY_MAXCMD	16
#define DISPLAY_VERTICEMAX 2048
#define DISPLAY_TEXTUREMAX 16

/******************************************************************************
* TYPEDEFS								      *
******************************************************************************/

typedef struct {
    signed   char x,y,z;	/* The vertice itself  	     */
    unsigned char u,v;		/* The texture coordinates   */
} JADFILEVERTEX;		/* The displayvertex in file */

typedef struct {
    int glcommand;	/* The opengl-command				 */
    int vtxcount; 	/* Number of vertices with texture - coordinates */
} JADVERTEXCMD;

typedef struct {
    JADDISPVERTEX *vertices;	/* Pointer on first vertex of vertice - list */
    int cmdcnt;			/* How many commands			     */
    JADVERTEXCMD vertexcmd[DISPLAY_MAXCMD];
} JADDISPTEX;

/* Wall textures: Each texture must be maximum 256 x 256.     		*/
typedef struct {
    GLTexture  gltex;
    JADDISPTEX jaddisptex[64]; /* Number if displaytexture */
} DISPLAYTEXTURE;

/******************************************************************************
* DATA									      *
******************************************************************************/

/* The data  for the standard display - textures */
static JADDISPVERTEX  DispVertice[DISPLAY_VERTICEMAX];
static JADDISPVERTEX  *DispVertices = &DispVertice[0]; /* "Run"-Pointer into buffer */
static DISPLAYTEXTURE DisplayTex[DISPLAY_TEXTUREMAX];

static int DispScreenX, DispScreenY;
// static GLfloat wallheight = DISPLAY_WALLHEIGHT;

static GLubyte displaycol[][4] = {
    { 0,   0,   0   },  /* 0. BLACK */
    { 0,   0,   170 },  /* 1. BLUE */
    { 0,   170, 0   },  /* 2. GREEN */
    { 0,   170, 170 },  /* 3. CYAN */
    { 170, 0,   0   },  /* 4. RED */
    { 170, 0,   170 },  /* 5. MAGENTA */
    { 170, 170, 0   },  /* 6. BROWN */
    { 170, 170, 170 },  /* 7. LIGHTGREY */
    { 85,  85,  85  },  /* 8. DARKGRAY */
    { 85,  85,  255 },  /* 9. LIGHTBLUE */
    { 85,  255, 85  },  /* 10. LIGHTGREEN */
    { 85,  255, 255 },  /* 11. LIGHTCYAN */
    { 255, 85,  85  },  /* 12. LIGHTRED */
    { 255, 85,  255 },  /* 13. LIGHTMAGENTA */
    { 255, 255, 85  },  /* 14. YELLOW */
    { 255, 255, 255 },  /* 15. WHITE */
    { 0x50, 0xC4, 0xFC }, /* 16. MENUCOL1 */
    { 0x50, 0x88, 0xFC }, /* 17. MENUCOL2 */
    { 0x38, 0x3C, 0xDC }  /* 18. MENUCOL3 */
};


/*
   The mesh is calculated only for the visible view:
   9 x 9 tiles. The players figure is standing on the tile in the midst.
   Because of this, the memory don't has to be malloced, it can be reserved
   fix in the data section

   Depending on the View-Angle, the tiles have to be drawn in a different or-
   der. The idea about is to avoid use of the Z-Buffer...
   Evry 45 Degrees, the tiles have to be drawn left + right to the middle
   or from edge to edge.
   The drawing order is described below on the base of a look at north.
   Directions:
   	NORTH:   0 Degress
        EAST:  270 Degrees
        SOUTH: 180 Degrees
        WEST:   90 Degrees 
*/

char TileDisplayNumbers[8][VIEWMESHCOUNT] = {
    { /* 337.5 .. 22.5, left / right to middle -> Middle to left right  */
       0,  1,  9,  8,  7, 17,  2, 10, 18,
       6, 16, 26,  3, 11, 19, 27,  5, 15,
      25, 35,  4, 12, 14, 20, 24, 28, 34,
      36, 44, 13, 21, 23, 29, 33, 37, 43,
      45, 53, 22, 30, 32, 38, 40, 46, 52,
      54, 62, 31, 39, 41, 47, 51, 55, 61,
      63, 71, 40, 48, 50, 56, 60, 64, 70,
      72, 80, 49, 57, 59, 65, 69, 73, 79,
      58, 66, 68, 74, 78, 67, 75, 77, 76
    },
    { /* 22.5 .. 67.5, Left top to right bottom */
       0,  1,  9,  2, 10, 18,  3, 11, 19,
      27,  4, 12, 20, 28, 36,  5, 13, 21,
      29, 37, 45,  6, 14, 22, 30, 38, 46,
      54,  7, 15, 23, 31, 39, 47, 55, 63,
       8, 16, 24, 32, 40, 48, 56, 64, 72,
      17, 25, 33, 41, 49, 57, 65, 73, 26,
      34, 42, 50, 58, 66, 74, 35, 43, 51,
      59, 67, 75, 44, 52, 60, 68, 76, 53,
      61, 69, 77, 62, 70, 78, 71, 79, 80
    },
    { /* 67.5 .. 112.5, left / right to middle -> Middle to left right  */
      72, 63, 73,  0,  9,  1, 54, 64, 74,
      18, 10,  2, 45, 55, 65, 75, 27, 19,
      11,  3, 36, 46, 28, 56, 20, 66, 12,
      76,  4, 37, 47, 29, 57, 21, 67, 13,
      77,  5, 38, 48, 30, 58, 22, 68, 14,
      78,  6, 39, 49, 31, 59, 23, 69, 15,
      79,  7, 40, 50, 32, 60, 24, 70, 16,
      80,  8, 41, 51, 33, 61, 25, 71, 17,
      42, 52, 34, 62, 26, 43, 53, 35, 44
    },
    { /* 112.5 .. 157.5, Left bottom to right top */
      72, 63, 73, 54, 64, 74, 45, 55, 65,
      75, 36, 46, 56, 66, 76, 27, 37, 47,
      57, 67, 77, 18, 28, 38, 48, 58, 68,
      78,  9, 19, 29, 39, 49, 59, 69, 79,
       0, 10, 20, 30, 40, 50, 60, 70, 80,
       1, 11, 21, 31, 41, 51, 61, 71,  2,
      12, 22, 32, 42, 52, 62,  3, 13, 23,
      33, 43, 53,  4, 14, 24, 34, 44,  5,
      15, 25, 25,  6, 16, 26,  7, 17,  8
    },
    { /* 157.5 .. 202.5, left / right to middle -> Middle to left right  */
      80, 79, 71, 72, 73, 63, 78, 70, 62,
      74, 64, 54, 77, 69, 61, 53, 75, 65,
      55, 45, 76, 68, 66, 60, 56, 52, 46,
      44, 36, 67, 59, 57, 51, 47, 43, 37,
      35, 27, 58, 50, 48, 42, 38, 34, 28,
      26, 18, 49, 41, 39, 33, 29, 25, 19,
      17,  9, 40, 32, 30, 24, 20, 16, 10,
       8,  0, 31, 23, 21, 15, 11,  7,  1,
      22, 14, 12,  6,  2, 13,  5,  3,  4
    },
    { /* 202.5 .. 247.5, Right bottom to left top */
      80, 79, 71, 78, 70, 62, 77, 69, 61,
      53, 76, 68, 60, 52, 44, 75, 67, 59,
      51, 43, 35, 74, 66, 58, 50, 42, 34,
      26, 73, 65, 57, 49, 41, 33, 25, 17,
      72, 64, 56, 48, 40, 32, 24, 16,  8,
      63, 55, 47, 39, 31, 23, 15,  7, 54,
      46, 38, 30, 22, 14,  6, 45, 37, 29,
      21, 13,  5, 36, 28, 20, 12,  4, 27,
      19, 11,  3, 18, 10,  2,  9,  1,  0
    },
    { /* 247.5 .. 292.5, left / right to middle -> Middle to left right  */
       8, 17,  7, 80, 71, 79, 26, 16,  6,
      62, 70, 78, 35, 25, 15,  5, 53, 61,
      69, 78, 44, 34, 52, 24, 60, 14, 68,
       4, 76, 43, 33, 51, 23, 59, 13, 67,
       3, 75, 42, 32, 50, 22, 58, 12, 66,
       2, 74, 41, 31, 49, 21, 57, 11, 65,
       1, 73, 40, 30, 48, 20, 56, 10, 64,
       0, 72, 39, 29, 47, 19, 55,  9, 63,
      38, 28, 46, 18, 54, 37, 27, 45, 36
    },
    { /* 202.5 .. 247.5, Right top to left bottom */
       8, 17,  7, 26, 16,  6, 35, 25, 15,
       5, 44, 34, 24, 14,  4, 53, 43, 33,
      23, 13,  3, 62, 52, 42, 32, 22, 12,
       2, 71, 61, 51, 41, 31, 21, 11,  1,
      80, 70, 60, 50, 40, 30, 20, 10,  0,
      79, 69, 59, 49, 39, 29, 19,  9, 78,
      68, 58, 48, 38, 28, 18, 77, 67, 57,
      47, 37, 27, 76, 66, 56, 46, 36, 75,
      65, 55, 45, 74, 64, 54, 73, 63, 72
    }
};


GLfloat TileDisplayPos[VIEWMESHCOUNT][2] = {
    /* Tile position from 0 .. 80, Line by line */
    /* 0 .. 8 */
    {-576.0,-576.0}, {-576.0,-448.0}, {-576.0,-320.0}, {-576.0,-192.0},
    {-576.0, -64.0}, {-576.0,  64.0}, {-576.0, 192.0} ,{-576.0, 320.0},
    {-576.0, 448.0},
    /*  9 .. 17 */
    {-448.0,-576.0}, {-448.0,-448.0}, {-448.0,-320.0}, {-448.0,-192.0},
    {-448.0, -64.0}, {-448.0,  64.0}, {-448.0, 192.0}, {-448.0, 320.0},
    {-448.0, 448.0},
    /* 18 .. 26 */
    {-320.0,-576.0}, {-320.0,-448.0}, {-320.0,-320.0}, {-320.0,-192.0},
    {-320.0, -64.0}, {-320.0,  64.0}, {-320.0, 192.0}, {-320.0, 320.0},
    {-320.0, 448.0},
    /* 27 ..  35*/
    {-192.0,-576.0}, {-192.0,-448.0}, {-192.0,-320.0}, {-192.0,-192.0},
    {-192.0, -64.0}, {-192.0,  64.0}, {-192.0, 192.0}, {-192.0, 320.0},
    {-192.0, 448.0},
    /* 36 .. 44 */
    {-64.0,-576.0}, {-64.0,-448.0}, {-64.0,-320.0}, {-64.0,-192.0},
    {-64.0, -64.0}, {-64.0,  64.0}, {-64.0, 192.0}, {-64.0, 320.0},
    {-64.0, 448.0},
    /* 45 .. 53 */
    {64.0,-576.0}, {64.0,-448.0}, {64.0,-320.0}, {64.0,-192.0},
    {64.0, -64.0}, {64.0,  64.0}, {64.0, 192.0}, {64.0, 320.0},
    {64.0, 448.0},
    /* 54 .. 62*/
    {192.0,-576.0}, {192.0,-448.0}, {192.0,-320.0}, {192.0,-192.0},
    {192.0, -64.0}, {192.0,  64.0}, {192.0, 192.0}, {192.0, 320.0},
    {192.0, 448.0},
    /* 63 .. 71 */
    {320.0,-576.0}, {320.0,-448.0}, {320.0,-320.0}, {320.0,-192.0},
    {320.0, -64.0}, {320.0,  64.0}, {320.0, 192.0}, {320.0, 320.0},
    {320.0, 448.0},
    /* 72 .. 80 */
    {448.0,-576.0}, {448.0,-448.0}, {448.0,-320.0}, {448.0,-192.0},
    {448.0, -64.0}, {448.0,  64.0}, {448.0, 192.0}, {448.0, 320.0},
    {448.0, 448.0}
};

JADDISPVERTEX TileSides[4][4] = {
    {
        { -64.0,  64.0,   0.0 },
        { -64.0, -64.0,   0.0 },
        { -64.0, -64.0, DISPLAY_WALLHEIGHT },
        { -64.0,  64.0, DISPLAY_WALLHEIGHT }
    }, /* North */
    {
        {  64.0,  64.0,   0.0 },
        { -64.0,  64.0,   0.0 },
        { -64.0,  64.0, DISPLAY_WALLHEIGHT },
        {  64.0,  64.0, DISPLAY_WALLHEIGHT }
    }, /* East */
    {
        {  64.0, -64.0,   0.0 },
        {  64.0,  64.0,   0.0 },
        {  64.0,  64.0, DISPLAY_WALLHEIGHT },
        {  64.0, -64.0, DISPLAY_WALLHEIGHT }
    }, /* South */
    {
        { -64.0, -64.0,   0.0 },
        {  64.0, -64.0,   0.0 },
        {  64.0, -64.0, DISPLAY_WALLHEIGHT },
        { -64.0, -64.0, DISPLAY_WALLHEIGHT }
    }  /* West */
};


JADDISPVERTEX TopTile[4] = {	/* Position at 0.0 */
    {  64.0, -64.0, DISPLAY_WALLHEIGHT },
    {  64.0,  64.0, DISPLAY_WALLHEIGHT },
    { -64.0,  64.0, DISPLAY_WALLHEIGHT },
    { -64.0, -64.0, DISPLAY_WALLHEIGHT },
};

/* Here is the "Display-List" */
DISPLAYTILE    Display_TileList[VIEWMESHCOUNT];


/******************************************************************************
* CODE									      *
******************************************************************************/

/*
 * Name:
 *     NewCameraPos
 * Description:
 *     Calculates depending on camera-angle and camera-distance the
 *     position on the x/y plane.
 * Input:
 *	cam:  Pointer on CAMERA
 *	pos:  Pointer on camera vertex to change
 */
void NewCameraPos(CAMERA *cam, JADVERTEX *pos)
{
    pos -> x = cos(cam -> angle * ONE_DEGREE) * cam -> dist;
    pos -> y = sin(cam -> angle * ONE_DEGREE) * cam -> dist;
}

/*
 * Name:
 *     Display_Init
 * Function:
 *     Sets up the minimum Data for Display
 * Input:
 *     Screenx, ScreenY: Size of Viewport in Pixels
 */
void Display_Init(CAMERA *cam)
{
    int i;

    DispScreenX = cam -> viewwidth;
    DispScreenY = cam -> viewheight;

    glClearColor(0.0, 0.0, 0.0, 0.0);	/* Set Clear-Color */
    glViewport(0, 0, DispScreenX, DispScreenY);	/* Set the Viewport to whole Screen */

    /* now set the backface-culling */
    glEnable(GL_CULL_FACE);
    /* Draw only the front-polygons */
    glPolygonMode(GL_FRONT, GL_FILL);

    /* Enable textures */
    glEnable(GL_TEXTURE_2D);

    /* The data initalization */
    DispVertices = &DispVertice[0];

    /* Reset the texture numbers */
    for (i = 0; i < DISPLAY_TEXTUREMAX; i++) {
    	DisplayTex[i].gltex.textureID = 0;
    }

    Display_SetCameraView(cam);
}


/*
 * Name:
 *     Display_Close
 * Description:
 *     Releases all internal used stuff.
 * Input:
 *     None
 */
void Display_Close(void)
{

}

/*
 * Name:
 *     Display_LoadTexture
 * Description:
 *     Load all textures for the display. Maximum 4 Textures
 * Input:
 *     displaytex:  Pointer on DISPLAYTEX - descriptors
 *     texno:	    Number of texturedescriptor to fill.
 */
void Display_LoadTexture(unsigned char *data, int texno)
{

    int vtxcount, cmdcnt, i, j;
    JADFILEVERTEX  *filevertex;
    DISPLAYTEXTURE *displaytexture;
    JADDISPTEX     *jaddisptex;

    displaytexture = &DisplayTex[texno];
    jaddisptex     = &displaytexture -> jaddisptex[0];

    /* Filename starts at index 10 in file buffer	*/
    GLTexture_Load(&displaytexture -> gltex, (signed char *)&data[10]);

    /* if (! glIsTexture(gltex.textureID))
        return;
    */

    data += 42;	    /* Point on the first data - descriptor */

    while ((data[0] == TEXFILE_CMDCOUNT) && (data[1] != 0)) {

        cmdcnt = data[1];			/* Number of commands */

        jaddisptex -> cmdcnt   = cmdcnt;
        jaddisptex -> vertices = DispVertices;	 /* Start with thisone    */

        data += 2;			/* Point on command flag	*/

        /* Now get the commands with the vertices */
        for (i = 0; i < cmdcnt; i++) {

            vtxcount = data[1];		/* That many vertices... */
            jaddisptex -> vertexcmd[i].vtxcount = vtxcount;

	    switch(data[0]) {
            	case TEXFILE_TRIANGLESTRIP:
                    jaddisptex -> vertexcmd[i].glcommand = GL_TRIANGLE_STRIP;
            	    break;

            	case TEXFILE_TRIANGLEFAN:
                    jaddisptex -> vertexcmd[i].glcommand = GL_TRIANGLE_FAN;
            	    break;

            	case TEXFILE_TRIANGLES:
                    jaddisptex -> vertexcmd[i].glcommand = GL_TRIANGLES;
            	    break;

            	case TEXFILE_QUADS:
		    jaddisptex -> vertexcmd[i].glcommand = GL_QUADS;
            	    break;

                case TEXFILE_QUADSTRIP:
                    jaddisptex -> vertexcmd[i].glcommand = GL_QUAD_STRIP;
                    break;

                case TEXFILE_LINESTRIP:
                    jaddisptex -> vertexcmd[i].glcommand = GL_LINE_STRIP;
                    break;

                case TEXFILE_LINELOOP:
                    jaddisptex -> vertexcmd[i].glcommand = GL_LINE_LOOP;
                    break;
            	default:
            	    /* That should never happen, that's an error */
                   return;
            }

            filevertex = (JADFILEVERTEX  *)&data[2]; /* Point on first vertex */
            for (j = 0; j < vtxcount; j++) {
	        DispVertices -> x = (GLfloat)filevertex -> x;
                DispVertices -> y = (GLfloat)filevertex -> y;
                DispVertices -> z = (GLfloat)filevertex -> z;
                DispVertices -> u = (GLfloat)filevertex -> u / 256.0;
                DispVertices -> v = (GLfloat)filevertex -> v / 256.0;
	        DispVertices++;			/* Forward in global list */
	        filevertex++;
	    }

            data = (unsigned char *)filevertex;

        } /* for (i = 0; i < cmdcnt; i++) */

        jaddisptex++;

    }  /* ((data[0] == TEXFILE_CMDCOUNT) && (data[1] != 0)) */	
}


/*
 * Name:
 *     Display_Begin2DMode
 *  Description:
 *     Sets the screen to an Orthogonal view. It's set up in a way,
 *     that mouse Coordinates == ScreenCoordinates
 */

void Display_Begin2DMode( void )
{
    // Disable texture
    glDisable(GL_TEXTURE_2D);

    // glPushAttrib( GL_ENABLE_BIT | GL_CURRENT_BIT );
    glMatrixMode(GL_MODELVIEW);         /* Select Modelview-Matrix	*/
    glPushMatrix();			/* Save it			*/
    glMatrixMode( GL_PROJECTION );	/* Select The Projection Matrix */
    glPushMatrix();			/* Save it 			*/
    glLoadIdentity();			/* Reset The Projection Matrix  */
    /* Now set the 2D-Screen */
    glOrtho(0.0, DispScreenX, DispScreenY, 0.0, -1.0, 1.0 );   /* Set Up An Ortho Screen */
    // glDisable(GL_DEPTH_TEST);
    // glDisable(GL_LIGHTING);
}

/*
 * Name:
 *     End2DMode
 *  Description:
 *     Goes back to the initiated 3D-Mode
 */
void Display_End2DMode( void )
{
    glMatrixMode( GL_PROJECTION );	/* Select The Projection Matrix */
    glPopMatrix();			/* Back to 3D-Mode		*/
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    //
    glEnable(GL_TEXTURE_2D);
    // glPopAttrib();
    // glEnable(GL_LIGHTING);
    // glEnable(GL_DEPTH_TEST);
    glFlush();
}

/*
 * Name:
 *     SetCameraView
 * Description:
 *     Set the actual view Matrices to the given camera.
 * Input:
 *    cam: Pointer on a CAMERA-Structure
 */

void Display_SetCameraView(CAMERA *cam)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(DISPLAY_VIEWANGLE, (GLdouble)DispScreenX / (GLdouble)DispScreenY, 128.0, 1280.0);
    /* Now move the Camera to the appropriate Point */
    gluLookAt(cam -> pos.x, cam -> pos.y, cam -> pos.z,
              cam -> track.x, cam -> track.y, cam -> track.z,
              0.0, 0.0, 1.0);
}

/*
 * Name:
 *     MoveCamera
 * Description:
 *     Moves the given Camera in the asked matter, by units
 * Input:
 *    cam:   Camera to work with
 *    kind:  Kind of movement
 *    units: Units to move, depending on kind
 *         CAMERA_UP 	   in Game Units
 *	   CAMERA_DOWN:    in Game Units
 *	   CAMERA_LEFT:    in Degrees
 *	   CAMERA_RIGHT:   in Degrees
 *	   CAMERA_ZOOMIN:  in Game Units, along axis Camera -> LookAt
 *	   CAMERA_ZOOMOUT: in Game Units, along axis Camera -> LookAt
 *    mode: 0:    The camera position is rotated around "lookat"
 *	    1:    "Lookat" is rotated around the camera position
 */

void Display_MoveCamera(CAMERA *cam, int kind, int units)
{
    JADVERTEX *pos;

    if (! cam -> mode) {
        pos = &cam -> pos;
    }
    else {
        pos = &cam -> track;
    }

    switch(kind) {
    	case CAMERA_UP:	/* Move Eye up Z-Axis */
            pos -> z += units;
            break;

	case CAMERA_DOWN:	/* Move Eye down Z-Axis */
            pos -> z -= units;
            break;

	case CAMERA_RIGHT:	/* Turn left around "LookAt" */
            cam -> angle += units;
            if (cam -> angle > 360.0)
                cam -> angle -= 360.0;
            NewCameraPos(cam, pos);
            break;

	case CAMERA_LEFT:	/* TUrn right around "LookAt" */
            cam -> angle -= units;
            if (cam -> angle < 0)
            	cam -> angle += 360.0;
            NewCameraPos(cam, pos);
            break;

	case CAMERA_ZOOMIN:
            cam -> dist -= units;
            NewCameraPos(cam, pos);
            break;

	case CAMERA_ZOOMOUT:
            cam -> dist += units;
            NewCameraPos(cam, pos);
            break;

        case CAMERA_SWITCHMODE:
            cam -> mode ^= 1;
    }

    Display_SetCameraView(cam);			/* Set new Projection */
}


	/******* Now the drawing itself ********/


/*
 *  Name:
 *	Display_Draw3DAxes
 *  Description:
 *	Draw the three Display - Axes, starting from 0,0,0
 *	X: Red
 *	Y: Green
 *	Z: Blue
 */

void Display_Draw3DAxes(void)
{
    glBegin(GL_LINES);
    	/* Draw X-Axis */
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(128.0, 0.0, 0.0);
        /* Draw Y-Axis */
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 128.0, 0.0 );
        /* Draw Z-Axis */
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 128.0);
    glEnd();
}

/*
 * Name:
 *     Display_DrawWallTexture
 * Description:
 *     Draws a wall texture with the given number in the given direction.
 * Input:
 *     wallno: Number of wall
 *     angle:   Angle to turn it)
 */
void Display_DrawWallTexture(int wallno, GLfloat angle)
{
    int cmdcnt, vertexcnt;
    JADDISPVERTEX *vertices;
    JADVERTEXCMD  *vertexcmd;

    glBindTexture (GL_TEXTURE_2D, DisplayTex[DISPLAY_TEXWALLS].gltex.textureID); 

    vertices  = DisplayTex[DISPLAY_TEXWALLS].jaddisptex[wallno].vertices;
    cmdcnt    = DisplayTex[DISPLAY_TEXWALLS].jaddisptex[wallno].cmdcnt;
    vertexcmd = &DisplayTex[DISPLAY_TEXWALLS].jaddisptex[wallno].vertexcmd[0];

    glPushMatrix();			/* Save Matrix			*/
    glRotatef(angle, 0.0, 0.0, 1.0);    /* Rotate around z-Axis 	*/

    while (cmdcnt > 0) {

        glBegin(vertexcmd -> glcommand);

        vertexcnt = vertexcmd -> vtxcount;

        while (vertexcnt > 0) {
            glTexCoord2fv((GLfloat *)&vertices -> u);
            glVertex3fv((GLfloat *)vertices);

            vertices++;
            vertexcnt--;
        }

        glEnd();

        cmdcnt--;
        vertexcmd++;			/* Next command */

    }

    glPopMatrix();
}



/*
 *  Name:
 *	DrawWall
 *  Description:
 *	Draws "walls" at 4 Sides of a Tile and all other kinds of wall
 * Input:
 *      vtile: Pointer on Displaytile to draw
 */
void DrawWall(DISPLAYTILE *vtile)
{
    int i, wallno;
    GLfloat angle;

    angle = 360.0;		/* The Angle has to be adjusted from */
    				/* clockwise to counterclockwise     */


    glColor3ubv(displaycol[DISPLAY_LIGHTBLUE]);

    /* First the sides */
    for (i = 0; i < 4; i++) {

    	    wallno = vtile -> side[i].wall;

            /* Each side */
            switch (wallno) {
                case -1: 	/* Hidden wall */
                    break;
                case 0:	// bottom
                    /* First check, if there is a door or a stair */
                    if (vtile -> direction == -1) {
                    	/* It's definitely a bottom tile */
		        glColor3ubv(displaycol[DISPLAY_LIGHTGREY]);
		        Display_DrawWallTexture(0, 0.0);
	                return;
                    }
                    break;
            	case 1:
            	case 2:
                    Display_DrawWallTexture(wallno, angle);
            	    break;
                case 3:  // Draw the door, initially closed
                    // First draw the bottom
                    glColor3ubv(displaycol[DISPLAY_LIGHTGREY]);
                    Display_DrawWallTexture(0, 0.0);

                    // Then draw the door itself
                    Animated_Doors(vtile -> doortype, vtile -> doorpos, vtile -> doormaxcicle,
                    						  DISPLAY_WALLHEIGHT, angle);
                    return;

            	case 4:    		/* Stair up		*/
            	case 5:    		/* Stair down 		*/
            	    Display_DrawWallTexture(wallno, angle);
                    return;

                default:
                    glColor3ubv(displaycol[wallno & 0x0F]);
                    Display_DrawWallTexture(wallno, angle);
            	    break;
            } /* switch (wallno) */

            /* Turn to the next side to be drawn	        */
            angle -= 90.0;
    } /* for i  */


    glBegin(GL_TRIANGLE_STRIP);
    /* Draw top  */
    	if (vtile -> flags & TILE_TOP) {
	    glColor3ubv(displaycol[DISPLAY_DARKGRAY]);
            glVertex3fv(&TopTile[0].x);
            glVertex3fv(&TopTile[1].x);
            glVertex3fv(&TopTile[3].x);
            glVertex3fv(&TopTile[2].x);
	} /* if TILE_TOP */
    glEnd();	/* GL_QUADS */
}

/*
 * Name:
 *     DrawEditRect
 * Description:
 *     Draws the Edit-Rectangle.
 *  Input:
 *     editside: On which side (0.. 3) to draw the edit-rectangle.
 *		 if -1, then draw it on top.
 */

void DrawEditRect(int side)
{
    int i;
    JADDISPVERTEX *ptile;

    glColor3f(1.0, 1.0, 0.0); 	/* yellow */

    if (side == -1) {
    	/* Draw it on top */
        ptile = &TopTile[0];
    }
    else {
	ptile = TileSides[side];
    }

    glBegin(GL_LINE_LOOP);
    	for (i = 0; i < 4; i++) {
	    glVertex3fv((GLfloat *)ptile);
	}
    glEnd();
}

/*
 * Name:
 *     Display_3DView
 * Description:
 *    Draws a simple Mesh in the 3D- View. The three Axes are drawn on the
 *    tile in the middle.
 * Input:
 *    disptiles: The list of tiles, that have to be displayed
 *    cam:	 The Camera to use for Display
 */

void Display_3DView(DISPLAYTILE *disptiles, CAMERA *cam)
{
    int viewdir;
    char *ptileidxno;
    char tileidx;
    int  meshcount;

     /* Set pointer on Tile-Numbers depending on Camera-Angle */
    viewdir = (int)cam -> angle + 22.5;
    viewdir /= 45;
    viewdir &= 7;		/* Maximum 7 */


    ptileidxno = &TileDisplayNumbers[viewdir][0];

    /* clear all pixels */
    glClear(GL_COLOR_BUFFER_BIT);


    for (meshcount = 0; meshcount < VIEWMESHCOUNT ; meshcount++) {

        tileidx = (*ptileidxno);	/* Get position where tile to draw */

	glMatrixMode(GL_MODELVIEW);	/* Select Modelview-Matrix	*/
	glPushMatrix();			/* Save it			*/

        /* Move the drawing rectangle at the right position		*/
	glTranslatef(TileDisplayPos[tileidx][0], TileDisplayPos[tileidx][1], 0.0);

        /* ---- Display-Type Walls ---- */
        DrawWall(&disptiles[tileidx]);

        /* ------- Display-Type "Animated" ----- */

        switch(disptiles[tileidx].side[0].walltype) {
            case 44:	/* Display the "Teleport"  */
                Animated_Teleport(cam -> angle);
                break;
            case 121:
                Animated_SpellStars(cam -> angle);
                break;
        }

        /* Now draw the Edit - Cursor if here */
        if (disptiles[tileidx].flags & TILE_ISEDIT)
            DrawEditRect(disptiles[tileidx].direction);


        /* ------- End Display ---------- */
        glMatrixMode(GL_MODELVIEW); /* Select Modelview-Matrix	*/
        glPopMatrix();		    /* Get it back		*/

        /* Point on next number of tile to be drawn */
        ptileidxno++;
    }

    glFlush();
}


/******************************************************************************
* SOME SIMPLE 2D-DRAWING-ROUTINES					      *
******************************************************************************/

/*
 * Name:
 *     Display_DrawShadowedRect:
 * Description:
 *     Draws a simple filled 2D-Rectangle, with a "Shadow"
 */
void Display_DrawShadowedRect(JADRECT *rect)
{
    int x, y, x2, y2;

    x = rect -> x;
    y = rect -> y;
    x2 = x + rect -> width - 1;
    y2 = y + rect -> height - 1;

    glBegin(GL_QUADS);	/* First the Background */
        glColor3ubv(displaycol[DISPLAY_MENUCOL1]); /* 0x50, 0xC4, 0xFC); */
        glVertex2i(x, y2);
        glVertex2i(x2, y2);
        glVertex2i(x2, y);
	glVertex2i(x, y);
    glEnd();

    glBegin(GL_LINES);
        /* Top Edge Line */
        glColor3ubv(displaycol[DISPLAY_MENUCOL2]); /* (0x50, 0x88, 0xFC); */
        glVertex2i(x, y);
        glVertex2i(x2, y);
        glVertex2i(x2, y);
        glVertex2i(x2, y2);
	/* Bottom Edge Line */
	glColor3ubv(displaycol[DISPLAY_MENUCOL3]); /* (0x38, 0x3C, 0xDC);	*/
        glVertex2i(x2, y2);
        glVertex2i(x, y2);
        glVertex2i(x, y2);
        glVertex2i(x, y);

        x  += 1;
	y  += 1;
	x2 -= 1;
        y2 -= 1;

        /* Top Edge Line */
	glColor3ubv(displaycol[DISPLAY_MENUCOL2]); /* (0x50, 0x88, 0xFC); */
        glVertex2i(x, y);
        glVertex2i(x2, y);
        glVertex2i(x2, y);
        glVertex2i(x2, y2);
	/* Bottom Edge Line */
        glColor3ubv(displaycol[DISPLAY_MENUCOL3]); /* (0x38, 0x3C, 0xDC) */
        glVertex2i(x2, y2);
        glVertex2i(x, y2);
        glVertex2i(x, y2);
        glVertex2i(x, y);
    glEnd();
}

/*
 * Name:
 *     Display_ShadowedPrompt
 * Description:
 *     Like shadowed rect, but with an inner black rectangle
 *
 */
void Display_ShadowedPrompt(JADRECT *rect)
{
    int x, y, x2, y2;

    x = rect -> x;
    y = rect -> y;
    x2 = rect -> x + rect -> width - 1;
    y2 = rect -> y + rect -> height - 1;

    glBegin(GL_LINES);
        /* Top Edge Line */
        glColor3ubv(displaycol[DISPLAY_MENUCOL3]);	/* (0x38, 0x3C, 0xDC); */
        glVertex2i(x, y);
        glVertex2i(x2, y);
        glVertex2i(x2, y);
        glVertex2i(x2, y2);
	/* Bottom Edge Line */
	glColor3ubv(displaycol[DISPLAY_MENUCOL2]);	/* (0x50, 0x88, 0xFC);	*/
        glVertex2i(x2, y2);
        glVertex2i(x, y2);
        glVertex2i(x, y2);
        glVertex2i(x, y);

        x  += 1;
	y  += 1;
	x2 -= 1;
        y2 -= 1;

        /* Top Edge Line */
	glColor3ubv(displaycol[DISPLAY_MENUCOL3]);	/* (0x38, 0x3C, 0xDC); */
        glVertex2i(x, y);
        glVertex2i(x2, y);
        glVertex2i(x2, y);
        glVertex2i(x2, y2);
	/* Bottom Edge Line */
        glColor3ubv(displaycol[DISPLAY_MENUCOL2]);	/* (0x50, 0x88, 0xFC); */
        glVertex2i(x2, y2);
        glVertex2i(x, y2);
        glVertex2i(x, y2);
        glVertex2i(x, y);
    glEnd();

    x  += 1;
    y  += 1;
    x2 -= 1;
    y2 -= 1;
    /* ---- Last Now draw the inner background */
    glBegin(GL_QUADS);
        glColor3ubv(displaycol[DISPLAY_BLACK]);
        glVertex2i(x, y2);
        glVertex2i(x2, y2);
        glVertex2i(x2, y);
	glVertex2i(x, y);
    glEnd();
}


/*
 * Name:
 *     Display_Arrow
 * Descripiton:
 *     Draws a black filled Triangle-Arrow, 12 x 12
 * Input:
 *    x,y: Where to draw the Arrow
 *    dir: Direction  (0.. 3)
 */

void Display_Arrow(int x, int y, int dir)
{
    static GLint ArrowPoints[4][6] = {
        { 6, 0+1, 0+1,12-1, 12-1,12-1},  {12-1,  6, 0+1,0+1,  0+1,12-1},
        { 6,12-1, 12-1,0+1,  0+1, 0+1},  {12-1,0+1, 0+1,  6, 12-1,12-1}
    };

    dir &= 3;		/* To be save */

    glColor3ubv(displaycol[DISPLAY_BLACK]);
    glBegin(GL_TRIANGLES);
        glVertex2i(ArrowPoints[dir][0] + x, ArrowPoints[dir][0+1] + y);
        glVertex2i(ArrowPoints[dir][2] + x, ArrowPoints[dir][2+1] + y);
        glVertex2i(ArrowPoints[dir][4] + x, ArrowPoints[dir][4+1] + y);
    glEnd();
}


/*
 * Name:
 *     Display_Rectangle
 * Descripiton:
 *     Draws a rectangle
 * Input:
 *    x,y:           Where to draw the rectangle
 *    width, height: Size of rectangle
 *    colorno:	     Number of dispaly color
 *    filled:	     1: yes 0: No
 */
void Display_Rectangle(JADRECT *rect, int colorno, int filled)
{
    /* Color first */
    glColor3ubv(displaycol[colorno]);

    if (filled)
    	glBegin(GL_QUADS);
    else
        glBegin(GL_LINE_LOOP);

        glVertex2i(rect -> x, rect -> y);
        glVertex2i(rect -> x, rect -> y + rect -> height -1);
        glVertex2i(rect -> x + rect -> width - 1, rect -> y + rect -> height -1);
        glVertex2i(rect -> x + rect -> width - 1, rect -> y);
    glEnd();
}
