#ifndef _MEXMOD
#define _MEXMOD
void swz_exit(int status);
void swz_fprintf_err(const char * str, ...);
/*int swz_fprintf_stdout(char *msg, ...);*/
/*int swz_printf(char *msg, ...);*/
extern int constant_seed;
#endif

#if defined(MATLAB_MEX_FILE) || defined(OCTAVE_MEX_FILE)
/*#include "matrix.h"*/
#include <dynmex.h>
#include <dynblas.h>
#include <dynlapack.h>

/*  //#undef printf   ansi-c*/
/*  //#define printf swz_printf   ansi-c*/
/*#define fflush(stdout) mexEvalString("drawnow;");*/

#undef printf
#undef exit

#define printf mexPrintf
#define exit swz_exit

#define swz_fprintf_stdout mexPrintf

#undef calloc
#undef realloc
#undef free

#define swzMalloc mxMalloc
#define calloc mxCalloc
#define realloc mxRealloc
#define free mxFree


#else
#define swz_fprintf_stdout printf
#define swzMalloc malloc
#endif
