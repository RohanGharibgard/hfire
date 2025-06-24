/*! 
 * \file Roth1972.h
 * \brief Routines for calculating steady-state rate of fire spread using Rothermel fire spread equation (1972).
 *  
 * \sa For more information consult RothFuelModel and RothPipeline
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
 
#ifndef	Roth1972_H
#define Roth1972_H

#include <stdlib.h>
#include <math.h>

#include "Err.h"
#include "Units.h"
#include "RothFuelModel.h"
#include "RothPipeline.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/* value for PI */
#define ROTH_1972_PI								(3.141592654)
 
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

/* converts degrees to radians */
#define ROTH_1972_DEG_TO_RAD(deg)					(((deg) * 2.0 * ROTH_1972_PI) / 360.0)

/* converts radians to degrees */
#define ROTH_1972_RAD_TO_DEG(rad)					(((rad) * 360.0) / (2.0 * ROTH_1972_PI))
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

/*! \fn int Roth1972FireSpreadSetFuelBed(RothFuelModel * rfm) 
 *	\brief Step 1 of FireSpread Pipeline.  			
 *
 *	Converts RothFuelModel structure containing individual fuel particles comprising
 *	fuel bed into a single representative fuel bed.
 *	If RothFuelModel units are METRIC, then converts values to ENGLISH.
 *	\sa	RothFuelModel
 *	\param rfm RothFuelModel structure containing fuel particle attributes
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int Roth1972FireSpreadSetFuelBed(RothFuelModel * rfm);

/*! \fn int Roth1972FireSpreadNoWindNoSlope(RothFuelModel * rfm, double d1hfm, double d10hfm, double d100hfm, double lhfm, double lwfm)
 *  \brief Step 2 of FireSpread Pipeline.
 * 
 *  Calculates the NoWind-NoSlope rate of spread through the fuel bed represented by RothFuelModel.
 *  RothFuelModel units must be ENGLISH.
 *  Step 1 in FireSpread Pipeline(SetFuelBed) must have been completed prior to calling this 
 *  function, however calling SetFuelBed prior to every call to NoWindNoSlope is unnecessary.
 *  Instead, SetFuelBed need only be called when any of the fuel particle attributes 
 *  (load,sav,dens,hc,stot,seff) have changed.
 *	\sa	RothFuelModel
 *  \param rfm RothFuelModel structure containing fuel particle attributes
 *  \param d1hfm dead 1 hour fuel moisture
 *  \param d10hfm dead 10 hour fuel moisture 
 *  \param d100hfm dead 100 hour fuel moisture 
 *  \param lhfm live herb fuel moisture 
 *  \param lwfm live woody fuel moisture 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int Roth1972FireSpreadNoWindNoSlope(RothFuelModel * rfm, double d1hfm, double d10hfm, double d100hfm, double lhfm, double lwfm);

/*! \fn int Roth1972FireSpreadWindSlopeMax(RothFuelModel * rfm, double wnd_fpm, double wnd_az, double slp_pcnt, double asp, double ell_adj)
 *  \brief Step 3 of FireSpread Pipeline.
 *
 * 	Calculates the maximum rate of spread through the fuel bed given the supplied
 * 	wind and slope related arguments.
 * 	RothFuelModel units must be ENGLISH.
 * 	Steps 1 and 2 in FireSpread Pipeline must have been completed without error
 * 	prior to calling this function.
 *	\sa	RothFuelModel
 *  \param rfm RothFuelModel structure containing fuel particle attributes
 *  \param wnd_fpm midflame windspeed in ft/min
 *  \param wnd_az azimuth from which wind is coming (same as measured by RAWS)
 *  \param slp_pcnt slope percent (as whole number eg 100 corresponds to 100% slope or [1 unit rise]/[1 unit run] )
 *  \param asp aspect (eg direction of maximum rate of change of slope aka downslope)
 *  \param ell_adj fire ellipse adjustment factor, < 1.0 = more circular, > 1.0 = more elliptical  
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int Roth1972FireSpreadWindSlopeMax(RothFuelModel * rfm, double wnd_fpm, double wnd_az, double slp_pcnt, double asp, double ell_adj);

/*! \fn int Roth1972FireSpreadGetAtAzimuth(RothFuelModel * rfm, double az) 
 *  \brief Step 4 of FireSpread Pipeline.
 *
 * 	Calculates the rate of spread through the fuel bed in the direction specified as an argument.
 * 	RothFuelModel units must be ENGLISH.
 * 	Steps 1, 2, and 3 in FireSpread Pipeline must have been completed without error
 * 	prior to calling this function.
 *	\sa	RothFuelModel
 *  \param rfm RothFuelModel structure containing fuel particle attributes
 *  \param az azimuth in the direction which rate-of-spread is calculated	 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int Roth1972FireSpreadGetAtAzimuth(RothFuelModel * rfm, double az);
  
#endif Roth1972_H		/* end of Roth1972.h */
