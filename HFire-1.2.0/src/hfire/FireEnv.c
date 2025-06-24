/*! 
 * \file FireEnv.c
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
 
#include "FireEnv.h"

FireEnv * InitFireEnv()		{
	FireEnv * fe 					= NULL;
		
	if ( (fe = (FireEnv *) malloc(sizeof(FireEnv))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize FireEnv, memory allocation failed. \n", ERR_ENOMEM);
		return fe;	
		}
	/* set all function pointers to NULL */
	fe->GetFuelsRegrowthFromProps	= NULL;
	fe->IsIgnitionNowFromProps		= NULL;
	fe->GetIgnitionLocFromProps		= NULL;
	fe->GetWindAzimuthFromProps		= NULL;
	fe->GetWindSpeedMpsFromProps	= NULL;
	fe->GetDeadFuelMoistFromProps	= NULL;
	fe->GetLiveFuelMoistFromProps	= NULL;	
	fe->IsSantaAnaNowFromProps		= NULL;
	fe->GetSantaAnaEnvFromProps		= NULL;

	return fe; 
	}

void FreeFireEnv(FireEnv * fe)	{
	if ( fe != NULL )	{
		free(fe);
		}
	fe = NULL;
	return;
	}
	 
/* end of FireEnv.c */
