/*! 
 *  \file HFire.c
 *
 *  \author Marco Morais
 *  \date 2000-2001 
 *
 *  HFire (Highly Optmized Tolerance Fire Spread Model) Library
 *  Copyright (C) 2001 Marco Morais
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "HFire.h"

#include <string.h>

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

void ZeroFltTwoDArray(FltTwoDArray * arr)
{
  int i;
  for ( i = 0; i < arr->size_rows; ++i )
  {
    memset(arr->array[i], 0, sizeof(float) * arr->size_cols);
  }
}

/* structure used to store burning cell parameters */
typedef struct
{
  int i;                                        /* cell row */
  int j;                                        /* cell col */
  double rwx;                                   /* cell real-world x coordinate */
  double rwy;                                   /* cell real-world y coordinate */
  double max_rosmps;                            /* maximum rate of spread */
  double rosmps[EIGHTNBR_NUM_NBR_CELLS];        /* rate of spread in direction of neighbor, in m/s */
  double distm[EIGHTNBR_NUM_NBR_CELLS];         /* distance spread in direction of neighbor, in m */
  double dist2ctrm[EIGHTNBR_NUM_NBR_CELLS];     /* distance to neighboring cell center, in m */
}
brn_cell_t;
    
int main(int argc, char * argv[])
{
  ChHashTable * proptbl = NULL;                 /* simulation properties read from file */
  GridData * elev = NULL;                       /* elev spatial data */
  GridData * slope = NULL;                      /* slope spatial data */
  GridData * aspect = NULL;                     /* aspect spatial data */
  GridData * std_age = NULL;                    /* stand age spatial data */
  GridData * fuels = NULL;                      /* fuels spatial data */
  FireTimer * ft = NULL;                        /* stores simulation time */
  FireEnv * fe = NULL;                          /* table of function ptrs for environment vars */
  List * fmlist = NULL;                         /* list of FuelModels */
  ChHashTable * fmtble = NULL;                  /* table of FuelModels */
  FireExport * fex = NULL;                      /* wrapper for program export */

  KeyVal * entry = NULL;                        /* ptr to hash table entry */
  ListElmt * lel = NULL;                        /* ptr to single element in a list */

  int timestep;                                 /* duration of simulation timestep */
  double exp_secs, iter_secs;                   /* duration of iteration during timestep */
  int domain_rows, domain_cols;                 /* simulation domain size */
  double cellsz;                                /* simulation cell resolution, in m */
  double xulcntr, yulcntr;                      /* simulation upper left coordinates */
  double rwx, rwy;                              /* real world xy coord pair */
  FireYear * fyr = NULL;                        /* ids of burned cells */
  CellState * cs = NULL;                        /* cell state */
  ByteTwoDArray * hrs_brn = NULL;               /* hours cell has been burning */
  FltTwoDArray * fract_brn = NULL;              /* fraction of the cell that has burned */
  FltTwoDArray * fract_brn_iter = NULL;         /* fraction of the cell that has burned this iteration */

  List * ig_cells_list = NULL;                  /* list of xy coordinates for ignited cells */
  double * ig_rwx, * ig_rwy;                    /* real world xy coordinate pair for ignited cell */

  List * brn_cells_list = NULL;                 /* list of burning cell parameters */
  double max_rosmps;                            /* maximum rate of spread, in m/s */
  int i, j;                                     /* spatial row and col */
  double cell_elev;                             /* cell elevation, in m */
  double cell_slope;                            /* cell slope */
  double cell_aspect;                           /* cell aspect */
  int cell_fmnum;                               /* cell fuel model number */
  FuelModel * fm = NULL;                        /* ptr to FuelModel */
  double fbedhgtm;                              /* fuel bed height, in m */
  double d1hfm, d10hfm, d100hfm;                /* dead fuel moisture, 1 hour, 10 hour, and 100 hour */
  double lhfm, lwfm;                            /* live fuel moisture */
  double waz;                                   /* wind azimuth */
  double wspmps, wspfpm;                        /* wind speed, in m/s and ft/min */
  double rosfpm, rosmps;                        /* rate of fire spread, in ft/min and m/s */
  brn_cell_t * brn_cell;                        /* burning cell parameters */
  int cell_az;                                  /* stores cell azimuth */
  int nbr_i, nbr_j;                             /* stores nbr cell index i,j */
  double nbr_elev;                              /* neighbor elevation, in m */
  double dist2ctrm;                             /* distance along the ground between cells, in m */
  double fract;
  int num_nbr;                                  /* count of cells which are not burnable, already ignited, or consumed */

  void (*RandInit)(long int seed) = randinit;   /* rng seed function from NLIBRand.h */

  char status_msg[HFIRE_STATUS_LINE_LENGTH] = {'\0'};

  /*
  ** Command line arguments
  **
  ** argc   = number of arguments (should be 2)
  ** argv[0]  = "name of executable"
  ** argv[1]  = "configuration file name"
  ** argv[2]  = NULL
  */
  if ( argc != 2 )
  {
    printf("Usage: %s <configuration filename> \n", argv[0]);
    return 0;
  }

  /* load properties from configuration file */
  if ( InitPropsFromFireConfig(&proptbl, argv[1]) )
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
      ||  InitFireTimerFromPropsFireConfig(proptbl, &ft)
      ||  InitFuelModelHashTableFromFuelModelListFireConfig(fmlist, &fmtble)
      ||  InitStandAgeFromPropsFireConfig(proptbl, elev, &std_age)
      ||  InitFireEnvFromPropsFireConfig(proptbl, &fe)
      ||  InitRandNumGenFromPropsFireConfig(proptbl, randinit)  )
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

    /* initialize fuels to be used during this year of simulation */
    if ( fe->GetFuelsRegrowthFromProps(proptbl, std_age, &fuels) )
    {
      QuitFatal(NULL);
    }
    FIRE_EXPORT_SET_FUELS(fex, fuels);

    /* get dimensions of fuels */
    domain_rows = fuels->ghdr->nrows;
    domain_cols = fuels->ghdr->ncols;
    cellsz    = (double) fuels->ghdr->cellsize;
    xulcntr = COORD_TRANS_XLLCORNER_TO_XULCNTR(fuels->ghdr->xllcorner, cellsz);
    yulcntr = COORD_TRANS_YLLCORNER_TO_YULCNTR(fuels->ghdr->yllcorner, cellsz, fuels->ghdr->nrows);

    /* initialize a fire year structure for year in simulation */
    fyr = InitFireYearFuels(ft->sim_cur_yr, fuels, fmtble);
    FIRE_EXPORT_SET_FIRE_YEAR(fex, fyr);

    /* initialize a cell state structure for year in simulation */
    cs = InitCellStateFuels(fuels, fmtble);

    /* initialize array to store number of hours a cell has been burning */
    hrs_brn = InitByteTwoDArraySizeIniValue(domain_rows, domain_cols, 0);

    /* initialize array to store the fraction into the cell center that the fire has burned */
    fract_brn = InitFltTwoDArraySizeIniValue(domain_rows, domain_cols, 0.0);
    fract_brn_iter = InitFltTwoDArraySizeIniValue(domain_rows, domain_cols, 0.0);

    /* ensure all structures properly initialized */
    if ( fyr == NULL || cs == NULL || hrs_brn == NULL || fract_brn == NULL || fract_brn_iter == NULL )
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
        if ( fe->GetIgnitionLocFromProps(proptbl, fyr, &ig_cells_list) )
        {
          QuitFatal(NULL);
        }
        /* set ignited cells to the burning state */
        for( lel = LIST_HEAD(ig_cells_list); lel != NULL; lel = LIST_GET_NEXT_ELMT(lel) )
        {
          ig_rwx = LIST_GET_DATA(lel);
          lel = LIST_GET_NEXT_ELMT(lel);
          ig_rwy = LIST_GET_DATA(lel);
          CellStateSetCellStateRealWorld(cs, *ig_rwx, *ig_rwy, EnumHasFireCellState); /* ignite the cell */
          FireYearSetCellNewFireIDRealWorld(fyr, *ig_rwx, *ig_rwy);
          FireExportIgLocsTxtFile(proptbl, *ig_rwx, *ig_rwy, ft);
        }
        /* empty the list of ignited cells */
        FreeList(ig_cells_list);
      }

      /*
      ** Loop Over Each Iteration in a Timestep
      */
      for ( exp_secs = 0.0; exp_secs < (double) timestep; exp_secs += iter_secs ) 
      {
        /* initialize an empty list of burning cell parameters */
        brn_cells_list = InitListEmpty(free);

        /* reset the maximum rate of fire spread this iteration */
        max_rosmps = 0.0;

        /* reset the fraction burned this iteration */
        ZeroFltTwoDArray(fract_brn_iter);

        /* compute the rate of fire spread to all neighbors from every ignited cell */
        for(i = 0; i < domain_rows; i++)  
        {
          for(j = 0; j < domain_cols; j++)
          {
            /* skip cells which are not currently ignited */
            if ( BYTETWODARRAY_GET_DATA(cs->state, i, j) != EnumHasFireCellState )
            {
              continue;
            }
            /* skip boundary cells */
            if ( i == 0 || j == 0 || i == (domain_rows - 1) || j == (domain_cols - 1) )
            {
              continue;
            }
            /* retrieve spatial attribute data */
            GRID_DATA_GET_DATA(elev, i, j, cell_elev);              
            GRID_DATA_GET_DATA(slope, i, j, cell_slope);
            GRID_DATA_GET_DATA(aspect, i, j, cell_aspect);                                  
            GRID_DATA_GET_DATA(fuels, i, j, cell_fmnum);
            /* retrieve fuel model attribute data */
            if ( ChHashTableRetrieve(fmtble, &cell_fmnum, (void *)&fm) )
            {
              QuitFatal(NULL);
            }
            fbedhgtm = fm->rfm->fdepth;
            if ( fm->rfm->units == EnumEnglishUnits ) fbedhgtm = UNITS_FT_TO_M(fbedhgtm);
            /* retrieve real-world coordinates of cell */
            if ( CoordTransSixParamRasterToRealWorld(i, j, cellsz, cellsz, xulcntr, yulcntr, 0.0, 0.0, &rwx, &rwy)  )
            {
              QuitFatal(NULL);
            }
            /* retrieve Santa Ana time-dependent attributes */
            if ( IsSantaAnaNowFromProps(proptbl, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy) )  
            {
              if ( GetSantaAnaEnvFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, &waz, fbedhgtm, &wspmps, &d1hfm, &d10hfm, &d100hfm) )
              {
                QuitFatal(NULL);
              }
              wspfpm = UNITS_MPSEC_TO_FTPMIN(wspmps);                 
            }
            /* retrieve non Santa Ana time-dependent attributes */
            else  
            {
              if ( fe->GetDeadFuelMoistFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, i, j, &d1hfm, &d10hfm, &d100hfm) )
              {
                QuitFatal(NULL);
              }
              if ( fe->GetWindAzimuthFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, rwx, rwy, &waz) )
              {
                QuitFatal(NULL);
              }
              if ( fe->GetWindSpeedMpsFromProps(proptbl, fbedhgtm, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, rwx, rwy, &wspmps) )
              {
                QuitFatal(NULL);
              }
              wspfpm = UNITS_MPSEC_TO_FTPMIN(wspmps); 
            }
            /* retrieve live fuel moisture */
            if ( fe->GetLiveFuelMoistFromProps(proptbl, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, i, j, &lhfm, &lwfm) )
            {
              QuitFatal(NULL);
            }
            /* calculate the no-wind and no-slope rate of spread */
            if ( Roth1972FireSpreadNoWindNoSlope(fm->rfm, d1hfm, d10hfm, d100hfm, lhfm, lwfm) ) 
            {
              QuitFatal(NULL);
            }
            /* calculate maximum rate of spread */
            if ( Roth1972FireSpreadWindSlopeMax(fm->rfm, wspfpm, waz, cell_slope, cell_aspect) )
            {
              QuitFatal(NULL);
            }
            rosfpm = fm->rfm->rp->ros_max;
            rosmps = UNITS_FTPMIN_TO_MPSEC(rosfpm);
            /* compare rate of spread to maximum observed during iteration */
            if ( rosmps > max_rosmps )
            {
              max_rosmps = rosmps;
            }
            /* initialize a cell parameter structure */
            if ( (brn_cell = malloc(sizeof(brn_cell_t))) == NULL )
            {
              QuitFatal(NULL);
            }
            brn_cell->i = i;
            brn_cell->j = j;
            brn_cell->rwx = rwx;
            brn_cell->rwy = rwy;
            brn_cell->max_rosmps = rosmps;
            /* iterate through all neighboring cell azimuths  */
            for (cell_az = 0, num_nbr = 0; cell_az < EIGHTNBR_NUM_NBR_CELLS; cell_az++) 
            {
              nbr_i = EIGHTNBR_ROW_INDEX_AT_AZIMUTH(i, cell_az);
              nbr_j = EIGHTNBR_COL_INDEX_AT_AZIMUTH(j, cell_az);
              /* skip computation for cells that are not burnable, already ignited, or consumed */
              if ( BYTETWODARRAY_GET_DATA(cs->state, nbr_i, nbr_j) != EnumNoFireCellState ) 
              {
                ++num_nbr;
                continue;
              }
              /* compute the rate of spread in direction of neighbor */
              if ( Roth1972FireSpreadGetAtAzimuth(fm->rfm, EIGHTNBR_AZIMUTH_AS_DBL(cell_az)) ) 
              {
                QuitFatal(NULL);
              }
              rosfpm = fm->rfm->rp->ros_any;
              brn_cell->rosmps[cell_az] = UNITS_FTPMIN_TO_MPSEC(rosfpm);
              /* get terrain distance (xyz) to cell center */
              GRID_DATA_GET_DATA(elev, nbr_i, nbr_j, nbr_elev);
              if ( dxyzCalcDist(j * cellsz, i * cellsz, cell_elev, nbr_j * cellsz, nbr_i * cellsz, nbr_elev, &dist2ctrm) )
              {
                QuitFatal(NULL);
              }
              brn_cell->dist2ctrm[cell_az] = dist2ctrm;
            }
            /* transition this cell to consumed state */
            if ( num_nbr == EIGHTNBR_NUM_NBR_CELLS )
            {
              BYTETWODARRAY_SET_DATA(cs->state, i, j, EnumConsumedCellState);
            }
            /* add cell to list of burning cells */
            lel = LIST_TAIL(brn_cells_list);
            ListInsertNext(brn_cells_list, lel, brn_cell); 
          }
        }

        /* compute the elapsed time during the current iteration */
        if ( max_rosmps > 0.0 )
        {
          iter_secs = (cellsz / max_rosmps) * 0.25; /* try scaling the iteration timestep to help distortion */
          /* saturate the elapsed time so that it does not exceed the current timestep */
          if ( (exp_secs + iter_secs) > (double) timestep )
          {
            iter_secs = (double) timestep - exp_secs + HFIRE_EPSILON;
          }
        }
        else
        {
          TimeStamp(ft, "NO CELLS BURNING");
          iter_secs = (double) timestep - exp_secs + HFIRE_EPSILON;
        }

        /* compute the distance traveled from each burning cell during this iteration */
        for( lel = LIST_HEAD(brn_cells_list); lel != NULL; lel = LIST_GET_NEXT_ELMT(lel) )
        {
          brn_cell = LIST_GET_DATA(lel);
          for (cell_az = 0; cell_az < EIGHTNBR_NUM_NBR_CELLS; cell_az++) 
          {
            nbr_i = EIGHTNBR_ROW_INDEX_AT_AZIMUTH(brn_cell->i, cell_az);
            nbr_j = EIGHTNBR_COL_INDEX_AT_AZIMUTH(brn_cell->j, cell_az);
            brn_cell->distm[cell_az] = brn_cell->rosmps[cell_az] * iter_secs;
            fract = brn_cell->distm[cell_az] / brn_cell->dist2ctrm[cell_az];
            if ( fract > FLTTWODARRAY_GET_DATA(fract_brn_iter, nbr_i, nbr_j) )
            {
              FLTTWODARRAY_SET_DATA(fract_brn_iter, nbr_i, nbr_j, fract);
            } 
          }
        }

        /* update the total fraction array */
        for(i = 0; i < domain_rows; i++)  
        {
          for(j = 0; j < domain_cols; j++)
          {
            if ( FLTTWODARRAY_GET_DATA(fract_brn_iter, i, j) > 0.0 )
            {
              fract = FLTTWODARRAY_GET_DATA(fract_brn, i, j) + FLTTWODARRAY_GET_DATA(fract_brn_iter, i, j);
              FLTTWODARRAY_SET_DATA(fract_brn, i, j, fract);
              if ( FLTTWODARRAY_GET_DATA(fract_brn, i, j) > 1.0 )
              {
                /* set the cell state to ignited */
                BYTETWODARRAY_SET_DATA(cs->state, i, j, EnumHasFireCellState);
                for (cell_az = 0; cell_az < EIGHTNBR_NUM_NBR_CELLS; cell_az++)
                {
                  nbr_i = EIGHTNBR_ROW_INDEX_AT_AZIMUTH(i, cell_az);
                  nbr_j = EIGHTNBR_COL_INDEX_AT_AZIMUTH(j, cell_az);
                  if ( BYTETWODARRAY_GET_DATA(cs->state, nbr_i, nbr_j) == EnumHasFireCellState )
                  {
                    /* assign the cell the same fire id as the cell from which the fire came */
                    INTTWODARRAY_SET_DATA(fyr->id, i, j, INTTWODARRAY_GET_DATA(fyr->id, nbr_i, nbr_j));
                  }
                }
              }
            }
          }
        }

        /* empty the list of burning cell parameters */
        FreeList(brn_cells_list);

        /* signal user */
        sprintf(status_msg, "T_exp: %f T_adapt: %f", exp_secs, iter_secs);
        TimeStamp(ft, status_msg);
      } /* End Iteration */

      /* increment simulation clock */
      FireTimerIncrementSeconds(ft, timestep);

      /* increment cell extinction clock */
      if ( UpdateExtinctionHOURS(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, cs, hrs_brn) )
      {
        QuitFatal(NULL);
      }

      /* export data */
      if ( FireExportSpatialData(proptbl, fex) )
      {
        QuitFatal(NULL);
      }

      /* signal user */
      TimeStamp(ft, NULL);
    } /* End Timestep */

    /* export data */
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

    /* signal user */
    TimeStamp(ft, "END SIM YEAR");

    /* increment simulation clock */
    ft->sim_cur_yr += 1;
  } /* End Year */

  /* free all memory */
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
