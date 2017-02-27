//
// MATLAB Compiler: 6.2 (R2016a)
// Date: Sun Sep 18 21:18:22 2016
// Arguments: "-B" "macro_default" "-W" "cpplib:random_walker" "-T" "link:lib"
// "-d"
// "D:\work\finish4Temp\random_walker_matlab_code\random_walker\for_testing"
// "-v" "D:\work\finish4Temp\random_walker_matlab_code\random_walker.m" 
//

#ifndef __random_walker_h
#define __random_walker_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_random_walker
#define PUBLIC_random_walker_C_API __global
#else
#define PUBLIC_random_walker_C_API /* No import statement needed. */
#endif

#define LIB_random_walker_C_API PUBLIC_random_walker_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_random_walker
#define PUBLIC_random_walker_C_API __declspec(dllexport)
#else
#define PUBLIC_random_walker_C_API __declspec(dllimport)
#endif

#define LIB_random_walker_C_API PUBLIC_random_walker_C_API


#else

#define LIB_random_walker_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_random_walker_C_API 
#define LIB_random_walker_C_API /* No special import/export declaration */
#endif

extern LIB_random_walker_C_API 
bool MW_CALL_CONV random_walkerInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_random_walker_C_API 
bool MW_CALL_CONV random_walkerInitialize(void);

extern LIB_random_walker_C_API 
void MW_CALL_CONV random_walkerTerminate(void);



extern LIB_random_walker_C_API 
void MW_CALL_CONV random_walkerPrintStackTrace(void);

extern LIB_random_walker_C_API 
bool MW_CALL_CONV mlxRandom_walker(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_random_walker
#define PUBLIC_random_walker_CPP_API __declspec(dllexport)
#else
#define PUBLIC_random_walker_CPP_API __declspec(dllimport)
#endif

#define LIB_random_walker_CPP_API PUBLIC_random_walker_CPP_API

#else

#if !defined(LIB_random_walker_CPP_API)
#if defined(LIB_random_walker_C_API)
#define LIB_random_walker_CPP_API LIB_random_walker_C_API
#else
#define LIB_random_walker_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_random_walker_CPP_API void MW_CALL_CONV random_walker(int nargout, mwArray& mask, mwArray& probabilities, const mwArray& img, const mwArray& seeds, const mwArray& labels, const mwArray& beta);

#endif
#endif
