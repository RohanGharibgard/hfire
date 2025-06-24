/*! 
 * \file FireYear.c
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
 
#include "FireYear.h"

#include <string.h>

FireYear * InitFireYearFuels(int year, GridData * fuels, ChHashTable * fmtble)	{
	FireYear * fy 	= NULL;
	FuelModel * fm 	= NULL;
	int cell_fm_num;
	int i, j;
	
	/* check args */
	if ( fuels == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize FireYear, fuels not initialized. \n", ERR_EINVAL);
		return fy;
	}
	if ( year < 0 )	{
		ERR_ERROR_CONTINUE("Unable to intialize FireYear, year cannot be less than 0. \n", ERR_EINVAL);
		return fy;
	}
		
	/* allocate memory for structure */
	if ( (fy = (FireYear *) malloc(sizeof(FireYear))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for FireYear. \n", ERR_ENOMEM);	
		return fy;
	}
		
	/* initialize structure members */
	fy->year = year;
	fy->num_fires = 0;
	fy->xllcorner = fuels->ghdr->xllcorner;
	fy->yllcorner = fuels->ghdr->yllcorner;
	fy->cellsize = fuels->ghdr->cellsize;
	
	/* allocate memory for fire ids */
	fy->id 	= InitIntTwoDArraySizeIniValue(fuels->ghdr->nrows, fuels->ghdr->ncols, FIRE_YEAR_ID_DEFAULT);
	if ( fy->id == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for fire ID raster. \n", ERR_ENOMEM);	
		if ( fy != NULL ) {
			free(fy);
    }
		fy = NULL;
		return fy;
	}

  /* allocate memory for santa ana history */
  fy->santa_ana = InitIntTwoDArraySizeIniValue(fuels->ghdr->nrows, fuels->ghdr->ncols, FIRE_YEAR_CELL_NOT_BURNED);
  if ( fy->santa_ana == NULL ) {
    ERR_ERROR_CONTINUE("Unable to allocate memory for santa ana history raster. \n", ERR_ENOMEM);
    FreeIntTwoDArray(fy->id );
    if ( fy != NULL ) {
      free(fy);
    }
    fy = NULL;
    return fy;
  }

	/* initialize fire ids and santa ana history rasters */
	for(i = 0; i < fuels->ghdr->nrows; i++)	{
		for(j = 0; j < fuels->ghdr->ncols; j++)	{
			/* retrieve fuel model attribute data */
			GRID_DATA_GET_DATA(fuels, i, j, cell_fm_num);
			if ( ChHashTableRetrieve(fmtble, &cell_fm_num, (void *)&fm) )	{
				ERR_ERROR_CONTINUE("Unable to retrieve fuel model from fuels GridData. \n", ERR_EBADFUNC);
				continue;
			}
			/* set fire id to unburnable */			
			if ( fm->type == EnumRoth && fm->rfm->brntype == EnumRothUnBurnable )	{
				INTTWODARRAY_SET_DATA(fy->id, i, j, FIRE_YEAR_ID_UNBURNABLE);
        INTTWODARRAY_SET_DATA(fy->santa_ana, i, j, FIRE_YEAR_CELL_UNBURNABLE);
			}
			else if ( fm->type == EnumPhys && fm->pfm->brntype == EnumPhysUnBurnable )	{
				INTTWODARRAY_SET_DATA(fy->id, i, j, FIRE_YEAR_ID_UNBURNABLE);
        INTTWODARRAY_SET_DATA(fy->santa_ana, i, j, FIRE_YEAR_CELL_UNBURNABLE);
			}
		}
	}

  /* initialize the fire info */
  memset(&fy->finfo[0], 0, sizeof(fy->finfo));

	return fy;
}	

int FireYearGetCellIDRowCol(FireYear * fy, int i, int j, int * id)	{
	/* check args */
	if ( fy == NULL || fy->id == NULL )	{
		ERR_ERROR("Unable to get FireYear ID at cell, ID array not initialized. \n", ERR_EINVAL);
	}
		
	*id = INTTWODARRAY_GET_DATA(fy->id, i, j);
		
	return ERR_SUCCESS;
}

int FireYearGetCellIDRealWorld(FireYear * fy, double rwx, double rwy, int * id)	{
	int i,j;
	
	/* check args */
	if ( fy == NULL || fy->id == NULL )	{
		ERR_ERROR("Unable to get FireYear ID at cell, ID array not initialized. \n", ERR_EINVAL);
	}
		
	/* transform real world coordinates to cell indecies */
	if ( CoordTransRealWorldToRaster(rwx, rwy, fy->cellsize, fy->cellsize, 
			COORD_TRANS_XLLCORNER_TO_XULCNTR(fy->xllcorner, fy->cellsize), 
			COORD_TRANS_YLLCORNER_TO_YULCNTR(fy->yllcorner, fy->cellsize, INTTWODARRAY_SIZE_ROW(fy->id)), &i, &j) ) {
		ERR_ERROR("Unable to get FireYear ID at cell, real world coordinates out of range. \n", ERR_ERANGE);
	}
		
	*id = INTTWODARRAY_GET_DATA(fy->id, i, j);
		
	return ERR_SUCCESS;
}
	
int FireYearIsCellBurnedRowCol(FireYear * fy, int i, int j)	{
	int id = INTTWODARRAY_GET_DATA(fy->id, i, j);
	
	if ( id == FIRE_YEAR_ID_UNBURNABLE || id == FIRE_YEAR_ID_DEFAULT )	{
		return 0;
	}
		
	return 1;
}

int FireYearIsCellBurnedRealWorld(FireYear * fy, double rwx, double rwy)	{
	int id, i, j;
	
	/* check args */
	if ( fy == NULL || fy->id == NULL )	{
		ERR_ERROR_CONTINUE("Unable to determine if cell burned, ID array not initialized. \n", ERR_EINVAL);
		return 0;
	}

	/* transform real world coordinates to cell indecies */
	if ( CoordTransRealWorldToRaster(rwx, rwy, fy->cellsize, fy->cellsize, 
			COORD_TRANS_XLLCORNER_TO_XULCNTR(fy->xllcorner, fy->cellsize), 
			COORD_TRANS_YLLCORNER_TO_YULCNTR(fy->yllcorner, fy->cellsize, INTTWODARRAY_SIZE_ROW(fy->id)), &i, &j) ) {
		ERR_ERROR_CONTINUE("Unable to get FireYear ID at cell, real world coordinates out of range. \n", ERR_ERANGE);
		return 0;
	}		
	
	id = INTTWODARRAY_GET_DATA(fy->id, i, j);
	
	if ( id == FIRE_YEAR_ID_UNBURNABLE || id == FIRE_YEAR_ID_DEFAULT )	{
		return 0;
	}
		
	return 1;
}

int FireYearSetCellNewFireIDRealWorld(FireYear * fy, double rwx, double rwy, FireTimer * ft, int is_sa)	{
	int i, j, id;
	
	/* check args */
	if ( fy == NULL || fy->id == NULL )	{
		ERR_ERROR("Unable to set cell fire ID, ID array not initialized. \n", ERR_EINVAL);
	}

  /* check that the current number of fires does not exceed the maximum */
  if ( fy->num_fires == FIRE_YEAR_ID_MAX ) {
    ERR_ERROR("Unable to initialize cell as a new fire, software limit on maximum number of fires reached. \n", ERR_ERANGE);
  }

	/* transform real world coordinates to cell indecies */
	if ( CoordTransRealWorldToRaster(rwx, rwy, fy->cellsize, fy->cellsize, 
			COORD_TRANS_XLLCORNER_TO_XULCNTR(fy->xllcorner, fy->cellsize), 
			COORD_TRANS_YLLCORNER_TO_YULCNTR(fy->yllcorner, fy->cellsize, INTTWODARRAY_SIZE_ROW(fy->id)), &i, &j) ) {
		ERR_ERROR("Unable to set cell fire ID, real world coordinates out of range. \n", ERR_ERANGE);
	}		

  /* get the fire id at the cell */
  id = INTTWODARRAY_GET_DATA(fy->id, i, j);

	/* if cell is burnable, update the fire attribute information */
	if ( id != FIRE_YEAR_ID_UNBURNABLE )	{
    /* increment the count of fires */
    fy->num_fires += 1;

    /* use the count of fires as the new fire id */
    id = fy->num_fires;

    /* if there is not already a fire in the cell then consider this a new fire */
    if ( ! FireYearIsCellBurnedRowCol(fy, i, j) ) {
      /* set the new id */
		  INTTWODARRAY_SET_DATA(fy->id, i, j, id);

      /* initialize the count of cells burned */
      fy->finfo[id].num_cells_burned = 1;

      if ( is_sa ) {
        fy->finfo[id].num_cells_burned_sa = 1;
        INTTWODARRAY_SET_DATA(fy->santa_ana, i, j, FIRE_YEAR_CELL_BURNED_SA);
      }
      else {
        INTTWODARRAY_SET_DATA(fy->santa_ana, i, j, FIRE_YEAR_CELL_BURNED_NO_SA);
      }
    }

    /* initialize the matching fire info */
    fy->finfo[id].id = fy->num_fires;
    fy->finfo[id].rwx = rwx;
    fy->finfo[id].rwy = rwy;
    fy->finfo[id].start_yr = fy->finfo[id].end_yr = ft->sim_cur_yr;
    fy->finfo[id].start_mo = fy->finfo[id].end_mo = ft->sim_cur_mo;
    fy->finfo[id].start_dy = fy->finfo[id].end_dy = ft->sim_cur_dy;
    fy->finfo[id].start_hr = fy->finfo[id].end_hr = ft->sim_cur_hr;
  }

	return ERR_SUCCESS;
}

void FireYearSetCellFireIDRowCol(FireYear * fy, int i, int j, int id, FireTimer * ft, int is_sa) {
  /* flag to indicate the time has changed since the last update */
  /* N.B. not worth doing this bc req. 4 comparisons for most cases */
  /*
  int has_time_changed = 
    fy->finfo[id].end_hr != ft->sim_cur_hr ||
    fy->finfo[id].end_dy != ft->sim_cur_dy ||
    fy->finfo[id].end_mo != ft->sim_cur_mo ||
    fy->finfo[id].end_hr != ft->sim_cur_hr;
  */

  /* set the fire id in the raster */
  INTTWODARRAY_SET_DATA(fy->id, i, j, id);

  /* increment the count of burning cells for this id */
  fy->finfo[id].num_cells_burned += 1;
  fy->finfo[id].end_yr  = ft->sim_cur_yr;
  fy->finfo[id].end_mo  = ft->sim_cur_mo;
  fy->finfo[id].end_dy  = ft->sim_cur_dy;
  fy->finfo[id].end_hr  = ft->sim_cur_hr;

  /* increment the count of cells that have burned during a santa ana */
  if ( is_sa ) {
    fy->finfo[id].num_cells_burned_sa += 1;
    INTTWODARRAY_SET_DATA(fy->santa_ana, i, j, FIRE_YEAR_CELL_BURNED_SA);
  }
  else {
    INTTWODARRAY_SET_DATA(fy->santa_ana, i, j, FIRE_YEAR_CELL_BURNED_NO_SA);
  }  
}

void FireYearSetFailedIgnitions(ChHashTable * proptbl, FireYear * fy) {
	KeyVal * entry					= NULL;				/* key/val instances from obtain */
  long failed_ig_num_cells;
  int i, j, id;

  /* retrieve the number of cells used as the failed ignition threshold */
  if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_FFIGNCELLS), (void *)&entry) ) {
    /* parameter not set, user is not using this feature */
 	  return;
  }
  failed_ig_num_cells = strtol(entry->val, NULL, 10);

  /* set flag for fires that are failed ignitions */
  for ( i = 1 ; i <= fy->num_fires ; ++i ) {
    if ( fy->finfo[i].num_cells_burned <= failed_ig_num_cells ) {
      fy->finfo[i].is_failed_ig = 1;
    }
  }

  /* iterate through the fire year raster */
  for ( i = 0; i < fy->id->size_rows; ++i ) {
    for ( j = 0; j < fy->id->size_cols; ++j ) {
      /* search for cells that have been burned */
      if ( FireYearIsCellBurnedRowCol(fy, i, j) ) {
        id = INTTWODARRAY_GET_DATA(fy->id, i, j);
        if ( fy->finfo[id].is_failed_ig ) {
          /* reset the fire id in the cell to the default, meaning 'no fire' */
		      INTTWODARRAY_SET_DATA(fy->id, i, j, FIRE_YEAR_ID_DEFAULT);
          INTTWODARRAY_SET_DATA(fy->santa_ana, i, j, FIRE_YEAR_CELL_NOT_BURNED);
        }
      }
    }
  }          

  return;
}

void FreeFireYear(FireYear * fy)	{
	if ( fy != NULL )	{
		if ( fy->id != NULL )	{
			FreeIntTwoDArray(fy->id);
		}
    if ( fy->santa_ana != NULL ) {
      FreeIntTwoDArray(fy->santa_ana);
    }
		free(fy);
	}		
	fy = NULL;
	return;
}
	
/* end of FireYear.c */
