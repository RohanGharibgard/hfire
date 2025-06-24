/*! 
 * \file PhysFuelModel.h
 * \brief Fuels description required for physically-based predictions of fire spread.
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
 
#ifndef	PhysFuelModel_H
#define PhysFuelModel_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Units.h"
#include "FStreamIO.h"
#include "Err.h"
#include "FuelSizeClasses.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/*fmd file constants */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define PHYS_FM_FMD_SEPARATOR_CHARS			(" =,\t")
#define PHYS_FM_FMD_COMMENT_CHAR			("#")
#define PHYS_FM_FMD_KEYWORD_METRIC			("METRIC")
#define PHYS_FM_FMD_KEYWORD_ENGLISH			("ENGLISH")

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


/* sequence of FMD file variables expected to be:								*/
/* 1H  10H  100H  LiveH LiveW  1HSAV LiveHSAV LiveWSAV Depth  XtMoist  DHt  LHt */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define PHYS_FM_FMD_D1H_LOAD				(0)
#define PHYS_FM_FMD_D10H_LOAD				(1)
#define PHYS_FM_FMD_D100H_LOAD				(2)
#define PHYS_FM_FMD_LH_LOAD					(3)
#define PHYS_FM_FMD_LW_LOAD					(4)
#define PHYS_FM_FMD_1H_SAV					(5)
#define PHYS_FM_FMD_LH_SAV					(6)
#define PHYS_FM_FMD_LW_SAV					(7)
#define PHYS_FM_FMD_FBED_DEPTH				(8)
#define PHYS_FM_FMD_EXT_MOIST				(9)
#define PHYS_FM_FMD_DHC						(10)
#define PHYS_FM_FMD_LHC						(11)
#define PHYS_FM_FMD_NUM_VALUES				(12)

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


/*! \def PHYS_FM_STD_FP_DENSITY_ENGLISH
 *	\brief particle density (lb/ft^3)
 *
 *	Recommended attribute constant.
 */
#define PHYS_FM_STD_FP_DENSITY_ENGLISH		(32.0)

/*! \def PHYS_FM_D10H_STD_SAV_ENGLISH
 *	\brief dead 10 hour sav	(ft^2/ft^3)
 *
 *	Recommended attribute constant.
 */
#define PHYS_FM_D10H_STD_SAV_ENGLISH		(109.0)

/*! \def PHYS_FM_D10H_STD_SAV_ENGLISH
 *	\brief dead 100 hour sav (ft^2/ft^3)
 *
 *	Recommended attribute constant.
 */
#define PHYS_FM_D100H_STD_SAV_ENGLISH		(30.0)

/*! \enum EnumPhysFuelType_
 *  \brief constant used to distinguish whether PhysFuelModel is burnable or unburnable
 *
 *  \note EnumPhysBurnable represents burnable fuel
 *	\note EnumPhysUnBurnable represents unburnable fuel
 */
enum EnumPhysFuelType_	{
	EnumPhysBurnable		= 0,
	EnumPhysUnBurnable		= 1
	};

/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/*! Type name for EnumPhysFuelType_ 
 *	\sa For a list of constants goto EnumPhysFuelType_
 */
typedef enum EnumPhysFuelType_ EnumPhysFuelType;

/*! Type name for PhysFuelModel_ 
 *	\sa For a list of members goto PhysFuelModel_
 */
typedef struct PhysFuelModel_ PhysFuelModel;

/*! \struct PhysFuelModel_ PhysFuelModel.h "PhysFuelModel.h"
 *  \brief structure used to store fuel attributes used by physically-based fire spread models
 */
struct PhysFuelModel_	{
	/*! identifies unburnable fuel types */
	EnumPhysFuelType brntype;
	/*! units values stored in */
	EnumUnitType units;					
	/*! if METRIC m, if ENGLICH ft */
	float fdepth;
	/*! number of fuel particles in fuel bed */						
	int num_fp;							
	/*! if METRIC kg/m^3, if ENGLISH lb/ft^3 */
	float fdens;
	/*! if METRIC m^2/m^3, if ENGLISH ft^2/ft^3 */						
	float fsav;
	/*! packing ratio, (unitless) */
	float pkrat;						
	/*! if METRIC kg/m^2, if ENGLISH lb/ft^2 */
	float load	[EnumNumSizeClasses];
	/*! if METRIC m^2/m^3, if ENGLISH ft^2/ft^3 */
	float sav	[EnumNumSizeClasses];
	/*! if METRIC kg/m^3, if ENGLISH lb/ft^3 */	
	float dens 	[EnumNumSizeClasses];	
	/*! surface area	(ft^2/ft^2)	*/	
	float sarea	[EnumNumSizeClasses];
	/*! effective heating number */	
	float effhn	[EnumNumSizeClasses];
	/*! surface area weighting factor */
	float awtg	[EnumNumSizeClasses];
	/*! awtg contributed by live fuels */
	float larea;
	/*! awtg contributed by dead fuels */						
	float darea;						
	/*! fuel moisture (fraction odw) */	
	float fm	[EnumNumSizeClasses];	
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

/*! \fn PhysFuelModel * InitPhysFuelModelEmpty()
 *	\brief Initializes an empty PhysFuelModel
 *
 *	\sa PhysFuelModel .
 *	\retval PhysFuelModel* Ptr to PhysFuelModel with empty attributes
 */
PhysFuelModel * InitPhysFuelModelEmpty();

/*! \fn PhysFuelModel * InitPhysFuelModelFMDFile(FILE * fstream, int model_num)
 * 	\brief Opens and reads a .fmd file containing fuel models and associated values
 *
 * 	Inserts the values of model_num in PhysFuelModel.
 * 	\sa PhysFuelModel
 *	\param fstream pointer to .fmd file
 * 	\param model_num fuel model number to search inside of .fmd file for and retrieve values
 * 	\retval PhysFuelModel* Ptr to PhysFuelModel with attributes from .fmd file
 */
PhysFuelModel * InitPhysFuelModelFMDFile(FILE * fstream, int model_num);

/*! \fn PhysFuelModel * InitPhysFuelModelUnBurnable()
 * 	\brief Creates a fuel model with attributes that correspond to unburnable fuel
 * 	\sa PhysFuelModel
 * 	\retval PhysFuelModel* Ptr to PhysFuelModel with attributes of an unburnable fuel.
 */
PhysFuelModel * InitPhysFuelModelUnBurnable();

/*!	\fn int PhysFuelModelMetricToEnglish(PhysFuelModel * pfm)
 * 	\brief Converts units of attributes in existing PhysFuelModel structure.
 * 
 *	Tests if data in structure is METRIC or ENGLISH units.
 * 	If METRIC, then converts data to ENGLISH.
 * 	If ENGLISH, then does nothing.
 *	\sa PhysFuelModel
 * 	\param pfm Ptr to existing PhysFuelModel structure
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int PhysFuelModelMetricToEnglish(PhysFuelModel * pfm);

/*! \fn int PhysFuelModelEnglishToMetric(PhysFuelModel * pfm)
 * 	\brief Converts units of attributes in existing PhysFuelModel structure.
 * 
 * 	Tests if data in structure is METRIC or ENGLISH units.
 * 	If METRIC, then does nothing.
 * 	If ENGLISH, then converts values to METRIC.
 *	\sa PhysFuelModel
 * 	\param pfm Ptr to existing PhysFuelModel structure
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int PhysFuelModelEnglishToMetric(PhysFuelModel * pfm);

/*!	\fn void PhysFuelModelDumpToStream(PhysFuelModel * pfm, FILE * fstream)
 * 	\brief Dumps contents of PhysFuelModel to stream.
 *	\sa PhysFuelModel
 * 	\param rfv ptr to PhysFuelModel
 * 	\param fstream an open stream to dump to, can be stdout, stderr, or open FILE stream
 */
void PhysFuelModelDumpToStream(PhysFuelModel * pfm, FILE * fstream);

/*! \fn void FreePhysFuelModel(void * vptr)
 * 	\brief Frees memory associated with PhysFuelModel structure.
 *
 * 	Subsequent calls to methods taking PhysFuelModel as argument will not work.
 *	\sa PhysFuelModel
 * 	\param vptr ptr to PhysFuelModel
 */	
void FreePhysFuelModel(void * vptr);
  
#endif PhysFuelModel_H		/* end of PhysFuelModel.h */
