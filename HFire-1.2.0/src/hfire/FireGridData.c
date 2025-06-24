/*! 
 * \file FireGridData.c
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
 
#include "FireGridData.h"
	
GridData * GetGridDataFromPropsFireGridData(ChHashTable * proptbl, char * data_keyword)	{
	GridData * grid 		= NULL;					/* returned grid */
	EnumGridType grid_type 	= EnumFltGrid;			/* default grid type is float */
	int is_ascii 			= -1;					/* raster data format set invalid by default */
	char * main_fname 		= NULL;					/* name of main file */
	char * hdr_fname 		= NULL;					/* name of hdr file if is_ascii == 0 */
	KeyVal * entry			= NULL;					/* entry retrieved from Hash Table */
	char key[FIRE_PROP_MAX_SIZE_KEYWORD];

	if ( proptbl == NULL || data_keyword == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize GridData with table. \n", ERR_EINVAL);
		return grid;
		}	

	/* determine RASTER_FORMAT */
	key[0] = '\0';			
	strcpy(key, data_keyword);
	strcat(key, FIRE_GRIDDATA_RASTER_FORMAT);
	if ( ChHashTableRetrieve(proptbl, key, (void *)&entry) )	{
		ERR_ERROR_CONTINUE("Unable to retrieve RASTER_FORMAT property for GridData. \n", ERR_EINVAL);
		return grid;
		}
	if ( strcmp(entry->val, GetFireVal(VAL_ASCII)) == 0  )	{
		is_ascii = 1;
		}
	else if ( strcmp(entry->val, GetFireVal(VAL_BINARY)) == 0  )	{
		is_ascii = 0;
		}
	else	{
		ERR_ERROR_CONTINUE("Unrecognized RASTER_FORMAT property for GridData. \n", ERR_EINVAL);
		return grid;
		}		
	
	/* determine MAIN_FILE */
	key[0] = '\0';
	strcpy(key, data_keyword);
	strcat(key, FIRE_GRIDDATA_MAIN_FILE);
	if ( ChHashTableRetrieve(proptbl, key, (void *)&entry) || strcmp(entry->val, GetFireVal(VAL_NULL)) == 0 )	{
		ERR_ERROR_CONTINUE("Unable to retrieve MAIN_FILE property for GridData. \n", ERR_EINVAL);
		return grid;
		}
	main_fname = (char *) entry->val;
	
	/* determine HDR_FILE */
	if ( is_ascii == 0 )	{
		/* only BINARY files have separate human readable header files */
		key[0] = '\0';
		strcpy(key, data_keyword);
		strcat(key, FIRE_GRIDDATA_HEADER_FILE);
		if ( ChHashTableRetrieve(proptbl, key, (void *)&entry) || strcmp(entry->val, GetFireVal(VAL_NULL)) == 0 )	{
			ERR_ERROR_CONTINUE("Unable to retrieve HDR_FILE property for GridData. \n", ERR_EINVAL);
			return grid;
			}
		hdr_fname = (char *) entry->val;
		}		

	/* determine GRID_TYPE */
	key[0] = '\0';
	strcpy(key, data_keyword);
	strcat(key, FIRE_GRIDDATA_DATA_TYPE);
	if ( ChHashTableRetrieve(proptbl, key, (void *)&entry) )	{
		ERR_ERROR_CONTINUE("Unable to retrieve GRID_DATA_TYPE property for GridData. \n", ERR_EINVAL);
		return grid;
		}		
	if ( strcmp(entry->val, GetFireVal(VAL_DOUBLE)) == 0 )
		grid_type = EnumDblGrid;
	else if ( strcmp(entry->val, GetFireVal(VAL_FLOAT)) == 0 )
		grid_type = EnumFltGrid;
	else if ( strcmp(entry->val, GetFireVal(VAL_LONGINT)) == 0 )
		grid_type = EnumLIntGrid;
	else if ( strcmp(entry->val, GetFireVal(VAL_INT)) == 0 )
		grid_type = EnumIntGrid;
	else if ( strcmp(entry->val, GetFireVal(VAL_BYTE)) == 0 )
		grid_type = EnumByteGrid;
	else	{
		ERR_ERROR_CONTINUE("Unrecognized GRID_DATA_TYPE property for GridData. \n", ERR_EINVAL);
		return grid;
		}	
	
	/* construct an instance of the grid */
	switch(is_ascii)	{
		case 1:
			grid = InitGridDataFromAsciiRaster(main_fname, grid_type);
			break;
		case 0:
			grid = InitGridDataFromBinaryRaster(main_fname, hdr_fname, grid_type);
			break;
		default:
			ERR_ERROR_CONTINUE("Raster format specified in config file should be ASCII or BINARY. \n", ERR_EFAILED);
			break;
		}
	
	return grid;	
	}

int IsGridDataMatchingFireGridData(GridData * agd, GridData * bgd)	{
	if ( agd != NULL && bgd != NULL )	{
		if ( agd->ghdr->nrows == bgd->ghdr->nrows 
			&& agd->ghdr->ncols == bgd->ghdr->ncols 
			&& agd->ghdr->xllcorner == bgd->ghdr->xllcorner
			&& agd->ghdr->yllcorner == bgd->ghdr->yllcorner )	{
			return 1;
			}
		}
	return 0;
	}
	 
/* end of FireGridData.c */
