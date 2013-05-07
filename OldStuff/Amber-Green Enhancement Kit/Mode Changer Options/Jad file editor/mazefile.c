/*
    MAZEFILE.C
    All the stuff to load and save files for the maze editor.

    Last changes: May - 30, 2001 / PM
*/

#include <stdio.h>

#include <string.h>	/* strcat */
#include "mazefile.h"
#include "display.h"

/******************************************************************************
* DEFINES								      *
******************************************************************************/

/******************************************************************************
* TYPEDEFS								      *
******************************************************************************/

/******************************************************************************
* DATA									      *
******************************************************************************/

FILE *mazefile = 0;
static char workdirectory[512] = "";
// static char anydata[32000]; 		/* Temporary data buffer */

/* --- Example data for loading the walls */
static unsigned char greywall[] = {
    /* Header */
    'J', 'A', 'D',	/* Any file for jad starts with this three chars */
    'T', 'X', 'D',	/* "TXD"	-> It's a texture description.	 */
    0x00, 0x0B,  	/* -> Two Bytes for JAD-Version: Hi version number / Lo version number */
    0x00, 0x00,		/*   -> File size (lo / hi byte), not used here  */
    /* 32 Bytes   Name of texture file, including File-Ending, actually */
    /* only "BMP"-Files are supported.				    */
    'G', 'R', 'E', 'Y', '.', 'B', 'M', 'P',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* The description itself, start with the bottom */
    TEXFILE_CMDCOUNT,   /* Start of a description block.  */
      1,		/* 1 Set of vertices: Count of commands */
    TEXFILE_TRIANGLEFAN,/* 4 Fold, start Middle  -> East  -> counterclockwise */
       10,
       0,   0, 0,  0, 159,
       0,  64, 0, 31, 159,
     192,  64, 0, 31, 128,
     192,   0, 0,  0, 128,
     192, 192, 0, 31, 128,
       0, 192, 0, 31, 159,
      64, 192, 0, 31, 128,
      64,   0, 0,  0, 128,
      64,  64, 0, 31, 128,
       0,  64, 0, 31, 159,
     /* Next descriptor: Wall Number one */
     TEXFILE_CMDCOUNT,     	/* 1 GL-Command */
        1,
     TEXFILE_TRIANGLEFAN,       /* It's a triangle fan	*/
        4,			/* with 4 vertices	*/
        192,  64,  0,  0, 63,
        192, 192,  0, 63, 63,
        192, 192, 92, 63, 17,
        192,  64, 92,  0, 17,
        /* Next descriptor: Wall Number two */
     TEXFILE_CMDCOUNT,
        1,
     TEXFILE_TRIANGLEFAN,
        4,
        192,  64,  0,  64, 63,
        192, 192,  0, 127, 63,
        192, 192, 92, 127, 17,
        192,  64, 92,  64, 17,
        /* Next descriptor: Doorwall - descriptor */
     TEXFILE_CMDCOUNT,
        1,
     TEXFILE_TRIANGLEFAN,
        4,
        192,  64,  0,  64, 63,
        192, 192,  0, 127, 63,
        192, 192, 92, 127, 17,
        192,  64, 92,  64, 17,
     /* ---------- Stair up ------------- */
     TEXFILE_CMDCOUNT,
       10,
      TEXFILE_TRIANGLEFAN, 	/* Sidewalls	*/
        4,
        192,  64, 164,  0, 63,
         64,  64, 164, 63, 63,
         64,  64,   0, 63, 17,
        192,  64,   0,  0, 17,
     TEXFILE_TRIANGLEFAN,
        4,
         64, 192,   0,  64, 63,
        192, 192,   0, 127, 63,
        192, 192,  92, 127, 17,
         64, 192,  92,  64, 17,
     TEXFILE_TRIANGLESTRIP,
        6,
        64, 192,   0,  0, 96,
        64,  64,   0, 63, 96,
        48, 192,   0,  0, 81,
        48,  64,   0, 63, 81,
        48, 192,  12,  0, 92,
        48,  64,  12, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,			/* Stair up	*/
        48, 192,  12,  0, 96,
        48,  64,  12, 63, 96,
        32, 192,  12,  0, 81,
        32,  64,  12, 63, 81,
        32, 192,  24,  0, 92,
        32,  64,  24, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,			/* Stair up	*/
        32, 192,  24,  0, 96,
        32,  64,  24, 63, 96,
        16, 192,  24,  0, 81,
        16,  64,  24, 63, 81,
        16, 192,  36,  0, 92,
        16,  64,  36, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,			/* Stair up	*/
        16, 192,  36,  0, 96,
        16,  64,  36, 63, 96,
         0, 192,  36,  0, 81,
         0,  64,  36, 63, 81,
         0, 192,  48,  0, 92,
         0,  64,  48, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,			/* Stair up	*/
         0, 192,  48,  0, 96,
         0,  64,  48, 63, 96,
       240, 192,  48,  0, 81,
       240,  64,  48, 63, 81,
       240, 192,  59,  0, 92,
       240,  64,  59, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,			/* Stair up	*/
       240, 192,  59,  0, 96,
       240,  64,  59, 63, 96,
       224, 192,  59,  0, 81,
       224,  64,  59, 63, 81,
       224, 192,  70,  0, 92,
       224,  64,  70, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,			/* Stair up	*/
       224, 192,  70,  0, 96,
       224,  64,  70, 63, 96,
       208, 192,  70,  0, 81,
       208,  64,  70, 63, 81,
       208, 192,  81,  0, 92,
       208,  64,  81, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,			/* Stair up	*/
       208, 192,  81,  0, 96,
       208,  64,  81, 63, 96,
       192, 192,  81,  0, 81,
       192,  64,  81, 63, 81,
       192, 192,  92,  0, 92,
       192,  64,  92, 63, 92,
     /* --------- Stair down ----------- */
     TEXFILE_CMDCOUNT,
        10,
     TEXFILE_TRIANGLEFAN, 	/* Sidewalls	*/
        4,
        192,  64, 164,  0, 63,
         64,  64, 164, 63, 63,
         64,  64,   0, 63, 17,
        192,  64,   0,  0, 17,
     TEXFILE_TRIANGLEFAN,
        4,
         64, 192, 164,  64, 63,
        192, 192, 164, 127, 63,
        192, 192,   0, 127, 17,
         64, 192,   0,  64, 17,
     TEXFILE_TRIANGLESTRIP,
        6,
        64, 192, 164,  0, 96,
        64,  64, 164, 63, 96,
        48, 192, 164,  0, 81,
        48,  64, 164, 63, 81,
        48, 192, 175,  0, 92,
        48,  64, 175, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,
        48, 192, 175,  0, 96,
        48,  64, 175, 63, 96,
        32, 192, 175,  0, 81,
        32,  64, 175, 63, 81,
        32, 192, 186,  0, 92,
        32,  64, 186, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,
        32, 192, 186,  0, 96,
        32,  64, 186, 63, 96,
        16, 192, 186,  0, 81,
        16,  64, 186, 63, 81,
        16, 192, 197,  0, 92,
        16,  64, 197, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,
        16, 192, 197,  0, 96,
        16,  64, 197, 63, 96,
         0, 192, 197,  0, 81,
         0,  64, 197, 63, 81,
         0, 192, 208,  0, 92,
         0,  64, 208, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,
         0, 192, 208,  0, 96,
         0,  64, 208, 63, 96,
       240, 192, 208,  0, 81,
       240,  64, 208, 63, 81,
       240, 192, 220,  0, 92,
       240,  64, 220, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,
       240, 192, 220,  0, 96,
       240,  64, 220, 63, 96,
       224, 192, 220,  0, 81,
       224,  64, 220, 63, 81,
       224, 192, 232,  0, 92,
       224,  64, 232, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,
       224, 192, 232,  0, 96,
       224,  64, 232, 63, 96,
       208, 192, 232,  0, 81,
       208,  64, 232, 63, 81,
       208, 192, 244,  0, 92,
       208,  64, 244, 63, 92,
     TEXFILE_TRIANGLESTRIP,
        6,
       208, 192, 244,  0, 96,
       208,  64, 244, 63, 96,
       192, 192, 244,  0, 81,
       192,  64, 244, 63, 81,
       192, 192,   0,  0, 92,
       192,  64,   0, 63, 92,
     TEXFILE_CMDCOUNT,          /* --> End sign				*/
     0x00	 		/* --> Zero commands means end of file	*/
};


/******************************************************************************
* CODE									      *
******************************************************************************/

/*
 * Name:
 *     MazeFile_Filename
 * Description:
 *    Extends the given filename with the path to the work-directory.
 *    Returns the filename with path.
 * Input:
 *    filename: Filename
 * Output:
 *    Extended filename
 */
char *MazeFile_Filename(char *filename)
{
    static char extfilename[640];

    extfilename[0] = 0;

    if (filename) {
        strcpy(extfilename, workdirectory);
        strcat(extfilename, filename);
    }

    return extfilename;
}


/*
 * Name:
 *     MazeFile_OpenWrite
 * Description:
 *     Opens the file with "fname". Opens the file in the actual directory
 *     for writing. Starts the file with "fsign"
 *  Input:
 *     fname: Name of the file to open
 */
int MazeFile_OpenWrite(char *fname, char *fend)
{
    char filename[512];

    sprintf(filename,"%s.%s", fname, fend);

    mazefile = fopen(MazeFile_Filename(filename),"wb");

    if (mazefile)
            return 1;

    return 0;
}


/*
 * Name:
 *     MazeFile_Write
 * Description:
 *     Writes the given data into the opened file.
 * Input:
 *     DataID:   Four chars as blockid
 *     size: size of data to write
 *     data: Pointer on the Mazedata
 */
int MazeFile_Write(int size, void *data)
{
    /* Write the size of data */
    if (fwrite(data, size, 1, mazefile) != 1)
        return 0;

    return 1;
}


/*
 * Name:
 *     MazeFile_Close
 * Description:
 */

int MazeFile_Close(void)
{
    if (fclose(mazefile) == 0)
    	return 1;
    else
    	return 0;
}

/*
 * Name:
 *     MazeFile_OpenRead
 * Description:
 *     Opens the file with "fname". Opens the file in the actual directory
 *     for reading.
 *  Input:
 *     fname: Name of the file to open
 *
 */
int MazeFile_OpenRead(char *fname, char *fend)
{
    char filename[1024];

    sprintf(filename,"%s.%s", fname, fend);

    mazefile = fopen(MazeFile_Filename(filename),"rb");

    if (mazefile) 
        return 1;

    return 0;
}

/*
 * Name:
 *     MazeFile_Read
 * Description:
 *     Reads the size of data form the opened file
 * Input:
 *     size: size of data to read
 *     data: Pointer on the buffer to read the data to.
 */
int MazeFile_Read(int size, void *data)
{
    /* Read the size of data */
    if (fread(data, size, 1, mazefile) != 1)
        return 0;

    return 1;
}


/*
 * Name:
 *     MazeFile_LoadInitialData
 * Description:
 *     Loads the initial data for the maze
 * Input:
 *     None
 */
int MazeFile_LoadInitialData(void)
{
    Display_LoadTexture(greywall, DISPLAY_TEXWALLS);
    return 1;
}


