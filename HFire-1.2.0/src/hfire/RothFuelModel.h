/*! 
 * \file RothFuelModel.h
 * \brief Fuels description required for predictions using the Rothermel fire spread equation.
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
 
#ifndef	RothFuelModel_H
#define RothFuelModel_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Units.h"
#include "FStreamIO.h"
#include "Err.h"
#include "FuelSizeClasses.h"
#include "RothPipeline.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/* fmd file constants */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define ROTH_FM_FMD_SEPARATOR_CHARS			(" =,\t")
#define ROTH_FM_FMD_COMMENT_CHAR			("#")
#define ROTH_FM_FMD_KEYWORD_METRIC			("METRIC")
#define ROTH_FM_FMD_KEYWORD_ENGLISH			("ENGLISH")

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/* sequence of FMD file variables expected to be:								*/
/* 1H  10H  100H  LiveH LiveW  1HSAV LiveHSAV LiveWSAV Depth  XtMoist  DHt  LHt */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define ROTH_FM_FMD_D1H_LOAD				(0)
#define ROTH_FM_FMD_D10H_LOAD				(1)
#define ROTH_FM_FMD_D100H_LOAD				(2)
#define ROTH_FM_FMD_LH_LOAD					(3)
#define ROTH_FM_FMD_LW_LOAD					(4)
#define ROTH_FM_FMD_1H_SAV					(5)
#define ROTH_FM_FMD_LH_SAV					(6)
#define ROTH_FM_FMD_LW_SAV					(7)
#define ROTH_FM_FMD_FBED_DEPTH				(8)
#define ROTH_FM_FMD_EXT_MOIST				(9)
#define ROTH_FM_FMD_DHC						(10)
#define ROTH_FM_FMD_LHC						(11)
#define ROTH_FM_FMD_NUM_VALUES				(12)

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
 
/*! \def ROTH_FM_STD_TOTAL_SILICA_UNITLESS
 * \brief total silica content, ovendry (unitless)
 *
 *	Recommended attribute constant. 
 */
#define ROTH_FM_STD_TOTAL_SILICA_UNITLESS 	(0.0555)
/*! \def ROTH_FM_STD_EFF_SILICA_UNITLESS
 * \brief effective silica content, ovendry (unitless)
 *
 *	Recommended attribute constant. 
 */
#define ROTH_FM_STD_EFF_SILICA_UNITLESS		(0.0100)
/*! \def ROTH_FM_STD_FP_DENSITY_ENGLISH
 * \brief particle density (lb/ft^3)
 *
 *	Recommended attribute constant. 
 */
#define ROTH_FM_STD_FP_DENSITY_ENGLISH		(32.0)
/*! \def ROTH_FM_D10H_STD_SAV_ENGLISH
 * \brief dead 10 hour sav	(ft^2/ft^3)
 *
 *	Recommended attribute constant. 
 */
#define ROTH_FM_D10H_STD_SAV_ENGLISH		(109.0)
/*! \def ROTH_FM_D100H_STD_SAV_ENGLISH
 * \brief dead 100 hour sav (ft^2/ft^3)
 *
 *	Recommended attribute constant. 
 */
#define ROTH_FM_D100H_STD_SAV_ENGLISH		(30.0)
/*! \def ROTH_FM_ZERO_ROS_ADJ_FACTOR
 * \brief rate of spread adjustment factor (unitless)
 *
 *	Recommended attribute constant. 
 */
#define ROTH_FM_ZERO_ROS_ADJ_FACTOR			(0.0)

/*! \enum EnumRothFuelType_
 *  \brief constant used to distinguish whether RothFuelModel is burnable or unburnable
 *
 *  \note EnumRothBurnable represents burnable fuel
 *	\note EnumRothUnBurnable represents unburnable fuel
 */
enum EnumRothFuelType_	{
	EnumRothBurnable		= 0,
	EnumRothUnBurnable		= 1
	};
 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/*! Type name for EnumRothFuelType_ 
 *	\sa For a list of constants goto EnumRothFuelType_
 */
typedef enum EnumRothFuelType_ EnumRothFuelType;

/*! Type name for RothFuelModel_ 
 *	\sa For a list of members goto RothFuelModel_
 */
typedef struct RothFuelModel_ RothFuelModel;
	
/*! \struct RothFuelModel_ RothFuelModel.h "RothFuelModel.h"
 *	\brief structure used to store Rothermel-style fuel attributes 
 */
struct RothFuelModel_	{
	/*! identifies unburnable fuel types */
	EnumRothFuelType brntype;				
	/*! units values stored in */
	EnumUnitType units;						
	/*! if METRIC m, if ENGLICH ft */
	float fdepth;							
	/*! fraction of 100 (unitless) */
	float mex;								
	/*! rate-of-spread adjustment factor */
	float ros_adj;							
	/*! number of fuel particles in fuel bed */
	int   num_fp;							
    /*! if METRIC kg/m^3, if ENGLISH lb/ft^3 */
    float fdens;                			
	/*! if METRIC m^2/m^3, if ENGLISH ft^2/ft^3 */    
    float fsav;					
    /*! packing ratio, (unitless) */
    float pkrat;							
	/*! if METRIC kg/m^2, if ENGLISH lb/ft^2 */	
	float load	[EnumNumSizeClasses];		
	/*! if METRIC m^2/m^3, if ENGLISH ft^2/ft^3 	*/
	float sav	[EnumNumSizeClasses];		
	/*! if METRIC kg/m^3, if ENGLISH lb/ft^3 */	
	float dens 	[EnumNumSizeClasses];		
	/*! if METRIC J/kg, if ENGLISH BTU/lb */	
	float hc	[EnumNumSizeClasses];		
	/*! total silica content	(unitless) */	
	float stot	[EnumNumSizeClasses];		
	/*! effective silica content (unitless) */	
	float seff	[EnumNumSizeClasses];		
	/*! surface area	(ft^2/ft^2)	*/	
	float sarea	[EnumNumSizeClasses];		
	/*! effective heating number	*/	
	float effhn	[EnumNumSizeClasses];
	/*! surface area weighting factor */	
	float awtg	[EnumNumSizeClasses];
	/*! awtg contributed by live fuels */
	float larea;							
	/*! awtg contributed by dead fuels */	
	float darea;
	/*! fuel moisture (fraction odw) */
	float fm	[EnumNumSizeClasses];		
	/*! scratch vars for rate-of-spread calcs */
	RothPipeline * rp;						
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

/*! \fn RothFuelModel * InitRothFuelModelEmpty()
 * 	\brief Initializes an empty RothFuelModel.
 *
 * 	\sa RothFuelModel
 *  \retval RothFuelModel* Ptr to RothFuelModel with empty attributes.
 */
RothFuelModel * InitRothFuelModelEmpty();

/*! \fn RothFuelModel * InitRothFuelModelFMDFile(FILE * fstream, int model_num)
 * 	\brief Opens and reads a .fmd file containing fuel models and associated values.
 *
 * 	Inserts the values of model_num in RothFuelModel. 
 * 	\sa RothFuelModel 
 * 	\param fstream pointer to .fmd file
 * 	\param model_num fuel model number to search inside of .fmd file for and retrieve values
 *  \retval RothFuelModel* Ptr to RothFuelModel with attributes from .fmd file
 */
RothFuelModel * InitRothFuelModelFMDFile(FILE * fstream, int model_num);

/*! \fn RothFuelModel * InitRothFuelModelUnBurnable()
 * 	\brief Creates a fuel model with attributes that correspond to unburnable fuel.
 * 	\sa RothFuelModel 
 *  \retval RothFuelModel* Ptr to RothFuelModel with attributes of an unburnable fuel.
 */	
RothFuelModel * InitRothFuelModelUnBurnable();

/*! \fn int RothFuelModelMetricToEnglish(RothFuelModel * rfm)
 * 	\brief Takes a Ptr to existing RothFuelModel structure.
 *
 * 	Tests if data in structure is METRIC or ENGLISH units.
 * 	If METRIC, then converts data to ENGLISH.
 * 	If ENGLISH, then does nothing.
 * 	\param rfm Ptr to existing RothFuelModel structure
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int RothFuelModelMetricToEnglish(RothFuelModel * rfm);

/*! \fn int RothFuelModelEnglishToMetric(RothFuelModel * rfm)
 * 	\brief Takes a Ptr to existing RothFuelModel structure.
 *
 * 	Tests if data in structure is METRIC or ENGLISH units.
 * 	If METRIC, then does nothing.
 * 	If ENGLISH, then converts values to METRIC.
 *	\sa RothFuelModel
 * 	\param rfm Ptr to existing RothFuelModel structure
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int RothFuelModelEnglishToMetric(RothFuelModel * rfm);

/*! \fn void RothFuelModelDumpToStream(RothFuelModel * rfm, FILE * fstream)
 * 	\brief Dumps contents of RothFuelModel to stream.
 *	\sa RothFuelModel 
 * 	\param rfv ptr to RothFuelModel
 * 	\param fstream an open stream to dump to, can be stdout, stderr, or open FILE stream
 */
void RothFuelModelDumpToStream(RothFuelModel * rfm, FILE * fstream);

/*! \fn void FreeRothFuelModel(void * vptr)
 * 	\brief Frees memory associated with RothFuelModel structure.
 *
 *  Subsequent calls to methods taking RothFuelModel as argument will not work.
 *	\sa RothFuelModel 
 * 	\param vptr ptr to RothFuelModel
 */	
void FreeRothFuelModel(void * vptr);
  
#endif RothFuelModel_H		/* end of RothFuelModel.h */
