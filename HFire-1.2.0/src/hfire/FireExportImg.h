/*! 
 * \file FireExportImg.h
 * \brief Routines for exporting simulation data as PNG and JPEG format images.
 * 
 * \sa Check the \htmlonly <a href="config_file_doc.html#EXPORT">config file documentation</a> \endhtmlonly 
 * Must have USING_GD defined in order to enable this functionality.
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
 
#ifndef	FireExportImg_H
#define FireExportImg_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "FireTimer.h"
#include "FireYear.h"
#include "FireProp.h"
#include "GridData.h"
#include "ChHashTable.h"
#include "KeyVal.h"
#include "Err.h"

/* contains font defintions for gd library */
#include "gdfontt.h"
#include "gdfonts.h"
#include "gdfontmb.h"
#include "gdfontl.h"
#include "gdfontg.h"
/* contains library wrappers for gd calls */
#include "GridDataImg.h"
#include "RGBColors.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define FIRE_EXPORT_IMG_DEFAULT_TITLE_COLOR							(EnumBlack)
#define FIRE_EXPORT_IMG_DEFAULT_NODATA_COLOR						(EnumLightGray)
#define FIRE_EXPORT_IMG_DEFAULT_FILENAME_SIZE						(256)
#define FIRE_EXPORT_IMG_DEFAULT_TITLE_SIZE							(40)

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

/*! \fn int FireExportFireIDPng(ChHashTable * proptbl, FireYear * fyr, FireTimer * ft)
 *	\brief Exports current maximum extent of all fires burning in simulation as PNG image.
 *
 *	Must have USING_GD defined in order to enable this functionality.
 *	\sa ChHashTable
 *	\sa FireYear
 *	\sa FireTimer
 *	\sa Check the \htmlonly <a href="config_file_doc.html#EXPORT">config file documentation</a> \endhtmlonly 
 *	\param proptbl ChHashTable of simulation properties
 *	\param fyr current extent of all active fires in FireYear
 *	\param ft current simulation time stored in FireTimer
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int FireExportFireIDPng(ChHashTable * proptbl, FireYear * fyr, FireTimer * ft);
  
#endif FireExportImg_H		/* end of FireExportImg.h */
