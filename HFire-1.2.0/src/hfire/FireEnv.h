/*! 
 * \file FireEnv.h
 * \brief Facility for storing pointers to functions related to simulation environmental parameters.
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
 
#ifndef	FireEnv_H
#define FireEnv_H

#include <stdlib.h>

#include "ChHashTable.h"
#include "GridData.h"
#include "FireYear.h"
#include "List.h"
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

/*! Type name for FireEnv_ 
 *	\sa For a list of members got FireEnv_
 */
typedef struct FireEnv_ FireEnv;

/*!	\struct FireEnv_ FireEnv.h "FireEnv.h"
 *	\brief structure stores pointers to functions that retrieve environmental parameters during simulation
 *	\sa Check the \htmlonly <a href="config_file_doc.html">config file documentation</a> \endhtmlonly 
 */
struct FireEnv_	{
	/*! retrieves a (potentially) time and space dependent fuels dataset */
	int			(* GetFuelsRegrowthFromProps)		(ChHashTable * proptbl, GridData * std_age, GridData ** fuels);
	/*! retrieves a (potentially) time dependent ignition occurence */
	int			(* IsIgnitionNowFromProps)			(ChHashTable * proptbl);
	/*! retrieves a (potentially) space dependent ignition location */
	int			(* GetIgnitionLocFromProps)			(ChHashTable * proptbl, FireYear * fy, List ** rwxylist);
	/*! retrieves a (potentially) time and space dependent wind direction */	
	int 		(* GetWindAzimuthFromProps)			(ChHashTable * proptbl, int month, int day, int hour, 
														double rwx, double rwy, double * waz);
	/*! retrieves a (potentially) time and space dependent windspeed in mps */
	int 		(* GetWindSpeedMpsFromProps)		(ChHashTable * proptbl, double fbedhgtm, 
														int month, int day, int hour, 
														double rwx, double rwy, double * wspmps);
	/*! retrieves a (potentially) time and space dependent dead fuel moisture */
	int			(* GetDeadFuelMoistFromProps)		(ChHashTable * proptbl, int month, int day, int hour, 
														double rwx, double rwy, 
                            double * d1hfm, double * d10hfm, double * d100hfm);
	/*! retrieves a (potentially) time and space dependent live fuel moisture */
	int 		(* GetLiveFuelMoistFromProps)		(ChHashTable * proptbl, int year, int month, int day, int hour,
														double rwx, double rwy, 
														double * lhfm, double * lwfm);																									
	/*! retrieves a (potentially) time dependent Santa Ana occurence */
	int			(* IsSantaAnaNowFromProps)			(ChHashTable * proptbl, int year, int month, int day);
	/*! retrieves a (potentially) time dependent Santa Ana wind direction, speed, and dead fuel moisture */
	int			(* GetSantaAnaEnvFromProps)			(ChHashTable * proptbl, int month, int day, int hour,
														double * waz, double fbedhgtm, double * wspmps,
														double * d1hfm, double * d10hfm, double * d100hfm);
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

/*!	\fn FireEnv * InitFireEnv()
 * 	\brief Initializes an empty FireEnv structure.
 * 	\sa FireEnv
 * 	\retval FireEnv* Ptr to initialized FireEnv structure
 */
FireEnv * InitFireEnv();

/*! \fn void FreeFireEnv(FireEnv * fe)
 * 	\brief Frees memory associated with a FireEnv structure
 *
 *  Subsequent calls to methods taking FuelModel as argument will not work. 
 *	\sa FireEnv
 * 	\param fe FireEnv struct to free memory of
 */ 							
void FreeFireEnv(FireEnv * fe); 

#endif FireEnv_H		/* end of FireEnv.h */
