/*! 
 * \file LiveFuelMoist.c
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
 
#include "LiveFuelMoist.h"

static int AdvanceRecToDate(DblTwoDArray * da, int month, int day, int * rec);

int GetLiveFuelMoistFIXEDFromProps(ChHashTable * proptbl, int year, int month, int day, int hour,
											double rwx, double rwy, 
											double * lhfm, double * lwfm)	{
	/* static variables used to store state across function calls */
	static int syear				= -1;
	static int smonth 				= 0;
	static int sday 				= 0;
	static int slh_srec				= 0;
	static int slw_srec				= 0;
	static double slhfm				= 0.0;
	static double slwfm				= 0.0;
	static DblTwoDArray * slhfm_tbl = NULL;
	static DblTwoDArray * slwfm_tbl	= NULL;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	
	if ( (smonth != month) || (sday != day) )	{
		/* new live herbaceous fuel moisture table needed */
		if ( slhfm_tbl == NULL )	{
			/* lh annual mean only needs to be initialized once */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_LFMHFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve LIVE_FUEL_MOIST_HERB_FILE property. \n", ERR_EINVAL);
				}
			/* open file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open file containing live herbaceous fuel moistures. \n", ERR_EIOFAIL);
				}		
			/* create the table of values */
			if ( (slhfm_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					LIVE_FUEL_MOIST_SEP_CHARS, LIVE_FUEL_MOIST_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to initialize historical Live Fuel Moisture table. \n", ERR_EFAILED);
				}
			fclose(fstream);
			}
		/* new live woody fuel moisture table needed */
		if ( slwfm_tbl == NULL )	{
			/* lw annual stdev only needs to be initialized once */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_LFMWFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve LIVE_FUEL_MOIST_WOOD_FILE property. \n", ERR_EINVAL);
				}
			/* open file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open file containing live woody fuel moistures. \n", ERR_EIOFAIL);
				}
			/* create the table of values */
			if ( (slwfm_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					LIVE_FUEL_MOIST_SEP_CHARS, LIVE_FUEL_MOIST_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to initialize historical Live Fuel Moisture table. \n", ERR_EFAILED);
				}
			fclose(fstream);					
			}		
		
		/* start new year */
		if ( syear != year )	{
			/* set current record numbers */
			slh_srec = 0;
			slw_srec = 0;
			/* advance to current date in table */
			if ( AdvanceRecToDate(slhfm_tbl, month, day, &slh_srec) 
				|| AdvanceRecToDate(slwfm_tbl, month, day, &slw_srec) )	{
				ERR_ERROR("Unable to find current date in data table. \n", ERR_EBADFUNC);
				}
			/* get first live herb */
			slhfm = DBLTWODARRAY_GET_DATA(slhfm_tbl, slh_srec, LIVE_FUEL_MOIST_FIXED_VAL_TBL_INDEX);
			slhfm /= 100.0;
			slh_srec++;			
			/* get first live woody */
			slwfm = DBLTWODARRAY_GET_DATA(slwfm_tbl, slw_srec, LIVE_FUEL_MOIST_FIXED_VAL_TBL_INDEX);
			slwfm /= 100.0;
			slw_srec++;			
			/* set year */
			syear = year;		
			}
		
		/* see if live herb table contains value for this day */
		if ( (slh_srec < DBLTWODARRAY_SIZE_ROW(slhfm_tbl))
				&& (month == DBLTWODARRAY_GET_DATA(slhfm_tbl, slh_srec, LIVE_FUEL_MOIST_MO_TBL_INDEX))
				&& (day == DBLTWODARRAY_GET_DATA(slhfm_tbl, slh_srec, LIVE_FUEL_MOIST_DY_TBL_INDEX)) )	{
			slhfm = DBLTWODARRAY_GET_DATA(slhfm_tbl, slh_srec, LIVE_FUEL_MOIST_FIXED_VAL_TBL_INDEX);
			slhfm /= 100.0;
			slh_srec++;
			}

		/* see if live woody table contains value for this day */
		if ( (slw_srec < DBLTWODARRAY_SIZE_ROW(slwfm_tbl))
				&& (month == DBLTWODARRAY_GET_DATA(slwfm_tbl, slw_srec, LIVE_FUEL_MOIST_MO_TBL_INDEX))
				&& (day == DBLTWODARRAY_GET_DATA(slwfm_tbl, slw_srec, LIVE_FUEL_MOIST_DY_TBL_INDEX)) )	{
			slwfm = DBLTWODARRAY_GET_DATA(slwfm_tbl, slw_srec, LIVE_FUEL_MOIST_FIXED_VAL_TBL_INDEX);
			slwfm /= 100.0;
			slw_srec++;
			}
					
		/* set {month, day} for future calls */
		smonth = month;
		sday = day;
		}

	*lhfm = slhfm;
	*lwfm = slwfm;
		
	return ERR_SUCCESS;
	}
																					
int GetLiveFuelMoistRANDHFromProps(ChHashTable * proptbl, int year, int month, int day, int hour,
											double rwx, double rwy, 
											double * lhfm, double * lwfm)	{
	/* static variables used to store state across function calls */
	static int syear				= -1;
	static int smonth 				= 0;
	static int sday 				= 0;
	static int slh_srec				= 0;
	static int slw_srec				= 0;
	static double slhfm				= 0.0;
	static double slwfm				= 0.0;
	static DblTwoDArray * slhfm_tbl = NULL;
	static DblTwoDArray * slwfm_tbl	= NULL;
	static double slh_amean			= -1.0;
	static double slh_asdev			= -1.0;
	static double slw_amean			= -1.0;
	static double slw_asdev			= -1.0;	
	static double slh_Z				= -1.0;
	static double slw_Z				= -1.0;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	char * val						= NULL;				/* val associated with keywords in file */
	FILE * fstream					= NULL;				/* file stream */
	
	if ( (smonth != month) || (sday != day) )	{
		/* new live herbaceous fuel moisture table needed */
		if ( slhfm_tbl == NULL )	{
			/* lh annual mean only needs to be initialized once */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_LFMHFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve LIVE_FUEL_MOIST_HERB_FILE property. \n", ERR_EINVAL);
				}
			/* open file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open file containing live herbaceous fuel moistures. \n", ERR_EIOFAIL);
				}
			/* retrieve string mean stored in */
			val = GetValFromKeyStringFStreamIO(fstream, LIVE_FUEL_MOIST_KEYWORD_ANNMEAN, 
									LIVE_FUEL_MOIST_SEP_CHARS, LIVE_FUEL_MOIST_COMMENT_CHAR);
			slh_amean = atof(val);
			free(val);
			/* retrieve string stdev stored in */
			val = GetValFromKeyStringFStreamIO(fstream, LIVE_FUEL_MOIST_KEYWORD_ANNSTDEV, 
									LIVE_FUEL_MOIST_SEP_CHARS, LIVE_FUEL_MOIST_COMMENT_CHAR);
			slh_asdev = atof(val);
			free(val);
			/* create the table of values */
			if ( (slhfm_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					LIVE_FUEL_MOIST_SEP_CHARS, LIVE_FUEL_MOIST_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to initialize historical Live Fuel Moisture table. \n", ERR_EFAILED);
				}
			fclose(fstream);
			}
		/* new live woody fuel moisture table needed */
		if ( slwfm_tbl == NULL )	{
			/* lw annual mean only needs to be initialized once */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_LFMWFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve LIVE_FUEL_MOIST_WOOD_FILE property. \n", ERR_EINVAL);
				}
			/* open file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open file containing live woody fuel moistures. \n", ERR_EIOFAIL);
				}
			/* retrieve string mean stored in */
			val = GetValFromKeyStringFStreamIO(fstream, LIVE_FUEL_MOIST_KEYWORD_ANNMEAN, 
									LIVE_FUEL_MOIST_SEP_CHARS, LIVE_FUEL_MOIST_COMMENT_CHAR);
			slw_amean = atof(val);
			free(val);
			/* retrieve string stdev stored in */
			val = GetValFromKeyStringFStreamIO(fstream, LIVE_FUEL_MOIST_KEYWORD_ANNSTDEV, 
									LIVE_FUEL_MOIST_SEP_CHARS, LIVE_FUEL_MOIST_COMMENT_CHAR);
			slw_asdev = atof(val);
			free(val);
			/* create the table of values */
			if ( (slwfm_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					LIVE_FUEL_MOIST_SEP_CHARS, LIVE_FUEL_MOIST_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to initialize historical Live Fuel Moisture table. \n", ERR_EFAILED);
				}
			fclose(fstream);					
			}		
		
		/* start new year */
		if ( syear != year )	{
			/* set annual normalization factors */
			slh_Z = (randg(slh_amean, slh_asdev) - slh_amean) / slh_asdev;
			slw_Z = (randg(slw_amean, slw_asdev) - slw_amean) / slw_asdev;
			/* set current record numbers */
			slh_srec = 0;
			slw_srec = 0;
			/* advance to current date in table */
			if ( AdvanceRecToDate(slhfm_tbl, month, day, &slh_srec) 
				|| AdvanceRecToDate(slwfm_tbl, month, day, &slw_srec) )	{
				ERR_ERROR("Unable to find current date in data table. \n", ERR_EBADFUNC);
				}
			/* get first live herb */
			slhfm = (slh_Z * DBLTWODARRAY_GET_DATA(slhfm_tbl, slh_srec, LIVE_FUEL_MOIST_STDEV_TBL_INDEX)) +
					DBLTWODARRAY_GET_DATA(slhfm_tbl, slh_srec, LIVE_FUEL_MOIST_MEAN_TBL_INDEX);
			slhfm /= 100.0;
			slh_srec++;			
			/* get first live woody */
			slwfm = (slw_Z * DBLTWODARRAY_GET_DATA(slwfm_tbl, slw_srec, LIVE_FUEL_MOIST_STDEV_TBL_INDEX)) +
					DBLTWODARRAY_GET_DATA(slwfm_tbl, slw_srec, LIVE_FUEL_MOIST_MEAN_TBL_INDEX);
			slwfm /= 100.0;
			slw_srec++;						
			/* set year */			
			syear = year;
			}
		
		/* see if live herb table contains value for this day */
		if ( (slh_srec < DBLTWODARRAY_SIZE_ROW(slhfm_tbl))
				&& (month == DBLTWODARRAY_GET_DATA(slhfm_tbl, slh_srec, LIVE_FUEL_MOIST_MO_TBL_INDEX))
				&& (day == DBLTWODARRAY_GET_DATA(slhfm_tbl, slh_srec, LIVE_FUEL_MOIST_DY_TBL_INDEX)) )	{
			slhfm = (slh_Z * DBLTWODARRAY_GET_DATA(slhfm_tbl, slh_srec, LIVE_FUEL_MOIST_STDEV_TBL_INDEX)) +
					DBLTWODARRAY_GET_DATA(slhfm_tbl, slh_srec, LIVE_FUEL_MOIST_MEAN_TBL_INDEX);
			slhfm /= 100.0;
			slh_srec++;
			}

		/* see if live woody table contains value for this day */
		if ( (slw_srec < DBLTWODARRAY_SIZE_ROW(slwfm_tbl))
				&& (month == DBLTWODARRAY_GET_DATA(slwfm_tbl, slw_srec, LIVE_FUEL_MOIST_MO_TBL_INDEX))
				&& (day == DBLTWODARRAY_GET_DATA(slwfm_tbl, slw_srec, LIVE_FUEL_MOIST_DY_TBL_INDEX)) )	{
			slwfm = (slw_Z * DBLTWODARRAY_GET_DATA(slwfm_tbl, slw_srec, LIVE_FUEL_MOIST_STDEV_TBL_INDEX)) +
					DBLTWODARRAY_GET_DATA(slwfm_tbl, slw_srec, LIVE_FUEL_MOIST_MEAN_TBL_INDEX);
			slwfm /= 100.0;
			slw_srec++;
			}
					
		/* set {month, day} for future calls */
		smonth = month;
		sday = day;
		}

	*lhfm = slhfm;
	*lwfm = slwfm;
		
	return ERR_SUCCESS;
	}						

int GetLiveFuelMoistSPATIALFromProps(ChHashTable * proptbl, int year, int month, int day, int hour,
											double rwx, double rwy, 
											double * lhfm, double * lwfm)	{
	/* static variables used to store state across function calls */
	static int smonth 			= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static StrTwoDArray * slfm_tbl	= NULL;
	static GridData * slh_grid 	= NULL, *slw_grid = NULL;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	int i,j;

	/* check to see if new tables are needed */
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new lfm tbl */
		if (slfm_tbl == NULL )	{
			/* retriev lfm filename from properties */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_LFMSPFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve LIVE_FUEL_MOIST_SPATIAL_FILE property. \n", ERR_EINVAL);
				}
			/* open lfm file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open LIVE_FUEL_MOIST_SPATIAL_FILE. \n", ERR_EIOFAIL);
				}
			/* retrieve contents of lfm file */
			if ( (slfm_tbl = GetStrTwoDArrayTableFStreamIO(fstream, LIVE_FUEL_MOIST_RAST_LFM_SEP_CHARS, 
						LIVE_FUEL_MOIST_RAST_LFM_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to parse LIVE_FUEL_MOIST_SPATIAL_FILE. \n", ERR_EINVAL);
				}
			/* cleanup */
			fclose(fstream);
			}							
		/* new lh grid */
		if ( slh_grid == NULL )	{
			if ( (slh_grid = InitGridDataFromAsciiRaster(STRTWODARRAY_GET_DATA(slfm_tbl, 0, LIVE_FUEL_MOIST_RAST_LFM_LFH_FILE_TBL_INDEX), 
								EnumDblGrid)) == NULL )	{
				ERR_ERROR("Unable to initialize GridData from file listed in LIVE_FUEL_MOIST_SPATIAL_FILE. \n", ERR_EINVAL);
				}
			}
    /* new lw grid */
		if ( slw_grid == NULL )	{
			if ( (slw_grid = InitGridDataFromAsciiRaster(STRTWODARRAY_GET_DATA(slfm_tbl, 0, LIVE_FUEL_MOIST_RAST_LFM_LFW_FILE_TBL_INDEX), 
								EnumDblGrid)) == NULL )	{
				ERR_ERROR("Unable to initialize GridData from file listed in LIVE_FUEL_MOIST_SPATIAL_FILE. \n", ERR_EINVAL);
				}
			}
		/* find record in lh table */
		for(i = 0; i < DBLTWODARRAY_SIZE_ROW(slfm_tbl); i++)	{
			/* parse the lfm table from strings to numbers */
			smonth = atoi(STRTWODARRAY_GET_DATA(slfm_tbl, i, LIVE_FUEL_MOIST_RAST_LFM_MO_TBL_INDEX));
			sday = atoi(STRTWODARRAY_GET_DATA(slfm_tbl, i, LIVE_FUEL_MOIST_RAST_LFM_DY_TBL_INDEX));
			shour = atoi(STRTWODARRAY_GET_DATA(slfm_tbl, i, LIVE_FUEL_MOIST_RAST_LFM_HR_TBL_INDEX)) / 100;
			/* find matching month and day and hour */
			if ( (smonth == month) && (sday == day) && (shour == hour) )	{
				/* free old grid */
				FreeGridData(slh_grid);
				/* initialize new grid */
				if ( (slh_grid = InitGridDataFromAsciiRaster(STRTWODARRAY_GET_DATA(slfm_tbl, i, LIVE_FUEL_MOIST_RAST_LFM_LFH_FILE_TBL_INDEX), 
									EnumDblGrid)) == NULL )	{
					ERR_ERROR("Unable to initialize GridData from file listed in LIVE_FUEL_MOIST_SPATIAL_FILE. \n", ERR_EINVAL);
					}
				/* free old grid */
				FreeGridData(slw_grid);
				/* initialize new grid */
				if ( (slw_grid = InitGridDataFromAsciiRaster(STRTWODARRAY_GET_DATA(slfm_tbl, i, LIVE_FUEL_MOIST_RAST_LFM_LFW_FILE_TBL_INDEX), 
									EnumDblGrid)) == NULL )	{
					ERR_ERROR("Unable to initialize GridData from file listed in LIVE_FUEL_MOIST_SPATIAL_FILE. \n", ERR_EINVAL);
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
	if ( CoordTransRealWorldToRaster(rwx, rwy, slh_grid->ghdr->cellsize, slh_grid->ghdr->cellsize,
			COORD_TRANS_XLLCORNER_TO_XULCNTR(slh_grid->ghdr->xllcorner, slh_grid->ghdr->cellsize),
			COORD_TRANS_YLLCORNER_TO_YULCNTR(slh_grid->ghdr->yllcorner, slh_grid->ghdr->cellsize, slh_grid->ghdr->nrows),
			&i, &j) )	{
		ERR_ERROR("Unable to transform real world coordinates to grid indecies. \n", ERR_ESING);
		}			
													
	/* retrieve lh at coordinate */
	GRID_DATA_GET_DATA(slh_grid, i, j, *lhfm);
  *lhfm = *lhfm / 100.0;

	/* transform array indecies into spatial coordinates */
	if ( CoordTransRealWorldToRaster(rwx, rwy, slw_grid->ghdr->cellsize, slw_grid->ghdr->cellsize,
			COORD_TRANS_XLLCORNER_TO_XULCNTR(slw_grid->ghdr->xllcorner, slw_grid->ghdr->cellsize),
			COORD_TRANS_YLLCORNER_TO_YULCNTR(slw_grid->ghdr->yllcorner, slw_grid->ghdr->cellsize, slw_grid->ghdr->nrows),
			&i, &j) )	{
		ERR_ERROR("Unable to transform real world coordinates to grid indecies. \n", ERR_ESING);
		}			
													
	/* retrieve lw at coordinate */
	GRID_DATA_GET_DATA(slw_grid, i, j, *lwfm);
  *lwfm = *lwfm / 100.0;

	return ERR_SUCCESS;
	}
	
int AdvanceRecToDate(DblTwoDArray * da, int month, int day, int * rec)	{
	int found = 0;
	
	/*check args */
	if ( da == NULL || DBLTWODARRAY_SIZE_ROW(da) < 1 )	{
		ERR_ERROR("Table of array values not initialized, unable to advance to current date in table. \n", ERR_EINVAL);
		}
	
	/* increment records until day and month is found */
	while ( *rec < DBLTWODARRAY_SIZE_ROW(da) )	{
		/* identical match found */
		if ( month == DBLTWODARRAY_GET_DATA(da, *rec, LIVE_FUEL_MOIST_MO_TBL_INDEX)
			&& day == DBLTWODARRAY_GET_DATA(da, *rec, LIVE_FUEL_MOIST_DY_TBL_INDEX) )	{
			found = 1;
			break;
			}
		/* no identical match, if possible get previous record in table */
		if ( month <= DBLTWODARRAY_GET_DATA(da, *rec, LIVE_FUEL_MOIST_MO_TBL_INDEX)
			&& day <= DBLTWODARRAY_GET_DATA(da, *rec, LIVE_FUEL_MOIST_DY_TBL_INDEX) )	{
			if ( *rec != 0 )	{
				*rec = *rec - 1;
				}
			found = 1;
			break;
			}	
		*rec = *rec + 1;
		}
		
	/* if day and month were not found, assign last date in file */
	if ( found == 0 )	{
		*rec = DBLTWODARRAY_SIZE_ROW(da) - 1;
		}
		
	return ERR_SUCCESS;
	}						
											 
/* end of LiveFuelMoist.c */
