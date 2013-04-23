/*
   JADEDIT.C
   A simple Main-Function to separate the calls to SDL from the rest of the
   code.

   January 2001, Paul Mueller
   Last change: May 13, 2001 / MP
   		May 17, 2001 / MP
                May 28, 2001 / MP
*/

/******************************************************************************
* INCLUDES                                                                    *
******************************************************************************/

/* Make sure that _MACOS is defined in MetroWerks compilers */
#if defined( __MWERKS__ ) && defined( macintosh )
	#define	_MACOS
#endif


#ifndef _MACOS
	#include <SDL/SDL.h>
#else
	#include <SDL.h>
#endif
#include <stdlib.h>	/* exit 		   */
#include "jadkeys.h"
#include "jadedit.h"

/******************************************************************************
* DEFINES                                                                     *
******************************************************************************/

#define CLOCK_MIN 12	/* The first twelve clocks are reserved for */
			/* internal purposes			   */
#define CLOCK_MAX 256

/******************************************************************************
* TYPEDEFS                                                                    *
******************************************************************************/

/* --- Times ---- */
typedef struct {
    int active;	/*  Active: 1, 0 */
    int time;	/* How long the clock has to run (gametime) */
} CLOCK;

typedef struct {       /* Replaces "controlvalue" + "controliskey" */
    int value;         /* The scancode or mask                     */
    int gamecode;      /* Effective code for use in the game.	    */
} CONTROL;

/******************************************************************************
* DATA                                                                        *
******************************************************************************/

static int jadfps = 0;		/* Holds the frames per second 	  */
static int framecounter = 0;	/* Counts the frames int the loop */

static int jadgameclockstopped = 0;

static int Screenx = 640;
static int Screeny = 480;
static CLOCK Clock[CLOCK_MAX];

static CONTROL InputCtrl[] = {
    /* --- Camera control --- */
    { SDLK_KP1, JAD_CAMERA_LEFT },
    { SDLK_KP3, JAD_CAMERA_RIGHT },
    { SDLK_KP_PLUS, JAD_CAMERA_IN },
    { SDLK_KP_MINUS, JAD_CAMERA_OUT },
    { SDLK_PAGEUP, JAD_CAMERA_UP },
    { SDLK_PAGEDOWN, JAD_CAMERA_DOWN },
    /* --- Character movement --- */
    { SDLK_KP8, JAD_FORWARD },
    { SDLK_KP2, JAD_BACKWARD },
    { SDLK_KP4, JAD_STEPLEFT },
    { SDLK_KP6, JAD_STEPRIGHT },
    { SDLK_KP7, JAD_TURNLEFT },
    { SDLK_KP9, JAD_TURNRIGHT },
    { SDLK_KP5, JAD_STOP },
    { SDLK_KP_PERIOD, JAD_CAMMODE },
    /* --- Edit commands --- */
    { SDLK_RETURN, JAD_ENTERKEY },
    { SDLK_ESCAPE, JAD_ESCAPEKEY },
    { SDLK_BACKSPACE, JAD_BACKSPACE },
    { SDLK_TAB, JAD_TAB },
    { SDLK_UP, JAD_ARROWUP },
    { SDLK_DOWN, JAD_ARROWDOWN },
    { SDLK_LEFT, JAD_ARROWLEFT },
    { SDLK_RIGHT, JAD_ARROWRIGHT },
    { 0 }
};

/******************************************************************************
*  CODE									      *
******************************************************************************/

/*
 * Name:
 *     TranslateKey
 * Description:
 *     Translate the given keysim to an internal inputcode. Unknown
 *     keysims are returned untranslated.
 */
static int TranslateKey(int keysim)
{
    CONTROL *pctrl;

    /* Loop through all the inputcontrols an look if there is one */
    pctrl = InputCtrl;

    while (pctrl -> value != 0) {
    
       if (pctrl -> value == keysim)
           return pctrl -> gamecode;

       pctrl++;
    }

    return keysim;	/* For standard keyboardinput */
}

static int CheckInput(void)
{
    SDL_Event event;
    int done;
    int button;		/* Mouse button pressed in SDL_MOUSEBUTTONDOWN */
    int key;


    SDL_PumpEvents();	/* Force filling the event queue	*/
    done = 0;

    if ( SDL_PollEvent(&event) ) {
        switch(event.type) {
            case SDL_QUIT:
            	return 1;

            case SDL_MOUSEMOTION:
                 JadEdit_HandleInput(event.motion.x, event.motion.y, JAD_MOUNONE, 0);
                 return 0;
                 
            case SDL_MOUSEBUTTONUP:
                 switch (event.button.button) {
	             case SDL_BUTTON_LEFT:
                         button = JAD_MOULEFT;
                         break;
                     case SDL_BUTTON_RIGHT:
                         button = JAD_MOURIGHT;
                         break;
                     case SDL_BUTTON_MIDDLE:
                     	 button = JAD_MOUMIDDLE;
                         break;
                     default:
                     	 button = JAD_MOUNONE;
                 }

                 return JadEdit_HandleInput(event.button.x, event.button.y, button, 0);

            case SDL_KEYDOWN: {
                /* Do Keyboard-Function */
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        return 1;
                    default:
                        key = TranslateKey(event.key.keysym.sym);
                }

                return JadEdit_HandleInput(0, 0, JAD_MOUNONE, key);
            } /* SDL_KEYDOWN */

        }  /* switch ( event.type ) */
    }  	/* while ( SDL_PollEvent(&event)) */

    return done;
}

/*
 * Name:
 *     Jad_ClearInput
 * Description:
 *     Clears the input buffer.
 *
 */
void Jad_ClearInput(void)
{
     SDL_Event event;

     /* Make rid of all possible input waiting to be handled. */
     while (SDL_PollEvent(&event));
}

/*
 * Name:
 *     JadEdit_Quit
 * Descriptions:
 *     Shuts down the game and quits SDL.
 * 
 */
void JadEdit_Quit(void)
{
    JadEdit_Shutdown();
    SDL_Quit();
}

/* ------------------------- Timer functions ------------------------------- */

/*
 * Name:
 *     JadEdit_ClockFunction
 * Description:
 *     Offers some clock functions.
 * Input:
 *     clockno: Clock to SET/GET...
 *     amount:  Amount of time to SET/ADD, if any.
 * Output:
 *     NEW:   Number, if new clock could be found, else 0. The clock is
 *	      set to the given amount.
 *     GET:   > 0: Clock still running.
 *	      When the given time has ended:
 *	          If the given amount is 0, then the clock is deleted.
 *	          If the given amount is > 0, then then the clock is set
 * 		  with the given amount.
 *     ADD:   New clock amount.
 *     CLEAR: Frees the given clock;
 *
 */
int JadEdit_ClockFunction(int clockno, int amount, int func)
{
    CLOCK *clock;
    int i;

    clock = &Clock[clockno];

    switch(func) {
        case JAD_CLOCKNEW:
            for (i = CLOCK_MIN, clock = Clock; i < CLOCK_MAX; i++, clock++) {
               if (clock -> active == 0) {
                   clock -> active  = 1;
	           clock -> time    = amount;
                   return i;		// Number of clock
               }
            }
            return 0;

        case JAD_CLOCKGET:
             if (clock -> time == 0) {
                 /* Now checkt the  given amount */
                 if (amount > 0) {
                     /* Keep the clock and set it new to the given amount */
                     clock -> active  = 1;
                     clock -> time = amount;
                     return 0;
                 }
                 else {
                     /* Clear the clock */
                     clock -> active = 0;
	             clock -> time = 0;
                 }
             }
             else
                 return clock -> time;

        case JAD_CLOCKADD:
            clock -> time += amount;
            return clock -> time;

        case JAD_CLOCKCLEAR:
            clock -> active = 0;
            clock -> time = 0;
            return 0;

        case JAD_CLOCKSTOP:
            /* Stops the gameclock */
            jadgameclockstopped = 1;
            break;

        case JAD_CLOCKSTART:
            jadgameclockstopped = 0;
            break;	
    }

    return 0;
}


/*
 * Name:
 *     JadEdit_ClockUpdate
 * Description:
 *     Updates all clocks.
 * Input:
 *	Time in milliseconds to subtract from the clocks.
 */
static void JadEdit_ClockUpdate(int tdiff)
{
    CLOCK *clock;
    int cnt;

    if ((! jadgameclockstopped) && (tdiff > 0)) {
    	/* only if the game timer is on */

	for (cnt = CLOCK_MIN, clock = &Clock[CLOCK_MIN];
       			                    cnt < CLOCK_MAX; cnt++, clock++) {
            if (clock -> active) {
                clock -> time -= tdiff;

                if (clock -> time <= 0) {
                    clock -> time = 0;
                }
            }
        }
    } /* if (! jadgameclockstopped) */

}


#pragma argsused
int main(int argc, char **argv)
{
    int done;
    char *wincaption;
    unsigned int mainclock,
	         frameclock,
                 newmainclock,
                 tickspassed;

    /* Initialize SDL for video output */
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
  	fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
	exit(1);
    }

    /* Create a 640x480 OpenGL screen */
    if ( SDL_SetVideoMode(Screenx, Screeny, 0, SDL_OPENGL) == NULL ) {
    	fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
    	SDL_Quit();
    	exit(2);
    }

    wincaption = JadEdit_Init(0, 0);
    if (! wincaption) {
        JadEdit_Quit();
        return 1;
    }

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    /* Set the title bar in environments that support it */
    SDL_WM_SetCaption(wincaption, NULL);

    /* Loop, drawing and checking events */


    /* Set the flag on not done */
    done = 0;

    /* Now init the glock */
    mainclock = newmainclock = frameclock = SDL_GetTicks();

    /* The framecounter... */
    jadfps = framecounter = 0;
    tickspassed = 0;		/* To hand to the Main-Function. Helps the */
    				/* main function to hold animations and so */
                                /* on frame independent			   */

    while (! done ) {

    	JadEdit_Main(jadfps, tickspassed);

        /* swap buffers to display, since we're double buffered.	*/	
        SDL_GL_SwapBuffers();

	done = CheckInput();	/* do the input-stuff */

        newmainclock = SDL_GetTicks();

        /* Get the fps, if a second has passed */
        if ((newmainclock - frameclock) >= 1000) {
            jadfps = framecounter;
            framecounter = 0;
            frameclock = newmainclock;
        }

        framecounter++;

        /* Update all clocks  */
        tickspassed = newmainclock - mainclock;
 	JadEdit_ClockUpdate(tickspassed);

        mainclock = newmainclock;
    }

    JadEdit_Quit();
    return 1;
}
