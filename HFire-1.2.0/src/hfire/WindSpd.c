/*! 
 * \file WindSpd.c
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
 
#include "WindSpd.h"

/* constant used to reference units of windspeed  returned by all functions */
static const EnumUnitVelocity smps = EnumMpsVelocity;

/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */

int GetWindSpeedMpsFIXEDFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour,
										double rwx, double rwy, double * wspmps)		{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static double swsp				= 0.0;
	static DblTwoDArray * swsp_tbl 	= NULL;
	static EnumUnitVelocity sunits	= EnumUnknownVelocity;	
  static EnumFireVal swspwaf = VAL_AB79;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	char * units					= NULL;
	int i, j;

	/* args not used in FIXED implementation */
	rwx = rwy = 0.0;
	
	/* check to see if new windspeed needed */
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new wind speed table */
		if (swsp_tbl == NULL )	{
			*wspmps = swsp;
			/* retrieve wsp filename */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WSPDFFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve WIND_SPEED_FIXED_FILE property. \n", ERR_EINVAL);
				}
			/* open file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open WIND_SPEED_FIXED_FILE. \n", ERR_EIOFAIL);
				}
			/* retrieve string indicating units windspeed stored in */
			if ( (units = GetValFromKeyStringFStreamIO(fstream, WIND_SPD_WSP_KEYWORD_UNITS, 
					WIND_SPD_WSP_SEP_CHARS, WIND_SPD_WSP_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to determine units for WIND_SPEED_FIXED_FILE. \n", ERR_EINVAL);
				}
			/* create the table of values */			
			if ( (swsp_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					WIND_SPD_WSP_SEP_CHARS, WIND_SPD_WSP_COMMENT_CHAR)) == NULL )	{
				free(units);
				fclose(fstream);
				ERR_ERROR("Unable to parse WIND_SPEED_FIXED_FILE. \n", ERR_EFAILED);
				}
			/* convert values in table to mps */
			if ( strcmp(units, WIND_SPD_WSP_KEYWORD_MILEPHR) == 0 )	{
				sunits = EnumMphVelocity;
				}
			else if ( strcmp(units, WIND_SPD_WSP_KEYWORD_KMPHR) == 0 )	{
				sunits = EnumKmphVelocity;
				}
			for(i = 0; i < DBLTWODARRAY_SIZE_ROW(swsp_tbl); i++)	{
				for(j = WIND_SPD_HR_TO_WSP_TBL_INDEX(0); j < DBLTWODARRAY_SIZE_COL(swsp_tbl); j++)	{
					if ( DBLTWODARRAY_GET_DATA(swsp_tbl, i, j) == WIND_SPD_WSP_NO_DATA_VALUE )	{
						continue;
						}
					ConvertVelocityUnits(sunits, DBLTWODARRAY_GET_DATA(swsp_tbl, i, j), smps, &swsp);
					DBLTWODARRAY_SET_DATA(swsp_tbl, i, j, swsp);
					}
				}
			/* cleanup */
			free(units);
			fclose(fstream);
      /* retrieve windspeed adjustment factor */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WSPWAF), (void *)&entry) )	{
        swspwaf = VAL_AB79; /* default */
				}
      else if ( strcmp(entry->val, GetFireVal(VAL_BHP)) == 0 ) {
        swspwaf = VAL_BHP;
        }
      else if ( strcmp(entry->val, GetFireVal(VAL_NOWAF)) == 0 ) {
        swspwaf = VAL_NOWAF;
        }
			}
		/* find record in table */
		for(i = 0; i < DBLTWODARRAY_SIZE_ROW(swsp_tbl); i++)	{
			/* find matching month and day */
			if ( DBLTWODARRAY_GET_DATA(swsp_tbl, i, WIND_SPD_MO_WSP_TBL_INDEX) == month 
				 && DBLTWODARRAY_GET_DATA(swsp_tbl, i, WIND_SPD_DY_WSP_TBL_INDEX) == day )	{
				/* retrieve wsp on that month and day */
				if ( DBLTWODARRAY_GET_DATA(swsp_tbl, i, WIND_SPD_HR_TO_WSP_TBL_INDEX(hour)) 
						!= WIND_SPD_WSP_NO_DATA_VALUE )	{
					swsp = DBLTWODARRAY_GET_DATA(swsp_tbl, i, WIND_SPD_HR_TO_WSP_TBL_INDEX(hour));
					}
				break;
				}
			}
		/* set {month, day, hour} for future calls */
		smonth = month;
		sday = day;
		shour = hour;
		}
	
	/* apply windspeed reduction */
  if ( swspwaf == VAL_BHP ) {
    *wspmps = ConvertWindSpeedAtRefHgtToArbitraryHgtBHP(swsp, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm);
  } else if ( swspwaf == VAL_NOWAF ) {
    *wspmps = swsp;
	} else { /* default */
    *wspmps = ConvertWindSpeedAtRefHgtToArbitraryHgtAB79(swsp, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm);
  }

	return ERR_SUCCESS;
	}

int GetWindSpeedMpsRANDUFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour,
										double rwx, double rwy, double * wspmps)		{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static double swsp				= 0.0;
	static List * rng_list			= NULL;				/* min and max args supplied to rng */
	static double * min_rng, * max_rng;
  static EnumFireVal swspwaf = VAL_AB79;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */	
	ListElmt * lel					= NULL;

	/* args not used in RANDU implementation */
	rwx = rwy = 0.0;
	
	/* check to see if new windspeed needed */	
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new min and max range */
		if ( rng_list == NULL )	{
			/* retrieve wind speed range property */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WSPDUNIRNG), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve WIND_SPEED_UNIFORM_RANGE property. \n", ERR_EINVAL);
				}	
			/* parse value into List */
			if ( (rng_list = GetDblListFromLineFStreamIO((char *) entry->val, 
								WIND_SPD_RNG_SEP_CHARS)) == NULL )	{
				ERR_ERROR("Unable to retrieve min and max from WIND_SPEED_UNIFORM_RANGE. \n", ERR_EINVAL);
				}
			/* parse string to numbers */
			if ( LIST_SIZE(rng_list) != 2 )	{
				ERR_ERROR("WIND_SPEED_UNIFORM_RANGE property should contain two floating point tokens. \n", ERR_EINVAL);
				}
			lel = LIST_HEAD(rng_list);
			min_rng = LIST_GET_DATA(lel);
			lel = LIST_GET_NEXT_ELMT(lel);
			max_rng = LIST_GET_DATA(lel);
      /* retrieve windspeed adjustment factor */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WSPWAF), (void *)&entry) )	{
        swspwaf = VAL_AB79; /* default */
				}
      else if ( strcmp(entry->val, GetFireVal(VAL_BHP)) == 0 ) {
        swspwaf = VAL_BHP;
        }
      else if ( strcmp(entry->val, GetFireVal(VAL_NOWAF)) == 0 ) {
        swspwaf = VAL_NOWAF;
        }
			}
		/* new windspeed */
		swsp = randu(*min_rng, *max_rng);
		/* set {month, day, hour} for future calls */
		smonth = month;
		sday = day;
		shour = hour;		
		}

	/* apply windspeed reduction */
  if ( swspwaf == VAL_BHP ) {
    *wspmps = ConvertWindSpeedAtRefHgtToArbitraryHgtBHP(swsp, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm);
  } else if ( swspwaf == VAL_NOWAF ) {
    *wspmps = swsp;
	} else { /* default */
    *wspmps = ConvertWindSpeedAtRefHgtToArbitraryHgtAB79(swsp, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm);
  }
				
	return ERR_SUCCESS;
	}

int GetWindSpeedMpsRANDHFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour,
										double rwx, double rwy, double * wspmps)		{										
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static double swsp				= 0.0;
	static DblTwoDArray * swsp_tbl 	= NULL;
	static EnumUnitVelocity sunits	= EnumUnknownVelocity;	
  static EnumFireVal swspwaf = VAL_AB79;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	char * units					= NULL;
	int i, j;
	
	/* args not used in RANDH implementation */
	rwx = rwy = 0.0;
	
	/* check to see if new windspeed needed */
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new wind speed needed */
		if (swsp_tbl == NULL )	{
			*wspmps = swsp;		
			/* retrieve wsp filename */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WSPDHFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve WIND_SPEED_HISTORICAL_FILE property. \n", ERR_EINVAL);
				}
			/* open file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open WIND_SPEED_HISTORICAL_FILE. \n", ERR_EIOFAIL);
				}
			/* retrieve string indicating units windspeed stored in */
			if ( (units = GetValFromKeyStringFStreamIO(fstream, WIND_SPD_WSP_KEYWORD_UNITS, 
					WIND_SPD_WSP_SEP_CHARS, WIND_SPD_WSP_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to determine units for WIND_SPEED_HISTORICAL_FILE. \n", ERR_EINVAL);
				}
			/* create the table of values */			
			if ( (swsp_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					WIND_SPD_WSP_SEP_CHARS, WIND_SPD_WSP_COMMENT_CHAR)) == NULL )	{
				free(units);
				fclose(fstream);
				ERR_ERROR("Unable to parse WIND_SPEED_HISTORICAL_FILE. \n", ERR_EFAILED);
				}
			/* convert values in table to mps */
			if ( strcmp(units, WIND_SPD_WSP_KEYWORD_MILEPHR) == 0 )	{
				sunits = EnumMphVelocity;
				}
			else if ( strcmp(units, WIND_SPD_WSP_KEYWORD_KMPHR) == 0 )	{
				sunits = EnumKmphVelocity;
				}
			for(i = 0; i < DBLTWODARRAY_SIZE_ROW(swsp_tbl); i++)	{
				for(j = WIND_SPD_HR_TO_WSP_TBL_INDEX(0); j < DBLTWODARRAY_SIZE_COL(swsp_tbl); j++)	{
					if ( DBLTWODARRAY_GET_DATA(swsp_tbl, i, j) == WIND_SPD_WSP_NO_DATA_VALUE )	{
						continue;
						}				
					ConvertVelocityUnits(sunits, DBLTWODARRAY_GET_DATA(swsp_tbl, i, j), smps, &swsp);
					DBLTWODARRAY_SET_DATA(swsp_tbl, i, j, swsp);
					}
				}
			/* cleanup */
			free(units);
			fclose(fstream);
      /* retrieve windspeed adjustment factor */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WSPWAF), (void *)&entry) )	{
        swspwaf = VAL_AB79; /* default */
				}
      else if ( strcmp(entry->val, GetFireVal(VAL_NOWAF)) == 0 ) {
        swspwaf = VAL_NOWAF;
        }
      else if ( strcmp(entry->val, GetFireVal(VAL_BHP)) == 0 ) {
        swspwaf = VAL_BHP;
        }			
			}
		/* find wind speed from random record in table that is not NO DATA */
		do	{
			/* retrieve a record at random from table within range 0 to num_recs */
			i = randi(0) % DBLTWODARRAY_SIZE_ROW(swsp_tbl);			
			j = WIND_SPD_HR_TO_WSP_TBL_INDEX(hour);
			/* retrieve wsp from random recno using current hour */
			swsp = DBLTWODARRAY_GET_DATA(swsp_tbl, i, j);		
			} while( swsp == WIND_SPD_WSP_NO_DATA_VALUE );
		/* set {month, day, hour} for future calls */
		smonth = month;
		sday = day;
		shour = hour;
		}

	/* apply windspeed reduction */
  if ( swspwaf == VAL_BHP ) {
    *wspmps = ConvertWindSpeedAtRefHgtToArbitraryHgtBHP(swsp, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm);
  } else if ( swspwaf == VAL_NOWAF ) {
    *wspmps = swsp;
	} else { /* default */
    *wspmps = ConvertWindSpeedAtRefHgtToArbitraryHgtAB79(swsp, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm);
  }

	return ERR_SUCCESS;	
	}
	
int GetWindSpeedMpsSPATIALFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour, 
										double rwx, double rwy, double * wspmps)		{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static StrTwoDArray * satm_tbl	= NULL;
	static EnumUnitVelocity sunits	= EnumUnknownVelocity;	
	static GridData * swsp_grid 	= NULL;
  static EnumFireVal swspwaf = VAL_AB79;
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	char * units					= NULL;
	double wsp_org_units = 0.0, wsp_mps_units = 0.0;
	int i,j;

	/* check to see if new tables are needed */
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new atm tbl */
		if (satm_tbl == NULL )	{
			/* retriev atm filename from properties */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WSPDSPFILE), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve WIND_SPEED_SPATIAL_FILE property. \n", ERR_EINVAL);
				}
			/* open atm file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open WIND_SPEED_SPATIAL_FILE. \n", ERR_EIOFAIL);
				}
			/* retrieve units from atm file */
			if ( IsKeyStringPresentFStreamIO(fstream, WIND_SPD_ATM_KEYWORD_ENGLISH, 
					WIND_SPD_ATM_COMMENT_CHAR) )	{
				sunits = EnumMphVelocity;
				}
			else	{
				rewind(fstream);
				if ( IsKeyStringPresentFStreamIO(fstream, WIND_SPD_ATM_KEYWORD_METRIC, 
						WIND_SPD_ATM_COMMENT_CHAR) )	{
					sunits = EnumKmphVelocity;
					}
				else	{
					fclose(fstream);
					ERR_ERROR("Unable to retrieve units from WIND_SPEED_SPATIAL_FILE. \n", ERR_EUNITS);
					}
				}
			/* retrieve contents of atm file */
			if ( (satm_tbl = GetStrTwoDArrayTableFStreamIO(fstream, WIND_SPD_ATM_SEP_CHARS, 
						WIND_SPD_ATM_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to parse WIND_SPEED_SPATIAL_FILE. \n", ERR_EINVAL);
				}
			/* cleanup */
			fclose(fstream);
      /* retrieve windspeed adjustment factor */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WSPWAF), (void *)&entry) )	{
        swspwaf = VAL_AB79; /* default */
				}
      else if ( strcmp(entry->val, GetFireVal(VAL_BHP)) == 0 ) {
        swspwaf = VAL_BHP;
        }
      else if ( strcmp(entry->val, GetFireVal(VAL_NOWAF)) == 0 ) {
        swspwaf = VAL_NOWAF;
        }
			}							
		/* new wsp grid */
		if ( swsp_grid == NULL )	{
			if ( (swsp_grid = InitGridDataFromAsciiRaster(STRTWODARRAY_GET_DATA(satm_tbl, 0, WIND_SPD_FILE_ATM_TBL_INDEX), 
								EnumDblGrid)) == NULL )	{
				ERR_ERROR("Unable to initialize GridData from file listed in WIND_SPEED_SPATIAL_FILE. \n", ERR_EINVAL);
				}
			}
		/* find record in atm table */
		for(i = 0; i < DBLTWODARRAY_SIZE_ROW(satm_tbl); i++)	{
			/* parse the atm table from strings to numbers */
			smonth = atoi(STRTWODARRAY_GET_DATA(satm_tbl, i, WIND_SPD_MO_ATM_TBL_INDEX));
			sday = atoi(STRTWODARRAY_GET_DATA(satm_tbl, i, WIND_SPD_DY_ATM_TBL_INDEX));
			shour = atoi(STRTWODARRAY_GET_DATA(satm_tbl, i, WIND_SPD_HR_ATM_TBL_INDEX)) / 100;
			/* find matching month and day and hour */
			if ( (smonth == month) && (sday == day) && (shour == hour) )	{
				/* free old grid */
				FreeGridData(swsp_grid);
				/* initialize new grid */
				if ( (swsp_grid = InitGridDataFromAsciiRaster(STRTWODARRAY_GET_DATA(satm_tbl, i, WIND_SPD_FILE_ATM_TBL_INDEX), 
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
	if ( CoordTransRealWorldToRaster(rwx, rwy, swsp_grid->ghdr->cellsize, swsp_grid->ghdr->cellsize,
			COORD_TRANS_XLLCORNER_TO_XULCNTR(swsp_grid->ghdr->xllcorner, swsp_grid->ghdr->cellsize),
			COORD_TRANS_YLLCORNER_TO_YULCNTR(swsp_grid->ghdr->yllcorner, swsp_grid->ghdr->cellsize, swsp_grid->ghdr->nrows),
			&i, &j) )	{
		ERR_ERROR("Unable to transform real world coordinates to grid indecies. \n", ERR_ESING);
		}			
													
	/* retrieve windspeed at coordinate */
	GRID_DATA_GET_DATA(swsp_grid, i, j, wsp_org_units);
	
	/* convert units of windspeed to meters per second */
	ConvertVelocityUnits(sunits, wsp_org_units, smps, &wsp_mps_units);

	/* apply windspeed reduction */
  if ( swspwaf == VAL_BHP ) {
    *wspmps = ConvertWindSpeedAtRefHgtToArbitraryHgtBHP(wsp_mps_units, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm);
  } else if ( swspwaf == VAL_NOWAF ) {
    *wspmps = wsp_mps_units;
	} else { /* default */
    *wspmps = ConvertWindSpeedAtRefHgtToArbitraryHgtAB79(wsp_mps_units, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm);
  }

	return ERR_SUCCESS;
	}

double ConvertWindSpeedAtRefHgtToArbitraryHgtAB79(double wsmps, double refhgtm, double hgtm)	{
  double waf = 1.0 / log(((refhgtm + (0.36*hgtm)) / (0.13*hgtm)));
	return wsmps * waf;
	}

double ConvertWindSpeedAtRefHgtToArbitraryHgtBHP(double wsmps, double refhgtm, double hgtm)	{
  double waf = (1.371817779 / log(((refhgtm + (0.36*hgtm)) / (0.13*hgtm)))) + 0.046171831;
  if ( wsmps > 0.0 ) { return wsmps * waf; } /* only apply when wind is present */
  return wsmps;
  }

/* end of WindSpd.c */
