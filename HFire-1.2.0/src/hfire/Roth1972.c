/*! 
 * \file Roth1972.c
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
 
#include "Roth1972.h"

#define ROTH_1972_WTG_CLASSES			(6)
static const int   TimeLagClass			[ROTH_1972_WTG_CLASSES] = {0, 0, 1, 1, 2, 2};
static const float WtgSzClassEnglish	[ROTH_1972_WTG_CLASSES] = {1200.0,  192.0,   96.0,   48.0,  16.0,  0.0};
static const float WtgSzClassMetric		[ROTH_1972_WTG_CLASSES] = {3633.61, 581.37, 290.68, 145.34, 48.44, 0.0};

int Roth1972FireSpreadSetFuelBed(RothFuelModel * rfm)	{
	double lload, 	dload;
	double lhc, 	dhc;
	double lseff, 	dseff;
	double letas, 	detas;

	double flive, beta_opt, ratio, aa, sigma_15, gamma, gamma_max, c, e;

	/* check args */
	if ( rfm == NULL ) 	{
		ERR_ERROR("RothFuelModel not initialized, FuelBed step in FireSpread Pipeline failed. \n", ERR_EINVAL);
		}
	if ( rfm->rp == NULL )	{
		ERR_ERROR("RothPipeline not initialized, FuelBed step in FireSpread Pipeline failed. \n", ERR_EINVAL);
		}
	
	/* return if RothFuelModel represents unburnable fuel */	
	if ( rfm->brntype == EnumRothUnBurnable )	{
		return ERR_SUCCESS;
		}
	
	/* reset pipeline to defualt state */
	if ( RothPipelineSetDefaultValues(rfm->rp) )	{
		ERR_ERROR("FuelBed step in FireSpread Pipeline failed. \n", ERR_EBADFUNC);
		}
	
	/* set pipeline state */
	rfm->rp->pipe = EnumSetFuelBedPipe;
		
	/* all calculations done in ENGLISH units */
	if ( rfm->units == EnumMetricUnits )	{
		if ( RothFuelModelMetricToEnglish(rfm) )	{
			ERR_ERROR("FuelBed step in FireSpread Pipeline failed. \n", ERR_EBADFUNC);
			}
		}
	
    /* if fuel bed has no surface area go no further */
	if ( UNITS_FP_IS_ZERO(rfm->larea + rfm->darea) )	{
		return ERR_SUCCESS;
		}

	/* initialize scratch vars */
	flive = beta_opt = ratio = aa = sigma_15 = gamma = gamma_max = c = e = 0.0;
			
	/* apply weights to generate fuel bed properties */
	lload =	(rfm->awtg[EnumLHSizeClass] * rfm->load[EnumLHSizeClass] * (1.0 - rfm->stot[EnumLHSizeClass])) +
			(rfm->awtg[EnumLWSizeClass] * rfm->load[EnumLWSizeClass] * (1.0 - rfm->stot[EnumLWSizeClass]));
	dload =	(rfm->awtg[EnumD1HSizeClass] * rfm->load[EnumD1HSizeClass] * (1.0 - rfm->stot[EnumD1HSizeClass])) +
			(rfm->awtg[EnumD10HSizeClass] * rfm->load[EnumD10HSizeClass] * (1.0 - rfm->stot[EnumD10HSizeClass])) +
			(rfm->awtg[EnumD100HSizeClass] * rfm->load[EnumD100HSizeClass] * (1.0 - rfm->stot[EnumD100HSizeClass]));
			
	lhc =	(rfm->awtg[EnumLHSizeClass] * rfm->hc[EnumLHSizeClass]) +
			(rfm->awtg[EnumLWSizeClass] * rfm->hc[EnumLWSizeClass]);
	dhc =	(rfm->awtg[EnumD1HSizeClass] * rfm->hc[EnumD1HSizeClass]) +
			(rfm->awtg[EnumD10HSizeClass] * rfm->hc[EnumD10HSizeClass]) +
			(rfm->awtg[EnumD100HSizeClass] * rfm->hc[EnumD100HSizeClass]);
			
	lseff =	(rfm->awtg[EnumLHSizeClass] * rfm->seff[EnumLHSizeClass]) +
			(rfm->awtg[EnumLWSizeClass] * rfm->seff[EnumLWSizeClass]);
	dseff =	(rfm->awtg[EnumD1HSizeClass] * rfm->seff[EnumD1HSizeClass]) +
			(rfm->awtg[EnumD10HSizeClass] * rfm->seff[EnumD10HSizeClass]) +
			(rfm->awtg[EnumD100HSizeClass] * rfm->seff[EnumD100HSizeClass]);

	/* etas */
	letas = 1.0;
	if ( UNITS_FP_GT_ZERO(lseff) )	{
		letas = 0.174 / pow(lseff, 0.19);
		if ( letas > 1.0 )	{
			letas = 1.0;
			}
		}
	detas = 1.0;
	if ( UNITS_FP_GT_ZERO(dseff) )	{
		detas = 0.174 / pow(dseff, 0.19);
		if ( detas > 1.0 )	{
			detas = 1.0;
			}
		}
		
	/* rx factors */
	rfm->rp->lrx = lload * lhc * letas;	
	rfm->rp->drx = dload * dhc * detas;
	
	/* residence time */
	rfm->rp->taur = 384.0 / rfm->fsav;
	
	/* propagating flux */
	rfm->rp->ppflux = exp((0.792 + 0.681 * sqrt(rfm->fsav)) * (rfm->pkrat + 0.1)) / (192.0 + 0.2595 * rfm->fsav);
	
    /* gamma */
    beta_opt = 3.348 / (pow(rfm->fsav, 0.8189));
    ratio = rfm->pkrat / beta_opt;
    aa = 133.0 / (pow(rfm->fsav, 0.7913));
    sigma_15 = pow(rfm->fsav, 1.5);
    gamma_max = sigma_15 / (495.0 + 0.0594 * sigma_15);
    gamma = gamma_max * pow(ratio, aa) * exp(aa * (1.0 - ratio));	

	/* factor gamma into rx factors */
	rfm->rp->lrx *= gamma;
	rfm->rp->drx *= gamma;
	
	/* slope and wind intermediaries */
	rfm->rp->slp_k = 5.275 * pow(rfm->pkrat, -0.3);
	rfm->rp->wnd_b = 0.02526 * pow(rfm->fsav, 0.54);
    c = 7.47 * exp(-0.133 * pow(rfm->fsav, 0.55));
    e = 0.715 * exp(-0.000359 * rfm->fsav);
    rfm->rp->wnd_k = c * pow(ratio, -e);
    rfm->rp->wnd_e = pow(ratio, e) / c;	
    
    /* if no live fuels in bed go no further */
    if ( UNITS_FP_IS_ZERO(lload) )	{
    	return ERR_SUCCESS;
    	}
    	
    /* live and dead fuel ratio and factor */
    if ( UNITS_FP_GT_ZERO(rfm->sav[EnumLHSizeClass]) )	{
    	flive += rfm->load[EnumLHSizeClass] * exp(-500.0 / rfm->sav[EnumLHSizeClass]);
    	}
    if ( UNITS_FP_GT_ZERO(rfm->sav[EnumLWSizeClass]) )	{
    	flive += rfm->load[EnumLWSizeClass] * exp(-500.0 / rfm->sav[EnumLWSizeClass]);
    	}
    rfm->rp->fdead = (rfm->load[EnumD1HSizeClass]  * rfm->effhn[EnumD1HSizeClass]) +
    				 (rfm->load[EnumD10HSizeClass] * rfm->effhn[EnumD10HSizeClass]) +
    				 (rfm->load[EnumD100HSizeClass] * rfm->effhn[EnumD100HSizeClass]);
    if ( UNITS_FP_GT_ZERO(flive) )	{
    	rfm->rp->lmex = 2.9 * rfm->rp->fdead / flive;
    	}    				 
				
	return ERR_SUCCESS;
	}

int Roth1972FireSpreadNoWindNoSlope(RothFuelModel * rfm, double d1hfm, double d10hfm, double d100hfm,
 														double lhfm, double lwfm)	{
	double letam, 	detam;
	double lm, 		dm;
	double lmex, 	dmex;
	 										
 	double wfmd, rbqig, fdmois, qig, ratio;
 	
 	int 	sz_cls	[EnumNumSizeClasses] = {0};
 	double 	tlag_cls[EnumNumSizeClasses] = {0.0};
 	int i, j;
 	
	/* check args */
	if ( rfm == NULL ) 	{
		ERR_ERROR("RothFuelModel not initialized, NoWindNoSlope step in FireSpread Pipeline failed. \n", ERR_EINVAL);
		}
	if ( rfm->rp == NULL )	{
		ERR_ERROR("RothPipeline not initialized, NoWindNoSlope step in FireSpread Pipeline failed. \n", ERR_EINVAL);
		}
	
	/* return if RothFuelModel represents unburnable fuel */	
	if ( rfm->brntype == EnumRothUnBurnable )	{
		return ERR_SUCCESS;
		}
	
	/* check and set pipeline state */
	if ( rfm->rp->pipe < EnumSetFuelBedPipe )	{
		ERR_ERROR("SetFuelBed step not complete, unable to execute NoWindNoSlope. \n", ERR_ESANITY);
		}
	rfm->rp->pipe = EnumNoWindNoSlopePipe;
	
	/* check for change in moisture */
	if ( (UNITS_FP_ARE_EQUAL(rfm->rp->d1hfm, d1hfm)) && (UNITS_FP_ARE_EQUAL(rfm->rp->d10hfm, d10hfm)) &&
		 (UNITS_FP_ARE_EQUAL(rfm->rp->d100hfm, d100hfm)) && (UNITS_FP_ARE_EQUAL(rfm->rp->lhfm, lhfm)) &&
		 (UNITS_FP_ARE_EQUAL(rfm->rp->lwfm, lwfm)) )	{
		return ERR_SUCCESS;
		}
	
	/* assign new moistures */
	rfm->rp->d1hfm = d1hfm;
	rfm->rp->d10hfm = d10hfm;
	rfm->rp->d100hfm = d100hfm;
	rfm->rp->lhfm = lhfm;
	rfm->rp->lwfm = lwfm;
	
	/* zero any previously set pipeline vars */
	rfm->rp->ros_0 = rfm->rp->hpua = rfm->rp->rxint = 0.0;
	rfm->rp->ros_max = rfm->rp->ros_az_max = 0.0;
	rfm->rp->ros_any = rfm->rp->ros_az_any = 0.0;

	/* initialize scratch vars */
 	wfmd = rbqig = fdmois = qig = ratio = 0.0;
	letam = detam = lm = dm = lmex = dmex = 0.0;

	/* accumulate moisture based upon size classes */
	for(i = 0; i < EnumNumSizeClasses; i++)		{
		for(j = 0; j < ROTH_1972_WTG_CLASSES; j++)	{
			if ( rfm->units == EnumEnglishUnits && rfm->sav[i] > WtgSzClassEnglish[j] )	{
				sz_cls[i] = j;
				break;
				}
			if ( rfm->units == EnumMetricUnits && rfm->sav[i] > WtgSzClassMetric[j] )	{
				sz_cls[i] = j;			
				break;
				}
			}
		}
	for(i = 0; i < EnumNumSizeClasses; i++)		{
		switch(TimeLagClass[sz_cls[i]])	{
			case 0:
				tlag_cls[i] = d1hfm;
				break;
			case 1:
				tlag_cls[i] = d10hfm;
				break;
			case 2:
				tlag_cls[i] = d100hfm;
				break;
			default:
				ERR_ERROR_CONTINUE("Unrecognized TimeLagClass, NoWindNoSlope in FireSpread Pipeline. \n", ERR_WARNING);
				tlag_cls[i] = 0.0;
				break;
			}		
		}
	wfmd = 	(tlag_cls[EnumD1HSizeClass] * rfm->effhn[EnumD1HSizeClass] * rfm->load[EnumD1HSizeClass]) +
			(tlag_cls[EnumD10HSizeClass] * rfm->effhn[EnumD10HSizeClass] * rfm->load[EnumD10HSizeClass]) +
			(tlag_cls[EnumD100HSizeClass] * rfm->effhn[EnumD100HSizeClass] * rfm->load[EnumD100HSizeClass]);
	rfm->fm[EnumD1HSizeClass] = tlag_cls[EnumD1HSizeClass];
	rfm->fm[EnumD10HSizeClass] = tlag_cls[EnumD10HSizeClass];
	rfm->fm[EnumD100HSizeClass] = tlag_cls[EnumD100HSizeClass];	
	rfm->fm[EnumLHSizeClass] = lhfm;
	rfm->fm[EnumLWSizeClass] = lwfm;
	
	/* live fuel extinction moisture */
	if ( UNITS_FP_GT_ZERO(rfm->load[EnumLHSizeClass]) || UNITS_FP_GT_ZERO(rfm->load[EnumLWSizeClass]) )	{
		if ( UNITS_FP_GT_ZERO(rfm->rp->fdead) )		{
			fdmois = wfmd / rfm->rp->fdead;
			}
		else	{
			fdmois = 0.0;
			}
		lmex = ((rfm->rp->lmex * (1.0 - fdmois / rfm->mex)) - 0.226);
		if ( lmex < rfm->mex )	{
			lmex = rfm->mex;
			}
		}
		
	/* dead fuel extinction moisture */		
	dmex = rfm->mex;

	/* accumulate category weighted moisture */
	for(i = 0; i < EnumNumSizeClasses; i++)	{
		qig = 250.0 + 1116.0 * rfm->fm[i];
		if ( i == EnumLHSizeClass || i == EnumLWSizeClass )	{
			rbqig += qig * rfm->awtg[i] * rfm->larea * rfm->effhn[i];
			}
		else	{
			rbqig += qig * rfm->awtg[i] * rfm->darea * rfm->effhn[i];
			}
		}
	rbqig *= rfm->fdens;
	lm = (rfm->awtg[EnumLHSizeClass] * rfm->fm[EnumLHSizeClass]) +
		 (rfm->awtg[EnumLWSizeClass] * rfm->fm[EnumLWSizeClass]);
	dm = (rfm->awtg[EnumD1HSizeClass] * rfm->fm[EnumD1HSizeClass]) +
		 (rfm->awtg[EnumD10HSizeClass] * rfm->fm[EnumD10HSizeClass]) +
		 (rfm->awtg[EnumD100HSizeClass] * rfm->fm[EnumD100HSizeClass]);

	/* reaction intensity contributed by live fuels */
	ratio = 0.0;
	if ( UNITS_FP_GT_ZERO(lmex) )	{
		ratio = lm / lmex;
		letam = 1.0 - 2.59 * ratio + 5.11 * ratio * ratio - 3.52 * ratio * ratio * ratio;
		}
	if ( lm >= lmex )	{
		letam = 0.0;
		}
	rfm->rp->rxint += rfm->rp->lrx * letam;
	
	/* reaction intensity contributed by dead fuels */
	ratio = 0.0;
	if ( UNITS_FP_GT_ZERO(dmex) )	{
		ratio = dm / dmex;
		detam = 1.0 - 2.59 * ratio + 5.11 * ratio * ratio - 3.52 * ratio * ratio * ratio;
		}
	if ( dm >= dmex )	{
		detam = 0.0;
		}
	rfm->rp->rxint += rfm->rp->drx * detam;
	
	/* heat per unit area */
	rfm->rp->hpua = rfm->rp->rxint * rfm->rp->taur;
	
	/* no wind no slope ros */
	if ( UNITS_FP_GT_ZERO(rbqig) )	{
		rfm->rp->ros_0 = rfm->rp->rxint * rfm->rp->ppflux / rbqig;
		}
	else	{
		rfm->rp->ros_0 = 0.0;
		}
		
	/* zero other ros vars */
	rfm->rp->ros_max = rfm->rp->ros_0;
	rfm->rp->ros_any = rfm->rp->ros_0;
	rfm->rp->ros_az_max = rfm->rp->ros_az_any = 0.0;
	
 	return ERR_SUCCESS;									
 	}
 	
int Roth1972FireSpreadWindSlopeMax(RothFuelModel * rfm, double wnd_fpm, double wnd_az, double slp_pcnt, double asp, double ell_adj)	{
    double upslp, az_max, phi_ew;
    double split_deg, split_rad;
    double slp_rate, wnd_rate, rv, spread_max;
    double x, y, al, a;
    double max_wnd, eff_wnd, lw_ratio, eccen;
    int do_eff_wnd, ck_wnd_lim, wnd_lim;
    
	/* check args */
	if ( rfm == NULL ) 	{
		ERR_ERROR("RothFuelModel not initialized, WindSlopeMax step in FireSpread Pipeline failed. \n", ERR_EINVAL);
		}
	if ( rfm->rp == NULL )	{
		ERR_ERROR("RothPipeline not initialized, WindSlopeMax step in FireSpread Pipeline failed. \n", ERR_EINVAL);
		}
	
	/* return if RothFuelModel represents unburnable fuel */	
	if ( rfm->brntype == EnumRothUnBurnable )	{
		return ERR_SUCCESS;
		}
	
	/* check and set pipeline state */
	if ( rfm->rp->pipe < EnumNoWindNoSlopePipe )	{
		ERR_ERROR("NoWindNoSlope step not complete, unable to execute WindSlopeMax. \n", ERR_ESANITY);
		}
	rfm->rp->pipe = EnumWindSlopeMaxPipe;

	/* convert slope to rise/run and check for change in slope */
	if ( UNITS_FP_LT_ZERO(slp_pcnt) )	{
		slp_pcnt = 0.0;
		}
	slp_pcnt /= 100.0;
	if ( !UNITS_FP_ARE_EQUAL(rfm->rp->slp, slp_pcnt) )	{
		rfm->rp->phi_s = rfm->rp->slp_k * slp_pcnt * slp_pcnt;
		rfm->rp->slp = slp_pcnt;
		}
		
	/* convert wind direction from 'out of' to 'to' */
	wnd_az = ((int)(wnd_az + 180.0)) % 360;
	
	/* check for change in windspeed */
	if ( !UNITS_FP_ARE_EQUAL(rfm->rp->wnd_fpm, wnd_fpm) )	{
		if ( UNITS_FP_GT_ZERO(wnd_fpm) )	{
			rfm->rp->phi_w = rfm->rp->wnd_k * pow(wnd_fpm, rfm->rp->wnd_b);
			}
		else	{
			rfm->rp->phi_w = 0.0;
			}
		rfm->rp->wnd_fpm = wnd_fpm;
		}
		
	/* combine wind and slope */
	phi_ew = rfm->rp->phi_s + rfm->rp->phi_w;
	wnd_lim = 0;
	lw_ratio = 1.0;
	eccen = 0.0;
	if (asp >= 180.0)	{
		upslp = asp - 180.0;
		}
	else	{
		upslp = asp + 180.0;
		}
		
	/* Situation 1: no fire spread or reaction intensity */
	if ( !UNITS_FP_GT_ZERO(rfm->rp->ros_0) )		{
		spread_max = az_max = 0.0;
        eff_wnd = 0.0;
        do_eff_wnd = ck_wnd_lim = 0;
        }
	/* Situation 2: no wind and no wind */
	else if ( !UNITS_FP_GT_ZERO(phi_ew) )	{
        phi_ew = eff_wnd = az_max = 0.0;
        spread_max = rfm->rp->ros_0;
        do_eff_wnd = ck_wnd_lim = 0;
		}
	/* Situation 3: wind with no slope */
	else if ( !UNITS_FP_GT_ZERO(slp_pcnt) )	{        
        eff_wnd = wnd_fpm;
		do_eff_wnd = 0;
		spread_max = rfm->rp->ros_0 * (1.0 + phi_ew);
		az_max = wnd_az;
        ck_wnd_lim = 1;
        }
	/* Situation 4: slope with no wind */
	else if ( !UNITS_FP_GT_ZERO(wnd_fpm) )	{
		spread_max = rfm->rp->ros_0 * (1.0 + phi_ew);
		az_max = upslp;
        do_eff_wnd = ck_wnd_lim = 1;
		}
	/* Situation 5: wind blows upslope */
	else if ( UNITS_FP_ARE_EQUAL(upslp, wnd_az) )	{        
		spread_max = rfm->rp->ros_0 * (1.0 + phi_ew);
		az_max = upslp;        
        do_eff_wnd = ck_wnd_lim = 1;        
        }
    /* Situation 6: wind blows cross slope */
    else	{
        /* recalculate spread rate in the optimal direction */
        if ( upslp <= wnd_az )	{
        	split_deg = wnd_az - upslp;
        	}
        else	{
        	split_deg = 360.0 - upslp + wnd_az;
        	}
		split_rad = ROTH_1972_DEG_TO_RAD(split_deg);
		slp_rate = rfm->rp->ros_0 * rfm->rp->phi_s;
		wnd_rate = rfm->rp->ros_0 * rfm->rp->phi_w;
		x = slp_rate + wnd_rate * cos(split_rad);
		y = wnd_rate * sin(split_rad);
		rv = sqrt( (x * x) + (y * y));
		spread_max = rfm->rp->ros_0 + rv;

        /* recalculate phi_ew in the optimal direction */
        phi_ew = spread_max / rfm->rp->ros_0 - 1.0;
        if ( UNITS_FP_GT_ZERO(phi_ew) )	{
        	do_eff_wnd = 1;
        	}
        else	{
        	do_eff_wnd = 0;
        	}
        ck_wnd_lim = 1;

        /* recalculate direction of maximum spread in azimuth degrees */
        al = asin(fabs(y) / rv);
        if ( x >= 0.0 )	{
        	if ( y >= 0.0 )	{
        		a = al;
        		}
        	else	{
        		a = ROTH_1972_PI + ROTH_1972_PI - al;
        		}
        	}
        else	{
        	if ( y >= 0.0 )	{
        		a = ROTH_1972_PI - al;
        		}
        	else	{
        		a = ROTH_1972_PI + al;
        		}
			}
        split_deg = ROTH_1972_RAD_TO_DEG(a);
        az_max = upslp + split_deg;
        if ( az_max > 360.0 )	{
            az_max -= 360.0;
            }
    	}
    	
    /* recalculate effective windspeed based upon phi_ew */
    if ( do_eff_wnd )	{
    	eff_wnd = pow((phi_ew * rfm->rp->wnd_e), (1.0 / rfm->rp->wnd_b));
    	}
    	
    /* if effective windspeed excedes max windspeed, scale back */
    if ( ck_wnd_lim )	{
    	max_wnd = 0.9 * rfm->rp->rxint;
    	if ( eff_wnd > max_wnd )	{
    		if ( !UNITS_FP_GT_ZERO(max_wnd) )	{
    			phi_ew = 0.0;
    			}
    		else	{
    			phi_ew = rfm->rp->wnd_k * pow(max_wnd, rfm->rp->wnd_b);
    			}
    		spread_max = rfm->rp->ros_0 * (1.0 + phi_ew);
    		eff_wnd = max_wnd;
    		wnd_lim = 1;
    		}
    	}
	
	/* determine fire ellipse parameters */
	if ( UNITS_FP_GT_ZERO(eff_wnd) )	{
		lw_ratio = 1.0 + 0.002840909 * ell_adj * eff_wnd;
		eccen = sqrt(lw_ratio * lw_ratio - 1.0) / lw_ratio;
		}
		
	/* store results */
	rfm->rp->asp = asp;
	rfm->rp->wnd_vec = wnd_az;
	rfm->rp->phi_ew = phi_ew;
	rfm->rp->wnd_eff = eff_wnd;
	rfm->rp->wnd_lim = wnd_lim;
	rfm->rp->ros_max = rfm->rp->ros_any = spread_max;
	rfm->rp->ros_az_max = rfm->rp->ros_az_any = az_max;
	rfm->rp->lwratio = lw_ratio;
	rfm->rp->eccen = eccen;
		
	return ERR_SUCCESS;
	}

int Roth1972FireSpreadGetAtAzimuth(RothFuelModel * rfm, double az)	{
	double dir_deg, dir_rad;
	
	/* check args */
	if ( rfm == NULL ) 	{
		ERR_ERROR("RothFuelModel not initialized, GetAtAzimuth step in FireSpread Pipeline failed. \n", ERR_EINVAL);
		}
	if ( rfm->rp == NULL )	{
		ERR_ERROR("RothPipeline not initialized, GetAtAzimuth step in FireSpread Pipeline failed. \n", ERR_EINVAL);
		}
	
	/* return if RothFuelModel represents unburnable fuel */	
	if ( rfm->brntype == EnumRothUnBurnable )	{
		return ERR_SUCCESS;
		}
	
	/* check and set pipeline state */
	if ( rfm->rp->pipe < EnumWindSlopeMaxPipe )	{
		ERR_ERROR("WindSlopeMax step not complete, unable to execute GetAtAzimuth. \n", ERR_ESANITY);
		}
	rfm->rp->pipe = EnumGetAtAzimuthPipe;

	/* no fire spread */
	if ( !UNITS_FP_GT_ZERO(rfm->rp->ros_max) )	{
		return ERR_SUCCESS;
		}
		
	/* combined wind slope factor is 0 or azimuth in direction of max spread */
	if ( !UNITS_FP_GT_ZERO(rfm->rp->phi_ew) || UNITS_FP_ARE_EQUAL(rfm->rp->ros_az_max, az) ) 	{
		rfm->rp->ros_any = rfm->rp->ros_max;
		}
	/* azimuth not in direction of max spread */
	else	{
		/* calculate angle btwn max ros and requested az */
		if ( (dir_deg = fabs(rfm->rp->ros_az_max - az)) > 180.0 )	{
			dir_deg = 360.0 - dir_deg;
			}
		dir_rad = ROTH_1972_DEG_TO_RAD(dir_deg);
		/* calculate ros in this direction */
		rfm->rp->ros_any = rfm->rp->ros_max * (1.0 - rfm->rp->eccen) / (1.0 - rfm->rp->eccen * cos(dir_rad));
		}

	/* store azimuth */		
	rfm->rp->ros_az_any = az;
	
	return ERR_SUCCESS;
	}
 
/* end of Roth1972.c */
