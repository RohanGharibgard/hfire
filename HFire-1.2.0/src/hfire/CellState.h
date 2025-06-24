/*! 
 * \file CellState.h
 * \brief Facility for storing the state of fuel inside of each cell in the simulation domain.
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
 
#ifndef	CellState_H
#define CellState_H

#include <stdlib.h>

#include "FuelModel.h"
#include "RothFuelModel.h"
#include "PhysFuelModel.h"

#include "GridData.h"
#include "ByteTwoDArray.h"
#include "ChHashTable.h"
#include "CoordTrans.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/*! \enum EnumCellState_
 *	\brief constant storing state of fire inside of cell
 *	\note EnumUnBurnableCellState cell is not burnable
 *	\note EnumNoFireCellState cell is burnable, but has no fire in cell
 *	\note EnumHasFireCellState fire in cell, but fuel in cell is not completely consumed
 *	\note EnumConsumedCellState all fuel in cell is consumed, fire will not propagate through cell
 */
enum EnumCellState_	{
	EnumUnBurnableCellState	= 0,
	EnumNoFireCellState		= 1,
	EnumHasFireCellState	= 2,
	EnumConsumedCellState	= 3
	};
	 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/*! Type name for EnumCellState_ 
 *	\sa For a list of constants goto EnumCellState_
 */
typedef enum EnumCellState_ EnumCellState;

/*! Type name for CellState_ 
 *	\sa For a list of members goto CellState_
 */
typedef struct CellState_ CellState;

/*!	\struct CellState_ CellState.h "CellState.h"
 *	\brief structure tracking state of fuel inside of each cell in the simulation domain
 */
struct CellState_ {
	/*! real world x cordinate of lower left corner */
	double xllcorner;
	/*! real world y coordinate of lower left corner */
	double yllcorner;
	/*! cellsize in real world units */		
	int cellsize;
	/*! each cell stores value corresponding to EnumCellState */
	ByteTwoDArray * state;			
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

/*! \fn CellState * InitCellStateFuels(GridData * fuels, ChHashTable * fmtble)
 *	\brief Allocates memory for CellState structure of same size as fuels array
 *	\sa CellState
 *	\param fuels Raster of fuel model numbers
 *	\param fmtble ChHashTable of fuel model attributes
 *	\retval CellState* Ptr to initialized CellState structure
 */
CellState * InitCellStateFuels(GridData * fuels, ChHashTable * fmtble);

/*! \fn int CellStateSetCellStateRowCol(CellState * cs, int i, int j, EnumCellState state)
 *	\brief Sets the state of the cell at array i,j location
 *	\sa CellState
 *	\param cs CellState struct to be set by function
 *	\param i row index
 *	\param j column index
 *	\param state state to assign to location
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */ 
int CellStateSetCellStateRowCol(CellState * cs, int i, int j, EnumCellState state);

/*!	\fn int CellStateSetCellStateRealWorld(CellState * cs, double rwx, double rwy, EnumCellState state)
 *	\brief Sets the state of the cell at real world x,y location
 *	\sa CellState
 *	\param cs CellState struct to be set by function
 *	\param rwx real world x coordinate
 *	\param rwy real world y coordinate
 *	\param state state to assign to location
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */ 
int CellStateSetCellStateRealWorld(CellState * cs, double rwx, double rwy, EnumCellState state);

/*! \fn void FreeCellState(CellState * cs)
 *  \brief Frees memory associated with CellState structure
 *
 * 	Subsequent calls to methods taking FuelModel as argument will not work.
 *	\sa CellState
 * 	\param cs ptr to CellState
 */
void FreeCellState(CellState * cs);
  
#endif CellState_H		/* end of CellState.h */
