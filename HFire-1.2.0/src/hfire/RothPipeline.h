/*! 
 * \file RothPipeline.h
 * \brief Facility for storing state of variables used in pipelined fire spread calculations.
 *  
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
 
#ifndef	RothPipeline_H
#define RothPipeline_H

#include <stdlib.h>

#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/*! \def ROTH_PIPELINE_NO_WIND_LIMIT
 *	\brief wind limit on rate of spread not reached
 */
#define ROTH_PIPELINE_NO_WIND_LIMIT				(0)
/*! \def ROTH_PIPELINE_AT_WIND_LIMIT
 *	\brief wind limit on rate of spread reached
 */
#define ROTH_PIPELINE_AT_WIND_LIMIT				(1)

/*! \enum EnumFireSpreadPipe_
 *	\brief constant storing current step in fire spread prediction pipe
 *
 *	\note EnumInitPipe memory allocated for pipeline structure 
 *	\note EnumSetFuelBedPipe step 1 completed
 *	\note EnumNoWindNoSlopePipe step 2 completed
 *	\note EnumWindSlopeMaxPipe step 3 completed
 *	\note EnumGetAtAzimuthPipe step 4 completed
 */
enum EnumFireSpreadPipe_	{
	EnumInitPipe			= 0,
	EnumSetFuelBedPipe		= 1,
	EnumNoWindNoSlopePipe	= 2,
	EnumWindSlopeMaxPipe	= 3,
	EnumGetAtAzimuthPipe	= 4	
	};
	
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/*! Type name for EnumFireSpreadPipe_ 
 *	\sa For a list of constants goto EnumFireSpreadPipe_
 */
typedef enum EnumFireSpreadPipe_ EnumFireSpreadPipe;

/*! Type name for RothPipeline_ 
 *	\sa For a list of members goto RothPipeline_
 */
typedef struct RothPipeline_ RothPipeline;

/*! \struct RothPipeline_ RothPipeline.h "RothPipeline.h"
 *	\brief structure used to store variables used in the Rothermel fire spread equation pipeline
 */
struct RothPipeline_	{
	/*! current step in fire spread prediction */
    EnumFireSpreadPipe pipe;	
	/*! dead fuel rx factors	*/
	double drx;
	/*! live fuel rx factors */
	double lrx;	
	/*! fine dead fuel ratio */				
    double fdead;
    /*! live fuel moisture extinction factor */
    double lmex;
    /*! residence time (min) */
    double taur;
    /*! propagating flux ratio */
    double ppflux;
    /*! slope parameter 'k' */				
	double slp_k;
	/*! wind parameter 'b' */				
	double wnd_b;
	/*! wind parameter (ratio**e/c) */				
	double wnd_e;
	/*! wind parameter (c * ratio**-e) */
	double wnd_k;				
	/*! d1h fuel moisture (fraction odw) */
    double d1hfm;
    /*! d10h fuel moisture (fraction odw) */				
    double d10hfm;				
    /*! d100h fuel moisture (fraction odw) */
    double d100hfm;				
	/*! lh fuel moisture (fraction odw) */
    double lhfm;				
	/*! lw fuel moisture (fraction odw) */
    double lwfm;				
	/*! wind speed (ft/min) */
    double wnd_fpm;				
	/*! wind vector (degrees from upslope) */
    double wnd_vec;				
	/*! slope (rise/run)	*/
    double slp;					
	/*! aspect (downslope) azimuth (compass degs) */
    double asp;					
    /*! reaction intensity (BTU/sqft/min) */
    double rxint;
    /*! no-wind, no-slope spread rate (ft/min) */               
    double ros_0;             
    /*! heat per unit area (BTU/sqft) */	
    double hpua;                
    /*! spread in direction of max spread (ft/min) */
    double ros_max;           	
    /*! direction of maximum spread (degrees) */
    double ros_az_max;          
    /*! effective windspeed */
    double wnd_eff;            	
    /*! length-to-width ratio for eff windspeed */
    double lwratio;             
	/*! eccentricity of ellipse for eff windspeed */
	double eccen;				
	/*! wind factor */
	double phi_w;				
	/*! slope factor	*/
	double phi_s;				
	/*! combined wind-slope factor */
	double phi_ew;				
	/*! is wind limit reached on rate-of-spread	*/
	int    wnd_lim;				
	/*! spread rate at arbitrary azimuth (ft/min) */
    double ros_any;				
    /*! direction of arbitrary spread (degrees) */
    double ros_az_any;			
	};
	 
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

/*! \fn RothPipeline * InitRothPipelineEmpty()
 * 	\brief Initilizes an empty RothPipeline
 *
 * 	\sa RothPipeline
 * 	\retval RothPipeline* Ptr to RothPipeline structure attributes initilized to default values.
 */
RothPipeline * InitRothPipelineEmpty();

/*! \fn int RothPipelineSetDefaultValues(RothPipeline * rp)
 * 	\brief Clears RothPipeline to default values.
 *
 * 	\param rp RothPipeline which has already been initialized
 *	\sa RothPipeline
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int RothPipelineSetDefaultValues(RothPipeline * rp);

/*! \fn void FreeRothPipeline(void * vptr)
 * 	\brief Frees memory associated with RothPipeline structure.
 * 
 * 	Subsequent calls to methods taking RothPipeline as argument will not work.
 *	\sa RothPipeline
 * 	\param vptr ptr to RothPipeline
 */	
void FreeRothPipeline(void * vptr);
  
#endif RothPipeline_H		/* end of RothPipeline.h */
