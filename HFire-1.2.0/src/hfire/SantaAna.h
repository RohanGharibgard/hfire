/*! 
 * \file SantaAna.h
 * \brief Routines for calculating occurence, length, and severity of conditions during Santa Ana events.
 *  
 * \sa Check the \htmlonly <a href="config_file_doc.html#SANTA_ANA">config file documentation</a> \endhtmlonly
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
 
#ifndef	SantaAna_H
#define SantaAna_H

#include <stdlib.h>
#include <stdio.h>

#include "Units.h"
#include "FireTimer.h"
#include "WindAzimuth.h"
#include "WindSpd.h"
#include "DeadFuelMoist.h"
#include "FireProp.h"
#include "FireExport.h"
#include "FStreamIO.h"
#include "NLIBRand.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* returns corresponding field index in Santa Ana file */
#define SANTA_ANA_YR_TBL_INDEX							(0)
#define SANTA_ANA_MO_TBL_INDEX							(1)
#define SANTA_ANA_DY_TBL_INDEX							(2)

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

/* takes an hour 0-23 and returns corresponding field index in Santa Ana file */
#define SANTA_ANA_HR_TO_TBL_INDEX(hour)					((hour) + 3)

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

/*!	\fn int IsSantaAnaNowFromProps(ChHashTable * proptbl, int year, int month, int day)
 * 	\brief Tests whether a Santa Ana event occurs during this day in the simulation.
 *
 * 	This method converts the value of the keyword SANTA_ANA_FREQUENCY_PER_YEAR to a probability
 * 	relative to the length of the fire season and uses a uniform random number generator to 
 * 	determine if a Santa Ana event occurs this day.
 *	\sa ChHashTable
 * 	\sa Check the \htmlonly <a href="config_file_doc.html#SANTA_ANA">config file documentation</a> \endhtmlonly 
 * 	\param proptbl ChHashTable of simulation properties
 * 	\param year current year in simulation
 * 	\param month current month in simulation 
 * 	\param day current day in simulation 
 * 	\retval int Returns 1 (true) if there is a Santa Ana event this day, or 0 (false) otherwise.
 */
int IsSantaAnaNowFromProps(ChHashTable * proptbl, int year, int month, int day);

/*!	\fn int GetSantaAnaEnvFromProps(ChHashTable * proptbl, int month, int day, int hour,
 									double * waz, double fbedhgtm, double * wspmps,
 									double * d1hfm, double * d10hfm, double * d100hfm)
 * 	\brief The procedure employed to mimic Santa Ana conditions during the simulation is similar
 *		   to that used for simulations incorporating wind speed, wind direction, and dead fuel
 * 		   moisture using the RANDOM_HISTORICAL option.
 *
 * 	At the start of each day designated as a Santa Ana, a uniform random number is
 * 	chosen to index the same year/month/day record from all three files.  The conditions 
 * 	experienced during the randomly chosen year/month/day are used during the given day in the
 * 	simulation.  For multi-day Santa Anas a new index is chosen at the start of the next day.
 *	\sa ChHashTable
 * 	\sa Check the \htmlonly <a href="config_file_doc.html#SANTA_ANA">config file documentation</a> \endhtmlonly 
 *  \param proptbl ChHashTable of simulation properties
 *  \param month 1 based month to retrieve conditions for
 *  \param day 1 based day in month to retrieve conditions for
 *  \param hour value of 0-23 corresponding to hour on given month and day to retrieve conditions for
 *  \param waz wind direction for the given mo-dy-hr
 *  \param fbedhgtm fuel bed height in meters (used to determine windspeed reduction factor)
 *  \param wspmps wind speed in meters per seconds for the given mo-dy-hr
 *  \param d1hfm dead 1 hour fuel moisture for the given mo-dy-hr
 *  \param d10hfm dead 10 hour fuel moisture for the given mo-dy-hr
 * 	\param d100hfm dead 100 hour fuel moisture for the given mo-dy-hr
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int GetSantaAnaEnvFromProps(ChHashTable * proptbl, int month, int day, int hour,
									double * waz, double fbedhgtm, double * wspmps,
									double * d1hfm, double * d10hfm, double * d100hfm);
  
#endif SantaAna_H		/* end of SantaAna.h */
