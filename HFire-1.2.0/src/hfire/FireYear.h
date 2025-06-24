/*! 
 * \file FireYear.h
 * \brief Facility for storing all active fires in the simulation domain during a single year.
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
 
#ifndef FireYear_H
#define FireYear_H

#include <stdlib.h>

#include "FireTimer.h"
#include "GridData.h"
#include "IntTwoDArray.h"
#include "ChHashTable.h"
#include "FireProp.h"
#include "KeyVal.h"
#include "FuelModel.h"
#include "CoordTrans.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/*!	\def FIRE_YEAR_ID_UNBURNABLE
 *	\brief identifies cells in the fire ID array that are outside domain 
 */
#define FIRE_YEAR_ID_UNBURNABLE         (-9999)

/*!	\def FIRE_YEAR_ID_DEFAULT
 *	\brief identifies the default value assigned to all burnable cells in domain 
 */
#define FIRE_YEAR_ID_DEFAULT            (0)

/*! \def FIRE_YEAR_ID_MAX
 *  \brief largest permitted fire ID, limits the max number of fires in a year
 */
#define FIRE_YEAR_ID_MAX                (4096)

/*! \def FIRE_YEAR_CELL_NOT_BURNED
 *  \brief cell is burnable, but did not burn
 */
#define FIRE_YEAR_CELL_NOT_BURNED       (0)

/*! \def FIRE_YEAR_CELL_BURNED_NO_SA
 *  \brief cell burned, but not during a Santa Ana event
 */
#define FIRE_YEAR_CELL_BURNED_NO_SA     (1)

/*! \def FIRE_YEAR_CELL_BURNED_SA
 *  \brief cell burned during a Santa Ana event
 */
#define FIRE_YEAR_CELL_BURNED_SA        (2)

/*! \def FIRE_YEAR_CELL_UNBURNABLE
 *  \brief cell is unburnable
 */
#define FIRE_YEAR_CELL_UNBURNABLE       (-9999)

/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/*! Type name FireInfo_
 *  \sa For a list of members goto FireInfo_
 */
typedef struct FireInfo_ FireInfo;

/*! \struct FireInfo_ FireYear.h "FireYear.h"
 *	\brief structure used to store metadata about a single fire
 */
struct FireInfo_ {
  /*! unique ID associated with fire */
  int       id;
  /*! fire point of origin, real-world x coordinate */
  double    rwx;
  /*! fire point of origin, real-world y coordinate */
  double    rwy;
  /*! start time of fire */
  int       start_yr;
  int       start_mo;
  int       start_dy;
  int       start_hr;
  /*! end time of fire */
  int       end_yr;
  int       end_mo;
  int       end_dy;
  int       end_hr;
  /*! number of cells burned */
  long int  num_cells_burned;
  /*! flag to indicate whether this fire was a 'failed ignition' */
  int       is_failed_ig;
  /*! number of cells burned during a Santa Ana */
  long int  num_cells_burned_sa;
};

/*! Type name for FireYear_ 
 *	\sa For a list of members goto FireYear_ 
 */
typedef struct FireYear_ FireYear;

/*! \struct FireYear_ FireYear.h "FireYear.h"
 *	\brief structure tracking the growth of all fires occuring during a single year of simulation
 */
struct FireYear_ {
	/*! year for which this structure represents fires */
	int year;						
	/*! number of fires associated with this year */	
	int num_fires;					
	/*! real world x cordinate of lower left corner */	
	double xllcorner;				
	/*! real world y coordinate of lower left corner */	
	double yllcorner;				
	/*! cellsize in real world units */	
	int cellsize;					
	/*! each contiguous fire area stored with unique ID */			
	IntTwoDArray * id;
  /*! santa ana fire history */
  IntTwoDArray * santa_ana;
  /*! array of FireInfo structures */
  FireInfo finfo[FIRE_YEAR_ID_MAX+1];
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

/*!	\fn FireYear * InitFireYearFuels(int year, GridData * fuels, ChHashTable * fmtble)
 * 	\brief Allocate memory for a FireYear structure with an underlying fire id array of size matching fuels GridData supplied as argument.
 *
 * 	Upon initialization all cells in underlying fire id array are set to FIRE_YEAR_ID_DEFAULT, 
 * 	indicating that no fire has occured in that cell for the given year.  In the case of cells
 * 	in the fuels GridData which represent unburnable fuel, the fire is reset to 
 * 	FIRE_YEAR_ID_UNBURNABLE to prevent the cell from being set as burned during the simulation.
 *	\sa FireYear
 *	\sa ChHashTable
 *	\sa GridData
 * 	\param year year for which this FireYear structure describes the fire history
 * 	\param fuels fuels spatial data
 * 	\param fmtble ChHashTable of FuelModels used to determine if num from fuels is burnable
 * 	\retval FireYear* Ptr to initialized FireYear structure
 */	
FireYear * InitFireYearFuels(int year, GridData * fuels, ChHashTable * fmtble);

/*! \fn int FireYearGetCellIDRowCol(FireYear * fy, int i, int j, int * id)
 *	\brief Retrieves the id of the fire ignited inside of a cell at index i,j, if any.
 *	\sa FireYear
 *	\param fy FireYear of all active fires
 *	\param i row index of cell to retrieve id
 *	\param j column index of cell to retrieve id
 *	\param id id of fire returned as dereferenced value 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int FireYearGetCellIDRowCol(FireYear * fy, int i, int j, int * id);

/*! \fn int FireYearGetCellIDRealWorld(FireYear * fy, double rwx, double rwy, int * id)
 *	\brief Retrieves the id of the fire ignited inside of a cell at real world coordinate x,y, if any.
 *	\sa FireYear 
 *	\param fy FireYear of all active fires
 *	\param rwx real world x coordinate to retrieve id
 *	\param rwy real world y coordinate to retrieve id
 *	\param id id of fire returned as dereferenced value 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int FireYearGetCellIDRealWorld(FireYear * fy, double rwx, double rwy, int * id);

/*! \fn int FireYearIsCellBurnedRowCol(FireYear * fy, int i, int j)
 *	\brief Returns 1 if the cell at index i,j is ignited, 0 otherwise.
 *	\sa FireYear 
 *	\param fy FireYear of all active fires
 *	\param i row index of cell to examine
 *	\param j column index of cell to examine
 *	\retval int 1 if cell is ignited, 0 otherwise
 */
int FireYearIsCellBurnedRowCol(FireYear * fy, int i, int j);

/*! \fn int FireYearIsCellBurnedRealWorld(FireYear * fy, double rwx, double rwy)
 *	\brief Returns 1 if the cell at real world coordinate x,y is ignited, 0 otherwise.
 *	\sa FireYear 
 *	\param fy FireYear of all active fires
 *	\param rwx real world x coordinate to examine
 *	\param rwy real world y coordinate to examine
 *	\retval int 1 if cell is ignited, 0 otherwise
 */
int FireYearIsCellBurnedRealWorld(FireYear * fy, double rwx, double rwy);

/*! \fn int FireYearSetCellNewFireIDRealWorld(FireYear * fy, double rwx, double rwy)
 *	\brief Assigns a unique id to identify the start of a new fire at real world coordinate x,y.
 *
 *	This function should only be called for a new ignition. The simulation handles assignment of ids
 * 	as fire grows and propagates through adjacent cells.
 *	\sa FireYear 
 *	\param fy FireYear of all active fires
 *	\param rwx real world x coordinate to assign new id
 *	\param rwy real world y coordinate to assign new id
 *	\param ft simulation timer stored in FireTimer 
 *  \param is_sa flag to indicate santa ana is active
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 */
int FireYearSetCellNewFireIDRealWorld(FireYear * fy, double rwx, double rwy, FireTimer * ft, int is_sa);

/*! \fn int FireYearSetCellFireIDRowCol(FireYear * fy, int i, int j, int id)
 *  \brief Assigns an existing id to a cell as in the case of an expanding fire.
 *
 *	\param fy FireYear of all active fires
 *	\param i row index of cell to examine
 *	\param j column index of cell to examine
 *	\param id id of fire at neighboring cell that is assigned to new cell
 *	\param ft simulation timer stored in FireTimer 
 *  \param is_sa flag to indicate santa ana is active
 */
void FireYearSetCellFireIDRowCol(FireYear * fy, int i, int j, int id, FireTimer * ft, int is_sa);

void FireYearSetFailedIgnitions(ChHashTable * proptbl, FireYear * fy);

/*!	\fn void FreeFireYear(FireYear * fy)
 * 	\brief Frees memory associated with FireYear object.
 *
 * 	Subsequent calls to methods taking FuelModel as argument will not work.
 *	\sa FireYear 
 *	\param fy ptr to FireYear
 */
void FreeFireYear(FireYear * fy);

#endif FireYear_H	/* end of FireYear.h */
