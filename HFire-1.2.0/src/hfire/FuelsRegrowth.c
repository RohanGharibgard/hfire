/*! 
 * \file FuelsRegrowth.c
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
 
#include "FuelsRegrowth.h"

int GetFuelsRegrowthFIXEDFromProps(ChHashTable * proptbl, GridData * std_age, GridData ** fuels)	{
	IntTwoDArray * farr 	= NULL;				/* temp array used to initialize fuels */
	KeyVal * entry			= NULL;				/* key/val instances from table */
	int domain_rows, domain_cols, fixed_fnum;
	int i, j, adata;
	
	/* check args */
	if ( proptbl == NULL || std_age == NULL )	{
		ERR_ERROR("Arguments supplied to initialize fuels data invalid. \n", ERR_EINVAL);
		}

	/* retrieve fixed num */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_FIXFNUM), (void *)&entry) 
			|| strcmp(entry->val, GetFireVal(VAL_NULL)) == 0 )	{
		ERR_ERROR("Unable to retrieve FUELS_FIXED_MODEL_NUM property. \n", ERR_EFAILED);
		}
	fixed_fnum = atoi(entry->val);

	/* get dimensions of fuels from stand age */	
	domain_rows = std_age->ghdr->nrows; 
	domain_cols = std_age->ghdr->ncols;
		
	/* create temp array to initialize fuels GridData */
	if ( (farr = InitIntTwoDArraySizeEmpty(domain_rows, domain_cols)) == NULL )	{
		ERR_ERROR("Unable to allocate memory for fuels TwoDArray. \n", ERR_ENOMEM);
		}

	/* insert fuels values into array */
	for(i = 0; i < domain_rows; i++)	{
		for(j = 0; j < domain_cols; j++)	{
			GRID_DATA_GET_DATA(std_age, i, j, adata);
			if ( adata == std_age->ghdr->NODATA_value )	{
				INTTWODARRAY_SET_DATA(farr, i, j, std_age->ghdr->NODATA_value);
				}
			else	{
				INTTWODARRAY_SET_DATA(farr, i, j, fixed_fnum);
				}
			}
		}

	/* initialize the fuels GridData from array */
	if ( (*fuels = InitGridDataFromIntTwoDArray(farr, std_age->ghdr->xllcorner, std_age->ghdr->yllcorner, 
								std_age->ghdr->cellsize, std_age->ghdr->NODATA_value)) == NULL )	{
		FreeIntTwoDArray(farr);
		ERR_ERROR("Unable to allocate memory for fuels GridData. \n", ERR_ENOMEM);
		}
	 	
	/* free the temp array */
	FreeIntTwoDArray(farr);
						
	return ERR_SUCCESS;
	}

int GetFuelsRegrowthSTATICFromProps(ChHashTable * proptbl, GridData * std_age, GridData ** fuels)	{
	if ( proptbl == NULL || std_age == NULL )	{
		ERR_ERROR("Arguments supplied to initialize fuels data invalid. \n", ERR_EINVAL);
		}
		
	/* retrieve fuels data */
	*fuels = GetGridDataFromPropsFireGridData(proptbl, FIRE_GRIDDATA_FUELS_STATIC_DATA);
		
	return ERR_SUCCESS;
	}

int GetFuelsRegrowthPNVFromProps(ChHashTable * proptbl, GridData * std_age, GridData ** fuels)	{
	GridData * pnv			= NULL;				/* (temp) pnv spatial data used to initialize fuels */
	IntTwoDArray * rgr_tbl 	= NULL;				/* (temp) parsed index table references pnv to fuel model num */
	IntTwoDArray * farr		= NULL;				/* (temp) underlying array of fuel values */
	KeyVal * entry			= NULL;				/* key/val instances from properties table */
	FILE * fstream			= NULL;				/* file ptr to rgr file */
	int domain_rows, domain_cols;
	int i, j, r;
	int pnv_cell = 0, age_cell = 0, fnum_cell = 0;
	int found_fnum = 0;

	/* check args */		
	if ( proptbl == NULL || std_age == NULL )	{
		ERR_ERROR("Arguments supplied to initialize fuels data invalid. \n", ERR_EINVAL);
		}

	/* retrieve rgr file name */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_RGRFILE), (void *)&entry)
	 		|| strcmp(entry->val, GetFireVal(VAL_NULL)) == 0 )	{
		ERR_ERROR("Unable to retrieve FUELS_PNV_RGR_FILE property. \n", ERR_EFAILED);
		}
		
	/* open rgr file stream */
	if ( (fstream = fopen(((char *)entry->val), "r")) == NULL )	{
		ERR_ERROR("Unable to open rgr file. \n", ERR_EIOFAIL);
		}

	/* initialize table of rgr values to cross reference pnv numbers to fuel model numbers */
	if ( (rgr_tbl = GetIntTwoDArrayTableFStreamIO(fstream, FUELS_REGROWTH_SEP_CHARS, 
			FUELS_REGROWTH_RGR_COMMENT_CHAR)) == NULL )	{
		fclose(fstream);
		ERR_ERROR("Unable to allocate memory for list backing rgr table. \n", ERR_EBADFUNC);
		}
	fclose(fstream);

	/* get dimensions of fuels from stand age */	
	domain_rows = std_age->ghdr->nrows; 
	domain_cols = std_age->ghdr->ncols;
	
	/* create temp fuels array to initialize a fuels GridData */
	if ( (farr = InitIntTwoDArraySizeEmpty(domain_rows, domain_cols)) == NULL )	{
		FreeIntTwoDArray(rgr_tbl);
		ERR_ERROR("Unable to allocate memory for fuels TwoDArray. \n", ERR_ENOMEM);
		}
	 	
	/* create temp pnv grid containing the potential natural vegetation class numbers */
	if ( (pnv = GetGridDataFromPropsFireGridData(proptbl, FIRE_GRIDDATA_FUELS_PNV_DATA)) == NULL )	{
		FreeIntTwoDArray(farr);
		FreeIntTwoDArray(rgr_tbl);	
		ERR_ERROR("Unable to initialize PNV raster spatial dataset. \n", ERR_EBADFUNC);
		}

	/* iterate through the pnv grid assigning fuel model numbers to fuels array based upon rgr table */
	for(i = 0; i < domain_rows; i++)	{
		for(j = 0; j < domain_cols; j++)	{
			/* retrieve stand age at cell */
			GRID_DATA_GET_DATA(std_age, i, j, age_cell);
			/* try to assign a fuel model num */
			if (  age_cell != std_age->ghdr->NODATA_value )	{
				found_fnum = 0;
				/* retrieve pnv at cell */
				GRID_DATA_GET_DATA(pnv, i, j, pnv_cell);
				/* retrieve the fuel model num from rgr_tbl */
				for(r = 0; r < INTTWODARRAY_SIZE_ROW(rgr_tbl); r++)	{
					if ( INTTWODARRAY_GET_DATA(rgr_tbl, r, 0) == pnv_cell )	{
						found_fnum = 1;
						if ( age_cell > (INTTWODARRAY_SIZE_COL(rgr_tbl) - 1) )
							/* age exceeds largest age in rgr table */
							fnum_cell = INTTWODARRAY_GET_DATA(rgr_tbl, r, (INTTWODARRAY_SIZE_COL(rgr_tbl) - 1));
						else
							/* index age directly from rgr table */
							fnum_cell = INTTWODARRAY_GET_DATA(rgr_tbl, r, age_cell);
						break;
						}
					}
				/* assign fuel model num to fuels array */
				if ( found_fnum == 1 )	{
					INTTWODARRAY_SET_DATA(farr, i, j, fnum_cell);
					}
				/* pnv num not found in rgr_tbl */
				else	{
					FreeIntTwoDArray(rgr_tbl);
					FreeIntTwoDArray(farr);
					FreeGridData(pnv);
					ERR_ERROR("Unable to find pnv number in rgr table. \n", ERR_EFAILED);
					}
				}
			else	{
				/* assign NO DATA */			
				INTTWODARRAY_SET_DATA(farr, i, j, std_age->ghdr->NODATA_value);
				}
			}
		}

	/* initialize the fuels GridData from array */
	if ( (*fuels = InitGridDataFromIntTwoDArray(farr, std_age->ghdr->xllcorner, std_age->ghdr->yllcorner, 
								std_age->ghdr->cellsize, std_age->ghdr->NODATA_value)) == NULL )	{
		FreeIntTwoDArray(rgr_tbl);
		FreeIntTwoDArray(farr);
		FreeGridData(pnv);
		ERR_ERROR("Unable to allocate memory for fuels GridData. \n", ERR_ENOMEM);
		}
		
	/* free memory associated with temp data structures */
	FreeIntTwoDArray(rgr_tbl);
	FreeIntTwoDArray(farr);	
	FreeGridData(pnv);
	
	return ERR_SUCCESS;
	}
			
/* end of FuelsRegrowth.c */
