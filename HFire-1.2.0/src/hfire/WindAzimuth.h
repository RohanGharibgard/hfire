/*! 
 * \file WindAzimuth.h
 * \brief Routines for calculating the wind azimuth during the simulation.
 *  
 * \sa Check the \htmlonly <a href="config_file_doc.html#WIND_AZIMUTH">config file documentation</a> \endhtmlonly
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
 
#ifndef	WindAzimuth_H
#define WindAzimuth_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "CoordTrans.h"
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

/* used in tokenizing WAZ file */
#define WIND_AZIMUTH_WAZ_COMMENT_CHAR							("#")
#define WIND_AZIMUTH_WAZ_SEP_CHARS								(" =,;\t")

/* returns corresponding field index in WAZ file */
#define WIND_AZIMUTH_YR_WAZ_TBL_INDEX							(0)
#define WIND_AZIMUTH_MO_WAZ_TBL_INDEX							(1)
#define WIND_AZIMUTH_DY_WAZ_TBL_INDEX							(2)
 
/* value of NO DATA used in WAZ files */
#define WIND_AZIMUTH_WAZ_NO_DATA_VALUE							(-1)

/* min and max values used to retrieve uniform random number */
#define WIND_AZIMUTH_RANDU_MIN_AZ								(0.0)
#define WIND_AZIMUTH_RANDU_MAX_AZ								(360.0)

/* used in tokenizing ATM file */
#define WIND_AZIMUTH_ATM_COMMENT_CHAR							("#")
#define WIND_AZIMUTH_ATM_SEP_CHARS								(" =,;\t")

/* returns corresponding field index in ATM file */
#define WIND_AZIMUTH_MO_ATM_TBL_INDEX							(0)
#define WIND_AZIMUTH_DY_ATM_TBL_INDEX							(1)
#define WIND_AZIMUTH_HR_ATM_TBL_INDEX							(2)
#define WIND_AZIMUTH_FILE_ATM_TBL_INDEX							(7)

/* keyword indicating units of windspeed in ATM */
#define WIND_AZIMUTH_ATM_KEYWORD_ENGLISH						("ENGLISH")		/* ENGLISH == MILEPHR */
#define WIND_AZIMUTH_ATM_KEYWORD_METRIC							("METRIC")		/* METRIC == KMPHR */

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

/* takes an hour 0-23 and returns corresponding field index in WAZ file */
#define WIND_AZIMUTH_HR_TO_WAZ_TBL_INDEX(hour)					((hour) + 3)

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

/*!	\fn int GetWindAzimuthFIXEDFromProps(ChHashTable * proptbl, int month, int day, int hour,
												double rwx, double rwy, double * waz)
 * 	\brief Returns a wind azimuth to be used for given {month, day, hour} of simulation.
 *
 * 	For FIXED implementations the value of keyword WIND_AZIMUTH_FIXED_FILE is used
 *	to load a DblTwoDArray of month, day, hour wind azimuth values. The supplied
 * 	arguments are used to index the table and retrieve the recorded azimuth. 
 * 	Static variables are used to ensure that the table is initialized only once and
 * 	that repeat table indexes are avoided.
 * 	\sa Check the \htmlonly <a href="config_file_doc.html#WIND_AZIMUTH">config file documentation</a> \endhtmlonly
 * 	\param proptbl HashTable of simulation properties
 * 	\param month 1 based month to retrieve azimuth for
 * 	\param day 1 based day in month to retrieve azimuth for
 * 	\param hour value of 0-23 corresponding to hour on given month and day to retrieve azimuth for
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 * 	\param waz double corresponding to value of wind azimuth in the range 0-360
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int GetWindAzimuthFIXEDFromProps(ChHashTable * proptbl, int month, int day, int hour,
									double rwx, double rwy, double * waz);

/*!	\fn int GetWindAzimuthRANDUFromProps(ChHashTable * proptbl, int month, int day, int hour,
												double rwx, double rwy, double * waz)
 * 	\brief Returns a wind azimuth to be used for given {month, day, hour} of simulation.
 *
 * 	For RANDU implementations a uniform random number in therange 0-360 is drawn
 * 	and returned as the wind azimuth. 
 * 	Static variables are used to ensure that the same wind azimuth is retrieved for
 * 	every call to this function during the same {month, day, hour}.
 * 	\sa Check the \htmlonly <a href="config_file_doc.html#WIND_AZIMUTH">config file documentation</a> \endhtmlonly
 * 	\param proptbl HashTable of simulation properties
 * 	\param month 1 based month to retrieve azimuth for
 * 	\param day 1 based day in month to retrieve azimuth for
 * 	\param hour value of 0-23 corresponding to hour on given month and day to retrieve azimuth for
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 * 	\param waz double corresponding to value of wind azimuth in the range 0-360
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int GetWindAzimuthRANDUFromProps(ChHashTable * proptbl, int month, int day, int hour,
									double rwx, double rwy, double * waz);

/*!	\fn int GetWindAzimuthRANDUFromProps(ChHashTable * proptbl, int month, int day, int hour,
												double rwx, double rwy, double * waz)
 * 	\brief Returns a wind azimuth to be used for given {month, day, hour} of simulation.
 *
 * 	For RANDH implementations the value of keyword WIND_AZIMUTH_HISTORICAL_FILE is used
 * 	to load a DblTwoDArray of month, day, hour wind azimuth values.  A random number
 * 	in the range 0 to num records in DblTwoDArray is chosen. The value of the azimuth
 * 	at this record number and hour specified is retrieved. If the azimuth is not equal
 * 	to WIND_AZIMUTH_WAZ_NO_DATA_VALUE, then it is returned. Otherwise, the process is
 * 	repeated until a non-NO DATA value is found.
 * 	Static variables are used to ensure that the table is initialized only once and
 * 	that repeat table indexes are avoided.
 * 	\sa Check the \htmlonly <a href="config_file_doc.html#WIND_AZIMUTH">config file documentation</a> \endhtmlonly
 * 	\param proptbl HashTable of simulation properties
 * 	\param month 1 based month to retrieve azimuth for
 * 	\param day 1 based day in month to retrieve azimuth for
 * 	\param hour value of 0-23 corresponding to hour on given month and day to retrieve azimuth for
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 * 	\param waz double corresponding to value of wind azimuth in the range 0-360
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */ 
int GetWindAzimuthRANDHFromProps(ChHashTable * proptbl, int month, int day, int hour,
									double rwx, double rwy, double * waz);

/*!	\fn int GetWindAzimuthSPATIALFromProps(ChHashTable * proptbl, int month, int day, int hour, 
												double rwx, double rwy, double * waz)
 * 	\brief Returns a wind azimuth to be used for given {month, day, hour} of simulation.
 *
 *	Using this option each cell in the simulation domain is assigned a unique wind azimuth
 * 	based upon index into a raster of wind azimuths
 * 	\sa Check the \htmlonly <a href="config_file_doc.html#WIND_AZIMUTH">config file documentation</a> \endhtmlonly
 * 	\param proptbl HashTable of simulation properties
 * 	\param month 1 based month to retrieve azimuth for
 * 	\param day 1 based day in month to retrieve azimuth for
 * 	\param hour value of 0-23 corresponding to hour on given month and day to retrieve azimuth for
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 * 	\param waz double corresponding to value of wind azimuth in the range 0-360
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */ 	
int GetWindAzimuthSPATIALFromProps(ChHashTable * proptbl, int month, int day, int hour, 
										double rwx, double rwy, double * waz);

#endif WindAzimuth_H		/* end of WindAzimuth.h */
