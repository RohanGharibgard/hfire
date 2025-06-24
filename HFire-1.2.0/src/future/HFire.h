/*! 
 * 	\file HFire.h
 * 	\brief Main function for HFire (Highly Optmized Tolerance Fire Spread Model) Library.
 * 
 * 	To start a simulation, invoke the HFire executable followed by the filename of the configuration file.
 * 	\sa Check the \htmlonly <a href="config_file_doc.html">config file documentation</a> \endhtmlonly
 * 	\author Marco Morais
 * 	\date 2000-2001 
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
 
#ifndef HFIRE_H
#define HFIRE_H
	
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* configuration routines headers */
#include "FireConfig.h"
#include "FireProp.h"

/* fire code headers */
#include "CellState.h"
#include "FireEnv.h"
#include "FireExport.h"
#include "FireTimer.h"
#include "FireYear.h"
#include "FuelsRegrowth.h"
#include "StandAge.h"
#include "Ignition.h"
#include "Extinction.h"
#include "EightNbr.h"
#include "FuelModel.h"
#include "Roth1972.h"
#include "FireRoth1972Config.h"

/* support code headers */
#include "CoordTrans.h"
#include "IntXY.h"
#include "IntXYZ.h"
#include "DblXY.h"
#include "DblXYZ.h"
#include "FltTwoDArray.h"
#include "ByteTwoDArray.h"
#include "GridData.h"
#include "List.h"
#include "ChHashTable.h"
#include "NLIBRand.h"
#include "Err.h"
#include "Units.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */
 
/* #define USING_GD */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* floating point value defining threshold for equality */
#define HFIRE_EPSILON										(1.0e-6)

/* floating point values defining zero fraction */
#define HFIRE_FRACT_ZERO									(HFIRE_EPSILON)

/* size in characters of default status string */
#define HFIRE_DEFAULT_STATUS_LINE_LENGTH					(512)

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
 
/*! \def HFIRE_EQUAL
 *	\brief macro used for floating point test of equality 
 */
#define HFIRE_EQUAL(a,b)									(fabs((a) - (b)) < (HFIRE_EPSILON))

/*!	\def HFIRE_IS_ZERO
 *	\brief macro used for floating point test of zero 
 */
#define HFIRE_IS_ZERO(a)									(fabs(a) < (HFIRE_EPSILON))

/*!	\def HFIRE_LT_ZERO
 *	\brief macro used for floating point test less than zero 
 */
#define HFIRE_LT_ZERO(a)									((a) < (HFIRE_EPSILON))

/*!	\def HFIRE_GT_ZERO
 * 	\brief macro used for floating point test less than zero 
 */
#define HFIRE_GT_ZERO(a)									((a) > (HFIRE_EPSILON))

/*!	\def HFIRE_MIN
 *	\brief returns min of arguments 
 */
#define HFIRE_MIN(x, y)             						(((x) < (y)) ? (x) : (y))

/*!	\def HFIRE_MAX
 *	\brief returns max of arguments 
 */
#define HFIRE_MAX(x, y)             						(((x) > (y)) ? (x) : (y))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */	

#endif HFIRE_H	/* end of HFire.h */
