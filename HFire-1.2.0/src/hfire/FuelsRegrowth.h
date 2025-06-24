/*! 
 * \file FuelsRegrowth.h
 * \brief Routines for initializing raster fuels data.
 *  
 *	\sa Check the \htmlonly <a href="config_file_doc.html#FUELS_REGROWTH">config file documentation</a> \endhtmlonly 
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
 
#ifndef	FuelsRegrowth_H
#define FuelsRegrowth_H

#include <stdlib.h>

#include "FireYear.h"
#include "FireProp.h"
#include "FireGridData.h"
#include "ChHashTable.h"
#include "GridData.h"
#include "KeyVal.h"
#include "FStreamIO.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define FUELS_REGROWTH_RGR_COMMENT_CHAR							("#")
#define FUELS_REGROWTH_SEP_CHARS								(" ;\t")

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

/*!	\fn int GetFuelsRegrowthFIXEDFromProps(ChHashTable * proptbl, GridData * std_age, GridData ** fuels)
 * 	\brief Returns fuels raster spatial data based upon values set in Hash Table property table.
 *
 * 	For FIXED implementations fuel model number from keyword FUELS_FIXED_MODEL_NUM is 
 * 	assigned to every cell that is not NO DATA in the std_age GridData.
 *	\sa ChHashTable
 *	\sa GridData
 *	\sa Check the \htmlonly <a href="config_file_doc.html#FUELS_REGROWTH">config file documentation</a> \endhtmlonly 
 * 	\param proptbl HashTable of simulation properties
 * 	\param std_age stand age GridData
 * 	\param fuels returned fuels GridData
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */  
int GetFuelsRegrowthFIXEDFromProps(ChHashTable * proptbl, GridData * std_age, GridData ** fuels);

/*!	\fn int GetFuelsRegrowthSTATICFromProps(ChHashTable * proptbl, GridData * std_age, GridData ** fuels)
 * 	\brief Returns fuels raster spatial data based upon values set in Hash Table property table.
 *
 * 	For STATIC implementations keyword FUELS_STATIC_XXX is used to create fuel raster data.
 *	\sa ChHashTable
 *	\sa GridData
 *	\sa Check the \htmlonly <a href="config_file_doc.html#FUELS_REGROWTH">config file documentation</a> \endhtmlonly 
 * 	\param proptbl HashTable of simulation properties
 * 	\param std_age stand age GridData
 * 	\param fuels returned fuels GridData
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */  
int GetFuelsRegrowthSTATICFromProps(ChHashTable * proptbl, GridData * std_age, GridData ** fuels);

/*!	\fn int GetFuelsRegrowthPNVFromProps(ChHashTable * proptbl, GridData * std_age, GridData ** fuels)
 * 	\brief Returns fuels raster spatial data based upon values set in Hash Table property table.
 *
 * 	For PNV implementations raster data in FUELS_PNV_XXX is indexed against the 
 * 	FUELS_PNV_RGR_FILE file contents to create a new fuels array.
 *	\sa ChHashTable
 *	\sa GridData
 *	\sa Check the \htmlonly <a href="config_file_doc.html#FUELS_REGROWTH">config file documentation</a> \endhtmlonly
 * 	\param proptbl HashTable of simulation properties
 * 	\param std_age stand age GridData
 * 	\param fuels returned fuels GridData
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */  												
int GetFuelsRegrowthPNVFromProps(ChHashTable * proptbl, GridData * std_age, GridData ** fuels);
																				
#endif FuelsRegrowth_H		/* end of FuelsRegrowth.h */
