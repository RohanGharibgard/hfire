/*! 
 * \file FireTimer.c
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
 
#include "FireTimer.h"

#include <time.h>

/* 0 based mo array...				  J   F   M   A   M   J   J   A   S   O   N   D */
static const int sdays_in_month [] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

FireTimer * InitFireTimer(int sim_start_yr, int sim_start_mo, int sim_start_dy, int sim_start_hr,
						  int sim_end_yr, int sim_end_mo, int sim_end_dy, int sim_end_hr)	{
	FireTimer * ft 	= NULL;

	if ( sim_start_yr > sim_end_yr )	{
		ERR_ERROR_CONTINUE("Unable to initialize FireTimer, end year precedes start year. \n", ERR_ESANITY);
		return ft;
		}
	if ( sim_start_mo > FIRE_TIMER_MAX_MO_YEAR || sim_end_mo > FIRE_TIMER_MAX_MO_YEAR )	{
		ERR_ERROR_CONTINUE("Unable to initialize FireTimer, month not in range 1 to 12. \n", ERR_ESANITY);
		return ft;
		}
	if ( FireTimerGetDaysInMonth(sim_start_mo) < sim_start_dy || FireTimerGetDaysInMonth(sim_end_mo) < sim_end_dy )	{
		ERR_ERROR_CONTINUE("Unable to initialize FireTimer, day exceeds days in month. \n", ERR_ESANITY);
		return ft;
		}
	if ( sim_start_hr > FIRE_TIMER_MAX_HOUR_DAY || sim_end_hr > FIRE_TIMER_MAX_HOUR_DAY )	{
		ERR_ERROR_CONTINUE("Unable to initialize FireTimer, hour not in range 0 to 23. \n", ERR_ESANITY);
		return ft;
		}	
	if ( (ft = (FireTimer *) malloc(sizeof(FireTimer))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize FireTimer, memory allocation failed. \n", ERR_ENOMEM);
		return ft;	
		}

	/* simulation start time */
	ft->sim_start_yr 	= sim_start_yr;
	ft->sim_start_mo 	= sim_start_mo;
	ft->sim_start_dy 	= sim_start_dy;
	ft->sim_start_hr 	= sim_start_hr;
	/* simulation end time */	
	ft->sim_end_yr		= sim_end_yr;
	ft->sim_end_mo		= sim_end_mo;
	ft->sim_end_dy		= sim_end_dy;		
	ft->sim_end_hr		= sim_end_hr;
	/* current simulation time */	
	ft->sim_cur_yr		= sim_start_yr;
	ft->sim_cur_mo		= sim_start_mo;	
	ft->sim_cur_dy		= sim_start_dy;
	ft->sim_cur_hr		= sim_start_hr;
	ft->sim_cur_secs	= FIRE_TIMER_HOUR_TO_SECS(sim_start_hr);
	
	return ft;		
	}

int FireTimerIncrementSeconds(FireTimer * ft, int secs)	{	
	if ( ft == NULL )	{
		ERR_ERROR("Unable to increment simulation seconds, FireTimer not initialized. \n", ERR_EINVAL);		
		}
	ft->sim_cur_secs += secs;
	/* test if hour has been exceeded */
	if ( FIRE_TIMER_SECS_TO_HOUR(ft->sim_cur_secs) > ft->sim_cur_hr )	{
		ft->sim_cur_hr += 1;
		/* test if day has been exceeded */
		if ( ft->sim_cur_hr > FIRE_TIMER_MAX_HOUR_DAY )	{
			ft->sim_cur_secs -= FIRE_TIMER_SECS_PER_DAY;		
			ft->sim_cur_hr = FIRE_TIMER_MIN_HOUR_DAY;
			/* if month has been exceeded, and increment mo and dy */
			if ( FireTimerGetDaysInMonth(ft->sim_cur_mo) == ft->sim_cur_dy)	{
				ft->sim_cur_mo += 1;
				ft->sim_cur_dy = 1;
				/* test if year has been exceeded */
				if ( ft->sim_cur_mo > FIRE_TIMER_MAX_MO_YEAR )	{
					ft->sim_cur_yr += 1;
					ft->sim_cur_mo = FIRE_TIMER_MIN_MO_YEAR;
					ft->sim_cur_dy = 1;
					ft->sim_cur_hr = FIRE_TIMER_MIN_HOUR_DAY;
					}
				}
			/* only increment day */
			else	{
				ft->sim_cur_dy += 1;
				}
			}
		}
	
	return ERR_SUCCESS;
	}
	
int FireTimerIsSimCurYearTimeExpired(FireTimer * ft)	{
	if ( ft->sim_cur_mo < ft->sim_end_mo )
		return 0;
	if ( ft->sim_cur_dy < ft->sim_end_dy )
		return 0;
	if ( ft->sim_cur_hr < ft->sim_end_hr )
		return 0;
	return 1;
	}
	
int FireTimerIsSimTimeExpired(FireTimer * ft)	{
	if ( ft->sim_cur_yr > ft->sim_end_yr )
		return 1;
	return 0;
	}

int FireTimerGetDaysInMonth(int mo)		{
	return sdays_in_month[mo - 1];
	}

int FireTimerGetDaysDifftime(int mo1, int dy1, int mo2, int dy2)	{
	int days_diff = 0;
	
	/* number of days in first month */
	days_diff += FireTimerGetDaysInMonth(mo1) - dy1;
	mo1++;
	
	/* number of days not in first or last months */	
	for( ; mo1 < mo2 && mo1 <=FIRE_TIMER_MAX_MO_YEAR; mo1++)	{
		days_diff += FireTimerGetDaysInMonth(mo1);
		}
	
	/* nunber of days in last month */
	days_diff += dy2;
	
	return days_diff;
	}

double FireTimerGetSecondsDifftime(FireTimer * ft0, FireTimer * ft1) {
  struct tm t0, t1;

  t0.tm_year  = ft0->sim_cur_yr - 1900;
  t0.tm_mon   = ft0->sim_cur_mo - 1;
  t0.tm_mday  = ft0->sim_cur_dy;
  t0.tm_hour  = ft0->sim_cur_hr;

  t1.tm_year  = ft1->sim_cur_yr - 1900;
  t1.tm_mon   = ft1->sim_cur_mo - 1;
  t1.tm_mday  = ft1->sim_cur_dy;
  t1.tm_hour  = ft1->sim_cur_hr;

  return difftime(mktime(&t1), mktime(&t0));
}

 void FreeFireTimer(FireTimer * fs)	{
 	if ( fs != NULL )	{
 		free(fs);
 		}
 	
 	fs = NULL;
 	return;	
 	}
	
/* end of FireTimer.c */
