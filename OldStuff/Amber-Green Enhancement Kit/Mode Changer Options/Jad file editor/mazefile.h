/*
    MAZEFILE.H
    Header for the code in MAZEFILE.C
*/

#ifndef MAZEFILE_H
#define MAZEFILE_H

/******************************************************************************
* CODE									      *
******************************************************************************/

    int MazeFile_OpenWrite(char *fname, char *fend);
    int MazeFile_Write(int size, void *data);
    int MazeFile_Close(void);

    int MazeFile_OpenRead(char *fname, char *fend);
    int MazeFile_Read(int size, void *data);

    int MazeFile_LoadInitialData(void);

#endif /* MAZEFILE_H */
