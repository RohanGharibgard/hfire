/*! 
 * \file Extinction.c
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
 
#include "Extinction.h"

/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */

int IsExtinguishedROS(ChHashTable * proptbl, double mpsros);

int IsExtinguishedHOURS(ChHashTable * proptbl, unsigned char hours);

/*
 * Visibility:
 * global
 *
 * Description:
 * Tests whether the rate of spread supplied as an argument is less than the extinction threshold.
 * This method returns a 1 if the mpsros is less than than the simulation property 
 * FIRE_EXTINCTION_ROS_MPS.  If the property FIRE_EXTINCTION_ROS_MPS is NULL or the mpsros is 
 * greater or equal, then this method returns 0.
 *
 * Arguments:
 * proptbl- HashTable of simulation properties
 * mpsros- rate of spread, in meters per second, being tested
 *
 * Returns:
 * Returns 1 (true) if mpsros less than threshold, or 0 (false) otherwise.
 */	
int IsExtinguishedROS(ChHashTable * proptbl, double mpsros)		{
	/* static variables used to store state across function calls */
	static int ext_ros_enabled	= -1;
	static double ext_mpsros 	= -1.0;
	/* stack variables */
	KeyVal * entry	= NULL;									/* key/val entries from table */
	
	/* check args */
	if ( proptbl == NULL || ext_ros_enabled == 0 )	{
		return 0;
		}
	/* determine if extinction threshold enabled, only done once */
	if ( ext_ros_enabled == -1 )	{
		if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_FEXROS), (void *)&entry) )	{
			ERR_ERROR_CONTINUE("Unable to retrieve FIRE_EXTINCTION_ROS_MPS property. \n", ERR_EINVAL);
			ext_ros_enabled = 0;
			return 0;
			}
		if ( strcmp(entry->val, GetFireVal(VAL_NULL)) == 0)	{
			/* disable extinction via this mechanism */
			ext_ros_enabled = 0;
			return 0;
			}
		ext_ros_enabled = 1;
		ext_mpsros = atof(entry->val);
		}
	/* test if extinction occurs */
	if ( mpsros < ext_mpsros )	{
		return 1;
		}
		
	return 0;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Tests whether the hours supplied as an argument are greater than or equal to the extinction
 * threshold. This method returns a 1 if the hours exceed or equal to simulation property
 * FIRE_EXTINCTION_HOURS.  If the property FIRE_EXTINCTION_HOURS is NULL or the hours are less
 * than the threshold, then this method returns 0.
 *
 * Arguments:
 * proptbl- HashTable of simulation properties
 * hours- hours being tested
 *
 * Returns:
 * Returns 1 (true) if hours greater than or equal to threshold, or 0 (false) otherwise.
 */	
int IsExtinguishedHOURS(ChHashTable * proptbl, unsigned char hours)	{
	/* static variables used to store state across function calls */
	static int ext_hrs_enabled		= -1;
	static int ext_hrs 				= -1;
	/* stack variables */
	KeyVal * entry	= NULL;									/* key/val entries from table */
	
	/* check args */
	if ( proptbl == NULL || ext_hrs_enabled == 0 )	{
		return 0;
		}
	/* determine if extinction threshold enabled, only done once */
	if ( ext_hrs_enabled == -1 )	{
		if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_FEXHR), (void *)&entry) )	{
			ERR_ERROR_CONTINUE("Unable to retrieve FIRE_EXTINCTION_HOURS property. \n", ERR_EINVAL);
			ext_hrs_enabled = 0;
			return 0;
			}
		if ( strcmp(entry->val, GetFireVal(VAL_NULL)) == 0)	{
			/* disable extinction via this mechanism */
			ext_hrs_enabled = 0;
			return 0;
			}
		ext_hrs_enabled = 1;
		ext_hrs = atoi(entry->val);
		}
	/* test if extinction occurs */
	if ( (int)hours >= ext_hrs )	{
		return 1;
		}

	return 0;	
	}

int UpdateExtinctionHOURS(ChHashTable * proptbl, int month, int day, int hour, CellState * cs, ByteTwoDArray * hrs_brn){
	/* static variables used to store state across function calls */
	static EnumExtinctionType ext_type 	= EnumExtinctionUnknown;
	static int smonth 					= 0;
	static int sday 					= 0;
	static int shour 					= 0;
	/* stack variables */
	KeyVal * entry	= NULL;									/* key/val entries from table */
	int domain_rows, domain_cols;
	int i,j;
	
	/* check args */
	if ( proptbl == NULL || cs == NULL || hrs_brn == NULL )		{
		ERR_ERROR("Unable to update extinction using hours criteria. \n", ERR_EINVAL);
		}

	/* set extinction type, only done once */
	if ( ext_type == EnumExtinctionUnknown )	{
		if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_FEXTYP), (void *)&entry) )	{
			ERR_ERROR("Unable to retrieve FIRE_EXTINCTION_TYPE property. \n", ERR_EINVAL);
			}
		if ( strcmp(entry->val, GetFireVal(VAL_CONS)) == 0)	{
			ext_type = EnumExtinctionConsume;
			}
		else if ( strcmp(entry->val, GetFireVal(VAL_REIG)) == 0)	{
			ext_type = EnumExtinctionReignite;
			}
		else	{
			ERR_ERROR("Unrecognized FIRE_EXTINCTION_TYPE property. \n", ERR_EINVAL);
			}
		}

	/* determine if at least one hour has passed since last call to this function */
	if ( (smonth != month) || (sday != day) || (shour != hour) )	{
		/* get domain dimensions */
		domain_rows = BYTETWODARRAY_SIZE_ROW(cs->state);
		domain_cols = BYTETWODARRAY_SIZE_COL(cs->state);
	
		/* increment extinction clock */
		for(i = 0; i < domain_rows; i++)	{
			for(j = 0; j < domain_cols; j++)	{
				if ( BYTETWODARRAY_GET_DATA(cs->state, i, j) == EnumHasFireCellState )	{ 
					/* increment the hrs_brned */
					BYTETWODARRAY_SET_DATA(hrs_brn, i, j, (BYTETWODARRAY_GET_DATA(hrs_brn, i, j) + 1));
					/* extinguish the cell if necessary */
					if ( IsExtinguishedHOURS(proptbl, BYTETWODARRAY_GET_DATA(hrs_brn, i, j)) )	{
						BYTETWODARRAY_SET_DATA(hrs_brn, i, j, 0);
						switch(ext_type)	{
							case EnumExtinctionConsume:
								/* cell cannot burn again this year */
								BYTETWODARRAY_SET_DATA(cs->state, i, j, EnumUnBurnableCellState);
								break;
							case EnumExtinctionReignite:
								/* cell can burn again this year */
								BYTETWODARRAY_SET_DATA(cs->state, i, j, EnumNoFireCellState);
								break;
							default:
								ERR_ERROR("Unable to determine FIRE_EXTINCTION_TYPE property. \n", ERR_ESANITY);
								break;
							}
						}
					}
				}
			}
	
		/* set {month, day, hour} for future calls */
		smonth = month;
		sday = day;
		shour = hour;
		}
		
	return ERR_SUCCESS;				
	}

int UpdateExtinctionROS(ChHashTable * proptbl, int i, int j, double mpsros, CellState * cs, ByteTwoDArray * hrs_brn){
	/* static variables used to store state across function calls */
	static EnumExtinctionType ext_type 	= EnumExtinctionUnknown;
	/* stack variables */
	KeyVal * entry	= NULL;									/* key/val entries from table */
	
	/* check args */
	if ( proptbl == NULL || cs == NULL || hrs_brn == NULL )		{
		ERR_ERROR_CONTINUE("Unable to update extinction using hours criteria. \n", ERR_EINVAL);
		return 0;
		}

	/* set extinction type, only done once */
	if ( ext_type == EnumExtinctionUnknown )	{
		if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_FEXTYP), (void *)&entry) )	{
			ERR_ERROR_CONTINUE("Unable to retrieve FIRE_EXTINCTION_TYPE property. \n", ERR_EINVAL);
			}
		if ( strcmp(entry->val, GetFireVal(VAL_CONS)) == 0)	{
			ext_type = EnumExtinctionConsume;
			}
		else if ( strcmp(entry->val, GetFireVal(VAL_REIG)) == 0)	{
			ext_type = EnumExtinctionReignite;
			}
		else	{
			ERR_ERROR_CONTINUE("Unrecognized FIRE_EXTINCTION_TYPE property. \n", ERR_EINVAL);
			return 0;
			}
		}

	/* extinguish the cell */
	if ( IsExtinguishedROS(proptbl, mpsros) )	{
		BYTETWODARRAY_SET_DATA(hrs_brn, i, j, 0);
		switch(ext_type)	{
			case EnumExtinctionConsume:
				/* cell cannot burn again this year */
				BYTETWODARRAY_SET_DATA(cs->state, i, j, EnumUnBurnableCellState);
				break;
			case EnumExtinctionReignite:
				/* cell can burn again this year */
				BYTETWODARRAY_SET_DATA(cs->state, i, j, EnumNoFireCellState);
				break;
			default:
				ERR_ERROR_CONTINUE("Unable to determine FIRE_EXTINCTION_TYPE property. \n", ERR_ESANITY);
				break;
			}
		return 1;
		}

	return 0;							
	}
	 
/* end of Extinction.c */
