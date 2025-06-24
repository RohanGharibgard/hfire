/*! 
 * \file SantaAna.c
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
 
#include "SantaAna.h"

/* constant used to reference units of windspeed  returned by all functions */
static const EnumUnitVelocity smps = EnumMpsVelocity;

IsSantaAnaNowFromProps(ChHashTable * proptbl, int year, int month, int day)		{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int syear				= 0;	
	static double sprob_sa			= -1.0;				/* daily annual Santa Ana probability */
	static int sis_sa_now			= 0;				/* flag set during Santa Ana event */
	static int sexp_sa_dy			= 0;				/* counter tracking expired days during current Santa Ana */
	/* stack variables */
	KeyVal * entry					= NULL;				/* key/val instances from table */	
	double sa_freq_yr				= 0.0;				/* number of Santa Ana events per year (avg) */
	int	st_mo, st_dy, end_mo, end_dy;					/* start and end month and day of simulation */


	/* check args */
	if ( proptbl == NULL ) 	{
		ERR_ERROR_CONTINUE("Arguments supplied to determine Santa Ana occurence invalid. \n", ERR_EINVAL);
		return 0;
		}

	/* retrieve Santa Ana frequency per year */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SANAFREQ), (void *)&entry) )	{
		ERR_ERROR_CONTINUE("Unable to retrieve SANTA_ANA_FREQUENCY_PER_YEAR property. \n", ERR_EINVAL);
		return 0;
		}
	sa_freq_yr = atof((char *) entry->val);		
	
	if( UNITS_FP_GT_ZERO(sa_freq_yr) && ((smonth != month) || (sday != day)) )	{
		/* retrieve num days in simulation year and calculate Santa Ana probability, only done once */
		if ( UNITS_FP_LT_ZERO(sprob_sa) )	{
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
			/* calculate daily annual probability */			 
			sprob_sa = sa_freq_yr / FireTimerGetDaysDifftime(st_mo, st_dy, end_mo, end_dy);
			}
		/* is there an ongoing Santa Ana event */
		if ( sis_sa_now == 1 )	{
			/* increment num of days that have expired during current Santa Ana */
			sexp_sa_dy++;
			/* retrieve duration of all Santa Anas */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SANANUMD), (void *)&entry) )	{
				ERR_ERROR_CONTINUE("Unable to retrieve SANTA_ANA_NUM_DAYS_DURATION property. \n", ERR_EINVAL);
				return 0;
				}
			/* the length of the current Santa Ana has reached the limit of its duration or a new year has started */
			if ( atoi((char *) entry->val) == sexp_sa_dy  || syear != year )	{
				sexp_sa_dy = 0;
				sis_sa_now = 0;
				}
			}
		/* draw a uniform random number and determine if santa ana occurs */
		else	{
			/* Santa Ana occurs */
			if ( randu(0.0, 1.0) < sprob_sa )	{
				/* set Santa Ana flag for subsequent calls to this method */
				sis_sa_now = 1;			
				/* write occurence into user-specified Santa Ana event occurences file */
				if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SANANUMD), (void *)&entry) )	{
					ERR_ERROR_CONTINUE("Unable to retrieve SANTA_ANA_NUM_DAYS_DURATION property. \n", ERR_EINVAL);
					return 0;
					}			
				if ( FireExportSantaAnaEvtTxtFile(proptbl, atoi((char *) entry->val), year, month, day) )	{
					ERR_ERROR_CONTINUE("Unable to insert Santa Ana event into EXPORT_SANTA_ANA_EVT_FILE. \n", ERR_EIOFAIL);
					return 0;
					}
				}
			}
		/* set {month, day, year} for future calls */
		smonth = month;
		sday = day;
		syear = year;
		}		
	
	return sis_sa_now;
	}

int GetSantaAnaEnvFromProps(ChHashTable * proptbl, int month, int day, int hour,
									double * waz, double fbedhgtm, double * wspmps,
									double * d1hfm, double * d10hfm, double * d100hfm)		{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	static int srec					= 0;
	static double swaz				= 0.0;
	static DblTwoDArray * swaz_tbl 	= NULL;	
	static double swsp				= 0.0;
	static EnumUnitVelocity sunits	= EnumUnknownVelocity;	
	static DblTwoDArray * swsp_tbl 	= NULL;
	static double sd1hfm			= 0.0;
	static double sd10hfm			= 0.0;
	static double sd100hfm			= 0.0;
	static DblTwoDArray * sd10h_tbl = NULL;
  static EnumFireVal swspwaf = VAL_AB79;
  static double sd1hfminc = 0.02;
  static double sd100hfminc = 0.02;
	/* stack variables */	
	KeyVal * entry					= NULL;				/* key/val instances from table */	
	FILE * fstream					= NULL;				/* file stream */
	char * units					= NULL;
	int i,j;
												
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* new wind azimuth table */
		if (swaz_tbl == NULL )	{
			*waz = swaz;		
			/* retrieve waz filename */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SANAWAZF), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve SANTA_ANA_WIND_AZIMUTH_FILE property. \n", ERR_EINVAL);
				}
			/* open file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open SANTA_ANA_WIND_AZIMUTH_FILE. \n", ERR_EIOFAIL);
				}
			/* create the table of values */
			if ( (swaz_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					WIND_AZIMUTH_WAZ_SEP_CHARS, WIND_AZIMUTH_WAZ_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to parse SANTA_ANA_WIND_AZIMUTH_FILE. \n", ERR_EFAILED);
				}
			fclose(fstream);
			}
		/* new wind speed table */
		if (swsp_tbl == NULL )	{					
			*wspmps = swsp;		
			/* retrieve wsp filename */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SANAWSPDF), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve SANTA_ANA_WIND_SPEED_FILE property. \n", ERR_EINVAL);
				}
			/* open file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open SANTA_ANA_WIND_SPEED_FILE. \n", ERR_EIOFAIL);
				}
			/* retrieve string indicating units windspeed stored in */
			if ( (units = GetValFromKeyStringFStreamIO(fstream, WIND_SPD_WSP_KEYWORD_UNITS, 
					WIND_SPD_WSP_SEP_CHARS, WIND_SPD_WSP_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to determine units for SANTA_ANA_WIND_SPEED_FILE. \n", ERR_EINVAL);
				}
			/* create the table of values */			
			if ( (swsp_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					WIND_SPD_WSP_SEP_CHARS, WIND_SPD_WSP_COMMENT_CHAR)) == NULL )	{
				free(units);
				fclose(fstream);
				ERR_ERROR("Unable to parse SANTA_ANA_WIND_SPEED_FILE. \n", ERR_EFAILED);
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
		/* new dead fuel moistures table needed, only done once */
		if (sd10h_tbl == NULL )	{
			/* initialize returned vars in case table not created */
			*d1hfm = sd1hfm;
			*d10hfm = sd10hfm;
			*d100hfm = sd100hfm;		
			/* table of fixed values only needs to be initialized once */
			if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SANADFMF), (void *)&entry) )	{
				ERR_ERROR("Unable to retrieve SANTA_ANA_DEAD_FUEL_MOIST_FILE property. \n", ERR_EINVAL);
				}
			/* open fixed azimuth file */
			if ( (fstream = fopen((char *) entry->val, "r")) == NULL )	{
				ERR_ERROR("Unable to open file containing Santa Ana Dead Fuel Moisture. \n", ERR_EIOFAIL);
				}
			/* create the table of values */
			if ( (sd10h_tbl = GetDblTwoDArrayTableFStreamIO(fstream, 
					DEAD_FUEL_MOIST_10H_SEP_CHARS, DEAD_FUEL_MOIST_10H_COMMENT_CHAR)) == NULL )	{
				fclose(fstream);
				ERR_ERROR("Unable to initialize Santa Ana Dead Fuel Moisture table. \n", ERR_EFAILED);
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
		
		/* new record index required */
		if ( (smonth != month) || (sday != day) )	{
			if ( (DBLTWODARRAY_SIZE_ROW(swaz_tbl) != DBLTWODARRAY_SIZE_ROW(swsp_tbl)) 
					&& (DBLTWODARRAY_SIZE_ROW(swsp_tbl) != DBLTWODARRAY_SIZE_ROW(sd10h_tbl)) )	{
				ERR_ERROR("Unable to retrieve Santa Ana conditions, table record numbers unequal. \n", ERR_EINVAL);
				}
			srec = randi(0) % DBLTWODARRAY_SIZE_ROW(swaz_tbl);
			}

		/* new environmental variables required */
		if ( UNITS_FP_GT_ZERO(DBLTWODARRAY_GET_DATA(swaz_tbl, srec, SANTA_ANA_HR_TO_TBL_INDEX(hour))) )
			swaz = DBLTWODARRAY_GET_DATA(swaz_tbl, srec, SANTA_ANA_HR_TO_TBL_INDEX(hour));
		if ( UNITS_FP_GT_ZERO(DBLTWODARRAY_GET_DATA(swsp_tbl, srec, SANTA_ANA_HR_TO_TBL_INDEX(hour))) )
			swsp = DBLTWODARRAY_GET_DATA(swsp_tbl, srec, SANTA_ANA_HR_TO_TBL_INDEX(hour));
		sd10hfm = DBLTWODARRAY_GET_DATA(sd10h_tbl, srec, SANTA_ANA_HR_TO_TBL_INDEX(hour)) / 100;
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
				
		/* set {month, day, hour} for future calls */
		smonth = month;
		sday = day;
		shour = hour;			
		}

	*waz = swaz;
  if ( swspwaf == VAL_BHP ) {
    *wspmps = ConvertWindSpeedAtRefHgtToArbitraryHgtBHP(swsp, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm);
  } else if ( swspwaf == VAL_NOWAF ) {
    *wspmps = swsp;
	} else { /* default */
    *wspmps = ConvertWindSpeedAtRefHgtToArbitraryHgtAB79(swsp, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm);
  }
	*d1hfm = sd1hfm;
	*d10hfm = sd10hfm;
	*d100hfm = sd100hfm;
				
	return ERR_SUCCESS;
	}

/* end of SantaAna.c */
