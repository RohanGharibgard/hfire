/*! 
 * \file FireExport.h
 * \brief Facility for exporting simulation output.
 *  
 *\sa Check the \htmlonly <a href="config_file_doc.html#EXPORT">config file documentation</a> \endhtmlonly 
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
 
#ifndef	FireExport_H
#define FireExport_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef USING_GD

#include "FireExportImg.h"

#endif /* INCLUDES SUPPORT FOR EXPORTING IMAGES FROM SIMULATION USING GD LIBRARY */

#include "FireTimer.h"
#include "FireYear.h"
#include "FireProp.h"
#include "GridData.h"
#include "ChHashTable.h"
#include "KeyVal.h"
#include "EightNbr.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */
 
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* defaults for exported filenames */
#define FIRE_EXPORT_DEFAULT_FILENAME_SIZE						(256)
#define FIRE_EXPORT_DEFAULT_TITLE_SIZE							(40)

/* number of bins in the age at burn histogram */
#define AGE_AT_BURN_NUM_HIST_BINS                   (100)

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/*! \enum EnumFireExportFreq_
 *	\brief constant indicating frequency that data is output from simulation
 *	\note EnumFreqTimestep data exported at the conclusion of every timestep (length timestep user-defined)
 *	\note EnumFreqDaily data exported at the conclusion of every day (0000 hours) during simulation
 *	\note EnumFreqAnnual data exported at the conclusion of every year during simulation
 */
enum EnumFireExportFreq_	{
	EnumFreqTimestep	= 0,
	EnumFreqDaily		  = 1,
	EnumFreqAnnual		= 2
};
	 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/*! Type name for EnumFireExportFreq_ 
 *	\sa For a list of constants goto EnumFireExportFreq_
 */
typedef enum EnumFireExportFreq_ EnumFireExportFreq;

/*! Type name for FireExport_ 
 *	\sa For a list of members goto FireExport_
 */
typedef struct FireExport_ FireExport;

/*! \struct FireExport_ FireExport.h "FireExport.h"
 *	\brief structure stores settings and function pointers used to control simulation output
 */
struct FireExport_	{
	/*! constant determines how often data exported */
	EnumFireExportFreq exp_freq;
	/*! ptr to simulation timer */
	FireTimer * ft;
	/*! ptr to fire ids dataset */
	FireYear * fyr;
	/*! ptr to fuels dataset */
	GridData * fuels;
	/*! ptr to stand age dataset */
	GridData * std_age;
	/*! function ptr to fire id export */
	int (* FireExportFireIDAscRaster)	(ChHashTable * proptbl, FireYear * fyr, FireTimer * ft);
	/*! function ptr to santa ana export */
	int (* FireExportSantaAnaAscRaster)	(ChHashTable * proptbl, FireYear * fyr, FireTimer * ft);
	/*! function ptr to fuels export */
	int (* FireExportFuelsAscRaster)	(ChHashTable * proptbl, GridData * fuels, FireTimer * ft);
	/*! function ptr to stand age export */
	int (* FireExportStandAgeAscRaster)	(ChHashTable * proptbl, GridData * std_age, FireTimer * ft);
	/*! function ptr to image export 
	 *	\note USING_GD must be defined at compile-time to enable this option
	 *	\note For more information, visit the 
	 *	\htmlonly <a href="http://www.boutell.com/gd/">gd library</a> \endhtmlonly page
	 */
	int (* FireExportFireIDPng)			(ChHashTable * proptbl, FireYear * fyr, FireTimer * ft);
	};
		 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */
 
/*!	\def FIRE_EXPORT_SET_FIRE_TIMER(fe, ft)
 *	\brief sets current timer used by all export functions 
 *	\param fe simulation FireExport structure
 *	\param ft simulation FIreTimer
 */
#define FIRE_EXPORT_SET_FIRE_TIMER(fe, ft)							((fe)->ft = (ft))

/*!	\def FIRE_EXPORT_SET_FIRE_YEAR(fe, fyr)
 *	\brief sets current fire year ids used by all export functions
 *	\param fe simulation FireExport structure
 *	\param fyr simulation FireYear structure
 */
#define FIRE_EXPORT_SET_FIRE_YEAR(fe, fyr)							((fe)->fyr = (fyr))

/*!	\def FIRE_EXPORT_SET_FUELS(fe, fuels)
 *	\brief sets current fuels used by all export functions
 *	\param fe simulation FireExport structure
 *	\param fuels simulation fuels data
 */
#define FIRE_EXPORT_SET_FUELS(fe, fuels)							((fe)->fuels = (fuels))

/*!	\def FIRE_EXPORT_SET_STAND_AGE(fe, std_age)
 *	\brief sets current stand age used by all export functions
 *	\param fe simulation FireExport structure
 *	\param std_age simulation stand age data
 */
#define FIRE_EXPORT_SET_STAND_AGE(fe, std_age)						((fe)->std_age = (std_age))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

/*! \fn FireExport * InitFireExport(ChHashTable * proptbl)
 *  \brief Initializes a FireExport structure using ChHashTable of simulation properties.
 *
 *  Simulation properties contain user-specified settings which control frequency and type of output generated.
 *	\sa ChHashTable
 *  \sa FireExport
 *	\sa Check the \htmlonly <a href="config_file_doc.html#EXPORT">config file documentation</a> \endhtmlonly 
 *  \param proptbl ChHashTable of simulation properties
 *  \retval FireExport* Ptr to initialized FireExport structure
 */	
FireExport * InitFireExport(ChHashTable * proptbl);

/*! \fn int FireExportSpatialData(ChHashTable * proptbl, FireExport * fe)
 *	\brief Iterates over function pointers in FireExport to generate output spatial data from simulation.
 *
 *  Simulation properties contain user-specified settings which control frequency and type of output generated.
 *	\sa ChHashTable
 *	\sa FireExport
 *	\sa Check the \htmlonly <a href="config_file_doc.html#EXPORT">config file documentation</a> \endhtmlonly  
 *	\param proptbl ChHashTable of simulation properties
 *	\param fe FireExport structure
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int FireExportSpatialData(ChHashTable * proptbl, FireExport * fe);

/*! \fn int FireExportInitTxtFileHeaders(ChHashTable * proptbl)
 *	\brief Inserts headers into tabular textfile output used by simulation.
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#EXPORT">config file documentation</a> \endhtmlonly 
 *	\param proptbl ChHashTable of simulation properties
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int FireExportInitTxtFileHeaders(ChHashTable * proptbl);

/*! \fn int FireExportIgLocsTxtFile(ChHashTable * proptbl, double rwx, double rwy, FireTimer * ft)
 *	\brief Writes a single record into table of ignition locations.
 *	\sa ChHashTable
 *	\sa FireTimer
 *	\sa Check the \htmlonly <a href="config_file_doc.html#EXPORT">config file documentation</a> \endhtmlonly  
 *	\param proptbl ChHashTable of simulation properties
 *  \param id fire id associated with ignition location
 *	\param rwx real world x coordinate of ignition location
 *	\param rwy real world y coordinate of ignition location
 *	\param ft simulation timer stored in FireTimer 
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int FireExportIgLocsTxtFile(ChHashTable * proptbl, int id, double rwx, double rwy, FireTimer * ft);

/*! \fn int FireExportSantaAnaEvtTxtFile(ChHashTable * proptbl, int duration, int year, int month, int day)
 *	\brief Writes a single record into table of Santa Ana occurences.
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#EXPORT">config file documentation</a> \endhtmlonly 
 *	\param proptbl ChHashTable of simulation properties
 *	\param duration duration of stochastically generated Santa Ana event
 *	\param year year during which Santa Ana occurs
 *	\param month month during which Santa Ana occurs
 *	\param day day during which Santa Ana occurs
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int FireExportSantaAnaEvtTxtFile(ChHashTable * proptbl, int duration, int year, int month, int day);

int FireExportFireAreaTxtFile(ChHashTable * proptbl, FireYear * fy);

int FireExportFireInfoTxtFile(ChHashTable * proptbl, FireYear * fy);

int FireExportAgeAtBurnHistTxtFile(ChHashTable * proptbl, FireYear * fy, GridData * std_age);

/*! \fn void FreeFireExport(FireExport * fe)
 * 	\brief Frees memory associated with FireExport structure.
 *
 * 	Subsequent calls to methods taking FireExport as argument will not work.
 *	\sa FireExport
 *	\sa Check the \htmlonly <a href="config_file_doc.html#EXPORT">config file documentation</a> \endhtmlonly 
 * 	\param fe ptr to FireExport
 */
void FreeFireExport(FireExport * fe);
  
#endif FireExport_H		/* end of FireExport.h */
