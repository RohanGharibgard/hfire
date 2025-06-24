/*! 
 * \file FireConfig.h
 * \brief Routines for reading in simulation properties and loading initialization data.
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
 
#ifndef	FireConfig_H
#define FireConfig_H

#include <stdlib.h>
#include <stdio.h>

/* configuration headers */
#include "FireProp.h"
#include "FireGridData.h"

/* simulation support headers */
#include "FireEnv.h"
#include "FireTimer.h"

/* abstract FuelModel headers */
#include "FuelModel.h"

/* FireEnv headers */
#include "FuelsRegrowth.h"
#include "Ignition.h"
#include "WindAzimuth.h"
#include "WindSpd.h"
#include "DeadFuelMoist.h"
#include "LiveFuelMoist.h"
#include "SantaAna.h"

/* generic support headers */
#include "GridData.h"
#include "List.h"
#include "ChHashTable.h"
#include "HashFuncs.h"
#include "KeyVal.h"
#include "FStreamIO.h"
#include "RandSeedTable.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* suggested capacity supplied to Hash Table initialization routine */
#define FIRE_CONFIG_TBL_INI_CAPCTY							(50)

/* initial size of growable character buffer used by GetFireConfigLine */
#define FIRE_CONFIG_INITIAL_SCAN_LINE_SIZE					(125)

/* used in tokenizing configuration file */
#define FIRE_CONFIG_COMMENT_CHAR							("#")
#define FIRE_CONFIG_SEPARATOR_CHARS							(" =,\t")

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

/*! \fn int InitPropsFromFireConfig(ChHashTable ** proptbl, char * cfg_fname)
 *	\brief Reads simulation configuration information from file and stores in ChHashTable.
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html">config file documentation</a> \endhtmlonly 
 *	\param proptbl ChHashTable of simulation properties
 *	\param cfg_fname filename where simulation properties stored
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */ 
int InitPropsFromFireConfig(ChHashTable ** proptbl, char * cfg_fname);

/*! \fn int InitGridsFromPropsFireConfig(ChHashTable * proptbl, GridData ** elev, GridData ** slope, GridData ** aspect)
 *	\brief Loads terrain-related raster data required for fire prediction.
 *	\sa ChHashTable
 *	\sa GridData
 *	\sa Check the \htmlonly <a href="config_file_doc.html#RASTER">config file documentation</a> \endhtmlonly 
 *	\param proptbl ChHashTable of simulation properties
 *	\param elev if function returns without error, initialized GridData of elevation values
 *	\param slope if function returns without error, initialized GridData of slope values 
 *	\param aspect if function returns without error, initialized GridData of aspect values 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int InitGridsFromPropsFireConfig(ChHashTable * proptbl, GridData ** elev, GridData ** slope, GridData ** aspect);

/*! \fn int InitFireTimerFromPropsFireConfig(ChHashTable * proptbl, FireTimer ** ft)
 *	\brief Initializes FireTimer structure used by simulation.
 *	\sa ChHashTable
 *	\sa FireTimer
 *	\sa Check the \htmlonly <a href="config_file_doc.html#SIMULATION">config file documentation</a> \endhtmlonly 
 *	\param proptbl ChHashTable of simulation properties
 *	\param ft if function returns without error, initialized simulation FireTimer
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int InitFireTimerFromPropsFireConfig(ChHashTable * proptbl, FireTimer ** ft);

/*! \fn int InitFuelModelHashTableFromFuelModelListFireConfig(List * fmlist, ChHashTable ** fmtble)
 *	\brief Initializes ChHashTable of FuelModels indexed by model number, using the supplied List.
 *	\sa List
 *	\sa ChHashTable
 *	\sa FuelModel
 *	\sa Check the \htmlonly <a href="config_file_doc.html#FUEL_PROPS">config file documentation</a> \endhtmlonly 
 *	\param fmlist List of FuelModels to insert into ChHashTable
 *	\param fmtble ChHashTable of FuelModels indexed by fuel model number
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int InitFuelModelHashTableFromFuelModelListFireConfig(List * fmlist, ChHashTable ** fmtble);

/*! \fn int InitStandAgeFromPropsFireConfig(ChHashTable * proptbl, GridData * elev, GridData ** std_age)
 *	\brief Loads stand age data for generating age-dependent fuels.
 *	\sa ChHashTable
 *	\sa GridData
 *	\sa Check the \htmlonly <a href="config_file_doc.html#STAND_AGE">config file documentation</a> \endhtmlonly 
 *	\param proptbl ChHashTable of simulation properties
 *	\param elev GridData of terrain elevation used to define the simulation boundaries
 *	\param std_age if function returns without error, initialized GridData of current stand age for each cell 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int InitStandAgeFromPropsFireConfig(ChHashTable * proptbl, GridData * elev, GridData ** std_age);

/*! \fn int InitFireEnvFromPropsFireConfig(ChHashTable * proptbl, FireEnv ** fe)
 *	\brief Assigns appropriate function pointers to FireEnv structure based upon user configuration.
 *	\sa ChHashTable
 *	\sa FireEnv
 *	\sa Check the \htmlonly <a href="config_file_doc.html">config file documentation</a> \endhtmlonly 
 *	\param proptbl ChHashTable of simulation properties
 *	\param fe if function returns without error, initialized FireEnv structure 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int InitFireEnvFromPropsFireConfig(ChHashTable * proptbl, FireEnv ** fe);

/*! \fn int InitRandNumGenFromPropsFireConfig(ChHashTable * proptbl, void(*RandInit)(long int seed))
 *	\brief Initializes simulation random number generator with user supplied seed or from seed table.
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#SIMULATION">config file documentation</a> \endhtmlonly 
 *	\param proptbl ChHashTable of simulation properties
 *	\param RandInit pointer to random number generating function to use during simulation 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int InitRandNumGenFromPropsFireConfig(ChHashTable * proptbl, void(*RandInit)(long int seed));

/*! \fn void FireConfigDumpPropsToStream(ChHashTable * proptbl, FILE * fstream)
 *	\brief Dumps ChHashTable of current simulation configuration properties to stream.
 *	\sa ChHashTable
 *	\sa EnumFireProp
 *	\sa Check the \htmlonly <a href="config_file_doc.html">config file documentation</a> \endhtmlonly 
 *	\param proptbl ChHashTable of simulation properties
 *	\param fstream stream to write properties to 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
void FireConfigDumpPropsToStream(ChHashTable * proptbl, FILE * fstream);

/*! \fn void FireConfigDumpFuelModelListToStream(List * pnvlist, FILE * fstream)
 *	\brief Dumps List of current FuelModels to stream.
 *	\sa List
 *	\sa FuelModel
 *	\sa Check the \htmlonly <a href="config_file_doc.html">config file documentation</a> \endhtmlonly 
 *	\param pnvlist FuelModels used by current simulation
 *	\param fstream stream to write to 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
void FireConfigDumpFuelModelListToStream(List * pnvlist, FILE * fstream);

#endif FireConfig_H		/* end of FireConfig.h */
