/*! 
 * \file Ignition.h
 * \brief Routines for generating and locating new fire ignitions in the simulation domain.
 *  
 * \sa Check the \htmlonly <a href="config_file_doc.html#IGNITION">config file documentation</a> \endhtmlonly 
 * \author Marco Morais
 * \date 2000-2001 
 *
 *	HFire (Highly Optmized Tolerance Fire Spread Model) Library
 *	Copyright (C) 2001 Marco Morais
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation; either
 *	version 2.1 of the License, or (at your option) any later version.
 *
 *	This library is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	Lesser General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public
 *	License along with this library; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#ifndef	Ignition_H
#define Ignition_H

#include <stdlib.h>

#include "FireYear.h"
#include "FireTimer.h"
#include "GridData.h"
#include "NLIBRand.h"
#include "ChHashTable.h"
#include "KeyVal.h"
#include "FireProp.h"
#include "FireGridData.h"
#include "List.h"
#include "FStreamIO.h"
#include "Err.h"
#include "DblTwoDArray.h"
#include "CoordTrans.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
 
/* number of seconds per hour */
#define IGNITION_SECS_PER_HOUR								(3600)

/* number of seconds in a day */
#define IGNITION_SECS_PER_DAY								((IGNITION_SECS_PER_HOUR) * 24)

/* used in tokenizing fixed ignition file */
#define IGNITION_IGS_COMMENT_CHAR							("#")
#define IGNITION_SEP_CHARS									(" =,\t")

/* max number of attempts to retrieve location within domain before failing */
#define IGNITION_RANDU_MAX_TRIALS							(1000000) 
#define IGNITION_RANDS_MAX_TRIALS							(500000)

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
 
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
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

/*!	\fn int IsIgnitionNowFIXEDFromProps(ChHashTable * proptbl)
 * 	\brief Tests whether an ignition occurs during current timestep.
 *
 * 	For FIXED implementations this method returns 1 (true) if FIXED supplied for IGNITION_TYPE keyword
 * 	AND if this the first time it has been called.  Otherwise this method returns 0.
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#IGNITION">config file documentation</a> \endhtmlonly 
 * 	\param proptbl HashTable of simulation properties
 *	\retval 1 (true) or 0 (false).
 */  
int IsIgnitionNowFIXEDFromProps(ChHashTable * proptbl);

/*!	\fn int IsIgnitionNowRANDFromProps(ChHashTable * proptbl)
 * 	\brief Tests whether an ignition occurs during current timestep.
 *
 * 	For RAND implementations this method converts the value of the keyword IGNITION_FREQUENCY_PER_DAY
 * 	to a probability in units per hour and uses a uniform random number generator to determine if
 * 	an ignition occurs this timestep.
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#IGNITION">config file documentation</a> \endhtmlonly 
 * 	\param proptbl HashTable of simulation properties
 *	\retval 1 (true) or 0 (false).
 */  
int IsIgnitionNowRANDFromProps(ChHashTable * proptbl);
 
/*!	\fn int GetIgnitionLocFIXEDFromProps(ChHashTable * proptbl, FireYear * fy, List ** rwxylist)
 * 	\brief Returns a list of ignition locations in real world coordinates.
 *
 * 	For FIXED implementations this method retrieves contents of keyword IGNITION_FIXED_IGS_FILE file
 * 	into temporary two dimensional array.  The array is used to construct a list of real world x,y
 * 	coordinate pairs.  The list is returned.  Bounds checking on the xy coordinates supplied
 * 	by the user is not performed by this function.
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#IGNITION">config file documentation</a> \endhtmlonly 
 * 	\param proptbl HashTable of simulation properties
 *	\param fy FireYear of all currently burning cells in simulation domain
 * 	\param rwxylist List of real world x and y coordinate locations returned by function
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */	
int GetIgnitionLocFIXEDFromProps(ChHashTable * proptbl, FireYear * fy, List ** rwxylist);

/*!	\fn int GetIgnitionLocRANDUFromProps(ChHashTable * proptbl, FireYear * fy, List ** rwxylist)
 * 	\brief Returns a list of ignition locations in real world coordinates.
 *
 * 	For RANDU implementations this method draws two uniform random numbers having ranges of
 * 	[llx,llx + (cols * cellsize)] and [lly,lly + (rows * cellsize)]. 
 * 	The two numbers are converted to cell indexes and the location is tested to determine
 * 	whether it falls inside a burnable fuel cell. The process is repeated until a real
 * 	world coordinate location is chosen that falls inside a burnable fuel cell.
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#IGNITION">config file documentation</a> \endhtmlonly 
 * 	\param proptbl HashTable of simulation properties
 *	\param fy FireYear of all currently burning cells in simulation domain
 * 	\param rwxylist List of real world x and y coordinate locations returned by function
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */	
int GetIgnitionLocRANDUFromProps(ChHashTable * proptbl, FireYear * fy, List ** rwxylist);

/*!	\fn int GetIgnitionLocRANDSFromProps(ChHashTable * proptbl, FireYear * fy, List ** rwxylist)
 * 	\brief Returns a list of ignition locations in real world coordinates.
 *
 * 	Two tests must be completed successfully for the RANDS function to return without error.
 * 	FIRST: The function GetIgnitionLocRANDUFromProps is called to retrieve a random uniformly
 * 	distributed real world x,y coordinate pair inside of a cell with burnable fuel.
 * 	SECOND: A temporary grid of cell-by-cell ignition probablities, specified with IGNITION_RSP
 * 	keywords, is loaded from disk. The real world x,y coordinates are used to determine what
 * 	the probability of ignition is at the x,y location. A uniform random number is drawn
 * 	and compared to the probability at the x,y location to determine if an ignition occurs.
 * 	If successful, the function returns the real world x,y coordinate pair. If unsuccessful,
 * 	the entire process is repeated until a successful trial occurs.
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#IGNITION">config file documentation</a> \endhtmlonly 
 * 	\param proptbl HashTable of simulation properties
 *	\param fy FireYear of all currently burning cells in simulation domain
 * 	\param rwxylist List of real world x and y coordinate locations returned by function
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */	
int GetIgnitionLocRANDSFromProps(ChHashTable * proptbl, FireYear * fy, List ** rwxylist);

#endif Ignition_H		/* end of Ignition.h */
