/*! 
 * \file LiveFuelMoist.h
 * \brief Routines for calculating the live fuel moisture of during the simulation.
 *  
 * \sa Check the \htmlonly <a href="config_file_doc.html#LIVE_FUEL">config file documentation</a> \endhtmlonly 
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
 
#ifndef	LiveFuelMoist_H
#define LiveFuelMoist_H

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

/* used in tokenizing LFH and LFW files */
#define LIVE_FUEL_MOIST_COMMENT_CHAR						    ("#")
#define LIVE_FUEL_MOIST_SEP_CHARS							      (" =,;\t")

/* returns corresponding field index in LFH and LFW files */
#define LIVE_FUEL_MOIST_MO_TBL_INDEX						    (0)
#define LIVE_FUEL_MOIST_DY_TBL_INDEX						    (1)
#define LIVE_FUEL_MOIST_FIXED_VAL_TBL_INDEX					(2)
#define LIVE_FUEL_MOIST_MEAN_TBL_INDEX						  (2)
#define LIVE_FUEL_MOIST_STDEV_TBL_INDEX						  (3)

/* keyword indicating annual mean and stddev of live fuel moisture */
#define LIVE_FUEL_MOIST_KEYWORD_ANNMEAN						  ("ANNMEAN")
#define LIVE_FUEL_MOIST_KEYWORD_ANNSTDEV					  ("ANNSTDEV")

/* used in tokenizing raster 10h file */
#define LIVE_FUEL_MOIST_RAST_LFM_COMMENT_CHAR				("#")
#define LIVE_FUEL_MOIST_RAST_LFM_SEP_CHARS					(" =,;\t")

/* returns corresponding field index in raster 10h file */
#define LIVE_FUEL_MOIST_RAST_LFM_MO_TBL_INDEX				(0)
#define LIVE_FUEL_MOIST_RAST_LFM_DY_TBL_INDEX				(1)
#define LIVE_FUEL_MOIST_RAST_LFM_HR_TBL_INDEX				(2)
#define LIVE_FUEL_MOIST_RAST_LFM_LFH_FILE_TBL_INDEX (3)
#define LIVE_FUEL_MOIST_RAST_LFM_LFW_FILE_TBL_INDEX	(4)

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

/*! \fn int GetLiveFuelMoistFIXEDFromProps(ChHashTable * proptbl, int year, int month, int day,
 											unsigned int row, unsigned int col, 
											double * lhfm, double * lwfm)
 *	\brief retrieves time and space dependent live fuel moisture value at a cell
 *
 *	Using this option each cell in the simulation domain is assigned a live fuel
 * 	moisture based upon index into a table of historical values
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#LIVE_FUEL">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of simulation properties
 *	\param year date to retreive live fuel moisture for
 *	\param month date to retreive live fuel moisture for 
 *	\param day date to retreive live fuel moisture for 
 * 	\param hour value of 0-23 corresponding to hour on given month and day to retrieve azimuth for
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 *	\param lhfm if function returns successfully, live herb fuel moisture
 *	\param lwfm if function returns successfully, live woody fuel moisture
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int GetLiveFuelMoistFIXEDFromProps(ChHashTable * proptbl, int year, int month, int day, int hour,
											double rwx, double rwy, 
											double * lhfm, double * lwfm);

/*! \fn int GetLiveFuelMoistRANDHFromProps(ChHashTable * proptbl, int year, int month, int day,
 											unsigned int row, unsigned int col,
											double * lhfm, double * lwfm)
 *	\brief retrieves time and space dependent live fuel moisture value at a cell
 *
 *	Using this option each cell in the simulation domain is assigned a live fuel
 * 	moisture based upon a probablity function of historical live fuel moisture values
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#LIVE_FUEL">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of simulation properties
 *	\param year date to retreive live fuel moisture for
 *	\param month date to retreive live fuel moisture for 
 *	\param day date to retreive live fuel moisture for 
 * 	\param hour value of 0-23 corresponding to hour on given month and day to retrieve azimuth for
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 *	\param lhfm if function returns successfully, live herb fuel moisture
 *	\param lwfm if function returns successfully, live woody fuel moisture
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */			
int GetLiveFuelMoistRANDHFromProps(ChHashTable * proptbl, int year, int month, int day, int hour,
											double rwx, double rwy, 
											double * lhfm, double * lwfm);

/*! \fn int GetLiveFuelMoistSPATIALFromProps(ChHashTable * proptbl, int year, int month, int day,
 											unsigned int row, unsigned int col,
											double * lhfm, double * lwfm)
 *	\brief retrieves time and space dependent live fuel moisture value at a cell
 *
 *	Using this option each cell in the simulation domain is assigned a live fuel
 * 	moisture based upon lookup into a raster of live fuel moisture values
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#LIVE_FUEL">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of simulation properties
 *	\param year date to retreive live fuel moisture for
 *	\param month date to retreive live fuel moisture for 
 *	\param day date to retreive live fuel moisture for 
 * 	\param hour value of 0-23 corresponding to hour on given month and day to retrieve azimuth for
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 *	\param lhfm if function returns successfully, live herb fuel moisture
 *	\param lwfm if function returns successfully, live woody fuel moisture
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int GetLiveFuelMoistSPATIALFromProps(ChHashTable * proptbl, int year, int month, int day, int hour,
											double rwx, double rwy, 
											double * lhfm, double * lwfm);
  
#endif LiveFuelMoist_H		/* end of LiveFuelMoist.h */
