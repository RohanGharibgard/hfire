/*! 
 * \file FireRoth1972Config.h
 * \brief Routines for reading fuel model attributes from .FMD files into simulation.
 *  
 * \sa Check the \htmlonly <a href="config_file_doc.html#FUEL_PROPS">config file documentation</a> \endhtmlonly  
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
 
#ifndef	FireRoth1972Config_H
#define FireRoth1972Config_H

#include <stdlib.h>

#include "FuelModel.h"
#include "RothFuelModel.h"
#include "Roth1972.h"
#include "FireProp.h"

#include "ChHashTable.h"
#include "Err.h"
#include "FStreamIO.h"
#include "KeyVal.h"
#include "List.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define FIRE_ROTH1972_CONFIG_FUEL_SEP_CHARS					(" ;")

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

/*! \fn int InitRothFuelModelListFromProps(ChHashTable * proptbl, List ** fmlist)
 *	\brief Creates a List of FuelModels with Rothermel-style attributes by reading 
 *			properties from a .FMD file
 *	\sa FuelModel
 *	\sa ChHashTable
 *	\sa List
 *	\sa Check the \htmlonly <a href="config_file_doc.html#FUEL_PROPS">config file documentation</a> \endhtmlonly 
 *	\param proptbl ChHashTable of simulation properties
 *	\param fmlist if function returns without error, initialized List of FuelModels
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */  
int InitRothFuelModelListFromProps(ChHashTable * proptbl, List ** fmlist);
  
#endif FireRoth1972Config_H		/* end of FireRoth1972Config.h */
