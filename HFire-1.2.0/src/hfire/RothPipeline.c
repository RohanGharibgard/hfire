/*! 
 * \file RothPipeline.c
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
 
#include "RothPipeline.h"

RothPipeline * InitRothPipelineEmpty()	{
	RothPipeline * rp = NULL;
	
	if ( (rp = (RothPipeline *) malloc(sizeof(RothPipeline))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for RothPipeline. \n", ERR_ENOMEM);
		return rp;
		}
		
	if ( RothPipelineSetDefaultValues(rp) )	{
		ERR_ERROR_CONTINUE("Initialization of RothPipeline failed. \n", ERR_EBADFUNC);
		}
	
	return rp;
	}

int RothPipelineSetDefaultValues(RothPipeline * rp)	{
	if ( rp == NULL )	{
		ERR_ERROR("RothPipeline not initialized, unable to set default values. \n", ERR_EINVAL);
		}

    rp->pipe = EnumInitPipe;
    rp->drx = rp->lrx = 0.0;
    rp->fdead = rp->lmex = rp->taur = 0.0;
    rp->ppflux = rp->slp_k = rp->wnd_b = rp->wnd_e = rp->wnd_k = 0.0;
    rp->d1hfm = rp->d10hfm = rp->d100hfm = rp->lhfm = rp->lwfm = 0.0;
    rp->wnd_fpm = rp->slp = rp->wnd_vec = rp->asp = 0.0;
    /* rp->wnd_fpm = rp->slp = -1.0; */
    rp->rxint = rp->ros_0 = rp->hpua = 0.0;
    rp->ros_max = rp->ros_az_max = rp->wnd_eff = 0.0;
    rp->lwratio = 1.0;
    rp->eccen = 0.0;
    rp->phi_w = rp->phi_s = rp->phi_ew = 0.0;
    rp->wnd_lim = ROTH_PIPELINE_NO_WIND_LIMIT;
	rp->ros_any = rp->ros_az_any = 0.0;
		
	return ERR_SUCCESS;
	}

void FreeRothPipeline(void * vptr)	{
	RothPipeline * rp = NULL;
 	if ( vptr != NULL )	{
 		rp = (RothPipeline *) vptr;
 		if ( rp != NULL )	{
 			free(rp);
 			}
 		}
 	rp = NULL;
 	return;	
 	}
 	
/* end of RothPipeline.c */
