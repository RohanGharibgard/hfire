/*! 
 * \file Ignition.c
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
 
#include "Ignition.h"
	
int IsIgnitionNowFIXEDFromProps(ChHashTable * proptbl)	{
	KeyVal * entry			= NULL;				/* key/val instances from table */
	static int fixed_ig_occured = 0;
	
	/* check args and retrieve frequency per day */
	if ( proptbl == NULL || ChHashTableRetrieve(proptbl, GetFireProp(PROP_IGTYP), (void *)&entry) )	{
		ERR_ERROR_CONTINUE("Unable to retrieve IGNITION_TYPE property. \n", ERR_EINVAL);
		return 0;
		}
		
	/* test if FIXED ignition specified */
	if ( strcmp(entry->val, GetFireVal(VAL_FIXED)) == 0 && fixed_ig_occured == 0)	{
		fixed_ig_occured = 1;
		return 1;	
		}

	return 0;
	}

int IsIgnitionNowRANDFromProps(ChHashTable * proptbl)	{
	KeyVal * entry					= NULL;				/* key/val instances from table */
	int	st_mo, st_dy, end_mo, end_dy;					/* start and end month and day of simulation */		
	double ig_freq_yr, timestep;						/* ig freq and length of sim timestep */
	/* static variables used to store state across function calls */	
	static double sprob_ig_ts		= -1.0;				/* ignition probability per timestep */	
	
	/* check args */
	if ( proptbl == NULL ) 	{
		ERR_ERROR_CONTINUE("Arguments supplied to determine Ignition Frequency invalid. \n", ERR_EINVAL);
		return 0;
		}

	/* retrieve frequency per year */		
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_IGFREQYR), (void *)&entry) )	{
		ERR_ERROR_CONTINUE("Unable to retrieve IGNITION_FREQUENCY_PER_YEAR property. \n", ERR_EINVAL);
		return 0;
		}
	ig_freq_yr = atof((char *) entry->val);	
	
	if ( UNITS_FP_GT_ZERO(ig_freq_yr) )	{
		/* retrieve num days in simulation year and calculate ignition probability per timestep, only done once */
		if ( UNITS_FP_LT_ZERO(sprob_ig_ts) )	{
			/* retrieve timestep */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMTSSEC), (void *)&entry) )	{
				ERR_ERROR_CONTINUE("Unable to retrieve SIMULATION_TIMESTEP_SECS property. \n", ERR_EINVAL);
				return 0;
				}	
			timestep = atof(entry->val);		
			/* find simulation start month */		
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMSTMO), (void *)&entry) )	{
				ERR_ERROR_CONTINUE("Unable to retrieve SIMULATION_START_MONTH property. \n", ERR_EINVAL);
				return 0;
				}
			st_mo = atoi((char *) entry->val);
			/* find simulation start day */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMSTDY), (void *)&entry) )	{
				ERR_ERROR_CONTINUE("Unable to retrieve SIMULATION_START_DAY property. \n", ERR_EINVAL);
				return 0;
				}
			st_dy = atoi((char *) entry->val);
			/* find simulation end month */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMEDMO), (void *)&entry) )	{
				ERR_ERROR_CONTINUE("Unable to retrieve SIMULATION_END_MONTH property. \n", ERR_EINVAL);
				return 0;
				}
			end_mo = atoi((char *) entry->val);
			/* find simulation end day */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMEDDY), (void *)&entry) )	{
				ERR_ERROR_CONTINUE("Unable to retrieve SIMULATION_END_DAY property. \n", ERR_EINVAL);
				return 0;
				}
			end_dy = atoi((char *) entry->val);
			/* calculate probability per timestep */			 
			sprob_ig_ts = ig_freq_yr / 
						  FireTimerGetDaysDifftime(st_mo, st_dy, end_mo, end_dy) / 
						  (IGNITION_SECS_PER_DAY / timestep);
			}
		}
	else	{
		/* ignition frequency per year < 0, ignitions not possible */
		return 0;		
		}
		
	/* test for ignition */
	if ( randu(0.0, 1.0) < (sprob_ig_ts) )
		return 1;

	return 0;
	}

int GetIgnitionLocFIXEDFromProps(ChHashTable * proptbl, FireYear * fy, List ** rwxylist)	{
	KeyVal * entry			= NULL;				/* key/val instances from table */	
	DblTwoDArray * da		= NULL;				/* (temp) array of ignition locations retrieved from file */
	FILE * fstream 			= NULL;
	ListElmt * lel			= NULL;
	double * rw				= NULL;
	int i;
	
	/* check args and retrieve fixed ig file  */
	if ( proptbl == NULL || fy == NULL 
			|| ChHashTableRetrieve(proptbl, GetFireProp(PROP_IGFILE), (void *)&entry) )	{
		ERR_ERROR("Arguments supplied to determine Ignition Location invalid. \n", ERR_EINVAL);
		}	
	
	/* open fixed igntion file */
	if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
		ERR_ERROR("Unable to open file containing fixed Ignition Locations. \n", ERR_EIOFAIL);
		}

	/* retrieve array of real world xy coordinates */	
	if ( (da = GetDblTwoDArrayTableFStreamIO(fstream, IGNITION_SEP_CHARS, IGNITION_IGS_COMMENT_CHAR)) == NULL )	{
		fclose(fstream);
		ERR_ERROR("Unable to parse fixed Ignition Locations from file. \n", ERR_EIOFAIL);
		}
	
	/* insert real world xy coordinates from array in list */
	*rwxylist = InitListEmpty(free);
	for(i = 0; i < DBLTWODARRAY_SIZE_ROW(da); i++)	{
		/* get real world x coordinate */
		if ((rw = (double *)malloc(sizeof(double))) != NULL)	{
			*rw = DBLTWODARRAY_GET_DATA(da, i, 0);
			lel = LIST_TAIL(*rwxylist);
			ListInsertNext(*rwxylist, lel, rw);
			}
		/* get real world y coordinate */
		if ((rw = (double *)malloc(sizeof(double))) != NULL)	{
			*rw = DBLTWODARRAY_GET_DATA(da, i, 1);
			lel = LIST_TAIL(*rwxylist);
			ListInsertNext(*rwxylist, lel, rw);
			}
		}
	
	/* free memory */
	fclose(fstream);
	FreeDblTwoDArray(da);

	return ERR_SUCCESS;
	}

int GetIgnitionLocRANDUFromProps(ChHashTable * proptbl, FireYear * fy, List ** rwxylist)	{
	ListElmt * lel 		= NULL;
	double * rw 		= NULL;
	double llx, lly, urx, ury;
	double rwx, rwy;
	int id;
	long int num_trials;
	
	/* check args */
	if ( proptbl == NULL || fy == NULL )	{
		ERR_ERROR("Arguments supplied to determine Ignition Location invalid. \n", ERR_EINVAL);
		}
	
	/* set up xy range to extract random numbers over */
	llx = fy->xllcorner;
	urx = llx + (INTTWODARRAY_SIZE_COL(fy->id) * fy->cellsize);
	lly = fy->yllcorner;
	ury = lly + (INTTWODARRAY_SIZE_ROW(fy->id) * fy->cellsize);

	/* keep retrieving numbers until one inside MASK found or num_trials exceeded */
	for (num_trials = 0; num_trials < IGNITION_RANDU_MAX_TRIALS; num_trials++)	{
		/* generate urn within range [llx, urx] and [lly, ury] */
		rwx = randu(llx, urx);
		rwy = randu(lly, ury);
		
		/* return id at urn */
		if ( FireYearGetCellIDRealWorld(fy, rwx, rwy, &id) )	{
			ERR_ERROR_CONTINUE("Unable to retrieve fire ID, repeating Ignition trial. \n", ERR_EBADFUNC);
			continue;
			}
		
		/* if id corresponds to burnable area, we are done */
		if ( id != FIRE_YEAR_ID_UNBURNABLE )	{
			break;
			}		
		}
	
	/* alert user trials exceeded */
	if ( num_trials == IGNITION_RANDU_MAX_TRIALS )	{
		ERR_ERROR("Unable to generate random Ignition Location, trials exceeded. \n", ERR_EMAXITER);
		}

	/* initialize returned structure */
	*rwxylist = InitListEmpty(free);
	if ((rw = (double *)malloc(sizeof(double))) != NULL)	{
		*rw = rwx;
		lel = LIST_TAIL(*rwxylist);
		ListInsertNext(*rwxylist, lel, rw);
		}
	if ((rw = (double *)malloc(sizeof(double))) != NULL)	{
		*rw = rwy;
		lel = LIST_TAIL(*rwxylist);		
		ListInsertNext(*rwxylist, lel, rw);
		}
								
	return ERR_SUCCESS;
	}

int GetIgnitionLocRANDSFromProps(ChHashTable * proptbl, FireYear * fy, List ** rwxylist)	{
	static GridData * igprob	= NULL;							/* grid of ignition probabilities */
	ListElmt * lel 		= NULL;
	double * rwx, * rwy;
	int i, j;	
	double xyprob = 0.0;
	long int num_trials;
		
	/* check args */
	if ( proptbl == NULL || fy == NULL )	{
		ERR_ERROR("Arguments supplied to determine Ignition Location invalid. \n", ERR_EINVAL);
		}

  if ( igprob == NULL ) {
	  /* initialize ignition probability Grid */	
	  if ( (igprob = GetGridDataFromPropsFireGridData(proptbl, FIRE_GRIDDATA_IGNITION_RSP_DATA)) == NULL )	{
		  ERR_ERROR("Unable to initialize ignition probability raster spatial dataset. \n", ERR_EBADFUNC);
		  }
  }

	/* keep retrieving numbers until ignition within probability or num_trials exceeded */	
	for (num_trials = 0; num_trials < IGNITION_RANDS_MAX_TRIALS; num_trials++)	{
		/* get ignition location in real world units */
		if ( GetIgnitionLocRANDUFromProps(proptbl, fy, rwxylist) )	{
			ERR_ERROR_CONTINUE("Unable to determine random Ignition Location, repeating trial. \n", ERR_EFAILED);
			continue;
			}
		lel = LIST_HEAD(*rwxylist);
		rwx = LIST_GET_DATA(lel);
		lel = LIST_GET_NEXT_ELMT(lel);
		rwy = LIST_GET_DATA(lel);
		/* transform real world units to ignition probability grid cell index */
		if ( CoordTransRealWorldToRaster(*rwx, *rwy, igprob->ghdr->cellsize, igprob->ghdr->cellsize,
			COORD_TRANS_XLLCORNER_TO_XULCNTR(igprob->ghdr->xllcorner, igprob->ghdr->cellsize), 
			COORD_TRANS_YLLCORNER_TO_YULCNTR(igprob->ghdr->yllcorner, igprob->ghdr->cellsize, igprob->ghdr->nrows), 
      &i, &j) )		{
			FreeList(*rwxylist);
			ERR_ERROR_CONTINUE("Unable to transform Ignition Location, repeating trial. \n", ERR_EFAILED);
			continue;			
			}
		/* retrieve probability at location */
		GRID_DATA_GET_DATA(igprob, i, j, xyprob);
		/* test probability against u.r.n.g */
		if ( randu(0.0,1.0) < xyprob )	{
			break;
			}
		/* trial failed, try again */
		FreeList(*rwxylist);
		}
	
	/* alert user trials exceeded */
	if ( num_trials == IGNITION_RANDU_MAX_TRIALS )	{
		ERR_ERROR("Unable to generate random Ignition Location, trials exceeded. \n", ERR_EMAXITER);
		}
								
	return ERR_SUCCESS;
	}
	
/* end of Ignition.c */
