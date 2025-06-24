/*! 
 * \file DeadFuelMoist.c
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
 
#include "DeadFuelMoist.h"

#include <math.h>

int GetDeadFuelMoistFIXEDFromProps(ChHashTable * proptbl, int month, int day, int hour, 
											double rwx, double rwy, 
                      double * d1hfm, double * d10hfm, double * d100hfm)		{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static double sd1hfm			= 0.0;
	static double sd10hfm			= 0.0;
	static double sd100hfm			= 0.0;
	static DblTwoDArray * sd10h_tbl = NULL;
  static double sd1hfminc = 0.02;
  static double sd100hfminc = 0.02;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	int i;

	/* check to see if new dead fuel moisture needed */	
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new dead fuel moistures table needed */
		if (sd10h_tbl == NULL )	{
			/* initialize returned vars in case table not created */
			*d1hfm = sd1hfm;
			*d10hfm = sd10hfm;
			*d100hfm = sd100hfm;
			/* table of fixed values only needs to be initialized once */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_DFMFFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve DEAD_FUEL_MOIST_FIXED_FILE property. \n", ERR_EINVAL);
				}
			/* open fixed dead fuel moisture file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{		
				ERR_ERROR("Unable to open file containing fixed Dead Fuel Moisture. \n", ERR_EIOFAIL);
				}
			/* create the table of values */
			if ( (sd10h_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					DEAD_FUEL_MOIST_10H_SEP_CHARS, DEAD_FUEL_MOIST_10H_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to initialize fixed Dead Fuel Moisture table. \n", ERR_EFAILED);
				}
			fclose(fstream);
      /* obtain the dead fuel moisture increments */
      if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_DFMD1HINC), (void *)&entry) ) {
        sd1hfminc = 0.02; /* default */
        }
      else {
        sd1hfminc = fabs(atof(entry->val)) / 100.0;
        }
      if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_DFMD100HINC), (void *)&entry) ) {
        sd100hfminc = 0.02; /* default */
        }
      else {
        sd100hfminc = fabs(atof(entry->val)) / 100.0;
        }
			}
		/* find record in table */
		for(i = 0; i < DBLTWODARRAY_SIZE_ROW(sd10h_tbl); i++)	{
			/* find matching month and day */
			if ( DBLTWODARRAY_GET_DATA(sd10h_tbl, i, DEAD_FUEL_MOIST_MO_10H_TBL_INDEX) == month 
				 && DBLTWODARRAY_GET_DATA(sd10h_tbl, i, DEAD_FUEL_MOIST_DY_10H_TBL_INDEX) == day )	{
				  /* retrieve dfm on that month and day */
				  sd10hfm = DBLTWODARRAY_GET_DATA(sd10h_tbl, i, DEAD_FUEL_MOIST_HR_TO_10H_TBL_INDEX(hour)) / 100.0;
				  if ( !UNITS_FP_GT_ZERO(sd10hfm) ) {
					  sd10hfm = 0.01;
            }
          /* compute d1h from d10h */
				  sd1hfm = sd10hfm - sd1hfminc;
				  if ( !UNITS_FP_GT_ZERO(sd1hfm) ) {
					  sd1hfm = 0.01;
            }
          /* compute d100h from d10h */
				  sd100hfm = sd10hfm + sd100hfminc;
				  if ( !UNITS_FP_GT_ZERO(sd100hfm) ) {
					  sd100hfm = 0.01;
            }
				  break;
				}
			}
		/* set {month, day, hour} for future calls */
		smonth = month;
		sday = day;
		shour = hour;
		}

	*d1hfm = sd1hfm;
	*d10hfm = sd10hfm;
	*d100hfm = sd100hfm;
					
	return ERR_SUCCESS;
	}

																					
int GetDeadFuelMoistRANDHFromProps(ChHashTable * proptbl, int month, int day, int hour, 
											double rwx, double rwy, 
                      double * d1hfm, double * d10hfm, double * d100hfm)		{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static int srec					= 0;
	static double sd1hfm			= 0.0;
	static double sd10hfm			= 0.0;
	static double sd100hfm			= 0.0;
	static DblTwoDArray * sd10h_tbl = NULL;
  static double sd1hfminc = 0.02;
  static double sd100hfminc = 0.02;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */

	/* check to see if new dead fuel moisture needed */
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new dead fuel moistures table needed */
		if (sd10h_tbl == NULL )	{
			/* initialize returned vars in case table not created */
			*d1hfm = sd1hfm;
			*d10hfm = sd10hfm;
			*d100hfm = sd100hfm;		
			/* table of fixed values only needs to be initialized once */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_DFMHFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve DEAD_FUEL_MOIST_HISTORICAL_FILE property. \n", ERR_EINVAL);
				}
			/* open fixed azimuth file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open file containing historical Dead Fuel Moisture. \n", ERR_EIOFAIL);
				}
			/* create the table of values */
			if ( (sd10h_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					DEAD_FUEL_MOIST_10H_SEP_CHARS, DEAD_FUEL_MOIST_10H_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to initialize historical Dead Fuel Moisture table. \n", ERR_EFAILED);
				}
			fclose(fstream);
      /* obtain the dead fuel moisture increments */
      if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_DFMD1HINC), (void *)&entry) ) {
        sd1hfminc = 0.02; /* default */
        }
      else {
        sd1hfminc = fabs(atof(entry->val)) / 100.0;
        }
      if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_DFMD100HINC), (void *)&entry) ) {
        sd100hfminc = 0.02; /* default */
        }
      else {
        sd100hfminc = fabs(atof(entry->val)) / 100.0;
        }
			}
		/* new random record number (day) from table needed */
		if ( (smonth != month) || (sday != day) )	{
			/* retrieve new record at random from table within range 0 to num_recs */
			srec = randi(0) % DBLTWODARRAY_SIZE_ROW(sd10h_tbl);
			}
		/* retrieve dead fuel moisture at hour from current day, otherwise use last value */
		if ( (DBLTWODARRAY_GET_DATA(sd10h_tbl, srec, DEAD_FUEL_MOIST_HR_TO_10H_TBL_INDEX(hour))) 
				!= DEAD_FUEL_MOIST_10H_NO_DATA_VALUE ) {
			sd10hfm = (DBLTWODARRAY_GET_DATA(sd10h_tbl, srec, DEAD_FUEL_MOIST_HR_TO_10H_TBL_INDEX(hour))) / 100.0;
			if ( !UNITS_FP_GT_ZERO(sd10hfm) ) {
				sd10hfm = 0.01;
        }
      /* compute d1h from d10h */
			sd1hfm = sd10hfm - sd1hfminc;
			if ( !UNITS_FP_GT_ZERO(sd1hfm) ) {
				sd1hfm = 0.01;
        }
      /* compute d100h from d10h */
			sd100hfm = sd10hfm + sd100hfminc;
			if ( !UNITS_FP_GT_ZERO(sd100hfm) ) {
				sd100hfm = 0.01;
        }
		}
		
		/* set {month, day, hour} for future calls */
		smonth = month;
		sday = day;
		shour = hour;
		}
		
	*d1hfm = sd1hfm;
	*d10hfm = sd10hfm;
	*d100hfm = sd100hfm;
					
	return ERR_SUCCESS;	
	}

int GetDeadFuelMoistSPATIALFromProps(ChHashTable * proptbl, int month, int day, int hour, 
											double rwx, double rwy, 
                      double * d1hfm, double * d10hfm, double * d100hfm)		{
	/* static variables used to store state across function calls */
	static int smonth 			= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static StrTwoDArray * s10h_tbl	= NULL;
	static GridData * s10h_grid 	= NULL;
  static double sd1hfminc = 0.02;
  static double sd100hfminc = 0.02;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	int i,j;

	/* check to see if new tables are needed */
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new 10h tbl */
		if (s10h_tbl == NULL )	{
			/* retriev 10h filename from properties */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_DFMSPFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve DEAD_FUEL_MOIST_SPATIAL_FILE property. \n", ERR_EINVAL);
				}
			/* open 10h file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open DEAD_FUEL_MOIST_SPATIAL_FILE. \n", ERR_EIOFAIL);
				}
			/* retrieve contents of 10h file */
			if ( (s10h_tbl = GetStrTwoDArrayTableFStreamIO(fstream, DEAD_FUEL_MOIST_RAST_10H_SEP_CHARS, 
						DEAD_FUEL_MOIST_RAST_10H_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to parse DEAD_FUEL_MOIST_SPATIAL_FILE. \n", ERR_EINVAL);
				}
			/* cleanup */
			fclose(fstream);
      /* obtain the dead fuel moisture increments */
      if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_DFMD1HINC), (void *)&entry) ) {
        sd1hfminc = 0.02; /* default */
        }
      else {
        sd1hfminc = fabs(atof(entry->val)) / 100.0;
        }
      if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_DFMD100HINC), (void *)&entry) ) {
        sd100hfminc = 0.02; /* default */
        }
      else {
        sd100hfminc = fabs(atof(entry->val)) / 100.0;
        }
			}							
		/* new 10h grid */
		if ( s10h_grid == NULL )	{
			if ( (s10h_grid = InitGridDataFromAsciiRaster(STRTWODARRAY_GET_DATA(s10h_tbl, 0, DEAD_FUEL_MOIST_RAST_10H_FILE_TBL_INDEX), 
								EnumDblGrid)) == NULL )	{
				ERR_ERROR("Unable to initialize GridData from file listed in DEAD_FUEL_MOIST_SPATIAL_FILE. \n", ERR_EINVAL);
				}
			}
		/* find record in 10h table */
		for(i = 0; i < DBLTWODARRAY_SIZE_ROW(s10h_tbl); i++)	{
			/* parse the 10h table from strings to numbers */
			smonth = atoi(STRTWODARRAY_GET_DATA(s10h_tbl, i, DEAD_FUEL_MOIST_RAST_10H_MO_TBL_INDEX));
			sday = atoi(STRTWODARRAY_GET_DATA(s10h_tbl, i, DEAD_FUEL_MOIST_RAST_10H_DY_TBL_INDEX));
			shour = atoi(STRTWODARRAY_GET_DATA(s10h_tbl, i, DEAD_FUEL_MOIST_RAST_10H_HR_TBL_INDEX)) / 100;
			/* find matching month and day and hour */
			if ( (smonth == month) && (sday == day) && (shour == hour) )	{
				/* free old grid */
				FreeGridData(s10h_grid);
				/* initialize new grid */
				if ( (s10h_grid = InitGridDataFromAsciiRaster(STRTWODARRAY_GET_DATA(s10h_tbl, i, DEAD_FUEL_MOIST_RAST_10H_FILE_TBL_INDEX), 
									EnumDblGrid)) == NULL )	{
					ERR_ERROR("Unable to initialize GridData from file listed in DEAD_FUEL_MOIST_SPATIAL_FILE. \n", ERR_EINVAL);
					}
				break;
				}
			}
		/* set {month, day, hour} for future calls */
		smonth = month;
		sday = day;
		shour = hour;
		}

	/* transform array indecies into spatial coordinates */
	if ( CoordTransRealWorldToRaster(rwx, rwy, s10h_grid->ghdr->cellsize, s10h_grid->ghdr->cellsize,
			COORD_TRANS_XLLCORNER_TO_XULCNTR(s10h_grid->ghdr->xllcorner, s10h_grid->ghdr->cellsize),
			COORD_TRANS_YLLCORNER_TO_YULCNTR(s10h_grid->ghdr->yllcorner, s10h_grid->ghdr->cellsize, s10h_grid->ghdr->nrows),
			&i, &j) )	{
		ERR_ERROR("Unable to transform real world coordinates to grid indecies. \n", ERR_ESING);
		}			
													
	/* retrieve d10h at coordinate */
	GRID_DATA_GET_DATA(s10h_grid, i, j, *d10hfm);
  *d10hfm = *d10hfm / 100.0;

  /* compute d1h from d10h */
	*d1hfm = *d10hfm - sd1hfminc;
	if ( !UNITS_FP_GT_ZERO(*d1hfm) ) {
		*d1hfm = 0.01;
    }
  /* compute d100h from d10h */
	*d100hfm = *d10hfm + sd100hfminc;
	if ( !UNITS_FP_GT_ZERO(*d100hfm) ) {
		*d100hfm = 0.01;
    }

	return ERR_SUCCESS;
	}

/* end of DeadFuelMoist.c */
