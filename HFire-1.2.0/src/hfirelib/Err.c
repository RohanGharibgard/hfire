#include "Err.h"

/* the currrent stream, stderr by default */
FILE * err_stream = NULL;

/*
 * Visibility:
 * local
 *
 * Description:
 * Prints a warning to the current error stream.
 * If no error stream is set, defaults to stderr, otherwise use err_set_stream to set a stream.
 *
 * Arguments:
 * label- heading to apply to output, generally either error or warning
 * file- file warning occured in
 * line- line inside of file where warning occured
 * reason- long description of warning
 * err_enum- enumeration corresponding to reason for calling this function
 * 
 * Returns:
 * void
 */
void err_stream_printf (const char *label, const char *file, int line, const char *reason, int err_enum)	{
	if (err_stream == NULL)	{
		err_stream = stderr;
		}
	fprintf (err_stream, "%s:%d: %s: %s (%d)\n", file, line, label, reason, err_enum);
	return;
	}
	
/*
 * Visibility:
 * global
 *
 * Description:
 * Prints an error to the current error stream.
 * If no error stream is set, defaults to stderr, otherwise use err_set_stream to set a stream.
 * Alternatively call err_warning to send a warning through the error stream (don't abuse this).
 *
 * Arguments:
 * reason- long description of error
 * file- file error occured in
 * line- line inside of file where error occured
 * err_enum- enumeration corresponding to reason for calling this function
 * 
 * Returns:
 * void
 */
void err_error (const char * reason, const char * file, int line, int err_enum)	{
	err_stream_printf("ERROR", file, line, reason, err_enum);
	return;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Prints a warning to the current error stream.
 * If no error stream is set, defaults to stderr, otherwise use err_set_stream to set a stream.
 * Alternatively call err_error to send an error through the error stream.
 *
 * Arguments:
 * reason- long description of warning
 * file- file warning occured in
 * line- line inside of file where warning occured
 * err_enum- enumeration corresponding to reason for calling this function
 * 
 * Returns:
 * void
 */
void err_warning (const char * reason, const char * file, int line, int err_enum)	{
	err_stream_printf("WARNING", file, line, reason, err_enum);
	return;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Sets the current error stream to the stream specified as an argument.
 * Returns the currently set stream.  If no stream had been set, returns NULL.
 *
 * Arguments:
 * new_stream- stream to redirect errors through
 * 
 * Returns:
 * Returns the currently set stream.  If no stream had been set, returns NULL.
 */
FILE * err_set_stream (FILE * new_stream)	{
	FILE * previous_stream;
	if (err_stream == NULL) {
		err_stream = stderr;		/* this way you don't return a NULL stream */
		}
	previous_stream = err_stream;
	err_stream = new_stream;
	return previous_stream;
	}

/* end of Err.c */
