/*! 
 * \file FuelSizeClasses.h
 * \brief Constants which represent fuel particle diameter size classes.
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
 
#ifndef	FuelSizeClasses_H
#define FuelSizeClasses_H

#include <stdlib.h>

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/*! \enum EnumFuelSizeClasses_
 *  \brief constant used to distinguish fuel size classes
 *
 *  \note EnumD1HSizeClass dead fuel particles <0.635 cm diameter
 *  \note EnumD10HSizeClass dead fuel particles 0.635 cm - 2.54 cm diameter 
 *  \note EnumD100HSizeClass dead fuel particles 2.54 cm - 7.62 cm diameter  
 *  \note EnumLHSizeClass fuel particles consisting of living foliage
 *  \note EnumLWSizeClass fuel particles consisting of living woody material
 *	\note EnumNumSizeClasses number of fuel particle size classes used for fuels description
 */
enum EnumFuelSizeClasses_	{
	EnumD1HSizeClass		= 0,
	EnumD10HSizeClass		= 1,
	EnumD100HSizeClass		= 2,
	EnumLHSizeClass			= 3,
	EnumLWSizeClass			= 4,
	EnumNumSizeClasses		= 5
	};
 
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/*! Type name for EnumFuelSizeClasses_ 
 * 	\sa For a list of constants goto EnumFuelSizeClasses_
 */
typedef enum EnumFuelSizeClasses_ EnumFuelSizeClasses;
 
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
  
#endif FuelSizeClasses_H		/* end of FuelSizeClasses.h */
