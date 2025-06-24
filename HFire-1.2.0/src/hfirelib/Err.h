#ifndef Err_H
#define Err_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */
enum { 
	ERR_SUCCESS  = 0, 
	ERR_FAILURE  = -1,
	ERR_CONTINUE = -2,  /* iteration has not converged */
	ERR_EDOM     = 1,   /* input domain error, e.g sqrt(-1) */
	ERR_ERANGE   = 2,   /* output range error, e.g. exp(1e100) */
	ERR_EFAULT   = 3,   /* invalid pointer */
	ERR_EINVAL   = 4,   /* invalid argument supplied by user */
	ERR_EFAILED  = 5,   /* generic failure */
	ERR_EFACTOR  = 6,   /* factorization failed */
	ERR_ESANITY  = 7,   /* sanity check failed - shouldn't happen */
	ERR_ENOMEM   = 8,   /* malloc failed */
	ERR_EBADFUNC = 9,   /* problem with user-supplied function */
	ERR_ERUNAWAY = 10,  /* iterative process is out of control */
	ERR_EMAXITER = 11,  /* exceeded max number of iterations */
	ERR_EZERODIV = 12,  /* tried to divide by zero */
	ERR_EBADTOL  = 13,  /* user specified an invalid tolerance */
	ERR_ETOL     = 14,  /* failed to reach the specified tolerance */
	ERR_EUNDRFLW = 15,  /* underflow */
	ERR_EOVRFLW  = 16,  /* overflow  */
	ERR_ELOSS    = 17,  /* loss of accuracy */
	ERR_EROUND   = 18,  /* failed because of roundoff error */
	ERR_EBADLEN  = 19,  /* matrix, vector lengths are not conformant */
	ERR_ENOTSQR  = 20,  /* matrix not square */
	ERR_ESING    = 21,  /* apparent singularity detected */
	ERR_EDIVERGE = 22,  /* integral or series is divergent */
	ERR_EUNSUP   = 23,  /* requested feature is not supported by the hardware */
	ERR_EUNIMPL  = 24,  /* requested feature not (yet) implemented */
	ERR_ECACHE   = 25,  /* cache table limit exceeded */
	ERR_ENOPROG  = 26,  /* iteration is not making progress towards solution */
	ERR_ENOPROGJ = 27,  /* jacobian evaluations are not improving the solution */
	ERR_WARNING  = 28,	/* no error, but strong warning */
	ERR_EIOFAIL	 = 29,	/* io error */
	ERR_EUNITS	 = 30	/* measurement unit mismatch */
	};

/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */
 
/* call the error handler, and return the error code */
#define ERR_ERROR(reason, err_enum) \
       do { \
       err_error (reason, __FILE__, __LINE__, err_enum) ; \
       return err_enum ; \
       } while (0)

/* call the error handler, and return the given value */
#define ERR_ERROR_RETURN(reason, err_enum, value) \
       do { \
       err_error (reason, __FILE__, __LINE__, err_enum) ; \
       return value ; \
       } while (0)

/* call the error handler, and then return (for void functions which still need to generate an error) */
#define ERR_ERROR_RETURN_NOTHING(reason, err_enum) \
       do { \
       err_error (reason, __FILE__, __LINE__, err_enum) ; \
       return ; \
       } while (0)

/* call the error handler and exit the program */
#define ERR_ERROR_FATAL(reason, err_enum) \
      do { \
      err_error (reason, __FILE__, __LINE__, err_enum) ; \
      exit(EXIT_FAILURE); \
      } while (0)

/* 
** call the error handler, and terminate the program
**
** Much of the HFire code was written assuming that errors would be
** handled at a higher level in the code than where they occured,
** but this philosophy has proven in retrospect to be a failure.
** As a result, the ERR_ERROR_CONTINUE macro now maps to the
** ERR_ERROR_FATAL macro resulting in program termination at the
** location in the program where this macro is called. - MEM
** 
*/
#define ERR_ERROR_CONTINUE(reason, err_enum)  ERR_ERROR_FATAL(reason, err_enum)

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */
 
void err_error (const char * reason, const char * file, int line, int err_enum); 

void err_warning (const char * reason, const char * file, int line, int err_enum);

FILE * err_set_stream (FILE * new_stream);
		
#endif Err_H	/* end of Err.h */
