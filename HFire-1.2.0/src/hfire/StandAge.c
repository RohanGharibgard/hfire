/*! 
 * \file StandAge.c
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
 
#include "StandAge.h"

int IncrementStandAge(FireYear * fy, GridData * std_age)	{
	int domain_rows;							/* size of domain rows */
	int domain_cols;							/* size of domain columns */
	long int age;
	int burn;
	int i, j;
		
	/* check args */		
	if ( fy == NULL || fy->id == NULL || std_age == NULL )	{
		ERR_ERROR("Arguments supplied to increment stand age invalid. \n", ERR_EINVAL);
	}

	/* increment stand age */
	domain_rows = INTTWODARRAY_SIZE_ROW(fy->id); 
	domain_cols = INTTWODARRAY_SIZE_COL(fy->id);	
	for(i = 0; i < domain_rows; i++)	{
		for(j = 0; j < domain_cols; j++)	{
			if ( FireYearGetCellIDRowCol(fy, i, j, &burn) ) {
				ERR_ERROR("Unable to retrieve FireYear data, stand age not incremented. \n", ERR_EBADFUNC);
      }
			if ( burn > FIRE_YEAR_ID_DEFAULT )	{
				/* cell burned, reset stand age */
				GRID_DATA_SET_DATA(std_age, i, j, 1);
			}
			else if ( burn != FIRE_YEAR_ID_UNBURNABLE )	{
				/* cell not burned, increment stand age */				
				GRID_DATA_GET_DATA(std_age, i, j, age);
				++age;
				GRID_DATA_SET_DATA(std_age, i, j, age);				
			}
		}
	}		
	
	return ERR_SUCCESS;
}

/* end of StandAge.c */
