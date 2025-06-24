/*! 
 * \file FireConfig.c
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

#include "FireConfig.h"
			
int InitPropsFromFireConfig(ChHashTable ** proptbl, char * cfg_fname)	{
	FILE * fstream 	= NULL;
	int i;
	char * val		= NULL;	

	if ( cfg_fname == NULL )	{
		ERR_ERROR("Must supply the name of configuration file to retrieve config values. \n", ERR_EINVAL);
		}
	if ( (fstream = fopen(cfg_fname, "r")) == NULL )	{
		ERR_ERROR("Unable to open configuration file. \n", ERR_EIOFAIL);
		}
	if ( (*proptbl = InitChHashTable(FIRE_CONFIG_TBL_INI_CAPCTY, UniversalStringHashFunc,
										MatchStringKeyVal, FreeKeyVal)) == NULL )	{
		ERR_ERROR("Unable to initialize Hash Table of simulation properties. \n", ERR_EFAILED);
		}

	/* retrieve all properties from cfg file and place in hash table */
	for(i = 0; i < PROP_UP_BOUND; i++)	{
		if ( (val = GetValFromKeyStringFStreamIO(fstream, GetFireProp((EnumFireProp)i), 
						FIRE_CONFIG_SEPARATOR_CHARS, FIRE_CONFIG_COMMENT_CHAR)) == NULL )	{
			/* insert a key with val string of NULL */
			ChHashTableInsert(*proptbl, GetFireProp((EnumFireProp)i), 
							InitKeyVal(GetFireProp((EnumFireProp)i), GetFireVal(VAL_NULL)));
			}
		else	{
			/*insert the key and val */
			ChHashTableInsert(*proptbl, GetFireProp((EnumFireProp)i), InitKeyVal(GetFireProp((EnumFireProp)i), val));
			}
		rewind(fstream);
		}
		
	fclose(fstream);
		
	return ERR_SUCCESS;
	}

int InitGridsFromPropsFireConfig(ChHashTable * proptbl, GridData ** elev, GridData ** slope, GridData ** aspect)	{
	if ( proptbl == NULL )	{
		ERR_ERROR("Unable to initialize GridData with unitialized property Hash Table. \n", ERR_EINVAL);
		}	

	/* retrieve elev data */
	*elev = GetGridDataFromPropsFireGridData(proptbl, FIRE_GRIDDATA_ELEV_DATA);

	/* retrieve slope data */
	*slope = GetGridDataFromPropsFireGridData(proptbl, FIRE_GRIDDATA_SLOPE_DATA);
	
	/* retrieve aspect data */
	*aspect = GetGridDataFromPropsFireGridData(proptbl, FIRE_GRIDDATA_ASPECT_DATA);
	
	if ( *elev == NULL || *slope == NULL || *aspect == NULL )	{
		ERR_ERROR("Unable to initialize one of the GridData structures specified in config file. \n", ERR_EFAILED);
		}	
	
	if ( !IsGridDataMatchingFireGridData(*elev, *slope) || !IsGridDataMatchingFireGridData(*slope, *aspect) )	{
		ERR_ERROR("GridData structures specified in config file have mismatched extent or cell dimensions. \n", ERR_EFAILED);
		}
	
	return ERR_SUCCESS;									
	}

int InitFireTimerFromPropsFireConfig(ChHashTable * proptbl, FireTimer ** ft)	{
	/* store values used to construct FireTimer: styr, stmo, stdy, sthr, edyr, edmo, eddy, edhr */
	int styr, stmo, stdy, sthr, edyr, edmo, eddy, edhr;
	KeyVal * entry	= NULL;	
		
	styr = stmo = stdy = sthr = edyr = edmo = eddy = edhr = 0;
	
	if ( proptbl == NULL )	{
		ERR_ERROR("Unable to initialize FireTimer with uninitialized property Hash Table. \n", ERR_EINVAL);
		}

	/* simulation start properties */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMSTYR), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve simulation start year from property HashTable. \n", ERR_EFAILED);
		}
	styr = atoi(entry->val);
	
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMSTMO), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve simulation start month from property HashTable. \n", ERR_EFAILED);
		}
	stmo = atoi(entry->val);	

	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMSTDY), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve simulation start day from property HashTable. \n", ERR_EFAILED);
		}
	stdy = atoi(entry->val);	

	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMSTHR), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve simulation start hour from property HashTable. \n", ERR_EFAILED);
		}
	sthr = atoi(entry->val);	

	/* simulation end properties */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMEDYR), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve simulation end year from property HashTable. \n", ERR_EFAILED);
		}
	edyr = atoi(entry->val);
	
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMEDMO), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve simulation end month from property HashTable. \n", ERR_EFAILED);
		}
	edmo = atoi(entry->val);	

	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMEDDY), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve simulation end day from property HashTable. \n", ERR_EFAILED);
		}
	eddy = atoi(entry->val);	

	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMEDHR), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve simulation end hour from property HashTable. \n", ERR_EFAILED);
		}
	edhr = atoi(entry->val);
	
	/* initialize the FireTimer */
	if ( (*ft = InitFireTimer(styr, stmo, stdy, sthr, edyr, edmo, eddy, edhr)) == NULL )	{
		ERR_ERROR("Unable to initialize FireTimer from config file. \n", ERR_EFAILED);
		}

	return ERR_SUCCESS;	
	}
	
int InitFuelModelHashTableFromFuelModelListFireConfig(List * fmlist, ChHashTable ** fmtble)	{
	ListElmt * lel 			= NULL;
	FuelModel * fm 			= NULL;
	
	if ( fmlist == NULL )	{
		ERR_ERROR("Unable to initialize fuel model Hash Table with empty FuelModel list. \n", ERR_EINVAL);
		}
		
	/* construct an instance of a ChHashTable backed by FuelModel list */
	if ( (*fmtble = InitChHashTable(LIST_SIZE(fmlist), SimpleDivisionIntHashFunc, 
								CmpNumToFuelModelNum, NULL)) == NULL )	{
		ERR_ERROR("Unable to initialize fuel model Hash Table. \n", ERR_EFAILED);
		}
	
	/* insert FuelModels one-by-one in Hash Table */	
	lel = LIST_HEAD(fmlist);
	while( lel != NULL )	{
		fm = LIST_GET_DATA(lel);
		if ( fm != NULL )	{
			ChHashTableInsert(*fmtble, &(fm->model_num), fm);
			}		
		lel = LIST_GET_NEXT_ELMT(lel);
		} 
	 			
	return ERR_SUCCESS;
	}

int InitStandAgeFromPropsFireConfig(ChHashTable * proptbl, GridData * elev, GridData ** std_age)	{
	IntTwoDArray * agearr	= NULL;				/* temporary stand age array used to initialize GridData */
	KeyVal * entry			= NULL;				/* key/val instances from table */
	int domain_rows;							/* size of domain rows */
	int domain_cols;							/* size of domain columns */
	double edata;
	int i, j;	
	int fixed_age;
		
	if ( proptbl == NULL || elev == NULL )	{
		ERR_ERROR("Arguments supplied to initialize stand age invalid. \n", ERR_EINVAL);
		}
		
	/* retrieve stand age type */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SAGETYP), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve properties for Stand Age. \n", ERR_EFAILED);
		}

	if	( strcmp(entry->val, GetFireVal(VAL_SPATIAL)) == 0)	{
		/* initialize stand age from grid */
		*std_age = GetGridDataFromPropsFireGridData(proptbl, FIRE_GRIDDATA_STD_AGE_DATA);
		}		
	else if ( strcmp(entry->val, GetFireVal(VAL_FIXED)) == 0)	{
		/* initialize stand age grid to a fixed value */
		if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SAGEFIX), (void *)&entry) )	{
			ERR_ERROR("Unable to retrieve properties for Stand Age. \n", ERR_EFAILED);
			}
		fixed_age = atoi((char *) entry->val);

		/* get dimensions for stand age from elev */	
		domain_rows = elev->ghdr->nrows; 
		domain_cols = elev->ghdr->ncols;
					
		/* create temp array to initialize stand age GridData */
		if ( (agearr = InitIntTwoDArraySizeEmpty(domain_rows, domain_cols)) == NULL )	{
			ERR_ERROR("Unable to allocate memory for stand age TwoDArray. \n", ERR_ENOMEM);
			}
			
		/* populate temp array with fixed_age, set no data same as elev data */
		for(i = 0; i < domain_rows; i++)	{
			for(j = 0; j < domain_cols; j++)	{
				GRID_DATA_GET_DATA(elev, i, j, edata);
				if ( ((int)edata) == elev->ghdr->NODATA_value )	{
					INTTWODARRAY_SET_DATA(agearr, i, j, elev->ghdr->NODATA_value);
					}
				else	{
					INTTWODARRAY_SET_DATA(agearr, i, j, fixed_age);
					}
				}
			}
			
		/* initialize the stand age GridData from array */
		if ( (*std_age = InitGridDataFromIntTwoDArray(agearr, elev->ghdr->xllcorner, elev->ghdr->yllcorner, 
								elev->ghdr->cellsize, elev->ghdr->NODATA_value)) == NULL )	{
			FreeIntTwoDArray(agearr);
			ERR_ERROR("Unable to allocate memory for stand age GridData. \n", ERR_ENOMEM);
			}
	 	
		/* free the temp array */
		FreeIntTwoDArray(agearr);			
		}
	else	{
		/* invalid entry */
		ERR_ERROR("Invalid keyword specified for STAND_AGE_TYPE. \n", ERR_EFAILED);
		} 

	if ( *std_age == NULL )	{
		ERR_ERROR("Unable to initialize Stand Age grid. \n", ERR_EFAILED);
		}
	
	return ERR_SUCCESS;
	}

int InitFireEnvFromPropsFireConfig(ChHashTable * proptbl, FireEnv ** fe)		{
	KeyVal * entry	= NULL;

	if ( proptbl == NULL || (*fe = InitFireEnv()) == NULL )	{
		ERR_ERROR("Unable to initialize FireEnv from properties HashTable. \n", ERR_EINVAL);
		}
		
	/* determine fuels regrowth type and set the appropriate function pointer in FireEnv */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_RGRTYP), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve FUELS_REGROWTH_TYPE property for FireEnv. \n", ERR_EFAILED);
		}
	if ( strcmp(entry->val, GetFireVal(VAL_FIXED)) == 0)	{
		(*fe)->GetFuelsRegrowthFromProps = GetFuelsRegrowthFIXEDFromProps;	
		}
	else if ( strcmp(entry->val, GetFireVal(VAL_STATIC)) == 0)	{
		(*fe)->GetFuelsRegrowthFromProps = GetFuelsRegrowthSTATICFromProps;
		}
	else if ( strcmp(entry->val, GetFireVal(VAL_PNV)) == 0)	{
		(*fe)->GetFuelsRegrowthFromProps = GetFuelsRegrowthPNVFromProps;
		}
	else	{
		FreeFireEnv(*fe);
		ERR_ERROR("Unable to retrieve FUELS_REGROWTH_TYPE property for FireEnv. \n", ERR_EFAILED);	
		}
		
	/* determine ignition type and set the appropriate function pointer in FireEnv */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_IGTYP), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve IGNITION_TYPE property for FireEnv. \n", ERR_EFAILED);
		}
	if ( strcmp(entry->val, GetFireVal(VAL_FIXED)) == 0)	{
		(*fe)->IsIgnitionNowFromProps 	= IsIgnitionNowFIXEDFromProps;
		(*fe)->GetIgnitionLocFromProps 	= GetIgnitionLocFIXEDFromProps;	
		}
	else if ( strcmp(entry->val, GetFireVal(VAL_RANDU)) == 0)	{
		(*fe)->IsIgnitionNowFromProps 	= IsIgnitionNowRANDFromProps;
		(*fe)->GetIgnitionLocFromProps 	= GetIgnitionLocRANDUFromProps;	
		}
	else if ( strcmp(entry->val, GetFireVal(VAL_RANDS)) == 0)	{
		(*fe)->IsIgnitionNowFromProps 	= IsIgnitionNowRANDFromProps;	
		(*fe)->GetIgnitionLocFromProps 	= GetIgnitionLocRANDSFromProps;	
		}
	else	{
		FreeFireEnv(*fe);
		ERR_ERROR("Unable to retrieve IGNITION_TYPE property for FireEnv. \n", ERR_EFAILED);	
		}
	
	/* determine wind azimuth type and set the appropriate function pointer in FireEnv */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WAZTYP), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve WIND_AZIMUTH_TYPE property for FireEnv. \n", ERR_EFAILED);
		}
	if ( strcmp(entry->val, GetFireVal(VAL_FIXED)) == 0)	{
		(*fe)->GetWindAzimuthFromProps 	= GetWindAzimuthFIXEDFromProps;
		}
	else if ( strcmp(entry->val, GetFireVal(VAL_RANDU)) == 0)	{
		(*fe)->GetWindAzimuthFromProps 	= GetWindAzimuthRANDUFromProps;
		}
	else if ( strcmp(entry->val, GetFireVal(VAL_RANDH)) == 0)	{
		(*fe)->GetWindAzimuthFromProps 	= GetWindAzimuthRANDHFromProps;	
		}		
	else if ( strcmp(entry->val, GetFireVal(VAL_SPATIAL)) == 0)	{
		(*fe)->GetWindAzimuthFromProps 	= GetWindAzimuthSPATIALFromProps;
		}
	else	{
		FreeFireEnv(*fe);
		ERR_ERROR("Unable to retrieve WIND_AZIMUTH_TYPE property for FireEnv. \n", ERR_EFAILED);	
		}
		
	/* determine wind speed type and set the appropriate function pointer in FireEnv */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_WSPDTYP), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve WIND_SPEED_TYPE property for FireEnv. \n", ERR_EFAILED);
		}
	if ( strcmp(entry->val, GetFireVal(VAL_FIXED)) == 0)	{
		(*fe)->GetWindSpeedMpsFromProps = GetWindSpeedMpsFIXEDFromProps;
		}
	else if ( strcmp(entry->val, GetFireVal(VAL_RANDU)) == 0)	{
		(*fe)->GetWindSpeedMpsFromProps = GetWindSpeedMpsRANDUFromProps;
		}
	else if ( strcmp(entry->val, GetFireVal(VAL_RANDH)) == 0)	{
		(*fe)->GetWindSpeedMpsFromProps = GetWindSpeedMpsRANDHFromProps;	
		}		
	else if ( strcmp(entry->val, GetFireVal(VAL_SPATIAL)) == 0)	{
		(*fe)->GetWindSpeedMpsFromProps = GetWindSpeedMpsSPATIALFromProps;
		}
	else	{
		FreeFireEnv(*fe);
		ERR_ERROR("Unable to retrieve WIND_SPEED_TYPE property for FireEnv. \n", ERR_EFAILED);	
		}

	/* determine dead fuel moisture type and set the appropriate function pointer in FireEnv */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_DFMTYP), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve DEAD_FUEL_MOIST_TYPE property for FireEnv. \n", ERR_EFAILED);
		}
	if ( strcmp(entry->val, GetFireVal(VAL_FIXED)) == 0)	{
		(*fe)->GetDeadFuelMoistFromProps = GetDeadFuelMoistFIXEDFromProps;
		}
	else if ( strcmp(entry->val, GetFireVal(VAL_RANDH)) == 0)	{
		(*fe)->GetDeadFuelMoistFromProps = GetDeadFuelMoistRANDHFromProps;	
		}		
	else if ( strcmp(entry->val, GetFireVal(VAL_SPATIAL)) == 0)	{
		(*fe)->GetDeadFuelMoistFromProps = GetDeadFuelMoistSPATIALFromProps;
		}
	else	{
		FreeFireEnv(*fe);
		ERR_ERROR("Unable to retrieve DEAD_FUEL_MOIST_TYPE property for FireEnv. \n", ERR_EFAILED);	
		}

	/* determine live fuel moisture type and set the appropriate function pointer in FireEnv */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_LFMTYP), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve LIVE_FUEL_MOIST_TYPE property for FireEnv. \n", ERR_EFAILED);
		}
	if ( strcmp(entry->val, GetFireVal(VAL_FIXED)) == 0)	{
		(*fe)->GetLiveFuelMoistFromProps = GetLiveFuelMoistFIXEDFromProps;
		}
	else if ( strcmp(entry->val, GetFireVal(VAL_RANDH)) == 0)	{
		(*fe)->GetLiveFuelMoistFromProps = GetLiveFuelMoistRANDHFromProps;	
		}		
	else if ( strcmp(entry->val, GetFireVal(VAL_SPATIAL)) == 0)	{
		(*fe)->GetLiveFuelMoistFromProps = GetLiveFuelMoistSPATIALFromProps;
		}
	else	{
		FreeFireEnv(*fe);
		ERR_ERROR("Unable to retrieve LIVE_FUEL_MOIST_TYPE property for FireEnv. \n", ERR_EFAILED);	
		}
					
	/* determine Santa Ana type and set the appropriate function pointer in FireEnv */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SANAFREQ), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve SANTA_ANA_FREQUENCY_PER_YEAR property for FireEnv. \n", ERR_EFAILED);
		}
	(*fe)->IsSantaAnaNowFromProps = IsSantaAnaNowFromProps;
	(*fe)->GetSantaAnaEnvFromProps = GetSantaAnaEnvFromProps;
						
	return ERR_SUCCESS;
	}

int InitRandNumGenFromPropsFireConfig(ChHashTable * proptbl, void(*RandInit)(long int seed))	{
	long int seed = 0;
	KeyVal * entry	= NULL;	
	
	/* check args */
	if ( (proptbl == NULL) || (RandInit == NULL) )	{
		ERR_ERROR("Unable to initialize Random Number Generator from properties HashTable. \n", ERR_EINVAL);
		}

	/* retrieve random number seed type */		
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMRNGSD), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve SIMULATION_RAND_NUM_SEED property. \n", ERR_EFAILED);
		}
		
	if ( strcmp(entry->val, GetFireVal(VAL_TABLE)) == 0 )	{
		RandInit(GetSeedRandRecordRandSeedTable());
		}
	else	{
		seed = atol(entry->val);
		RandInit(seed);
		}
		
	return ERR_SUCCESS;
	}
		
void FireConfigDumpPropsToStream(ChHashTable * proptbl, FILE * fstream)	{
	int i;
	KeyVal * entry = NULL;
	if ( proptbl != NULL && fstream != NULL )	{
		fprintf(fstream, "\n...Fire Simulation Configuration Properties Dump...\n");
		for(i = 0; i < PROP_UP_BOUND; i++)	{		
			if ( ChHashTableRetrieve(proptbl, GetFireProp((EnumFireProp)i), (void *)&entry) )	{
				fprintf(fstream,"%-35s \t <WARNING! NOT INITIALIZED> \n", GetFireProp((EnumFireProp)i));
				}
			else	{
				fprintf(fstream,"%-35s \t %-35s \n", GetFireProp((EnumFireProp)i), entry->val);
				}
			}
		fprintf(fstream, "\n");			
		}
	else	{
		ERR_ERROR_CONTINUE("Property table or stream supplied to FireConfigDump not initialized. \n", ERR_EINVAL);
		}
	return;
	}
	
void FireConfigDumpFuelModelListToStream(List * fmlist, FILE * fstream)	{
	ListElmt * lel			= NULL;
	FuelModel * fm 			= NULL;
	
	if ( fmlist != NULL && fstream != NULL )	{
		/* retrieve and print FuelModels one-by-one */	
		lel = LIST_HEAD(fmlist);
		while( lel != NULL )	{
			fm = LIST_GET_DATA(lel);
			if ( fm != NULL )	{
				FuelModelDumpToStream(fm, stdout);
				}		
			lel = LIST_GET_NEXT_ELMT(lel);
			} 
		}
	return;	
	}
		
/* end of FireConfig.c */
