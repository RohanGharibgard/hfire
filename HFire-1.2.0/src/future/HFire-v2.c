/*! 
 * 	\file HFire.c
 *
 * 	\author Marco Morais
 * 	\date 2000-2001 
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

#include "HFire.h"

void QuitFatal(char * emessage)	
{
	if ( emessage != NULL )	
  {
		fprintf(stderr, "%s", emessage);
	}
	
	fprintf(stderr, "\n FATAL ERROR. Program Aborting.\n");
		
	exit(1);
}

void TimeStamp(FireTimer * ft, char * message)	
{
	int mt = 0;

	if ( ft != NULL )
  {	
		mt = FIRE_TIMER_GET_MILITARY_TIME(ft);
		if ( message == NULL )	
    {
			fprintf(stdout, "SIM TIME... YR: %d MO: %02d DY: %02d HR: %04d SEC: %d \n", ft->sim_cur_yr,
			 								ft->sim_cur_mo, ft->sim_cur_dy, mt, ft->sim_cur_secs);
		}
		else	
    {
			fprintf(stdout, "%s... YR: %d MO: %02d DY: %02d HR: %04d SEC: %d \n", message,
			 								ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt, ft->sim_cur_secs);
		}
	}
	return;
}

/* structure used to store burning cell parameters */
typedef struct brn_cell_t
{
  int r;                                        /* cell row */
  int c;                                        /* cell col */
  double rwx;                                   /* cell real-world x coordinate */
  double rwy;                                   /* cell real-world y coordinate */
  double max_rosmps;                            /* maximum rate of spread */
  double rosmps[EIGHTNBR_NUM_NBR_CELLS];        /* rate of spread in direction of all neighbors */
  double distm[EIGHTNBR_NUM_NBR_CELLS];         /* distance spread in direction of all neighbors */
};

double GetMaximumRateOfSpread
  (
    ChHashTable * proptbl,
    GridData * elev,
    GridData * slope,
    GridData * aspect,
    GridData * fuels,
    ChHashTable * fmtble,
    FireTimer * ft,
    FireEnv * fe,
    CellState * cs,
    int is_santa_ana,
    List * brn_cells_list
  )
{
  int r, c;
  double rwx, rwy, xul, yul;
  double cell_elev, cell_slope, cell_aspect;
  int cell_fmnum;
  FuelModel fm;
  double fbedhgtm;                          /* fuel bed height, in m */
  double d1hfm, d10hfm, d100hfm;            /* dead fuel moisture */
  double waz, wsp;                          /* wind azimuth and speed */
  double lhfm, lwfm;                        /* live fuel moisture */
  double rosmps;                            /* rate of spread, in m/s */
  brn_cell_t * brn_cell;

  /* intialize maximum rate of spread */
  double max_rosmps = 0.0;

	/* initialize an empty list of burning cells */
	brn_cells_list 	= InitListEmpty(free);

  /* compute upper left coordinates of domain */
  xul = COORD_TRANS_XLLCORNER_TO_XULCNTR(fuels->ghdr->xllcorner, fuels->ghdr->cellsize);
	yul = COORD_TRANS_YLLCORNER_TO_YULCNTR(fuels->ghdr->yllcorner, fuels->ghdr->cellsize, fuels->ghdr->nrows);

  /* iterate through every cell in domain */
  for ( r = 0; r < fuels->ghdr->nrows; ++r )
  {
    for ( c = 0; c < fuels->ghdr->ncols; ++c )
    {
      /* check if the cell is burning */
      if ( BYTETWODARRAY_GET_DATA(cs->state, r, c) != EnumHasFireCellState )
      {
        continue; /* skip the cell if it is not burning */
      }

      /* get spatial attribute data */
      GRID_DATA_GET_DATA(elev, r, c, cell_elev);
      GRID_DATA_GET_DATA(slope, r, c, cell_slope);
      GRID_DATA_GET_DATA(aspect, r, c, cell_aspect);

      /* get fuel model data */
      GRID_DATA_GET_DATA(fuels, r, c, cell_fmnum); 
      if ( ChHashTableRetrieve(fmtble, &cell_fmnum, &fm) )
      {
        QuitFatal(NULL);
      }
      fbedhgtm = fm.rfm->fdepth;
      if ( fm.rfm->units == EnumEnglishUnits ) fbedhgtm = UNITS_FT_TO_M(fbedhgtm);

      /* get dead fuel moisture, wind azimuth and speed */
      if ( is_santa_ana )
      {
        if ( GetSantaAnaEnvFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, &waz, fbedhgtm, &wsp, &d1hfm, &d10hfm, &d100hfm) )
        {
          QuitFatal(NULL);
        }
      }
      else
      {
        /* compute the real world coordinates of the cell */
        if ( CoordTransSixParamRasterToRealWorld(r, c, fuels->ghdr->cellsize, fuels->ghdr->cellsize, xul, yul, 0.0, 0.0, &rwx, &rwy) )
        {
          QuitFatal(NULL);
        }
        if ( fe->GetWindAzimuthFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, rwx, rwy, &waz) )
        {
          QuitFatal(NULL);
        }
        if ( fe->GetWindSpeedMpsFromProps(proptbl, fbedhgtm, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, rwx, rwy, &wsp) )
        {
          QuitFatal(NULL);
        }
        if ( fe->GetDeadFuelMoistFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, r, c, &d1hfm, &d10hfm, &d100hfm) )
        {
          QuitFatal(NULL);
        }
      }
      wsp = UNITS_MPSEC_TO_FTPMIN(wsp); /* convert windspeed from m/s to ft/min */

      /* get live fuel moisture */
      if ( fe->GetLiveFuelMoistFromProps(proptbl, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, r, c, &lhfm, &lwfm) )
      {
        QuitFatal(NULL);
      }

      /* calculate the no-wind and no-slope rate of spread */
      if ( Roth1972FireSpreadNoWindNoSlope(fm.rfm, d1hfm, d10hfm, d100hfm, lhfm, lwfm) )
      {
        QuitFatal(NULL);
      }

      /* calculate the maximum rate of spread */
      if ( Roth1972FireSpreadWindSlopeMax(fm.rfm, wsp, waz, cell_slope, cell_aspect) )
      {
        QuitFatal(NULL);
      }
      rosmps = UNITS_FTPMIN_TO_MPSEC(fm.rfm->rp->ros_max);

      /* compare to maximum observed rate of spread */
      if ( rosmps > max_rosmps )
      {
        max_rosmps = rosmps;
      }

      /* add the cell to the list of burning cells */
      if ( (brn_cell = malloc(sizeof(brn_cell_t))) == NULL )
      {
        QuitFatal(NULL);
      }
      brn_cell->r = r;
      brn_cell->c = c;
      brn_cell->rwx = rwx;
      brn_cell->rwy = rwy;
      brn_cell->max_rosmps = rosmps;
      ListInsertNext(brn_cells_list, LIST_TAIL(brn_cells_list), brn_cell);
    }
  }

  return max_rosmps;
}

int main(int argc, char * argv[]) 
{
	/*
  ** Simulation inputs
  */
	ChHashTable * proptbl		= NULL;									/* simulation properties read from file */
	GridData	* elev 			= NULL;										/* elev spatial data */
	GridData	* slope			= NULL;										/* slope spatial data */
	GridData	* aspect		= NULL;										/* aspect spatial data */
	GridData 	* std_age		= NULL;										/* stand age spatial data */
	GridData	* fuels			= NULL;										/* fuels spatial data */
	FireTimer  	* ft 			= NULL;										/* stores simulation time */
	FireEnv		* fe			= NULL;										  /* table of function ptrs for env vars */
	List		* fmlist		= NULL;										  /* list of FuelModels */
	ChHashTable * fmtble		= NULL;									/* table of FuelModels */	
	FireExport	* fex			= NULL;										/* wrapper for program export */

  /*
  ** Simulation outputs
  */
	FireYear * fyr				= NULL;										/* ids of burned cells */
	CellState * cs				= NULL;										/* current state of cell wrt fire */
	ByteTwoDArray * hrs_brn		= NULL;								/* hours cell has been burning */
	FltTwoDArray * fract_brn	= NULL;								/* fraction of distance fire must travel to cell center */
	FltTwoDArray * fract_iter	= NULL;								/* fastest fraction burned any one iteration */
	List * brn_cells_list 		= NULL;								/* list xy coords of burning cells */

  /* local variables */
	KeyVal * entry				= NULL;										/* vals returned from Hash Table */
	ListElmt * lel 				= NULL;										/* ptr to single element in list */
	double * brn_x, * brn_y;												/* real world coords of burning cells */
  int is_santa_ana;                               /* flag indicating current santa ana event */
	int timestep;                                   /* stores duration of simulation timestep */
  double exp_secs, iter_secs;										  /* stores duration of iteration during timestep */
  double max_rosmps;                              /* maximum rate of spread during current iteration */
	void (*RandInit)(long int seed) = randinit;			/* rng seed function from NLIBRand.h */	

	char status_msg[HFIRE_DEFAULT_STATUS_LINE_LENGTH] = {'\0'};

	/*
  ** Command line arguments
  **
	** argc		= number of arguments (should be 2)
	** argv[0] 	= "name of executable"
	** argv[1] 	= "configuration file name"
  ** argv[2]	= NULL
  */
	if ( argc != 2 )	
  {
		printf("Usage: %s <configuration filename> \n", argv[0]);
		return 0;
  }

	/* load properties from configuration file */
	if ( InitPropsFromFireConfig(&proptbl, argv[1])	)	
  {
		QuitFatal(NULL);
  }
	FireConfigDumpPropsToStream(proptbl, stdout);

	/* load fuel models from properties */
	if ( InitRothFuelModelListFromProps(proptbl, &fmlist) )	
  {
		QuitFatal(NULL);
  }
	FireConfigDumpFuelModelListToStream(fmlist, stdout);

	/* load initialization parameters from properties */
	if (    InitGridsFromPropsFireConfig(proptbl, &elev, &slope, &aspect)
			||	InitFireTimerFromPropsFireConfig(proptbl, &ft)
			||	InitFuelModelHashTableFromFuelModelListFireConfig(fmlist, &fmtble) 
			||  InitStandAgeFromPropsFireConfig(proptbl, elev, &std_age)
			||  InitFireEnvFromPropsFireConfig(proptbl, &fe)
			||	InitRandNumGenFromPropsFireConfig(proptbl, randinit)	)	
  {
		QuitFatal(NULL);
  }

	/* set simulation export properties */
	if ( (fex = InitFireExport(proptbl)) == NULL ) 	
  {
		QuitFatal(NULL);
  }
	FIRE_EXPORT_SET_FIRE_TIMER(fex, ft);
	FIRE_EXPORT_SET_STAND_AGE(fex, std_age);

	/* set simulation timestep */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMTSSEC), (void *)&entry) )	
  {
		ERR_ERROR_CONTINUE("Unable to retrieve SIMULATION_TIMESTEP_SECS property.\n", ERR_EINVAL);
		QuitFatal(NULL);
  }	
	timestep = atoi(entry->val);

	/* 
  ** Loop Over Each Year in a Simulation 
  */
	while( !FireTimerIsSimTimeExpired(ft) )	
  {
		/* advance timer to start of fire season */
		ft->sim_cur_mo = ft->sim_start_mo;
		ft->sim_cur_dy = ft->sim_start_dy;
		ft->sim_cur_hr = ft->sim_start_hr;
		
		/* signal user */
		TimeStamp(ft, "START SIM YEAR");
		
		/* 
    ** Allocate data structures used during Year
    */
		
		/* initialize fuels to be used during this year of simulation */
		if ( fe->GetFuelsRegrowthFromProps(proptbl, std_age, &fuels) )
    {
      QuitFatal(NULL);
		}
		FIRE_EXPORT_SET_FUELS(fex, fuels);

		/* initialize a fire year structure for year in simulation */				 
		fyr = InitFireYearFuels(ft->sim_cur_yr, fuels, fmtble);
		FIRE_EXPORT_SET_FIRE_YEAR(fex, fyr);

		/* initialize a cell state structure for year in simulation */
		cs = InitCellStateFuels(fuels, fmtble);

		/* initialize a hours burned structure for year in simulation */
		hrs_brn = InitByteTwoDArraySizeIniValue(fuels->ghdr->nrows, fuels->ghdr->ncols, 0);

		/* initialize a fraction burned structure for year in simulation */
		fract_brn = InitFltTwoDArraySizeIniValue(fuels->ghdr->nrows, fuels->ghdr->ncols, HFIRE_FRACT_ZERO);

		/* initialize a fraction burned iter structure for year in simulation */
		fract_iter = InitFltTwoDArraySizeIniValue(fuels->ghdr->nrows, fuels->ghdr->ncols, HFIRE_FRACT_ZERO);

		/* ensure all structures properly initialized */
		if ( fyr == NULL || cs == NULL || hrs_brn == NULL || fract_brn == NULL || fract_iter == NULL )
    {
      QuitFatal(NULL);
    }

    /* 
    ** Loop Over Each Timestep in a Year 
    */
		while( !FireTimerIsSimCurYearTimeExpired(ft) )	
    {
			/* determine if ignition occurs during this timestep */
			if ( fe->IsIgnitionNowFromProps(proptbl) )	
      {
        /* obtain coordinates of ignited cells */
				if ( fe->GetIgnitionLocFromProps(proptbl, fyr, &brn_cells_list) )
        {
					QuitFatal(NULL);
        }
        /* set ignited cells to the burning state */
				for( lel = LIST_HEAD(brn_cells_list); lel != NULL; lel = LIST_GET_NEXT_ELMT(lel) )	
        {
					brn_x = LIST_GET_DATA(lel);
					lel = LIST_GET_NEXT_ELMT(lel);
					brn_y = LIST_GET_DATA(lel);
					CellStateSetCellStateRealWorld(cs, *brn_x, *brn_y, EnumHasFireCellState);
					FireYearSetCellNewFireIDRealWorld(fyr, *brn_x, *brn_y);
					FireExportIgLocsTxtFile(proptbl, *brn_x, *brn_y, ft);
        }
        /* empty the list of ignited cells */
				FreeList(brn_cells_list);
      }

      /* determine if we are having a santa ana event */
      is_santa_ana = IsSantaAnaNowFromProps(proptbl, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy);

      /*
      ** Loop Over Each Iteration in a Timestep 
      */
      for ( exp_secs = 0.0; exp_secs < (double) timestep; exp_secs += iter_secs )
      {
        /* determine the maximum rate of spread in the domain */
        max_rosmps = GetMaximumRateOfSpread(proptbl, elev, slope, aspect, fuels, fmtble, ft, fe, cs, is_santa_ana, brn_cells_list );

        /* compute the elapsed time during the current iteration */
        if ( max_rosmps > 0.0 )
        {
          iter_secs = (fuels->ghdr->cellsize / max_rosmps);
          /* saturate the elapsed time so that it does not exceed the current timestep */
          if ( (exp_secs + iter_secs) > (double) timestep )
          {
            iter_secs = (double) timestep - exp_secs + HFIRE_EPSILON;
          }
        }
        else
        {
          iter_secs = (double) timestep - exp_secs + HFIRE_EPSILON;
        }

        /* advance the fire spread during the current iteration */

				/* empty the list of burning cells */
				FreeList(brn_cells_list);

        /* signal user */
				sprintf(status_msg, "T_exp: %d T_adapt: %d", exp_secs, iter_secs);
				TimeStamp(ft, status_msg);
      } /* End Iteration */

			/* increment simulation clock by one timestep */
			FireTimerIncrementSeconds(ft, timestep);
			
			/* increment cell extinction clock */
			if ( UpdateExtinctionHOURS(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, cs, hrs_brn) )	
      {
        QuitFatal(NULL);
      }

			/* export data from timestep */
			if ( FireExportSpatialData(proptbl, fex) )
      {
        QuitFatal(NULL);
      }

			/* signal user */
			TimeStamp(ft, NULL);
    } /* End Timestep */

		/* export data from Year */
		if ( FireExportSpatialData(proptbl, fex) ) 
    { 
      QuitFatal(NULL);
		}

		/* increment stand age */
		IncrementStandAge(fyr, std_age);	

		/* free memory for short-term (yearly) data structures */
		FreeGridData(fuels);		
		FreeFireYear(fyr);
		FreeCellState(cs);
		FreeByteTwoDArray(hrs_brn);
		FreeFltTwoDArray(fract_brn);		
		FreeFltTwoDArray(fract_iter);		

		/* signal user */
		TimeStamp(ft, "END SIM YEAR");

		/* increment simulation clock by one year */
		ft->sim_cur_yr += 1;
	} /* End Year */
	
	/*
  ** free all memory
  */
	FreeFireExport(fex);
	FreeFireEnv(fe);
	FreeGridData(std_age);
	FreeChHashTable(fmtble);
	FreeFireTimer(ft);
	FreeGridData(aspect);
	FreeGridData(slope);
	FreeGridData(elev);
	FreeList(fmlist);
	FreeChHashTable(proptbl);	
	return 0;
}

/* end of HFire.c */
