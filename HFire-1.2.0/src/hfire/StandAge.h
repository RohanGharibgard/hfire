/*! 
 * \file StandAge.h
 * \brief Routines for incrementing the age of each cell in the simulation at the conclusion of a fire season.
 *  
 *	\sa Check the \htmlonly <a href="config_file_doc.html#STAND_AGE">config file documentation</a> \endhtmlonly 
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
 
#ifndef	StandAge_H
#define StandAge_H

#include <stdlib.h>

#include "GridData.h"
#include "FireYear.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */
 
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

/*! \fn int IncrementStandAge(FireYear * fy, GridData * std_age)
 *	\brief If cell was unburned during recent fire season, age of cell incremented. Otherwise age of cell reset to 0.
 *
 *	\sa FireYear
 *	\sa GridData
 *	\sa Check the \htmlonly <a href="config_file_doc.html#STAND_AGE">config file documentation</a> \endhtmlonly 
 *	\param fy FireYear of fire perimeters for completed fire season
 *	\param std_age raster of current stand age, values will be updated upon successful return
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode 
 */	
int IncrementStandAge(FireYear * fy, GridData * std_age);

#endif StandAge_H		/* end of StandAge.h */
