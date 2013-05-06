/*
    GLHEAD.H
    Little help to make the include of OpenGL-Headers OS-Independent,
    like in the GLUT-Header
*/

#ifndef GLHEAD_H
#define GLHEAD_H

/* XXX This is from Win32's <windef.h> */
#  ifndef APIENTRY
#   if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__) /* Borland is needed */
#    define APIENTRY    __stdcall
#   else
#    define APIENTRY
#   endif
#  endif
   /* XXX This is from Win32's <winnt.h> */
#  ifndef CALLBACK
#   if (defined(_M_MRX000) || defined(_M_IX86) || defined(_M_ALPHA) || defined(_M_PPC)) && !defined(MIDL_PASS)
#    define CALLBACK __stdcall
#   else
#    define CALLBACK
#   endif
#  endif
   /* XXX This is from Win32's <wingdi.h> and <winnt.h> */
#  ifndef WINGDIAPI
#   define WINGDIAPI __declspec(dllimport)
#  endif
   /* XXX This is from Win32's <ctype.h> */
#  ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#   define _WCHAR_T_DEFINED
#  endif

#include <GL/gl.h> 		/* Header File For The OpenGL32 Library	*/
#include <GL/glu.h>             /* Header File For The GLu32 Library    */

#endif /* GLHEAD_H */