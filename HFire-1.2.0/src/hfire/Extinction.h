/*! 
 * \file Extinction.h
 * \brief Routines for extinguishing ignited cells in simulation domain after fuel has been consumed.
 *  
 * \sa Check the \htmlonly <a href="config_file_doc.html#FIRE_EXTINCTION">config file documentation</a> \endhtmlonly 
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
 
#ifndef	Extinction_H
#define Extinction_H

#include <stdlib.h>
#include <string.h>

#include "FireProp.h"
#include "CellState.h"
#include "ByteTwoDArray.h"
#include "FltTwoDArray.h"
#include "Err.h"
#include "ChHashTable.h"
#include "KeyVal.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

enum EnumExtinctionType_	{
	EnumExtinctionUnknown	= 0,
	EnumExtinctionConsume	= 1,
	EnumExtinctionReignite	= 2
	};
 
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

typedef enum EnumExtinctionType_ EnumExtinctionType;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

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

/*! \fn int UpdateExtinctionHOURS(ChHashTable * proptbl, int month, int day, int hour, CellState * cs, ByteTwoDArray * hrs_brn)
 *	\brief Extinguishes cell  if it has been ignited for longer than threshold in simulation properties table
 *	\sa Check the \htmlonly <a href="config_file_doc.html#FIRE_EXTINCTION">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of current simulation properties
 *	\param month current simulation month, 1-based index
 *	\param day current simulation day, 1-based index
 *	\param hour current simulation hour, 0000-2400 
 *	\param cs simulation CellState
 *	\param hrs_brn two-dimensional array storing number of hours cell has been ignited
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int UpdateExtinctionHOURS(ChHashTable * proptbl, int month, int day, int hour, CellState * cs, ByteTwoDArray * hrs_brn);

/*! \fn int UpdateExtinctionROS(ChHashTable * proptbl, int i, int j, double mpsros, CellState * cs, ByteTwoDArray * hrs_brn)
 *	\brief Extinguishes cell if rate of spread below threshold in simulation properties table
 *	\sa Check the \htmlonly <a href="config_file_doc.html#FIRE_EXTINCTION">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of current simulation properties
 *	\param i array row index of cell
 *	\param j array column index of cell
 *	\param mpsros rate of spread in meters per second of cell at i,j
 *	\param cs simulation CellState
 *	\param hrs_brn two-dimensional array storing number of hours cell has been ignited
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int UpdateExtinctionROS(ChHashTable * proptbl, int i, int j, double mpsros, CellState * cs, ByteTwoDArray * hrs_brn);
  
#endif Extinction_H		/* end of Extinction.h */
