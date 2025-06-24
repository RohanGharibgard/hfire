/*! 
 * \file FireProp.h
 * \brief Constant property keys and enumerated values used by the simulation HashTable.
 *  
 * \sa Check the \htmlonly <a href="config_file_doc.html">config file documentation</a> \endhtmlonly 
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
 
#ifndef	FireProp_H
#define FireProp_H

#include <stdlib.h>

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/* maximum size of any of the property keywords */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define FIRE_PROP_MAX_SIZE_KEYWORD								(64)

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/*! \enum EnumFireProp_
 *	\brief constant property keys used to retrieve values from simulation properties ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html">config file documentation</a> \endhtmlonly
 */
enum EnumFireProp_ {
	PROP_LOW_BOUND 	= -1,				/* DO NOT EDIT- LOWER ENUMERATION BOUNDS */
	PROP_SIMSTYR 	  = 0, 				/*"SIMULATION_START_YEAR"*/
	PROP_SIMSTMO 	  = 1,				/*"SIMULATION_START_MONTH"*/
	PROP_SIMSTDY 	  = 2,				/*"SIMULATION_START_DAY"*/
	PROP_SIMSTHR	  = 3,				/*"SIMULATION_START_HOUR"*/
	PROP_SIMEDYR 	  = 4, 				/*"SIMULATION_END_YEAR"*/	
	PROP_SIMEDMO 	  = 5, 				/*"SIMULATION_END_MONTH"*/	
	PROP_SIMEDDY 	  = 6, 				/*"SIMULATION_END_DAY"*/		
	PROP_SIMEDHR	  = 7,				/*"SIMULATION_END_HOUR"*/
	PROP_SIMTSSEC	  = 8,				/*"SIMULATION_TIMESTEP_SECS"*/	
	PROP_SIMRNGSD	  = 9,				/*"SIMULATION_RAND_NUM_SEED"*/
	PROP_FPTYP		  = 10,				/*"FUELS_PROPS_TYPE" */
	PROP_FPFMD		  = 11,				/*"FUELS_PROPS_FMD_FILE"	*/
	PROP_FPIMP		  = 12,				/*"FUELS_PROPS_FM_NUMS_IMPORT"*/
	PROP_FPUNB		  = 13,				/*"FUELS_PROPS_FM_NUMS_UNBURNABLE"*/
	PROP_ELEVFMT	  = 14,				/*"ELEV_RASTER_FORMAT"*/	
	PROP_ELEVMN		  = 15,				/*"ELEV_RASTER_MAIN_FILE"*/		
	PROP_ELEVHDR	  = 16, 			/*"ELEV_RASTER_HEADER_FILE"*/	
	PROP_ELEVTYP	  = 17, 			/*"ELEV_RASTER_TYPE"*/		
	PROP_SLPFMT		  = 18, 			/*"SLOPE_RASTER_FORMAT"*/
	PROP_SLPMN		  = 19,				/*"SLOPE_RASTER_MAIN_FILE"*/	
	PROP_SLPHDR		  = 20, 			/*"SLOPE_RASTER_HEADER_FILE"*/	
	PROP_SLPTYP		  = 21, 			/*"SLOPE_RASTER_TYPE"*/			
	PROP_ASPFMT		  = 22, 			/*"ASPECT_RASTER_FORMAT"*/
	PROP_ASPMN		  = 23, 			/*"ASPECT_RASTER_MAIN_FILE"*/	
	PROP_ASPHDR		  = 24, 			/*"ASPECT_RASTER_HEADER_FILE"*/
	PROP_ASPTYP		  = 25, 			/*"ASPECT_RASTER_TYPE"*/		
	PROP_EXPFREQ	  = 26,				/*"EXPORT_FREQUENCY"*/
	PROP_EXPFIDDIR	= 27,				/*"EXPORT_FIRE_ID_RASTER_DIR"*/
	PROP_EXPFUELDIR	= 28,				/*"EXPORT_FUELS_RASTER_DIR"*/
	PROP_EXPSAGEDIR	= 29,				/*"EXPORT_STAND_AGE_RASTER_DIR"*/
	PROP_EXPFAREAF	= 30,				/*"EXPORT_FIRE_AREA_FILE"*/
	PROP_EXPFPERMF	= 31,				/*"EXPORT_FIRE_PERIMTER_FILE"*/
	PROP_EXPIGLCF	  = 32,				/*"EXPORT_IGNITION_LOCS_FILE"*/
	PROP_EXPSANAEVF	= 33,				/*"EXPORT_SANTA_ANA_EVT_FILE"*/	
	PROP_EXPFPDIR	  = 34,				/*"EXPORT_FIRE_ID_PNG_DIRECTORY"*/
	PROP_EXPFPICMF	= 35,				/*"EXPORT_FIRE_ID_PNG_ICM_FILE"*/
	PROP_EXPFPWID	  = 36,				/*"EXPORT_FIRE_ID_PNG_IMG_WIDTH"*/
	PROP_EXPFPHGT	  = 37,				/*"EXPORT_FIRE_ID_PNG_IMG_HGT"*/
	PROP_EXPFPTTXT	= 38,				/*"EXPORT_FIRE_ID_PNG_TITLE_TXT"*/
	PROP_EXPFPTF	  = 39,				/*"EXPORT_FIRE_ID_PNG_TITLE_FNT"*/
	PROP_EXPFPTP	  = 40,				/*"EXPORT_FIRE_ID_PNG_TITLE_POS"*/
	PROP_FEXTYP		  = 41,				/*"FIRE_EXTINCTION_TYPE"*/
	PROP_FEXHR		  = 42,				/*"FIRE_EXTINCTION_HOURS"*/
	PROP_FEXROS		  = 43,				/*"FIRE_EXTINCTION_ROS_MPS"*/
	PROP_RGRTYP		  = 44, 			/*"FUELS_REGROWTH_TYPE"*/
	PROP_FIXFNUM	  = 45, 			/*"FUELS_FIXED_MODEL_NUM"*/
	PROP_RGRFILE	  = 46, 			/*"FUELS_PNV_RGR_FILE"*/
	PROP_PNVFMT		  = 47, 			/*"FUELS_PNV_RASTER_FORMAT"*/
	PROP_PNVMN		  = 48,				/*"FUELS_PNV_RASTER_MAIN_FILE"*/
	PROP_PNVHDR		  = 49, 			/*"FUELS_PNV_RASTER_HEADER_FILE"*/
	PROP_PNVTYP		  = 50, 			/*"FUELS_PNV_RASTER_TYPE"*/
	PROP_STATFMT	  = 51, 			/*"FUELS_STATIC_RASTER_FORMAT"*/
	PROP_STATMN		  = 52, 			/*"FUELS_STATIC_RASTER_MAIN_FILE"*/
	PROP_STATHDR	  = 53, 			/*"FUELS_STATIC_RASTER_HEADER_FILE"*/
	PROP_STATTYP	  = 54, 			/*"FUELS_STATIC_RASTER_TYPE"*/
	PROP_SAGETYP	  = 55,				/*"STAND_AGE_TYPE"*/
	PROP_SAGEFIX 	  = 56,				/*"STAND_AGE_FIXED_AGE"*/	
	PROP_SAGEFMT 	  = 57,				/*"STAND_AGE_RASTER_FORMAT"*/
	PROP_SAGEMN 	  = 58,				/*"STAND_AGE_RASTER_MAIN_FILE"*/
	PROP_SAGEHDR 	  = 59,				/*"STAND_AGE_RASTER_HEADER_FILE"*/
	PROP_SAGERTYP   = 60,				/*"STAND_AGE_RASTER_TYPE"*/	
	PROP_IGTYP		  = 61, 			/*"IGNITION_TYPE"*/
	PROP_IGFILE		  = 62, 			/*"IGNITION_FIXED_IGS_FILE"*/
	PROP_IGFREQYR	  = 63, 			/*"IGNITION_FREQUENCY_PER_YEAR"*/
	PROP_IGRSPFMT	  = 64, 			/*"IGNITION_RSP_RASTER_FORMAT"*/
	PROP_IGRSPMN	  = 65, 			/*"IGNITION_RSP_RASTER_MAIN_FILE"*/
	PROP_IGRSPHDR	  = 66, 			/*"IGNITION_RSP_RASTER_HEADER_FILE"*/
	PROP_IGRSPTYP	  = 67, 			/*"IGNITION_RSP_RASTER_TYPE"*/	
	PROP_WAZTYP		  = 68, 			/*"WIND_AZIMUTH_TYPE"*/
	PROP_WAZHFILE	  = 69, 			/*"WIND_AZIMUTH_HISTORICAL_FILE"*/
	PROP_WAZFFILE	  = 70, 			/*"WIND_AZIMUTH_FIXED_FILE"*/
	PROP_WAZSPFILE	= 71, 			/*"WIND_AZIMUTH_SPATIAL_FILE"*/	
	PROP_WSPDTYP	  = 72, 			/*"WIND_SPEED_TYPE"*/
	PROP_WSPDHFILE	= 73, 			/*"WIND_SPEED_HISTORICAL_FILE"*/
	PROP_WSPDFFILE	= 74, 			/*"WIND_SPEED_FIXED_FILE"*/
	PROP_WSPDSPFILE	= 75, 			/*"WIND_SPEED_SPATIAL_FILE"*/
	PROP_WSPDUNIRNG = 76,				/*"WIND_SPEED_UNIFORM_RANGE"*/
	PROP_DFMTYP		  = 77, 			/*"DEAD_FUEL_MOIST_TYPE"*/
	PROP_DFMHFILE	  = 78, 			/*"DEAD_FUEL_MOIST_HISTORICAL_FILE"*/
	PROP_DFMFFILE	  = 79, 			/*"DEAD_FUEL_MOIST_FIXED_FILE"*/
	PROP_DFMSPFILE	= 80, 			/*"DEAD_FUEL_MOIST_SPATIAL_FILE"*/
	PROP_LFMTYP		  = 81,				/*"LIVE_FUEL_MOIST_TYPE"*/
	PROP_LFMHFILE	  = 82,				/*"LIVE_FUEL_MOIST_HERB_FILE"*/
	PROP_LFMWFILE	  = 83,				/*"LIVE_FUEL_MOIST_WOOD_FILE"*/
	PROP_LFMSPFILE	= 84,				/*"LIVE_FUEL_MOIST_SPATIAL_FILE"*/
	PROP_SANAFREQ	  = 85,				/*"SANTA_ANA_FREQUENCY_PER_YEAR"*/
	PROP_SANANUMD	  = 86,				/*"SANTA_ANA_NUM_DAYS_DURATION"*/
	PROP_SANAWAZF	  = 87,				/*"SANTA_ANA_WIND_AZIMUTH_FILE"*/
	PROP_SANAWSPDF	= 88,				/*"SANTA_ANA_WIND_SPEED_FILE"*/
	PROP_SANADFMF	  = 89,				/*"SANTA_ANA_DEAD_FUEL_MOIST_FILE"*/
  PROP_FEAF       = 90,	      /*"FIRE_ELLIPSE_ADJUSTMENT_FACTOR"*/
  PROP_WSPWAF     = 91,       /*"WIND_SPEED_WIND_ADJUSTMENT_FACTOR"*/
  PROP_DFMD1HINC  = 92,       /*"DEAD_FUEL_MOIST_D1H_INCREMENT"*/
  PROP_DFMD100HINC = 93,      /*"DEAD_FUEL_MOIST_D100H_INCREMENT"*/
  PROP_EXPFINOF   = 94,       /*"EXPORT_FIRE_INFO_FILE"*/
  PROP_FFIGNCELLS = 95,       /*"FIRE_FAILED_IGNITION_NUM_CELLS"*/
  PROP_EXPSADIR   = 96,       /*"EXPORT_SANTA_ANA_RASTER_DIR"*/
  PROP_EXPAABHF   = 97,       /*"EXPORT_AGE_AT_BURN_HIST_FILE"*/
	PROP_UP_BOUND	  = 98				/* DO NOT EDIT- UPPER ENUMERATION BOUNDS */	
};

/*! \enum EnumFireVal_
 *	\brief constant enumerated values stored in simulation properties ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html">config file documentation</a> \endhtmlonly 
 */
enum EnumFireVal_	{
	VAL_LOW_BOUND 	= -1,				/* DO NOT EDIT- LOWER ENUMERATION BOUNDS */
	VAL_ASCII		    = 0,				/*"ASCII"*/
	VAL_BINARY		  = 1,				/*"BINARY"*/
	VAL_NULL		    = 2,				/*"NULL"*/
	VAL_DOUBLE		  = 3,				/*"DOUBLE"*/
	VAL_FLOAT		    = 4,				/*"FLOAT"*/
	VAL_LONGINT		  = 5,				/*"LONGINT"*/
	VAL_INT			    = 6,				/*"INT"*/
	VAL_BYTE		    = 7,				/*"BYTE"*/
	VAL_FIXED		    = 8,				/*"FIXED"*/
	VAL_PNV			    = 9,				/*"PNV"*/
	VAL_STATIC		  = 10,				/*"STATIC"*/
	VAL_SPATIAL		  = 11,				/*"SPATIAL"*/
	VAL_RANDU		    = 12,				/*"RANDOM_UNIFORM"*/
	VAL_RANDH		    = 13,				/*"RANDOM_HISTORICAL"*/
	VAL_RANDS		    = 14,				/*"RANDOM_SPATIAL"*/
	VAL_TABLE		    = 15,				/*"TABLE"*/
	VAL_TIMESTEP	  = 16,				/*"TIMESTEP"*/
	VAL_DAILY		    = 17,				/*"DAILY"*/
	VAL_ANNUAL		  = 18,				/*"ANNUAL"*/
	VAL_TINY		    = 19,				/*"TINY"*/
	VAL_SMALL		    = 20,				/*"SMALL"*/
	VAL_MEDBOLD		  = 21,				/*"MEDBOLD"*/
	VAL_LARGE		    = 22,				/*"LARGE"*/
	VAL_GIANT		    = 23,				/*"GIANT"*/
	VAL_UL			    = 24,				/*"UL"*/
	VAL_LL			    = 25,				/*"LL"*/
	VAL_LR			    = 26,				/*"LR"*/
	VAL_UR			    = 27,				/*"UR"*/
	VAL_CONS		    = 28,				/*"CONSUME"*/
	VAL_REIG		    = 29,				/*"REIGNITE"*/
	VAL_ROTH		    = 30,				/*"ROTH"*/
	VAL_PHYS		    = 31,				/*"PHYS"*/	
  VAL_AB79        = 32,       /*"AB79"*/
  VAL_BHP         = 33,       /*"BHP"*/
  VAL_NOWAF       = 34,       /*"NOWAF"*/
	VAL_UP_BOUND	  = 35				/* DO NOT EDIT- UPPER ENUMERATION BOUNDS */		
};
	 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/*! Type name for EnumFireProp_ 
 *	\sa For a list of constants goto EnumFireProp_
 */ 
typedef enum EnumFireProp_ EnumFireProp;

/*! Type name for EnumFireVal_ 
 *	\sa For a list of constants goto EnumFireVal_
 */
typedef enum EnumFireVal_ EnumFireVal;

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

/*! \fn const char * GetFireProp(EnumFireProp p)
 *	\brief Returns descriptive string associated with constant property
 *	\sa EnumFireProp 
 *	\sa Check the \htmlonly <a href="config_file_doc.html">config file documentation</a> \endhtmlonly 
 *	\param p One of EnumFireProp constants
 *	\retval char* descriptive C-style string associated with constant
 */
const char * GetFireProp(EnumFireProp p);

/*! \fn const char * GetFireVal(EnumFireVal v)
 *	\brief Returns descriptive string associated with constant enumerated value
 *	\sa EnumFireVal 
 *	\sa Check the \htmlonly <a href="config_file_doc.html">config file documentation</a> \endhtmlonly 
 *	\param p One of EnumFireVal constants
 *	\retval char* descriptive C-style string associated with constant
 */
const char * GetFireVal(EnumFireVal v);
 
#endif FireProp_H		/* end of FireProp.h */
