/*! 
 * \file FuelModel.h
 * \brief Fuels description which contains attribute sets used by several types of fire spread models.
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
 
#ifndef FuelModel_H
#define FuelModel_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Err.h"
#include "FuelSizeClasses.h"
#include "RothFuelModel.h"
#include "PhysFuelModel.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/*! \def FUELMODEL_UNNAMED_MODEL
 *	\brief default name assigned to FuelModels 
 */
#define FUELMODEL_UNNAMED_MODEL						("UNNAMED")

/*! \enum FuelModelType_
 *	\brief constant used to distinguish different types of fuel models
 *	\note EnumUnknownFuelModelType unrecognized attribute set
 *	\note EnumRoth Rothermel-style fuel attribute set
 *	\note EnumPhys physically-based model attribute set
 */
enum FuelModelType_		{
	EnumUnknownFuelModelType 	= 0,
	EnumRoth 					= 1,
	EnumPhys 					= 2
	};
	
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/*! Type name for FuelModelType_ 
 *	\sa For a list of constants goto FuelModelType_
 */
typedef enum FuelModelType_ FuelModelType;

/*! Type name for FuelModel_ 
 *	\sa For a list of members goto FuelModel_
 */
typedef struct FuelModel_ FuelModel;

/*! \struct FuelModel_ FuelModel.h "FuelModel.h"
 *	\brief structure containing attribute sets for various types of fire spread models
 */
struct FuelModel_		{
	/*! integer number assigned to model */
	int model_num;							
	/*! (optional) user-specified model name */
	char * model_name;						
	/*! (optional) user-specified model description */
	char * model_desc;						
	/*! identifies the attributes available in struct */ 	
	FuelModelType type;						
	/*! ptr to fuel attribs for Roth-style fire predict */
	RothFuelModel * rfm;					
	/*! ptr to fuel attribs for phys-based fire predict */
	PhysFuelModel * pfm;					
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

/*! \fn FuelModel * InitFuelModelEmpty(int fm_num, char * fm_name, char * fm_desc)
 *  \brief Initializes an empty FuelModel structure.
 *
 *  Does not initialize structure members (eg RothFuelModel or PhysFuelModel) containing attributes.
 *	\sa FuelModel
 *  \sa RothFuelModel
 *  \sa PhysFuelModel
 *  \param fm_num number of newly initialized FuelModel
 *  \param fm_name name opf newly initialized FuelModel
 *  \param fm_desc description of newly initialized FuelModel
 *  \retval FuelModel* Ptr to initialized FuelModel structure
 */	
FuelModel * InitFuelModelEmpty(int fm_num, char * fm_name, char * fm_desc);

/*! \fn FuelModel * InitFuelModelFMDFile(int fm_num, char * fm_name, char * fm_desc, char * fmd_fname, FuelModelType type)
 *  \brief Initializes the the FuelModel structure with attributes from a .FMD file.
 *
 *  If FuelModelType is EnumRoth, then RothFuelModel structure populated.
 *  If FuelModelType is EnumPhys, then PhysFuelModel structure populated.
 *	\sa FuelModel 
 *  \sa RothFuelModel
 *  \sa PhysFuelModel 
 *  \param fm_num number of newly initialized FuelModel
 *  \param fm_name name opf newly initialized FuelModel
 *  \param fm_desc description of newly initialized FuelModel
 *  \param fmd_fname name of fmd file containing FuelModel attributes
 *  \param type which style of attributes (Roth or Phys) should be populated
 *  \retval FuelModel* Ptr to initialized FuelModel structure
 */	
FuelModel * InitFuelModelFMDFile(int fm_num, char * fm_name, char * fm_desc, char * fmd_fname, FuelModelType type);

/*! \fn FuelModel * InitFuelModelUnBurnable(int fm_num, char * fm_name, char * fm_desc, FuelModelType type)
 * 	\brief Initializes the the FuelModel structure as unburnable fuel.
 *
 * 	If FuelModelType is EnumRoth, then RothFuelModel structure populated.
 * 	Else FuelModelType is EnumPhys, then PhysFuelModel structure populated.
 *	\sa FuelModel  
 * 	\param fm_num number of newly initialized FuelModel
 * 	\param fm_name name opf newly initialized FuelModel
 * 	\param fm_desc description of newly initialized FuelModel
 * 	\param type which style of attributes (Roth or Phys) should be populated 
 *  \retval FuelModel* Ptr to initialized FuelModel structure
 */	
FuelModel * InitFuelModelUnBurnable(int fm_num, char * fm_name, char * fm_desc, FuelModelType type);

/*! \fn void FuelModelDumpToStream(FuelModel * fm, FILE * fstream)
 * 	\brief Dumps contents of Fuel Model to stream.
 *	\sa FuelModel
 * 	\param fm ptr to FuelModel
 * 	\param fstream an open stream to dump to, can be stdout, stderr, or open FILE stream
 */
void FuelModelDumpToStream(FuelModel * fm, FILE * fstream);

/*! \fn int CmpNumToFuelModelNum(const void * fm_num, const void * fm)
 * \brief Tests if model num given by first argument is same as model num in FuelModel.
 *
 * Can be passed as function pointer for comparison tests.
 * \param fm_num an int fuel model number to match to
 * \param fm ptr to FuelModel
 * \retval int 1 if numbers match, else 0
 */
int CmpNumToFuelModelNum(const void * fm_num, const void * fm);

/*! \fn void FreeFuelModel(void * vptr)
 * 	\brief Frees memory associated with FuelModel structure.
 *
 * 	Subsequent calls to methods taking FuelModel as argument will not work.
 *	\sa FuelModel 
 * 	\param vptr ptr to FuelModel
 */
void FreeFuelModel(void * vptr);
		
#endif FuelModel_H	/* end of FuelModel.h */
