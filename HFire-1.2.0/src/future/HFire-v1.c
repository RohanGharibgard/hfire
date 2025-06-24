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
	
void QuitFatal(char * emessage)	{
	if ( emessage != NULL )	{
		fprintf(stderr, "%s", emessage);
		}
	
	fprintf(stderr, "\n FATAL ERROR. Program Aborting.\n");
		
	exit(1);
	}

void TimeStamp(FireTimer * ft, char * message)	{
	int mt = 0;
	
	if ( ft != NULL )	{	
		mt = FIRE_TIMER_GET_MILITARY_TIME(ft);
		if ( message == NULL )	{
			fprintf(stdout, "SIM TIME... YR: %d MO: %02d DY: %02d HR: %04d SEC: %d \n", ft->sim_cur_yr,
			 								ft->sim_cur_mo, ft->sim_cur_dy, mt, ft->sim_cur_secs);
			}
		else	{
			fprintf(stdout, "%s... YR: %d MO: %02d DY: %02d HR: %04d SEC: %d \n", message,
			 								ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt, ft->sim_cur_secs);
			}
		}
	return;
	}
 		 	
int main(int argc, char * argv[]) {
	/* argc		= number of arguments (should be 2) */
	/* argv[0] 	= "name of executable" */
	/* argv[1] 	= "configuration file name" */
	/* argv[2]	= NULL */

	/*VARIABLE DECLARATION*/
	ChHashTable * proptbl		= NULL;										/* simulation properties read from file */
	GridData	* elev 			= NULL;										/* elev spatial data */
	GridData	* slope			= NULL;										/* slope spatial data */
	GridData	* aspect		= NULL;										/* aspect spatial data */
	GridData 	* std_age		= NULL;										/* stand age spatial data */
	GridData	* fuels			= NULL;										/* fuels spatial data */
	FireTimer  	* ft 			= NULL;										/* stores simulation time */
	FireEnv		* fe			= NULL;										/* table of function ptrs for env vars */
	List		* fmlist		= NULL;										/* list of FuelModels */
	ChHashTable * fmtble		= NULL;										/* table of FuelModels */	
	FireExport	* fex			= NULL;										/* wrapper for program export */

	int i, j;																/* spatial row and col */

	IntXY * cell 				= NULL;										/* stores cell index i,j */
			
	double rwx, rwy;														/* stores real world xy coord pair */
		
	ListElmt * lel 				= NULL;										/* ptr to single element in list */
	
	int domain_rows, domain_cols;											/* simulation domain size */
	
	double cellsz;															/* simulation cell resolution */
	
	double xulcntr, yulcntr;												/* simulation ul coordinates */
	
	List * brn_cells_list 		= NULL;										/* list xy coords of burning cells */
	
	double * brn_x, * brn_y;												/* real world coords of burning cells */
	
	FuelModel * fm				= NULL;										/* ptr to FuelModel */
	
	double cell_elev, nbr_elev;												/* stores cell and nbr elevation */
	
	double cell_slope;														/* stores cell slope */
	
	double cell_aspect;														/* stores cell aspect */
	
	double d1hfm, d10hfm, d100hfm;											/* dead fuel moisture */
	
	double lhfm, lwfm;														/* live fuel moisture */
	
	double cell_waz;														/* stores cell wind azimuth */
	
	double cell_wsp;														/* stores cell wind speed */
	
	int cell_fm_num;														/* stores cell fm number */
	
	int cell_az;															/* stores cell azimuth */
	
	int num_brn_nbr;														/* stores num of burning nbr cells */
	
	int nbr_i, nbr_j;														/* stores nbr cell index i,j */

	int timestep, exp_secs, iter_secs;										/* stores time related vars */

	FireYear * fyr				= NULL;										/* ids of burned cells */
	
	CellState * cs				= NULL;										/* current state of cell wrt fire */
			
	ByteTwoDArray * hrs_brn		= NULL;										/* hours cell has been burning */
	
	FltTwoDArray * fract_brn	= NULL;										/* fraction of distance fire must travel to cell center */
	
	FltTwoDArray * fract_iter	= NULL;										/* fastest fraction burned any one iteration */
	
	double fpmros, mpsros;													/* rate of sprd, ENGLISH and METRIC */
	
	double dist2ctr, disttrvl;												/* distance btwn cells fire has traveled */
	
	double fracttrvl, fractover;											/* fraction of distance btwn cells fire has traveled */
	
	KeyVal * entry				= NULL;										/* vals returned from Hash Table */
	
	void (*RandInit)(long int seed) = randinit;								/* rng seed function from NLIBRand.h */	
	
	char status_msg[HFIRE_DEFAULT_STATUS_LINE_LENGTH] = {'\0'};

	/*VARIABLE INITIALIZATION*/	
	if ( argc != 2 )	{
		printf("Usage: %s <configuration filename> \n", argv[0]);
		return 0;
		}

	/* load properties from configuration file */
	if ( InitPropsFromFireConfig(&proptbl, argv[1])	)	{
		QuitFatal(NULL);
		}
	FireConfigDumpPropsToStream(proptbl, stdout);
			
	/* load fuel models from properties */
	if ( InitRothFuelModelListFromProps(proptbl, &fmlist) )	{
		QuitFatal(NULL);
		}
	FireConfigDumpFuelModelListToStream(fmlist, stdout);
						
	/* load initialization parameters from properties */
	if ( InitGridsFromPropsFireConfig(proptbl, &elev, &slope, &aspect)
			||	InitFireTimerFromPropsFireConfig(proptbl, &ft)
			||	InitFuelModelHashTableFromFuelModelListFireConfig(fmlist, &fmtble) 
			||  InitStandAgeFromPropsFireConfig(proptbl, elev, &std_age)
			||  InitFireEnvFromPropsFireConfig(proptbl, &fe)
			||	InitRandNumGenFromPropsFireConfig(proptbl, randinit)	)	{
		QuitFatal(NULL);
		}
	
	/* set simulation export properties */
	if ( (fex = InitFireExport(proptbl)) == NULL ) 	{
		QuitFatal(NULL);
		}
	FIRE_EXPORT_SET_FIRE_TIMER(fex, ft);
	FIRE_EXPORT_SET_STAND_AGE(fex, std_age);
	
	/* set simulation timestep */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_SIMTSSEC), (void *)&entry) )	{
		ERR_ERROR_CONTINUE("Unable to retrieve SIMULATION_TIMESTEP_SECS property.\n", ERR_EINVAL);
		QuitFatal(NULL);
		}	
	timestep = atoi(entry->val);
	
	/*SIMULATION LOOP*/
	/* new year */
	while( !FireTimerIsSimTimeExpired(ft) )	{
		/* advance timer to start of fire season */
		ft->sim_cur_mo = ft->sim_start_mo;
		ft->sim_cur_dy = ft->sim_start_dy;
		ft->sim_cur_hr = ft->sim_start_hr;
		
		/* signal user */
		TimeStamp(ft, "START SIM YEAR");
		
		/* allocate data structures */
		
		/* initialize fuels to be used during this year of simulation */
		if ( fe->GetFuelsRegrowthFromProps(proptbl, std_age, &fuels) )	{
			QuitFatal(NULL);
			}
		FIRE_EXPORT_SET_FUELS(fex, fuels);
		
		/* get dimensions of fuels */		 	
		domain_rows = fuels->ghdr->nrows; 
		domain_cols = fuels->ghdr->ncols;	
		cellsz		= (double) fuels->ghdr->cellsize;
		xulcntr = COORD_TRANS_XLLCORNER_TO_XULCNTR(fuels->ghdr->xllcorner, cellsz);
		yulcntr = COORD_TRANS_YLLCORNER_TO_YULCNTR(fuels->ghdr->yllcorner, cellsz, fuels->ghdr->nrows);
	
		/* initialize a fire year structure for year in simulation */				 
		fyr = InitFireYearFuels(ft->sim_cur_yr, fuels, fmtble);
		FIRE_EXPORT_SET_FIRE_YEAR(fex, fyr);
		
		/* initialize a cell state structure for year in simulation */
		cs = InitCellStateFuels(fuels, fmtble);
						
		/* initialize a hours burned structure for year in simulation */
		hrs_brn = InitByteTwoDArraySizeIniValue(domain_rows, domain_cols, 0);
		
		/* initialize a fraction burned structure for year in simulation */
		fract_brn = InitFltTwoDArraySizeIniValue(domain_rows, domain_cols, HFIRE_FRACT_ZERO);
		
		/* initialize a fraction burned iter structure for year in simulation */
		fract_iter = InitFltTwoDArraySizeIniValue(domain_rows, domain_cols, HFIRE_FRACT_ZERO);
				
		/* ensure all structures properly initialized */
		if ( fyr == NULL || cs == NULL || hrs_brn == NULL || fract_brn == NULL || fract_iter == NULL )	{
		 	QuitFatal(NULL);
		 	}
				
		/* new timestep */
		while( !FireTimerIsSimCurYearTimeExpired(ft) )	{
			/* determine if ignition occurs and, if so, ignition location */
			if ( fe->IsIgnitionNowFromProps(proptbl) )	{
				if ( fe->GetIgnitionLocFromProps(proptbl, fyr, &brn_cells_list) )	{
					QuitFatal(NULL);
					}
				for( lel = LIST_HEAD(brn_cells_list); lel != NULL; lel = LIST_GET_NEXT_ELMT(lel) )	{
					brn_x = LIST_GET_DATA(lel);
					lel = LIST_GET_NEXT_ELMT(lel);
					brn_y = LIST_GET_DATA(lel);
					CellStateSetCellStateRealWorld(cs, *brn_x, *brn_y, EnumHasFireCellState);
					FireYearSetCellNewFireIDRealWorld(fyr, *brn_x, *brn_y);
					FireExportIgLocsTxtFile(proptbl, *brn_x, *brn_y, ft);
					}
				FreeList(brn_cells_list);
				}
	
			/* inner loop meant to adaptively size exp_secs until one timestep has been exhausted */
			for(exp_secs = 0; exp_secs < timestep; exp_secs += iter_secs)	{
				/* reset size of current iteration to zero */
				iter_secs = timestep + 1;
	
				/* initialize list of burning cells */
				brn_cells_list 	= InitListEmpty(FreeIntXY);
									
				/* (1) find time expired this timestep, minimum iter_secs = cellsize / max mpsros */
				for(i = 0; i < domain_rows; i++)	{
					for(j = 0; j < domain_cols; j++)	{
						/* zero the fraction burned this iteration structure */
						FLTTWODARRAY_SET_DATA(fract_iter, i, j, HFIRE_FRACT_ZERO);
						/* find max mpsros for all currently burning cells */
						if ( BYTETWODARRAY_GET_DATA(cs->state, i, j) == EnumHasFireCellState )	{
							/* immediately skip over boundary cells */
							if ( i == 0 || j == 0 || i == (domain_rows - 1) || j == (domain_cols - 1) )	{
								continue;
								}
							/* retrieve spatial attribute data */
							GRID_DATA_GET_DATA(elev, i, j, cell_elev);							
							GRID_DATA_GET_DATA(slope, i, j, cell_slope);
							GRID_DATA_GET_DATA(aspect, i, j, cell_aspect);																	
							GRID_DATA_GET_DATA(fuels, i, j, cell_fm_num);
							/* retrieve fuel model attribute data */
							if ( ChHashTableRetrieve(fmtble, &cell_fm_num, (void *)&fm) )	{
								QuitFatal(NULL);
								}
							/* retrieve real-world coordinates of cell */
							if ( CoordTransSixParamRasterToRealWorld(i, j, cellsz, cellsz, xulcntr, yulcntr,
									0.0, 0.0, &rwx, &rwy)	)	{
								ERR_ERROR("Unable to transform real world coordinates to grid indexcies. \n", ERR_ESING);
								}
							/* retrieve Santa Ana time-dependent attributes */
							if ( IsSantaAnaNowFromProps(proptbl, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy) )	{
								if ( GetSantaAnaEnvFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr,
									&cell_waz, UNITS_FT_TO_M(fm->rfm->fdepth), &cell_wsp, &d1hfm, &d10hfm, &d100hfm) )	{
									QuitFatal(NULL);
									}
								cell_wsp = UNITS_MPSEC_TO_FTPMIN(cell_wsp);									
								}
							/* retrieve non Santa Ana time-dependent attributes */
							else	{
								if ( fe->GetDeadFuelMoistFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr,
								 				i, j, &d1hfm, &d10hfm, &d100hfm) )	{
									QuitFatal(NULL);
									}
								if ( fe->GetWindAzimuthFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr,
								 				rwx, rwy, &cell_waz) )	{
								 	QuitFatal(NULL);
								 	}
								if ( fe->GetWindSpeedMpsFromProps(proptbl, UNITS_FT_TO_M(fm->rfm->fdepth),
										ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, rwx, rwy, &cell_wsp) )	{
								 	QuitFatal(NULL);
									}
								cell_wsp = UNITS_MPSEC_TO_FTPMIN(cell_wsp);	
								}
							/* retrieve live fuel moisture */
							if ( fe->GetLiveFuelMoistFromProps(proptbl, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, 
										i, j, &lhfm, &lwfm)	)	{
								QuitFatal(NULL);
								}
							/* calculate max mpsros */							
							if ( Roth1972FireSpreadNoWindNoSlope(fm->rfm, d1hfm, d10hfm, d100hfm, lhfm, lwfm) 
								|| Roth1972FireSpreadWindSlopeMax(fm->rfm, cell_wsp, cell_waz, cell_slope, cell_aspect) )	{
								QuitFatal(NULL);
								}
							fpmros = fm->rfm->rp->ros_max;
							mpsros = UNITS_FTPMIN_TO_MPSEC(fpmros);
							/* test if iter_secs is the minimum */
							if ( (cellsz / mpsros) < iter_secs )	{
								iter_secs = cellsz / mpsros;
								}															
							/* add cell to list of all burning cells */
							lel = LIST_TAIL(brn_cells_list);
							ListInsertNext(brn_cells_list, lel, InitIntXY(j,i));
							}
						}
					}
	
				/* (2) increment timestep according to iter_secs */
				if ( LIST_SIZE(brn_cells_list) > 0 )	{
					/* when a cell is burning at least one full second must always expire */
					if ( iter_secs < 1 )	{
						iter_secs = 1;
						}
					/* truncate to timestep if necessary */
					if ( iter_secs + exp_secs > timestep )	{
						iter_secs = timestep - exp_secs;
						}
					}
				else	{
					/* nothing is burning */
					TimeStamp(ft, "NO CELLS BURNING");
					FreeList(brn_cells_list);
					break;
					}
				
				/* (3) apply changes during iter_secs, advance fire distance = mpsros * iter_secs */
				for( lel = LIST_HEAD(brn_cells_list); lel != NULL; lel = LIST_GET_NEXT_ELMT(lel) )	{
					cell = LIST_GET_DATA(lel);									
					i = cell->y;
					j = cell->x;
					/* retrieve spatial attribute data */
					GRID_DATA_GET_DATA(elev, i, j, cell_elev);				
					GRID_DATA_GET_DATA(slope, i, j, cell_slope);
					GRID_DATA_GET_DATA(aspect, i, j, cell_aspect);																	
					GRID_DATA_GET_DATA(fuels, i, j, cell_fm_num);
					/* retrieve fuel model attribute data */
					if ( ChHashTableRetrieve(fmtble, &cell_fm_num, (void *)&fm) )	{
						QuitFatal(NULL);
						}
					/* retrieve real-world coordinates of cell */
					if ( CoordTransSixParamRasterToRealWorld(i, j, cellsz, cellsz, xulcntr, yulcntr,
							0.0, 0.0, &rwx, &rwy)	)	{
						ERR_ERROR("Unable to transform real world coordinates to grid indexcies. \n", ERR_ESING);
						}						
					/* retrieve Santa Ana time-dependent attributes */
					if ( IsSantaAnaNowFromProps(proptbl, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy) )	{
						if ( GetSantaAnaEnvFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr,
							&cell_waz, UNITS_FT_TO_M(fm->rfm->fdepth), &cell_wsp, &d1hfm, &d10hfm, &d100hfm) )	{
							QuitFatal(NULL);
							}
						cell_wsp = UNITS_MPSEC_TO_FTPMIN(cell_wsp);
						}
					/* retrieve non Santa Ana time-dependent attributes */
					else	{
						if ( fe->GetDeadFuelMoistFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr,
						 				i, j, &d1hfm, &d10hfm, &d100hfm) )	{
							QuitFatal(NULL);
							}
						if ( fe->GetWindAzimuthFromProps(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr,
						 				rwx, rwy, &cell_waz) )	{
						 	QuitFatal(NULL);
						 	}
						if ( fe->GetWindSpeedMpsFromProps(proptbl, UNITS_FT_TO_M(fm->rfm->fdepth),
								ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, rwx, rwy, &cell_wsp) )	{
						 	QuitFatal(NULL);
							}
						cell_wsp = UNITS_MPSEC_TO_FTPMIN(cell_wsp);	
						}
					/* retrieve live fuel moisture */
					if ( fe->GetLiveFuelMoistFromProps(proptbl, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, 
								i, j, &lhfm, &lwfm)	)	{
						QuitFatal(NULL);
						}						
					/* counter to determine number of burning neighbors */
					num_brn_nbr = 0;
					/* intiate fire spread pipeline for fuel model number at i,j */
					if ( Roth1972FireSpreadNoWindNoSlope(fm->rfm, d1hfm, d10hfm, d100hfm, lhfm, lwfm) 
						|| Roth1972FireSpreadWindSlopeMax(fm->rfm, cell_wsp, cell_waz, cell_slope, cell_aspect) )	{
						QuitFatal(NULL);
						}
					fpmros = fm->rfm->rp->ros_max;
					mpsros = UNITS_FTPMIN_TO_MPSEC(fpmros);
					/* extinguish the cell if ros insufficient */					
					if ( UpdateExtinctionROS(proptbl, i, j, mpsros, cs, hrs_brn) )	{
						/* cell extinguished */
						FLTTWODARRAY_SET_DATA(fract_brn, i, j, HFIRE_FRACT_ZERO);
						FLTTWODARRAY_SET_DATA(fract_iter, i, j, HFIRE_FRACT_ZERO);						
						continue;
						}
					/* iterate through all neighboring azimuths spreading fire */							
					for (cell_az = 0; cell_az < EIGHTNBR_NUM_NBR_CELLS; cell_az++)		{
						nbr_i = EIGHTNBR_ROW_INDEX_AT_AZIMUTH(i, cell_az);
						nbr_j = EIGHTNBR_COL_INDEX_AT_AZIMUTH(j, cell_az);
						/* immediately skip over boundary cells */
						if ( nbr_i == 0 || nbr_j == 0 || nbr_i == (domain_rows - 1) || nbr_j == (domain_cols - 1) )	{
							num_brn_nbr++;
							continue;
							}	
						/* if cell has fire then it is already in brn_cells_list */
						if ( BYTETWODARRAY_GET_DATA(cs->state, nbr_i, nbr_j) != EnumNoFireCellState )	{
							num_brn_nbr++;
							continue;
							}
						/* determine how far fire spreads, get mpsros at azimuth */				
						if ( Roth1972FireSpreadGetAtAzimuth(fm->rfm, EIGHTNBR_AZIMUTH_AS_DBL(cell_az)) ) 	{
							QuitFatal(NULL);
							}
						fpmros = fm->rfm->rp->ros_any;
						mpsros = UNITS_FTPMIN_TO_MPSEC(fpmros);
						/* get distance fire traveled */
						disttrvl = mpsros * iter_secs;
						/* get terrain distance (xyz) to cell center */
						GRID_DATA_GET_DATA(elev, nbr_i, nbr_j, nbr_elev);
						if ( dxyzCalcDist(j * cellsz, i * cellsz, cell_elev, 
											nbr_j * cellsz, nbr_i * cellsz, nbr_elev, &dist2ctr) )	{
							QuitFatal(NULL);
							}
						/* convert absolute distance to fraction of pathway */
						fracttrvl = disttrvl / dist2ctr;
						/* apply distance to fract_brn array */
						if ( FLTTWODARRAY_GET_DATA(fract_brn, nbr_i, nbr_j) <= HFIRE_FRACT_ZERO )	{
							FLTTWODARRAY_SET_DATA(fract_brn, nbr_i, nbr_j, fracttrvl);
							}
						/* only replace new fraction when larger than any other fraction this iteration */
						else if ( fracttrvl > FLTTWODARRAY_GET_DATA(fract_iter, nbr_i, nbr_j) )	{
							/* subtract the old fract */
							FLTTWODARRAY_SET_DATA(fract_brn, nbr_i, nbr_j, (FLTTWODARRAY_GET_DATA(fract_brn, nbr_i, nbr_j) - FLTTWODARRAY_GET_DATA(fract_iter, nbr_i, nbr_j)));
							/* replace with new fract */
							FLTTWODARRAY_SET_DATA(fract_brn, nbr_i, nbr_j, (FLTTWODARRAY_GET_DATA(fract_brn, nbr_i, nbr_j) + fracttrvl));
							FLTTWODARRAY_SET_DATA(fract_iter, nbr_i, nbr_j, fracttrvl);							
							}
						/* fraction traveled smaller than max this timestep */
						else	{
							continue;
							}
						/* determine if cell center reached, if so set cell to burning state */
						fractover = FLTTWODARRAY_GET_DATA(fract_brn, nbr_i, nbr_j) - 1.0;
						if ( fractover > 1.0 )	{
							/* set new cell state */
							BYTETWODARRAY_SET_DATA(cs->state, nbr_i, nbr_j, EnumHasFireCellState);
							/* increment has fire neighbor count */
							num_brn_nbr++;
							/* assign new burning cell id same as center cell id */
							INTTWODARRAY_SET_DATA(fyr->id, nbr_i, nbr_j, INTTWODARRAY_GET_DATA(fyr->id, i, j));
							/* apply overflow to nbr */
							nbr_i = EIGHTNBR_ROW_INDEX_AT_AZIMUTH(nbr_i, cell_az);
							nbr_j = EIGHTNBR_COL_INDEX_AT_AZIMUTH(nbr_j, cell_az);							
							if ( BYTETWODARRAY_GET_DATA(cs->state, nbr_i, nbr_j) == EnumNoFireCellState )	{
								if ( FLTTWODARRAY_GET_DATA(fract_brn, nbr_i, nbr_j) <= HFIRE_FRACT_ZERO )	{
									FLTTWODARRAY_SET_DATA(fract_brn, nbr_i, nbr_j, fractover);
									}
								else if ( fractover > FLTTWODARRAY_GET_DATA(fract_iter, nbr_i, nbr_j) )	{
									/* subtract the old fract */
									FLTTWODARRAY_SET_DATA(fract_brn, nbr_i, nbr_j, (FLTTWODARRAY_GET_DATA(fract_brn, nbr_i, nbr_j) - FLTTWODARRAY_GET_DATA(fract_iter, nbr_i, nbr_j)));
									/* replace with new fract */
									FLTTWODARRAY_SET_DATA(fract_brn, nbr_i, nbr_j, (FLTTWODARRAY_GET_DATA(fract_brn, nbr_i, nbr_j) + fractover));
									FLTTWODARRAY_SET_DATA(fract_iter, nbr_i, nbr_j, fractover);	
									}
								}
							}
						}
					/* determine whether this cell consumed */
					if ( num_brn_nbr >= EIGHTNBR_NUM_NBR_CELLS )	{
						BYTETWODARRAY_SET_DATA(cs->state, i, j, EnumConsumedCellState);
						}
					}
				
				/* (4) cleanup for one cycle through adaptive timestep */
				FreeList(brn_cells_list);
				sprintf(status_msg, "T_exp: %d T_adapt: %d", exp_secs, iter_secs);
				TimeStamp(ft, status_msg);
				}
			
			/* increment simulation clock */
			FireTimerIncrementSeconds(ft, timestep);
			
			/* increment cell extinction clock */
			if ( UpdateExtinctionHOURS(proptbl, ft->sim_cur_mo, ft->sim_cur_dy, ft->sim_cur_hr, cs, hrs_brn) )	{
				QuitFatal(NULL);
				}
	
			/* export data */
			if ( FireExportSpatialData(proptbl, fex) )	{
				QuitFatal(NULL);
				}
	
			/* signal user */
			TimeStamp(ft, NULL);
			
			/* end of timestep */
			}

		/* export data */
		if ( FireExportSpatialData(proptbl, fex) )	{
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
						
		/* increment simulation clock */
		ft->sim_cur_yr += 1;
			
		/* end of year */
		}
	
	/* CLEANUP */
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
