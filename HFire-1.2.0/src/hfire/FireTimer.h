/*! 
 * \file FireTimer.h
 * \brief Facility for storing and updating simulation clock.
 *  
 * \sa Check the \htmlonly <a href="config_file_doc.html#SIMULATION">config file documentation</a> \endhtmlonly 
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
 
#ifndef FireTimer_H
#define FireTimer_H

#include <stdlib.h>

#include "Err.h"
	
/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */
 
/*! \def FIRE_TIMER_SECS_PER_MIN
 * 	\brief number of seconds per minute 
 */
#define FIRE_TIMER_SECS_PER_MIN							(60)
 
/*! \def FIRE_TIMER_SECS_PER_HOUR
 *	\brief number of seconds per hour 
 */
#define FIRE_TIMER_SECS_PER_HOUR						(3600)

/*!	\def FIRE_TIMER_SECS_PER_DAY
 *	\brief number of seconds in a day 
 */
#define FIRE_TIMER_SECS_PER_DAY							((FIRE_TIMER_SECS_PER_HOUR) * 24)

/*!	\def FIRE_TIMER_MIN_HOUR_DAY
 *	\brief minimum hour in a day 
 */
#define FIRE_TIMER_MIN_HOUR_DAY							(0)

/*!	\def FIRE_TIMER_MAX_HOUR_DAY
 *	\brief maximum hour in a day 
 */
#define FIRE_TIMER_MAX_HOUR_DAY							(23)

/*! \def FIRE_TIMER_MIN_MO_YEAR
 *	\brief minimum month in a year 
 */
#define FIRE_TIMER_MIN_MO_YEAR							(1)

/*! \def FIRE_TIMER_MAX_MO_YEAR
 *	\brief maximumm month in a year 
 */
#define FIRE_TIMER_MAX_MO_YEAR							(12)

/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/*! Type name for FireTimer_ 
 *	\sa For a list of members goto FireTimer_
 */
typedef struct FireTimer_ FireTimer;

/*! \struct FireTimer_ FireTimer.h "FireTimer.h"
 *	\brief structure used to store information associated with simulation clock
 */
struct FireTimer_	{
	/*! simulation start time, absolute year eg not offset from 1900 */
	int sim_start_yr;								
	/*! simulation start time, 1-based month index, eg Jan=1 not 0 */	
	int sim_start_mo;
	/*! simulation start time,1-based day index, eg first day mo=1 */									
	int sim_start_dy;									
	/*! simulation start time, 1-based hour index, eg noon=12 not 11 */
	int sim_start_hr;									
	/*! simulation end time, absolute year eg not offset from 1900 */	
	int sim_end_yr;
	/*! simulation end time, 1-based month index, eg Jan=1 not 0 */
	int sim_end_mo;	
	/*! simulation end time, 1-based day index, eg first day mo=1 */
	int sim_end_dy;	
	/*! simulation end time, 1-based hour index, eg noon=12 not 11 */
	int sim_end_hr;	
	/*! current simulation time, absolute year eg not offset from 1900 */	
	int sim_cur_yr;
	/*! current simulation time, 1-based month index, eg Jan=1 not 0 */
	int sim_cur_mo;
	/*! current simulation time, 1-based day index, eg first day mo=1 */
	int sim_cur_dy;	
	/*! current simulation time, 1-based hour index, eg noon=12 not 11 */
	int sim_cur_hr;	
	/*! current simulation time, never exceeds FIRE_TIMER_SECS_PER_DAY */
	int sim_cur_secs;
	};
		 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */

/*! \def FIRE_TIMER_HOUR_TO_SECS(hr)
 *	\brief returns the number of seconds in the hour supplied as argument 
 */
#define FIRE_TIMER_HOUR_TO_SECS(hr)						((hr) * FIRE_TIMER_SECS_PER_HOUR)

/*!	\def FIRE_TIMER_MIN_TO_SECS(min)
 * 	\brief returns the number of hours in the seconds supplied as argument 
 */
#define FIRE_TIMER_SECS_TO_HOUR(secs)					((secs) / FIRE_TIMER_SECS_PER_HOUR)

/*!	\def FIRE_TIMER_MIN_TO_SECS(min)
 * 	\brief returns the number of seconds in the minutes supplied as argument 
 */
#define FIRE_TIMER_MIN_TO_SECS(min)						((min) * FIRE_TIMER_SECS_PER_MIN)

/*!	\def FIRE_TIMER_SECS_TO_MIN(secs)
 * 	\brief returns the number of minutes in the seconds supplied as argument 
 */
#define FIRE_TIMER_SECS_TO_MIN(secs)					((secs) / FIRE_TIMER_SECS_PER_MIN)

/*!	\def FIRE_TIMER_SECS_TO_END_HOUR(secs)
 * 	\brief returns the number of seconds remaining in the hour 
 */
#define FIRE_TIMER_SECS_TO_END_HOUR(secs)				(FIRE_TIMER_SECS_PER_HOUR - (secs))

/*! \def FIRE_TIMER_GET_MILITARY_TIME(ft)
 * 	\brief returns a four digit military-time style representation of the current time 
 */
#define FIRE_TIMER_GET_MILITARY_TIME(ft)				((((ft)->sim_cur_hr) * 100) + (FIRE_TIMER_SECS_TO_MIN((ft)->sim_cur_secs) % 60))

 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

/*! \fn FireTimer * InitFireTimer(int sim_start_yr, int sim_start_mo, int sim_start_dy, int sim_start_hr, int sim_end_yr, int sim_end_mo, int sim_end_dy, int sim_end_hr)
 * 	\brief Allocates memory associated with FireTimer structure and initializes members.
 *
 * 	\sa FireTimer
 *	\sa Check the \htmlonly <a href="config_file_doc.html#SIMULATION">config file documentation</a> \endhtmlonly
 * 	\param sim_start_yr year to start simulation (not offset from 1900)
 * 	\param sim_start_mo month to start simulation (1-based month index)
 * 	\param sim_start_dy day to start simulation (1-based day index)
 * 	\param sim_start_hr hour to start simulation (1-based hour index)
 * 	\param sim_end_yr year to end simulation (not offset from 1900)
 * 	\param sim_end_mo month to end simulation (1-based month index)
 * 	\param sim_end_dy day to end simulation (1-based day index)
 * 	\param sim_end_hr hour to end simulation (1-based hour index)
 * 	\retval FireTimer* Ptr to initialized FireTimer
 */	 
FireTimer * InitFireTimer(int sim_start_yr, int sim_start_mo, int sim_start_dy, int sim_start_hr,
						  int sim_end_yr, int sim_end_mo, int sim_end_dy, int sim_end_hr);

/*! \fn int FireTimerIncrementSeconds(FireTimer * ft, int secs)
 * 	\brief Increments the current time stored in the FireTimer by the number of seconds supplied as argument.
 *
 * 	Propagates necessary changes into hr, dy, mo, and year fields if necessary.
 *	\sa FireTimer
 *	\sa Check the \htmlonly <a href="config_file_doc.html#SIMULATION">config file documentation</a> \endhtmlonly 
 * 	\param ft ptr to FireTimer
 *	\param secs number of seconds to increment the simulation clock
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int FireTimerIncrementSeconds(FireTimer * ft, int secs);

/*!	\fn int FireTimerIsSimCurYearTimeExpired(FireTimer * ft)
 *	\brief Returns 1 if the current simulation time has reached the end of the fire season for this year.
 *
 *	For multiple-year simulations the simulation will begin at the start of the fire season during the next year.
 *	\sa FireTimer
 *	\sa Check the \htmlonly <a href="config_file_doc.html#SIMULATION">config file documentation</a> \endhtmlonly  
 *	\retval int 1 if current simulation year expired, 0 otherwise
 */
int FireTimerIsSimCurYearTimeExpired(FireTimer * ft);

/*! \fn int FireTimerIsSimTimeExpired(FireTimer * ft)
 *	\brief Returns 1 if the current simulation time has reached the simulation end time.
 *	\sa FireTimer
 *	\sa Check the \htmlonly <a href="config_file_doc.html#SIMULATION">config file documentation</a> \endhtmlonly  
 *	\retval int 1 if simulation time expired, 0 otherwise
 */
int FireTimerIsSimTimeExpired(FireTimer * ft);

/*!	\fn int FireTimerGetDaysInMonth(int mo)
 *	\brief Returns the number of days in the month supplied as an argument.
 *	\sa FireTimer
 *	\sa Check the \htmlonly <a href="config_file_doc.html#SIMULATION">config file documentation</a> \endhtmlonly  
 *	\param mo 1-based month
 *	\retval int number of days in month
 */
int FireTimerGetDaysInMonth(int mo);

/*!	\fn int FireTimerGetDaysDifftime(int mo1, int dy1, int mo2, int dy2)
 *	\brief Calculates the number of days between the first and second dates supplied as arguments.
 *	\sa FireTimer
 *	\sa Check the \htmlonly <a href="config_file_doc.html#SIMULATION">config file documentation</a> \endhtmlonly  
 *	\param mo1 1-based month
 *	\param dy1 1-based day
 *	\param mo2 1-based month
 *	\param dy2 1-based day 
 *	\retval int number of days difference
 */
int FireTimerGetDaysDifftime(int mo1, int dy1, int mo2, int dy2);

double FireTimerGetSecondsDifftime(FireTimer * ft0, FireTimer * ft1);

/*! \fn void FreeFireTimer(FireTimer * fs)
 * 	\brief Frees memory associated with FireTimer structure.
 *
 * 	Subsequent calls to methods taking FireTimer as argument will not work.
 *	\sa FireTimer
 *	\sa Check the \htmlonly <a href="config_file_doc.html#SIMULATION">config file documentation</a> \endhtmlonly  
 * 	\param fs ptr to FireTimer
 */
void FreeFireTimer(FireTimer * fs); 
 
#endif FireTimer_H		/* end of FireTimer.h */
