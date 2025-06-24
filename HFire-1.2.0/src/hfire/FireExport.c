/*! 
 * \file FireExport.c
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
 
#include "FireExport.h"

#include <math.h>

/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */
 
int FireExportFireIDAscRaster(ChHashTable * proptbl, FireYear * fyr, FireTimer * ft);

int FireExportSantaAnaAscRaster(ChHashTable * proptbl, FireYear * fyr, FireTimer * ft);

int FireExportFuelsAscRaster(ChHashTable * proptbl, GridData * fuels, FireTimer * ft);

int FireExportStandAgeAscRaster(ChHashTable * proptbl, GridData * std_age, FireTimer * ft);

FireExport * InitFireExport(ChHashTable * proptbl)		{
	FireExport * fe 					= NULL;				/* initialized structure */
	KeyVal * entry						= NULL;				/* key/val instances from table */
	
	/* check args */
	if ( proptbl == NULL || ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFREQ), (void *)&entry) ) 	{
		ERR_ERROR_CONTINUE("Unable to retrieve EXPORT_FREQUENCY property. \n", ERR_EINVAL);
		return fe;
	}

	/* allocate memory */				
	if ( (fe = (FireExport *) malloc(sizeof(FireExport))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize FireExport, memory allocation failed. \n", ERR_ENOMEM);
		return fe;	
	}
	
	/* assign an export frequency enumeration */	
	if ( strcmp(entry->val, GetFireVal(VAL_TIMESTEP)) == 0 )		{
		fe->exp_freq = EnumFreqTimestep;
	}
	else if ( strcmp(entry->val, GetFireVal(VAL_DAILY)) == 0 )		{
		fe->exp_freq = EnumFreqDaily;
	}
	else if ( strcmp(entry->val, GetFireVal(VAL_ANNUAL)) == 0 )	{	
		fe->exp_freq = EnumFreqAnnual;
	}
	else	{
		ERR_ERROR_CONTINUE("Unable to initialize FireExport, EXPORT_FREQUENCY property incorrect. \n", ERR_EINVAL);
		FreeFireExport(fe);
		return fe;
	}
	
	/* write out all headers for text files */
	if ( FireExportInitTxtFileHeaders(proptbl) )	{
		ERR_ERROR_CONTINUE("Unable to initialize FireExport, EXPORT textfile properties incorrect. \n", ERR_EINVAL);
		FreeFireExport(fe);
		return fe;
	}
		
	/* set all members to NULL */
	fe->ft 							= NULL;
	fe->fyr 						= NULL;
	fe->fuels						= NULL;
	fe->std_age						= NULL;
	/* set all function pointers */
	fe->FireExportFireIDAscRaster	= FireExportFireIDAscRaster;
  fe->FireExportSantaAnaAscRaster = FireExportSantaAnaAscRaster;
	fe->FireExportFuelsAscRaster	= FireExportFuelsAscRaster;
	fe->FireExportStandAgeAscRaster	= FireExportStandAgeAscRaster;
	#ifdef USING_GD
		fe->FireExportFireIDPng		= FireExportFireIDPng;
	#else
		fe->FireExportFireIDPng		= NULL;
	#endif /* INCLUDES SUPPORT FOR EXPORTING IMAGES FROM SIMULATION USING GD LIBRARY */

	return fe; 
}
	
int FireExportSpatialData(ChHashTable * proptbl, FireExport * fe)	{
	/* static variables used to store state across function calls */
	static int smonth 				= 0;
	static int sday 				= 0;
	static int shour 				= 0;
	/* stack variables */
	int do_export					= 0;
	
	/* check args */
	if ( proptbl == NULL || fe == NULL ) 	{
		ERR_ERROR("Unable to retrieve FireExport information. \n", ERR_EINVAL);
	}
	if ( fe->ft == NULL ) 	{
		ERR_ERROR("Must have a FireTimer set in order to use FireExport. \n", ERR_EINVAL);
	}
	
	/* determine if export occurs */
	switch(fe->exp_freq)	{
		case EnumFreqTimestep:
			/* always export when freq is timestep */
			do_export = 1;
			break;
		case EnumFreqDaily:
			if ( (smonth != fe->ft->sim_cur_mo) || (sday != fe->ft->sim_cur_dy) )	{
				do_export = 1;
				/* set {month, day} for future calls */			
				smonth = fe->ft->sim_cur_mo;
				sday = fe->ft->sim_cur_dy;
				}
			break;
		case EnumFreqAnnual:
			if ( FireTimerIsSimCurYearTimeExpired(fe->ft) )	{
				do_export = 1;
				}
			break;
		default:
			do_export = 0;
			break;
	}
			
	if ( do_export == 1 )	{	
		/* export fire ids */	
		if ( fe->fyr != NULL && fe->FireExportFireIDAscRaster != NULL )	{
			if ( fe->FireExportFireIDAscRaster(proptbl, fe->fyr, fe->ft) )	{
				ERR_ERROR("Unable to export fire ids raster. \n", ERR_EBADFUNC);
			}
		}		
    /* export santa ana */
		if ( fe->fyr != NULL && fe->FireExportSantaAnaAscRaster != NULL )	{
			if ( fe->FireExportSantaAnaAscRaster(proptbl, fe->fyr, fe->ft) )	{
				ERR_ERROR("Unable to export santa ana raster. \n", ERR_EBADFUNC);
			}
		}		
		/* export fuels */
		if ( fe->fuels != NULL && fe->FireExportFuelsAscRaster != NULL )	{
			if ( fe->FireExportFuelsAscRaster(proptbl, fe->fuels, fe->ft) )		{
				ERR_ERROR("Unable to export fuels raster. \n", ERR_EBADFUNC);
			}
		}		
		/* export stand age */
		if ( fe->std_age != NULL && fe->FireExportStandAgeAscRaster != NULL ) 	{
			if ( fe->FireExportStandAgeAscRaster(proptbl, fe->std_age, fe->ft) )		{
				ERR_ERROR("Unable to export stand age raster. \n", ERR_EBADFUNC);
			}
		}		
		/* export fire id images */	
		if ( fe->fyr != NULL && fe->FireExportFireIDPng != NULL )	{
			if ( fe->FireExportFireIDPng(proptbl, fe->fyr, fe->ft) )	{
				ERR_ERROR("Unable to export fire ids images. \n", ERR_EBADFUNC);
			}
		}
	}

	return ERR_SUCCESS;
}
	
int FireExportFireIDAscRaster(ChHashTable * proptbl, FireYear * fyr, FireTimer * ft)	{
	KeyVal * entry										= NULL;				/* key/val instances from table */
	char fid_fname[FIRE_EXPORT_DEFAULT_FILENAME_SIZE] 	= {'\0'};
	int mt = 0;

	/* check args */	
	if ( proptbl == NULL  || ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFIDDIR), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve EXPORT_FIRE_ID_RASTER_DIR property. \n", ERR_EINVAL);
	}
		
	/* return if no export options specified */
	if	( strcmp(entry->val, GetFireVal(VAL_NULL)) == 0)	{
		return ERR_SUCCESS;
	}
	
	/* generate name for output asc file of format fidYYYYMMDDHHHH.asc */
	mt = FIRE_TIMER_GET_MILITARY_TIME(ft);
	#ifdef USING_PC
	sprintf(fid_fname, "%s\\fid%d%02d%02d%04d.asc", entry->val, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt);
	#endif
	#ifdef USING_UNIX
	sprintf(fid_fname, "%s//fid%d%02d%02d%04d.asc", entry->val, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt);
	#endif
	
	/* export data */
	if ( ExportIntTwoDArrayAsAsciiRaster(fyr->id, fyr->xllcorner, fyr->yllcorner, 
											fyr->cellsize, FIRE_YEAR_ID_UNBURNABLE, fid_fname) )	{
		ERR_ERROR("Unable to export fire ID data in function FireExportFireIDAscRaster. \n", ERR_EBADFUNC);
	}
			 		
	return ERR_SUCCESS;
}

int FireExportSantaAnaAscRaster(ChHashTable * proptbl, FireYear * fyr, FireTimer * ft) {
	KeyVal * entry										= NULL;				/* key/val instances from table */
	char sana_fname[FIRE_EXPORT_DEFAULT_FILENAME_SIZE] 	= {'\0'};
	int mt = 0;

	/* check args */	
	if ( proptbl == NULL  || ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPSADIR), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve EXPORT_SANTA_ANA_RASTER_DIR property. \n", ERR_EINVAL);
	}
		
	/* return if no export options specified */
	if	( strcmp(entry->val, GetFireVal(VAL_NULL)) == 0)	{
		return ERR_SUCCESS;
	}
	
	/* generate name for output asc file of format fidYYYYMMDDHHHH.asc */
	mt = FIRE_TIMER_GET_MILITARY_TIME(ft);
	#ifdef USING_PC
	sprintf(sana_fname, "%s\\sana%d%02d%02d%04d.asc", entry->val, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt);
	#endif
	#ifdef USING_UNIX
	sprintf(sana_fname, "%s//sana%d%02d%02d%04d.asc", entry->val, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt);
	#endif
	
	/* export data */
	if ( ExportIntTwoDArrayAsAsciiRaster(fyr->santa_ana, fyr->xllcorner, fyr->yllcorner, 
											fyr->cellsize, FIRE_YEAR_CELL_UNBURNABLE, sana_fname) )	{
		ERR_ERROR("Unable to export santa ana data in function FireExportSantaAnaAscRaster. \n", ERR_EBADFUNC);
	}
			 		
	return ERR_SUCCESS;
}

int FireExportFuelsAscRaster(ChHashTable * proptbl, GridData * fuels, FireTimer * ft)	{
	KeyVal * entry										= NULL;				/* key/val instances from table */
	char fl_fname[FIRE_EXPORT_DEFAULT_FILENAME_SIZE] 	= {'\0'};
	int mt = 0;

	/* check args */	
	if ( proptbl == NULL  || ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFUELDIR), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve EXPORT_FUELS_RASTER_DIR property. \n", ERR_EINVAL);
	}
		
	/* return if no export options specified */
	if	( strcmp(entry->val, GetFireVal(VAL_NULL)) == 0)	{
		return ERR_SUCCESS;
	}
	
	/* generate name for output asc file of format flYYYYMMDDHHHH.asc */
	mt = FIRE_TIMER_GET_MILITARY_TIME(ft);	
	#ifdef USING_PC
	sprintf(fl_fname, "%s\\fuels%d%02d%02d%04d.asc", entry->val, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt);
	#endif
	#ifdef USING_UNIX
	sprintf(fl_fname, "%s//fuels%d%02d%02d%04d.asc", entry->val, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt);
	#endif
		
	/* export data */
	if ( ExportGridDataAsAsciiRaster(fuels, fl_fname) )	{
		ERR_ERROR("Unable to export fuels data in function FireExportFuelsAscRaster. \n", ERR_EBADFUNC);
	}
			 		
	return ERR_SUCCESS;
}

int FireExportStandAgeAscRaster(ChHashTable * proptbl, GridData * std_age, FireTimer * ft)	{
	KeyVal * entry										= NULL;				/* key/val instances from table */
	char sa_fname[FIRE_EXPORT_DEFAULT_FILENAME_SIZE] 	= {'\0'};
	int mt = 0;

	/* check args */	
	if ( proptbl == NULL  || ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPSAGEDIR), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve EXPORT_STAND_AGE_RASTER_DIR property. \n", ERR_EINVAL);
	}
		
	/* return if no export options specified */
	if	( strcmp(entry->val, GetFireVal(VAL_NULL)) == 0)	{
		return ERR_SUCCESS;
	}
	
	/* generate name for output asc file of format saYYYYMMDDHHHH.asc */
	mt = FIRE_TIMER_GET_MILITARY_TIME(ft);	
	#ifdef USING_PC
	sprintf(sa_fname, "%s\\sage%d%02d%02d%04d.asc", entry->val, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt);
	#endif
	#ifdef USING_UNIX
	sprintf(sa_fname, "%s//sage%d%02d%02d%04d.asc", entry->val, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt);
	#endif
		
	/* export data */
	if ( ExportGridDataAsAsciiRaster(std_age, sa_fname) )	{
		ERR_ERROR("Unable to export stand age data in function FireExportStandAgeAscRaster. \n", ERR_EBADFUNC);
	}
			 		
	return ERR_SUCCESS;
}

int FireExportInitTxtFileHeaders(ChHashTable * proptbl)	{
  const EnumFireProp file_props[] = {
    PROP_EXPIGLCF,                        /* EXPORT_IGNITION_LOCS_FILE */
    PROP_EXPFAREAF,                       /* EXPORT_FIRE_AREA_FILE */
    PROP_EXPSANAEVF,                      /* EXPORT_SANTA_ANA_EVT_FILE */
    PROP_EXPFINOF,                        /* EXPORT_FIRE_INFO_FILE */
    PROP_EXPAABHF                         /* EXPORT_AGE_AT_BURN_HIST_FILE */
  };
  const char * file_headers[] = {
    "YYYY, MO, DY, HHHH, X, Y, FID",      /* EXPORT_IGNITION_LOCS_FILE */
    "YYYY, FID, NUM_CELLS, NUM_CELLS_SA", /* EXPORT_FIRE_AREA_FILE */
    "YYYY, MO, DY, NUM_DAYS",             /* EXPORT_SANTA_ANA_EVT_FILE */
    "FID, X, Y, "                         /* EXPORT_FIRE_INFO_FILE */
      "START_YYYY, START_MO, START_DY, START_HR, "
      "END_YYYY, END_MO, END_DY, END_HR, "
      "NUM_BURNED, IS_FAILED_IG, NUM_BURNED_SA",
    "YYYY, AGE, NUM_UNBURNED, NUM_BURNED, NUM_BURNED_SA"
                                          /* EXPORT_AGE_AT_BURN_HIST_FILE */
  };
  const int num_files = sizeof(file_props) / sizeof(file_props[0]);

  KeyVal * fname;
  FILE * fstream = NULL;
	int i;

	/* check args */	
	if ( proptbl == NULL )	{
		ERR_ERROR("Unable to retrieve any text file export properties. \n", ERR_EINVAL);
	}

  for ( i = 0 ; i < num_files; ++i ) {
    /* retrieve the file name */
    if ( ChHashTableRetrieve(proptbl, GetFireProp(file_props[i]), (void *)&fname) ) {
      continue; /* user did not specfiy this property */
    }
    /* write the file header */
    if ( strcmp(fname->val, GetFireVal(VAL_NULL)) != 0 ) {
      if ( (fstream = fopen((char *)fname->val, "w")) == NULL ) {
        ERR_ERROR("Unable to open text file for writing header. \n", ERR_EINVAL);
      }
      fprintf(fstream, "%s\n", file_headers[i]);
      fclose(fstream);
    }
  }
				
	return ERR_SUCCESS;		
}
	
int FireExportIgLocsTxtFile(ChHashTable * proptbl, int id, double rwx, double rwy, FireTimer * ft)		{
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	
	/* check args */	
	if ( proptbl == NULL  || ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPIGLCF), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve EXPORT_IGNITION_LOCS_FILE property. \n", ERR_EINVAL);
	}
		
	/* return if no export options specified */
	if	( strcmp(entry->val, GetFireVal(VAL_NULL)) == 0)	{
		return ERR_SUCCESS;
	}
		
	/* open append stream to ignition location file */
	if ( (fstream = fopen((char *) entry->val, "a")) == NULL )	{		
		ERR_ERROR("Unable to append to file containing Ignition Locations. \n", ERR_EIOFAIL);
	}

	/* formatted output: "YYYY, MM, DY, HHHH, X, Y, ID" */
	fprintf(fstream, "%04d, %02d, %02d, %04d, %0.6f, %0.6f, %d\n", ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, FIRE_TIMER_GET_MILITARY_TIME(ft), rwx, rwy, id);
	fclose(fstream);
	
	return ERR_SUCCESS;				
}

int FireExportSantaAnaEvtTxtFile(ChHashTable * proptbl, int duration, int year, int month, int day)	{
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
		
	/* check args */	
	if ( proptbl == NULL  || ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPSANAEVF), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve EXPORT_SANTA_ANA_EVT_FILE property. \n", ERR_EINVAL);
	}
		
	/* return if no export options specified */
	if	( strcmp(entry->val, GetFireVal(VAL_NULL)) == 0)	{
		return ERR_SUCCESS;
	}
		
	/* open append stream to santa ana file */
	if ( (fstream = fopen((char *) entry->val, "a")) == NULL )	{		
		ERR_ERROR("Unable to append to file containing Santa Ana event occurences. \n", ERR_EIOFAIL);
	}

	/* formatted output: "YYYY, MM, DY, NUM_DAYS" */
	fprintf(fstream, "%04d, %02d, %02d, %d \n", year, month, day, duration);
	fclose(fstream);
	
	return ERR_SUCCESS;				
}

int FireExportFireAreaTxtFile(ChHashTable * proptbl, FireYear * fy) {
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	int i, j, id;
  long int num_unburned = 0L, num_unburnable = 0L;

	/* check args */	
	if ( proptbl == NULL  || ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFAREAF), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve EXPORT_FIRE_AREA_FILE property. \n", ERR_EINVAL);
	}
		
	/* return if no export options specified */
	if	( strcmp(entry->val, GetFireVal(VAL_NULL)) == 0)	{
		return ERR_SUCCESS;
	}

  /* compute the count of unburnable and unburned cells */
  for ( i = 0; i < fy->id->size_rows; ++i ) {
    for ( j = 0; j < fy->id->size_cols; ++j ) {
      /* increment the count of cells with matching id */
      id = INTTWODARRAY_GET_DATA(fy->id, i, j);
      if        ( id == FIRE_YEAR_ID_UNBURNABLE ) {
        ++num_unburnable;
      } else if ( id == FIRE_YEAR_ID_DEFAULT ) {
        ++num_unburned;
      }
    }
  }

	/* open append stream to fire area file */
	if ( (fstream = fopen((char *) entry->val, "a")) == NULL )	{	
		ERR_ERROR("Unable to append to file containing Fire Area. \n", ERR_EIOFAIL);
	}

  /* print the count of unburnable and unburned cells */
  fprintf(fstream, "%04d, %d, %ld, %ld\n", 
    fy->year, FIRE_YEAR_ID_UNBURNABLE, num_unburnable, 0L);
  fprintf(fstream, "%04d, %d, %ld, %ld\n", 
    fy->year, FIRE_YEAR_ID_DEFAULT, num_unburned, 0L);

  /* use the counts from the fire info structure for burned cells */
  for ( i = 1 ; i <= fy->num_fires; ++i ) {
    fprintf(fstream, "%04d, %d, %ld, %ld\n",
      fy->year, fy->finfo[i].id,
      fy->finfo[i].is_failed_ig ? 0 : fy->finfo[i].num_cells_burned,
      fy->finfo[i].is_failed_ig ? 0 : fy->finfo[i].num_cells_burned_sa
    );
  }
	fclose(fstream);

	return ERR_SUCCESS;	
}

int FireExportFireInfoTxtFile(ChHashTable * proptbl, FireYear * fy) {
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
	int i;

	/* check args */	
	if ( proptbl == NULL  || ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFINOF), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve EXPORT_FIRE_INFO_FILE property. \n", ERR_EINVAL);
	}
		
	/* return if no export options specified */
	if	( strcmp(entry->val, GetFireVal(VAL_NULL)) == 0)	{
		return ERR_SUCCESS;
	}

	/* open append stream to fire info file */
	if ( (fstream = fopen((char *) entry->val, "a")) == NULL )	{		
		ERR_ERROR("Unable to append to file containing fire info attributes. \n", ERR_EIOFAIL);
	}

  for ( i = 1 ; i <= fy->num_fires; ++i ) {
    fprintf(fstream, "%d, %0.6f, %0.6f, %04d, %02d, %02d, %02d, %04d, %02d, %02d, %02d, %ld, %d, %ld\n",
      fy->finfo[i].id, fy->finfo[i].rwx, fy->finfo[i].rwy,
      fy->finfo[i].start_yr, fy->finfo[i].start_mo, fy->finfo[i].start_dy, fy->finfo[i].start_hr,
      fy->finfo[i].end_yr, fy->finfo[i].end_mo, fy->finfo[i].end_dy, fy->finfo[i].end_hr,
      fy->finfo[i].num_cells_burned,
      fy->finfo[i].is_failed_ig, 
      fy->finfo[i].num_cells_burned_sa);
  }
	fclose(fstream);
	
	return ERR_SUCCESS;	
}

int FireExportAgeAtBurnHistTxtFile(ChHashTable * proptbl, FireYear * fy, GridData * std_age) {
	KeyVal * entry					= NULL;				/* key/val instances from table */
	FILE * fstream					= NULL;				/* file stream */
  long int 
    num_unburned[AGE_AT_BURN_NUM_HIST_BINS], 
    num_burned[AGE_AT_BURN_NUM_HIST_BINS], 
    num_burned_sa[AGE_AT_BURN_NUM_HIST_BINS];
	int i, j, id, sana;
  long int age;

	/* check args */	
	if ( proptbl == NULL  || ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPAABHF), (void *)&entry) )	{
		ERR_ERROR("Unable to retrieve EXPORT_AGE_AT_BURN_HIST_FILE property. \n", ERR_EINVAL);
	}
		
	/* return if no export options specified */
	if	( strcmp(entry->val, GetFireVal(VAL_NULL)) == 0)	{
		return ERR_SUCCESS;
	}

  /* initialize the state of all of the bins in the age at burn histograms */
  memset(&num_unburned,  0,  sizeof(num_unburned[0])  * AGE_AT_BURN_NUM_HIST_BINS);
  memset(&num_burned,    0,  sizeof(num_burned[0])    * AGE_AT_BURN_NUM_HIST_BINS);
  memset(&num_burned_sa, 0,  sizeof(num_burned_sa[0]) * AGE_AT_BURN_NUM_HIST_BINS);

	/* open append stream to fire info file */
	if ( (fstream = fopen((char *) entry->val, "a")) == NULL )	{		
		ERR_ERROR("Unable to append to file containing age at burn histogram. \n", ERR_EIOFAIL);
	}

  /* increment the counts in each bin based upon the fire year rasters */
  for ( i = 0; i < fy->id->size_rows; ++i ) {
    for ( j = 0; j < fy->id->size_cols; ++j ) {
      /* extract the fire id at the cell */
      id   = INTTWODARRAY_GET_DATA(fy->id, i, j);

      /* cell is unburnable, skip analysis */
      if ( id == FIRE_YEAR_ID_UNBURNABLE ) {
        continue;
      }

      /* extract the stand age and saturate to histogram limits */
      GRID_DATA_GET_DATA(std_age, i, j, age);
      if      ( age < 1 )     
        { age = 1; }      /* should not happen */
      else if ( age > AGE_AT_BURN_NUM_HIST_BINS ) 
        { age = AGE_AT_BURN_NUM_HIST_BINS; }

      /* cell was not burned */
      if ( id == FIRE_YEAR_ID_DEFAULT ) {
        ++num_unburned[age-1];      
      }
      /* cell was burned */
      else {
        ++num_burned[age-1];
        sana = INTTWODARRAY_GET_DATA(fy->santa_ana, i, j);
        if ( sana == FIRE_YEAR_CELL_BURNED_SA ) {
          ++num_burned_sa[age-1];
        }
      }
    }
  }

  /* print the contents of the histogram to file */
  for ( i = 0; i < AGE_AT_BURN_NUM_HIST_BINS; ++i ) {
    fprintf(fstream, "%04d, %ld, %ld, %ld, %ld\n",
      fy->year,
      i+1, /* stand age */
      num_unburned[i],
      num_burned[i],
      num_burned_sa[i]);
  }
  fclose(fstream);

  return ERR_SUCCESS;
}

void FreeFireExport(FireExport * fe)	{
	if ( fe != NULL )	{
		free(fe);
	}
	fe = NULL;
	return;
}
	
/* end of FireExport.c */
