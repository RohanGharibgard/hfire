/*! 
 * \file WindAzimuth.c
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
 
#include "WindAzimuth.h"

int GetWindAzimuthFIXEDFromProps(ChHashTable * proptbl, int month, int day, int hour,
									double rwx, double rwy, double * waz)	{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static double swaz				= 0.0;
	static DblTwoDArray * swaz_tbl 	= NULL;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	int i;

	/* args not used in FIXED implementation */
	rwx = rwy = 0.0;
	
	/* check to see if new wind azimuth needed */
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new wind azimuth table */
		if (swaz_tbl == NULL )	{
			*waz = swaz;		
			/* retrieve waz filename */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WAZFFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve WIND_AZIMUTH_FIXED_FILE property. \n", ERR_EINVAL);
				}
			/* open file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open WIND_AZIMUTH_FIXED_FILE. \n", ERR_EIOFAIL);
				}
			/* create the table of values */
			if ( (swaz_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					WIND_AZIMUTH_WAZ_SEP_CHARS, WIND_AZIMUTH_WAZ_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to parse WIND_AZIMUTH_FIXED_FILE. \n", ERR_EFAILED);
				}
			fclose(fstream);
			}
		/* find record in table */
		for(i = 0; i < DBLTWODARRAY_SIZE_ROW(swaz_tbl); i++)	{
			/* find matching month and day */
			if ( DBLTWODARRAY_GET_DATA(swaz_tbl, i, WIND_AZIMUTH_MO_WAZ_TBL_INDEX) == month 
				 && DBLTWODARRAY_GET_DATA(swaz_tbl, i, WIND_AZIMUTH_DY_WAZ_TBL_INDEX) == day )	{
				/* retrieve waz on that month and day */
				swaz = DBLTWODARRAY_GET_DATA(swaz_tbl, i, WIND_AZIMUTH_HR_TO_WAZ_TBL_INDEX(hour));
				break;
				}
			}
		/* set {month, day, hour} for future calls */
		smonth = month;
		sday = day;
		shour = hour;
		}

	*waz = swaz;

	return ERR_SUCCESS;	
	}

int GetWindAzimuthRANDUFromProps(ChHashTable * proptbl, int month, int day, int hour,
									double rwx, double rwy, double * waz)	{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static double swaz				= 0.0;
	
	/* args not used in RANDU implementation */
	rwx = rwy = 0.0;
	
	if ( proptbl == NULL )	{
		ERR_ERROR("Simulation properties table not initialized. \n", ERR_EINVAL);
		}

	/* check to see if new wind azimuth needed */
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{	
		/* new wind azimuth */
		swaz = randu(WIND_AZIMUTH_RANDU_MIN_AZ, WIND_AZIMUTH_RANDU_MAX_AZ);
		/* set {month, day, hour} for future calls */
		smonth = month;
		sday = day;
		shour = hour;		
		}

	*waz = swaz;

	return ERR_SUCCESS;
	}

int GetWindAzimuthRANDHFromProps(ChHashTable * proptbl, int month, int day, int hour,
									double rwx, double rwy, double * waz)	{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static double swaz				= 0.0;
	static DblTwoDArray * swaz_tbl 	= NULL;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	int i,j;
	
	/* args not used in RANDH implementation */
	rwx = rwy = 0.0;
	
	/* check to see if new wind azimuth needed */
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new wind azimuth table */
		if (swaz_tbl == NULL )	{
			*waz = swaz;		
			/* retrieve waz filename */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WAZHFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve WIND_AZIMUTH_HISTORICAL_FILE property. \n", ERR_EINVAL);
				}
			/* open file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open WIND_AZIMUTH_HISTORICAL_FILE. \n", ERR_EIOFAIL);
				}
			/* create the table of values */
			if ( (swaz_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					WIND_AZIMUTH_WAZ_SEP_CHARS, WIND_AZIMUTH_WAZ_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to parse WIND_AZIMUTH_HISTORICAL_FILE. \n", ERR_EFAILED);
				}
			fclose(fstream);
			}
		/* find wind azimuth from random record in table that is not NO DATA */
		do	{
			/* retrieve a record at random from table within range 0 to num_recs */
			i = randi(0) % DBLTWODARRAY_SIZE_ROW(swaz_tbl);
			j = WIND_AZIMUTH_HR_TO_WAZ_TBL_INDEX(hour);
			/* retrieve waz from random recno using current hour */
			swaz = DBLTWODARRAY_GET_DATA(swaz_tbl, i, j);		
			} while( swaz == WIND_AZIMUTH_WAZ_NO_DATA_VALUE );
		/* set {month, day, hour} for future calls */
		smonth = month;
		sday = day;
		shour = hour;
		}

	*waz = swaz;

	return ERR_SUCCESS;			
	}

int GetWindAzimuthSPATIALFromProps(ChHashTable * proptbl, int month, int day, int hour, 
										double rwx, double rwy, double * waz)	{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static StrTwoDArray * satm_tbl	= NULL;
	static GridData * swaz_grid 	= NULL;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	double waz_deg = 0.0;
	int i,j;

	/* check to see if new tables are needed */
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new atm tbl */
		if (satm_tbl == NULL )	{
			/* retriev atm filename from properties */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WAZSPFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve WIND_AZIMUTH_SPATIAL_FILE property. \n", ERR_EINVAL);
				}
			/* open atm file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open WIND_AZIMUTH_SPATIAL_FILE. \n", ERR_EIOFAIL);
				}
			/* retrieve units from atm file */
			if ( IsKeyStringPresentFStreamIO(fstream, WIND_AZIMUTH_ATM_KEYWORD_ENGLISH, 
					WIND_AZIMUTH_ATM_COMMENT_CHAR) )	{
				}
			else	{
				rewind(fstream);
				if ( IsKeyStringPresentFStreamIO(fstream, WIND_AZIMUTH_ATM_KEYWORD_METRIC, 
						WIND_AZIMUTH_ATM_COMMENT_CHAR) )	{
					}
				else	{
					fclose(fstream);
					ERR_ERROR("Unable to retrieve units from WIND_AZIMUTH_SPATIAL_FILE. \n", ERR_EUNITS);
					}
				}
			/* retrieve contents of atm file */
			if ( (satm_tbl = GetStrTwoDArrayTableFStreamIO(fstream, WIND_AZIMUTH_ATM_SEP_CHARS, 
						WIND_AZIMUTH_ATM_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to parse WIND_AZIMUTH_SPATIAL_FILE. \n", ERR_EINVAL);
				}
			/* cleanup */
			fclose(fstream);
			}							
		/* new waz grid */
		if ( swaz_grid == NULL )	{
			if ( (swaz_grid = InitGridDataFromAsciiRaster(STRTWODARRAY_GET_DATA(satm_tbl, 0, WIND_AZIMUTH_FILE_ATM_TBL_INDEX), 
								EnumDblGrid)) == NULL )	{
				ERR_ERROR("Unable to initialize GridData from file listed in WIND_AZIMUTH_SPATIAL_FILE. \n", ERR_EINVAL);
				}
			}
		/* find record in atm table */
		for(i = 0; i < DBLTWODARRAY_SIZE_ROW(satm_tbl); i++)	{
			/* parse the atm table from strings to numbers */
			smonth = atoi(STRTWODARRAY_GET_DATA(satm_tbl, i, WIND_AZIMUTH_MO_ATM_TBL_INDEX));
			sday = atoi(STRTWODARRAY_GET_DATA(satm_tbl, i, WIND_AZIMUTH_DY_ATM_TBL_INDEX));
			shour = atoi(STRTWODARRAY_GET_DATA(satm_tbl, i, WIND_AZIMUTH_HR_ATM_TBL_INDEX)) / 100;
			/* find matching month and day and hour */
			if ( (smonth == month) && (sday == day) && (shour == hour) )	{
				/* free old grid */
				FreeGridData(swaz_grid);
				/* initialize new grid */
				if ( (swaz_grid = InitGridDataFromAsciiRaster(STRTWODARRAY_GET_DATA(satm_tbl, i, WIND_AZIMUTH_FILE_ATM_TBL_INDEX), 
									EnumDblGrid)) == NULL )	{
					ERR_ERROR("Unable to initialize GridData from file listed in WIND_SPEED_SPATIAL_FILE. \n", ERR_EINVAL);
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
	if ( CoordTransRealWorldToRaster(rwx, rwy, swaz_grid->ghdr->cellsize, swaz_grid->ghdr->cellsize,
			COORD_TRANS_XLLCORNER_TO_XULCNTR(swaz_grid->ghdr->xllcorner, swaz_grid->ghdr->cellsize),
			COORD_TRANS_YLLCORNER_TO_YULCNTR(swaz_grid->ghdr->yllcorner, swaz_grid->ghdr->cellsize, swaz_grid->ghdr->nrows),
			&i, &j) )	{
		ERR_ERROR("Unable to transform real world coordinates to grid indecies. \n", ERR_ESING);
		}			
													
	/* retrieve wind azimuth at coordinate */
	GRID_DATA_GET_DATA(swaz_grid, i, j, waz_deg);

	*waz = waz_deg;
		
	return ERR_SUCCESS;
	}
 
/* end of WindAzimuth.c */
