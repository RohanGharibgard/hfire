/*! 
 * \file DeadFuelMoist.h
 * \brief Routines for calculating the dead fuel moisture during the simulation.
 *  
 * \sa Check the \htmlonly <a href="config_file_doc.html#DEAD_FUEL">config file documentation</a> \endhtmlonly 
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
 
#ifndef	DeadFuelMoist_H
#define DeadFuelMoist_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "CoordTrans.h"
#include "Units.h"
#include "NLIBRand.h"
#include "FireProp.h"
#include "ChHashTable.h"
#include "KeyVal.h"
#include "DblTwoDArray.h"
#include "StrTwoDArray.h"
#include "GridData.h"
#include "FStreamIO.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */
 
#ifndef DOXYGEN_SHOULD_SKIP_THIS
 
/* used in tokenizing 10h files */
#define DEAD_FUEL_MOIST_10H_COMMENT_CHAR						("#")
#define DEAD_FUEL_MOIST_10H_SEP_CHARS							  (" =,;\t")

/* returns corresponding field index in WAZ file */
#define DEAD_FUEL_MOIST_YR_10H_TBL_INDEX						(0)
#define DEAD_FUEL_MOIST_MO_10H_TBL_INDEX						(1)
#define DEAD_FUEL_MOIST_DY_10H_TBL_INDEX						(2)

/* value of NO DATA used in 10H files */
#define DEAD_FUEL_MOIST_10H_NO_DATA_VALUE						(-1)

/* used in tokenizing raster 10h file */
#define DEAD_FUEL_MOIST_RAST_10H_COMMENT_CHAR				("#")
#define DEAD_FUEL_MOIST_RAST_10H_SEP_CHARS					(" =,;\t")

/* returns corresponding field index in raster 10h file */
#define DEAD_FUEL_MOIST_RAST_10H_MO_TBL_INDEX				(0)
#define DEAD_FUEL_MOIST_RAST_10H_DY_TBL_INDEX				(1)
#define DEAD_FUEL_MOIST_RAST_10H_HR_TBL_INDEX				(2)
#define DEAD_FUEL_MOIST_RAST_10H_FILE_TBL_INDEX			(3)

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* takes an hour 0-23 and returns corresponding field index in 10H file */
#define DEAD_FUEL_MOIST_HR_TO_10H_TBL_INDEX(hour)				((hour) + 3)

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

/*! \fn int GetDeadFuelMoistFIXEDFromProps(ChHashTable * proptbl, int month, int day, int hour, 
											unsigned int row, unsigned int col, 
											double * d1hfm, double * d10hfm, double * d100hfm)
 *	\brief retrieves time and space dependent dead fuel moisture value at a cell
 *
 *	Using this option each cell in the simulation domain is assigned a dead fuel
 * 	moisture based upon index into a table of historical values
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#DEAD_FUEL">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of simulation properties
 *	\param year date to retreive live fuel moisture for
 *	\param month date to retreive live fuel moisture for 
 *	\param day date to retreive live fuel moisture for 
 * 	\param hour value of 0-23 corresponding to hour on given month and day to retrieve azimuth for
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 *	\param d1hfm if function returns successfully, dead 1 hour fuel moisture
 *	\param d10hfm if function returns successfully, dead 10 hour fuel moisture
 *	\param d100hfm if function returns successfully, dead 100 hour fuel moisture 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int GetDeadFuelMoistFIXEDFromProps(ChHashTable * proptbl, int month, int day, int hour, 
											double rwx, double rwy, 
                      double * d1hfm, double * d10hfm, double * d100hfm);

/*! \fn int GetDeadFuelMoistRANDHFromProps(ChHashTable * proptbl, int month, int day, int hour, 
											unsigned int row, unsigned int col,
											double * d1hfm, double * d10hfm, double * d100hfm)
 *	\brief retrieves time and space dependent dead fuel moisture value at a cell
 *
 *	Using this option each cell in the simulation domain is assigned a dead fuel
 * 	moisture based upon random index into a table of historical values
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#DEAD_FUEL">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of simulation properties
 *	\param year date to retreive live fuel moisture for
 *	\param month date to retreive live fuel moisture for 
 *	\param day date to retreive live fuel moisture for 
 * 	\param hour value of 0-23 corresponding to hour on given month and day to retrieve azimuth for
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 *	\param d1hfm if function returns successfully, dead 1 hour fuel moisture
 *	\param d10hfm if function returns successfully, dead 10 hour fuel moisture
 *	\param d100hfm if function returns successfully, dead 100 hour fuel moisture 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */																					
int GetDeadFuelMoistRANDHFromProps(ChHashTable * proptbl, int month, int day, int hour, 
											double rwx, double rwy, 
                      double * d1hfm, double * d10hfm, double * d100hfm);

/*! \fn int GetDeadFuelMoistSPATIALFromProps(ChHashTable * proptbl, int month, int day, int hour, 
											unsigned int row, unsigned int col, 
											double * d1hfm, double * d10hfm, double * d100hfm)
 *	\brief retrieves time and space dependent dead fuel moisture value at a cell
 *
 *	Using this option each cell in the simulation domain is assigned a dead fuel
 * 	moisture based upon lookup into a raster of dead fuel moisture values
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#DEAD_FUEL">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of simulation properties
 *	\param year date to retreive live fuel moisture for
 *	\param month date to retreive live fuel moisture for 
 *	\param day date to retreive live fuel moisture for 
 * 	\param hour value of 0-23 corresponding to hour on given month and day to retrieve azimuth for
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 *	\param d1hfm if function returns successfully, dead 1 hour fuel moisture
 *	\param d10hfm if function returns successfully, dead 10 hour fuel moisture
 *	\param d100hfm if function returns successfully, dead 100 hour fuel moisture 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */										
int GetDeadFuelMoistSPATIALFromProps(ChHashTable * proptbl, int month, int day, int hour, 
											double rwx, double rwy, 
                      double * d1hfm, double * d10hfm, double * d100hfm);
 																			 
#endif DeadFuelMoist_H		/* end of DeadFuelMoist.h */
