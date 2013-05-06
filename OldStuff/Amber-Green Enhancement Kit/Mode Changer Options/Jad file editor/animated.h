/*
    ANIMATED.H
    Ex "Speceff"
*/

#ifndef ANIMATED_H
#define ANIMATED_H

/******************************************************************************
* CODE									      *
******************************************************************************/

    int  Animated_Init(void);
    int  Animated_Shutdown(void);
    void Animated_Clock(int tickspassed);	/* in 1000th of a second */
    void Animated_Teleport(int angle);
    void Animated_SpellStars(int angle);
    void Animated_Doors(int type, int position, int maxsteps, float height, float angle);

#endif   /* ANIMATED_H */
