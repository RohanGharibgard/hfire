/*! 
 * \file CellState.c
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
 
#include "CellState.h"

CellState * InitCellStateFuels(GridData * fuels, ChHashTable * fmtble)	{
	CellState * cs 	= NULL;
	FuelModel * fm 	= NULL;
	int cell_fm_num;
	int i, j;
	
	/* check args */
	if ( fuels == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize CellState, fuels not initialized. \n", ERR_EINVAL);
		return cs;
		}
		
	/* allocate memory for structure */
	if ( (cs = (CellState *) malloc(sizeof(CellState))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for CellState. \n", ERR_ENOMEM);	
		return cs;
		}
		
	/* initialize structure members */
	cs->xllcorner = fuels->ghdr->xllcorner;
	cs->yllcorner = fuels->ghdr->yllcorner;
	cs->cellsize = fuels->ghdr->cellsize;
	
	/* allocate memory for cell state */
	cs->state = InitByteTwoDArraySizeIniValue(fuels->ghdr->nrows, fuels->ghdr->ncols, EnumNoFireCellState);
	if ( cs->state == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for state array underlying CellState. \n", ERR_ENOMEM);	
		if ( cs != NULL )
			free(cs);
		cs = NULL;
		return cs;
		}

	/* initialize state */
	for(i = 0; i < fuels->ghdr->nrows; i++)	{
		for(j = 0; j < fuels->ghdr->ncols; j++)	{
			/* retrieve fuel model attribute data */
			GRID_DATA_GET_DATA(fuels, i, j, cell_fm_num);
			if ( ChHashTableRetrieve(fmtble, &cell_fm_num, (void *)&fm) )	{
				ERR_ERROR_CONTINUE("Unable to retrieve fuel model from fuels GridData. \n", ERR_EBADFUNC);
				continue;
				}
			/* set cell state to unburnable */		
			if ( fm->type == EnumRoth && fm->rfm->brntype == EnumRothUnBurnable )	{
				BYTETWODARRAY_SET_DATA(cs->state, i, j, EnumUnBurnableCellState);
				}
			else if ( fm->type == EnumPhys && fm->pfm->brntype == EnumPhysUnBurnable )	{
				BYTETWODARRAY_SET_DATA(cs->state, i, j, EnumUnBurnableCellState);
				}
			}
		}
				
	return cs;
	}

int CellStateSetCellStateRowCol(CellState * cs, int i, int j, EnumCellState state)	{
	/* check args */
	if ( cs == NULL )	{
		ERR_ERROR("Unable to set cell state, ByteTwoDArray not initialized. \n", ERR_EINVAL);
		}

	/* set the cell state */
	if ( BYTETWODARRAY_GET_DATA(cs->state, i, j) != EnumUnBurnableCellState )	{
		BYTETWODARRAY_SET_DATA(cs->state, i, j, state);
		}		
			
	return ERR_SUCCESS;		
	}
		
int CellStateSetCellStateRealWorld(CellState * cs, double rwx, double rwy, EnumCellState state)	{
	int i,j;
	
	/* check args */
	if ( cs == NULL )	{
		ERR_ERROR("Unable to set cell state, ByteTwoDArray not initialized. \n", ERR_EINVAL);
		}
		
	/* retrieve array row and column indecies from real world coordinates */
	if ( CoordTransRealWorldToRaster(rwx, rwy, cs->cellsize, cs->cellsize,
			COORD_TRANS_XLLCORNER_TO_XULCNTR(cs->xllcorner, cs->cellsize), 
			COORD_TRANS_YLLCORNER_TO_YULCNTR(cs->yllcorner, cs->cellsize, BYTETWODARRAY_SIZE_ROW(cs->state)), &i, &j) )		{
		ERR_ERROR("Unable to set cell state, real world coordinates not inside domain. \n", ERR_ERANGE);
		}
		
	/* set the cell state */
	if ( BYTETWODARRAY_GET_DATA(cs->state, i, j) != EnumUnBurnableCellState )	{
		BYTETWODARRAY_SET_DATA(cs->state, i, j, state);
		}		
			
	return ERR_SUCCESS;
	}

void FreeCellState(CellState * cs)	{
	if ( cs != NULL )	{
		if ( cs->state != NULL )	{
			FreeByteTwoDArray(cs->state);
			}
		free(cs);
		}		
	cs = NULL;
	return;
	}
	
/* end of CellState.c */
