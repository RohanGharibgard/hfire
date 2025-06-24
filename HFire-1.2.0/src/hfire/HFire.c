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

/* structure used to store burning cell parameters */
typedef struct
{
  int i;                                        /* cell row */
  int j;                                        /* cell col */
  double rwx;                                   /* cell real-world x coordinate */
  double rwy;                                   /* cell real-world y coordinate */
  double max_rosmps;                            /* maximum rate of spread */
  int    max_ros_az;                            /* azimuth of maximum rate of spread */
  double eccen;                                 /* eccentricity of fire */
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
  ListElmt * lel = NULL, * prev = NULL;         /* ptr to single element in a list */

  int timestep;                                 /* duration of simulation timestep */
  double exp_secs, iter_secs;                   /* duration of iteration during timestep */
  int domain_rows, domain_cols;                 /* simulation domain size */
  double cellsz;                                /* simulation cell resolution, in m */
  double xulcntr, yulcntr;                      /* simulation upper left coordinates */
  double rwx, rwy;                              /* real world xy coord pair */
  FireYear * fyr = NULL;                        /* ids of burned cells */
  CellState * cs = NULL;                        /* cell state */
  ByteTwoDArray * hrs_brn = NULL;               /* hours cell has been burning */
  List * brn_cells_list = NULL;                 /* list of burning cell parameters */
  brn_cell_t ** brn_cells_map = NULL;           /* array of pointers to burning cell parameters */

  List * ig_cells_list = NULL;                  /* list of xy coordinates for ignited cells */
  double * ig_rwx, * ig_rwy;                    /* real world xy coordinate pair for ignited cell */

  double ell_adj;                               /* fire ellipse adjustment factor */
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
  brn_cell_t * brn_cell, * new_brn_cell;        /* burning cell parameters */
  brn_cell_t * nbr_brn_cell;
  double az;                                    /* floating point azimuth */
  int cell_az;                                  /* stores cell azimuth */
  int cell, nbr_az[3], brn_az[3];               /* stores neighbor and burning azimuths */
  int nbr_i, nbr_j;                             /* stores nbr cell index i,j */
  double nbr_elev;                              /* neighbor elevation, in m */
  double dist2ctrm;                             /* distance along the ground between cells, in m */
  int num_nbr;                                  /* count of cells which are not burnable, already ignited, or consumed */
  int is_sa = 0;                                /* flag to indicate santa ana is active */

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

  /* set fire ellipse adjustment factor */
  if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_FEAF), (void *)&entry) )
  {
    ell_adj = 1.0;
  }
  else
  {
    ell_adj = atof(entry->val);
  }

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

    /* initialize an empty list of burning cells */
    brn_cells_list = InitListEmpty(free);

    /* initialize an array of pointers to burning cells */
    brn_cells_map = malloc(sizeof(brn_cell_t *) * domain_rows * domain_cols);
    if ( brn_cells_map != NULL ) memset(brn_cells_map, 0, sizeof(brn_cell_t *) * domain_rows * domain_cols);

    /* ensure all structures properly initialized */
    if ( fyr == NULL || cs == NULL || hrs_brn == NULL || brn_cells_list == NULL || brn_cells_map == NULL )
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
          /* obtain real world coordinates of ignition */
          ig_rwx = LIST_GET_DATA(lel);
          lel = LIST_GET_NEXT_ELMT(lel);
          ig_rwy = LIST_GET_DATA(lel);
          /* transform real world coordinates to row and column */
          if ( CoordTransRealWorldToRaster(*ig_rwx, *ig_rwy, cellsz, cellsz, xulcntr, yulcntr, &i, &j) )
          {
            QuitFatal(NULL);
          }
          /* ignite the cell */
          CellStateSetCellStateRealWorld(cs, *ig_rwx, *ig_rwy, EnumHasFireCellState);
          if ( FireYearSetCellNewFireIDRealWorld(fyr, *ig_rwx, *ig_rwy, ft, is_sa) )
          {
            QuitFatal(NULL);
          }
          FireExportIgLocsTxtFile(proptbl, fyr->num_fires, *ig_rwx, *ig_rwy, ft);
          /* initialize a new burning cell structure */
          if ( (new_brn_cell = malloc(sizeof(brn_cell_t))) == NULL )
          {
            QuitFatal(NULL);
          }
          memset(new_brn_cell, 0, sizeof(brn_cell_t));
          new_brn_cell->i = i;
          new_brn_cell->j = j;
          new_brn_cell->rwx = *ig_rwx;
          new_brn_cell->rwy = *ig_rwy;
          /* add new burning cell to list of burning cells */
          ListInsertNext(brn_cells_list, LIST_TAIL(brn_cells_list), new_brn_cell);
          /* insert pointer to burning cell into map */
          brn_cells_map[i * domain_cols + j] = new_brn_cell;
        }

        /* empty the list of ignited cells */
        FreeList(ig_cells_list);
      }

      /*
      ** Loop Over Each Iteration in a Timestep
      */
      for ( exp_secs = 0.0; exp_secs < (double) timestep; exp_secs += iter_secs ) 
      {
        /* reset the maximum rate of fire spread this iteration */
        max_rosmps = 0.0;

        /* compute the rate of fire spread from every burning cell to its neighbors */
        for( lel = LIST_HEAD(brn_cells_list), prev = NULL; lel != NULL;  )
        {
          brn_cell = LIST_GET_DATA(lel);
          /* retrieve coordinate information for burning cell */
          i = brn_cell->i;
          j = brn_cell->j;
          /* retrieve real-world coordinates of cell */
          if ( CoordTransSixParamRasterToRealWorld(i, j, cellsz, cellsz, xulcntr, yulcntr,0.0, 0.0, &rwx, &rwy) )
          {
            QuitFatal(NULL);
          }
          brn_cell->rwx = rwx;
          brn_cell->rwy = rwy;
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
          /* retrieve Santa Ana time-dependent attributes */
          is_sa = IsSantaAnaNowFromProps(proptbl, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy);
          if ( is_sa )  
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
            if ( fe->GetDeadFuelMoistFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, rwx, rwy, &d1hfm, &d10hfm, &d100hfm) )
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
          if ( fe->GetLiveFuelMoistFromProps(proptbl, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, rwx, rwy, &lhfm, &lwfm) )
          {
            QuitFatal(NULL);
          }
          /* calculate the no-wind and no-slope rate of spread */
          if ( Roth1972FireSpreadNoWindNoSlope(fm->rfm, d1hfm, d10hfm, d100hfm, lhfm, lwfm) ) 
          {
            QuitFatal(NULL);
          }
          /* calculate maximum rate of spread */
          if ( Roth1972FireSpreadWindSlopeMax(fm->rfm, wspfpm, waz, cell_slope, cell_aspect, ell_adj) )
          {
            QuitFatal(NULL);
          }
          rosfpm = fm->rfm->rp->ros_max;
          rosmps = UNITS_FTPMIN_TO_MPSEC(rosfpm);
          brn_cell->max_rosmps = rosmps;
          /* compare rate of spread to maximum observed during iteration */
          if ( brn_cell->max_rosmps > max_rosmps )
          {
            max_rosmps = brn_cell->max_rosmps;
          }
          /* set the azimuth of the maximum rate of spread */
          az = fm->rfm->rp->ros_az_max / 45.0;
          if ( az < 0.5 )       brn_cell->max_ros_az = 0; /*   0 */
          else if ( az < 1.5 )  brn_cell->max_ros_az = 1; /*  45 */
          else if ( az < 2.5 )  brn_cell->max_ros_az = 2; /*  90 */
          else if ( az < 3.5 )  brn_cell->max_ros_az = 3; /* 135 */
          else if ( az < 4.5 )  brn_cell->max_ros_az = 4; /* 180 */
          else if ( az < 5.5 )  brn_cell->max_ros_az = 5; /* 225 */
          else if ( az < 6.5 )  brn_cell->max_ros_az = 6; /* 270 */
          else if ( az < 7.5 )  brn_cell->max_ros_az = 7; /* 315 */
          else                  brn_cell->max_ros_az = 0; /* 360 */
          /* set the eccentricity of the burning fire */
          brn_cell->eccen = fm->rfm->rp->eccen;
          /* iterate through all neighboring cell azimuths  */
          for (cell_az = 0, num_nbr = 0; cell_az < EIGHTNBR_NUM_NBR_CELLS; cell_az++) 
          {
            /* retrieve coordinate information for neighbor cell */
            nbr_i = EIGHTNBR_ROW_INDEX_AT_AZIMUTH(i, cell_az);
            nbr_j = EIGHTNBR_COL_INDEX_AT_AZIMUTH(j, cell_az);
            /* zero out the rate of spread into neighbor for this iteration */
            brn_cell->rosmps[cell_az] = 0.0;
            /* skip computation for cells that are not burnable, already ignited, or consumed */
            if ( BYTETWODARRAY_GET_DATA(cs->state, nbr_i, nbr_j) != EnumNoFireCellState ) 
            {
              ++num_nbr;
              continue;
            }
            /* skip computation for boundary cells */
            if ( nbr_i == 0 || nbr_j == 0 || nbr_i == (domain_rows - 1) || nbr_j == (domain_cols - 1) )
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
            /* clear pointer to burning cell in map */
            brn_cells_map[i * domain_cols + j] = NULL;
            /* remove cell from list of burning cells */
            if ( lel == LIST_HEAD(brn_cells_list) )
            {
              ListRemoveNext(brn_cells_list, NULL, &brn_cell);
              lel = LIST_HEAD(brn_cells_list);
              prev = NULL;
            }
            else
            {
              ListRemoveNext(brn_cells_list, prev, &brn_cell);
              lel = LIST_GET_NEXT_ELMT(prev);
            }
            if ( brn_cell != NULL )
            {
              free(brn_cell);
            }
          }
          /* check for fire extinction */
          else if ( UpdateExtinctionROS(proptbl, i, j, brn_cell->max_rosmps, cs, hrs_brn) )
          {
            /* clear pointer to burning cell in map */
            brn_cells_map[i * domain_cols + j] = NULL;
            /* remove cell from list of burning cells */
            if ( lel == LIST_HEAD(brn_cells_list) )
            {
              ListRemoveNext(brn_cells_list, NULL, &brn_cell);
              lel = LIST_HEAD(brn_cells_list);
              prev = NULL;
            }
            else
            {
              ListRemoveNext(brn_cells_list, prev, &brn_cell);
              lel = LIST_GET_NEXT_ELMT(prev);
            }
            if ( brn_cell != NULL )
            {
              free(brn_cell);
            }
          }
          /* cell will continue to burn */
          else
          {
            prev = lel;
            lel = LIST_GET_NEXT_ELMT(lel);
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

        /* compute the distance traveled from the computed rate and elapsed time */
        for( lel = LIST_HEAD(brn_cells_list); lel != NULL; lel = LIST_GET_NEXT_ELMT(lel) )
        {
          brn_cell = LIST_GET_DATA(lel);
          /* retrieve coordinate information for burning cell */
          i = brn_cell->i;
          j = brn_cell->j;
          for (cell_az = 0; cell_az < EIGHTNBR_NUM_NBR_CELLS; cell_az++) 
          {
            /* retrieve coordinate information for neighbor cell */
            nbr_i = EIGHTNBR_ROW_INDEX_AT_AZIMUTH(i, cell_az);
            nbr_j = EIGHTNBR_COL_INDEX_AT_AZIMUTH(j, cell_az);
            /* skip computation for boundary cells */
            if ( nbr_i == 0 || nbr_j == 0 || nbr_i == (domain_rows - 1) || nbr_j == (domain_cols - 1) )
            {
              continue;
            }
            /* update the distance spread from the burning cell */
            switch ( BYTETWODARRAY_GET_DATA(cs->state, nbr_i, nbr_j) )
            {
            case EnumUnBurnableCellState:
              /* cell is not burnable, do nothing */
              break;
            case EnumConsumedCellState:
              /* cell is consumed, do nothing */
              break;
            case EnumHasFireCellState:
              /* cell is already burning, do nothing */
              break;
            case EnumNoFireCellState:
              /* skip computation if rate of spread is zero */
              if ( brn_cell->rosmps[cell_az] > 0.0 )
              {
                /* cell is not yet burning, increment the distance traveled to this cell during this iteration */
                brn_cell->distm[cell_az] += brn_cell->rosmps[cell_az] * iter_secs;
                /* compare the distance traveled with the distance to the cell center */
                if ( brn_cell->distm[cell_az] > brn_cell->dist2ctrm[cell_az] )
                {
                  /* set the cell state to ignited */
                  BYTETWODARRAY_SET_DATA(cs->state, nbr_i, nbr_j, EnumHasFireCellState);
                  /* assign the cell the same fire id as the cell from which the fire came */
                  FireYearSetCellFireIDRowCol(fyr, nbr_i, nbr_j, INTTWODARRAY_GET_DATA(fyr->id, i, j), ft, is_sa);
                  /* initialize a new burning cell structure */
                  if ( (new_brn_cell = malloc(sizeof(brn_cell_t))) == NULL )
                  {
                    QuitFatal(NULL);
                  }
                  memset(new_brn_cell, 0, sizeof(brn_cell_t));
                  new_brn_cell->i = nbr_i;
                  new_brn_cell->j = nbr_j;
                  new_brn_cell->distm[cell_az] = brn_cell->distm[cell_az] - brn_cell->dist2ctrm[cell_az]; /* slop over */
                  /* add new burning cell to list of burning cells */
                  ListInsertNext(brn_cells_list, LIST_TAIL(brn_cells_list), new_brn_cell);
                  /* insert pointer to burning cell into map */
                  brn_cells_map[nbr_i * domain_cols + nbr_j] = new_brn_cell;
                }
              }
              break;
            }
          }
        }

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

    /* set failed igntions */
    FireYearSetFailedIgnitions(proptbl, fyr);

    /* export data */
    if ( FireExportSpatialData(proptbl, fex) )
    {
      QuitFatal(NULL);
    }

    /* export fire area */
    if ( FireExportFireAreaTxtFile(proptbl, fyr) )
    {
      QuitFatal(NULL);
    }

    /* export fire info */
    if ( FireExportFireInfoTxtFile(proptbl, fyr) )
    {
      QuitFatal(NULL);
    }

    /* export age at burn histogram file */
    if ( FireExportAgeAtBurnHistTxtFile(proptbl, fyr, std_age) )
    {
      QuitFatal(NULL);
    }

    /* free pointers to burning cells */
    if ( brn_cells_map != NULL ) free(brn_cells_map);

    /* empty the list of burning cells */
    FreeList(brn_cells_list);

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
