/*! 
 * \file RothFuelModel.c
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
#include "RothFuelModel.h"

static int RothFuelModelSetDerivedProperties(RothFuelModel * rfm);

RothFuelModel * InitRothFuelModelEmpty()	{
	RothFuelModel * rfm = NULL;
	int i;
	
	if ( (rfm = (RothFuelModel *) malloc(sizeof(RothFuelModel))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for RothFuelModel. \n", ERR_ENOMEM);
		return rfm;
		}
		
	rfm->brntype	= EnumRothUnBurnable;
	rfm->units		= EnumUnknownUnits;
	rfm->fdepth = rfm->mex = rfm->ros_adj = 0.0;
	rfm->num_fp = 0;
	rfm->fdens = rfm->fsav = rfm->pkrat = 0.0;
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		rfm->load[i] = rfm->sav[i] = rfm->dens[i] = rfm->hc[i] = rfm->stot[i] = rfm->seff[i] = 0.0;
		rfm->sarea[i] = rfm->effhn[i] = 0.0;
		rfm->awtg[i] = 0.0;
		rfm->larea = rfm->darea = 0.0;
		rfm->fm[i] = 0.0;
		}
	if ( (rfm->rp = InitRothPipelineEmpty()) == NULL )	{
		free(rfm);
		return rfm;
		}
		
	return rfm;
	}

RothFuelModel * InitRothFuelModelFMDFile(FILE * fstream, int model_num)	{
	RothFuelModel * rfm = NULL;
	int c, i = 0;
	char * line = NULL, * token = NULL;
	float fmd_vals[ROTH_FM_FMD_NUM_VALUES];
	
	if ( fstream == NULL )	{
		ERR_ERROR_CONTINUE("Stream to FMD file not open, RothFuelModel not retrieved. \n", ERR_EIOFAIL);
		return rfm;
		}
		
	if ( (rfm = InitRothFuelModelEmpty()) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for RothFuelModel. \n", ERR_ENOMEM);
		return rfm;
		}

	rfm->brntype = EnumRothBurnable;
	rfm->units = EnumEnglishUnits;
	
	/* retrieve fmd contents */
	while ( (c = fgetc(fstream)) != EOF )	{
		ungetc(c, fstream);
		/* test line is not empty */
		if ( (line = GetLineFStreamIO(fstream)) != NULL )		{
			/* test line is not comment and parse into tokens */
			if ( line[0] != *ROTH_FM_FMD_COMMENT_CHAR 
				 && (token = strtok(line, ROTH_FM_FMD_SEPARATOR_CHARS)) != NULL )		{
				/* ENGLISH UNITS */				
				if ( strcmp(token, ROTH_FM_FMD_KEYWORD_ENGLISH) == 0)	{
					rfm->units = EnumEnglishUnits;
					}
				/* METRIC UNITS */				
				else if ( strcmp(token, ROTH_FM_FMD_KEYWORD_METRIC) == 0)	{
					rfm->units = EnumMetricUnits;
					}
				/* FUEL VALUES */
				else if ( model_num == atoi(token) )	{
					/* model num matches, populate the attributes of the fuel model */						
					while ( (token = strtok(NULL, ROTH_FM_FMD_SEPARATOR_CHARS)) != NULL )	{
						fmd_vals[i] = atof(token);
						i++;
						}
					}				
				}
			free(line);
			}
		/* get another line in file */
		}

	/* test if attribute values retrieved */
	if ( i == 0 && rfm != NULL )	{
		ERR_ERROR_CONTINUE("Attribute values for model num not retrieved from .FMD file. \n", ERR_ERANGE);
		FreeRothFuelModel(rfm);
		rfm = NULL;
		return rfm;
		}

	/* populate static fuel bed properties */
	rfm->fdepth 					= (rfm->units == EnumEnglishUnits) ?
			fmd_vals[ROTH_FM_FMD_FBED_DEPTH] : UNITS_CM_TO_M(fmd_vals[ROTH_FM_FMD_FBED_DEPTH]);
	rfm->mex 						= fmd_vals[ROTH_FM_FMD_EXT_MOIST] / 100;
	rfm->ros_adj 					= ROTH_FM_ZERO_ROS_ADJ_FACTOR;

	/* fuel particle load */
	rfm->load[EnumD1HSizeClass] 	= (rfm->units == EnumEnglishUnits) ?
			fmd_vals[ROTH_FM_FMD_D1H_LOAD] 		: UNITS_MGPHA_TO_KGPMSQ(fmd_vals[ROTH_FM_FMD_D1H_LOAD]);
	rfm->load[EnumD10HSizeClass] 	= (rfm->units == EnumEnglishUnits) ?
			fmd_vals[ROTH_FM_FMD_D10H_LOAD] 	: UNITS_MGPHA_TO_KGPMSQ(fmd_vals[ROTH_FM_FMD_D10H_LOAD]);	
	rfm->load[EnumD100HSizeClass] 	= (rfm->units == EnumEnglishUnits) ?
			fmd_vals[ROTH_FM_FMD_D100H_LOAD] 	: UNITS_MGPHA_TO_KGPMSQ(fmd_vals[ROTH_FM_FMD_D100H_LOAD]);	
	rfm->load[EnumLHSizeClass] 		= (rfm->units == EnumEnglishUnits) ?
			fmd_vals[ROTH_FM_FMD_LH_LOAD] 		: UNITS_MGPHA_TO_KGPMSQ(fmd_vals[ROTH_FM_FMD_LH_LOAD]);	
	rfm->load[EnumLWSizeClass] 		= (rfm->units == EnumEnglishUnits) ?
			fmd_vals[ROTH_FM_FMD_LW_LOAD] 		: UNITS_MGPHA_TO_KGPMSQ(fmd_vals[ROTH_FM_FMD_LW_LOAD]);
	/* fuel particle sav */			
	rfm->sav[EnumD1HSizeClass]		= (rfm->units == EnumEnglishUnits) ?
			fmd_vals[ROTH_FM_FMD_1H_SAV] 		: UNITS_CMSAV_TO_MSAV(fmd_vals[ROTH_FM_FMD_1H_SAV]);
	rfm->sav[EnumD10HSizeClass]		= (rfm->units == EnumEnglishUnits) ? 
			ROTH_FM_D10H_STD_SAV_ENGLISH 		: UNITS_FTSAV_TO_MSAV(ROTH_FM_D10H_STD_SAV_ENGLISH);
	rfm->sav[EnumD100HSizeClass]	= (rfm->units == EnumEnglishUnits) ? 
			ROTH_FM_D100H_STD_SAV_ENGLISH 		: UNITS_FTSAV_TO_MSAV(ROTH_FM_D100H_STD_SAV_ENGLISH);
	rfm->sav[EnumLHSizeClass]		= (rfm->units == EnumEnglishUnits) ? 
			fmd_vals[ROTH_FM_FMD_LH_SAV] 		: UNITS_CMSAV_TO_MSAV(fmd_vals[ROTH_FM_FMD_LH_SAV]);
	rfm->sav[EnumLWSizeClass]		= (rfm->units == EnumEnglishUnits) ? 
			fmd_vals[ROTH_FM_FMD_LW_SAV] 		: UNITS_CMSAV_TO_MSAV(fmd_vals[ROTH_FM_FMD_LW_SAV]);
	/* fuel particle dens */
	rfm->dens[EnumD1HSizeClass] 	= (rfm->units == EnumEnglishUnits) ? 
			ROTH_FM_STD_FP_DENSITY_ENGLISH 		: UNITS_LBPFT3_TO_KGPM3(ROTH_FM_STD_FP_DENSITY_ENGLISH);
	rfm->dens[EnumD10HSizeClass] 	= (rfm->units == EnumEnglishUnits) ? 
			ROTH_FM_STD_FP_DENSITY_ENGLISH 		: UNITS_LBPFT3_TO_KGPM3(ROTH_FM_STD_FP_DENSITY_ENGLISH);
	rfm->dens[EnumD100HSizeClass] 	= (rfm->units == EnumEnglishUnits) ? 
			ROTH_FM_STD_FP_DENSITY_ENGLISH 		: UNITS_LBPFT3_TO_KGPM3(ROTH_FM_STD_FP_DENSITY_ENGLISH);
	rfm->dens[EnumLHSizeClass]		= (rfm->units == EnumEnglishUnits) ? 
			ROTH_FM_STD_FP_DENSITY_ENGLISH 		: UNITS_LBPFT3_TO_KGPM3(ROTH_FM_STD_FP_DENSITY_ENGLISH);
	rfm->dens[EnumLWSizeClass]		= (rfm->units == EnumEnglishUnits) ? 
			ROTH_FM_STD_FP_DENSITY_ENGLISH 		: UNITS_LBPFT3_TO_KGPM3(ROTH_FM_STD_FP_DENSITY_ENGLISH);
	/* fuel particle heat content */
	rfm->hc[EnumD1HSizeClass] 		= (rfm->units == EnumEnglishUnits) ? 
			fmd_vals[ROTH_FM_FMD_DHC]			: UNITS_KJPKG_TO_JPKG(fmd_vals[ROTH_FM_FMD_DHC]);
	rfm->hc[EnumD10HSizeClass]		= (rfm->units == EnumEnglishUnits) ? 
			fmd_vals[ROTH_FM_FMD_DHC]			: UNITS_KJPKG_TO_JPKG(fmd_vals[ROTH_FM_FMD_DHC]);
	rfm->hc[EnumD100HSizeClass]		= (rfm->units == EnumEnglishUnits) ? 
			fmd_vals[ROTH_FM_FMD_DHC]			: UNITS_KJPKG_TO_JPKG(fmd_vals[ROTH_FM_FMD_DHC]);
	rfm->hc[EnumLHSizeClass]		= (rfm->units == EnumEnglishUnits) ? 
			fmd_vals[ROTH_FM_FMD_LHC]			: UNITS_KJPKG_TO_JPKG(fmd_vals[ROTH_FM_FMD_LHC]);
	rfm->hc[EnumLWSizeClass]		= (rfm->units == EnumEnglishUnits) ? 
			fmd_vals[ROTH_FM_FMD_LHC]			: UNITS_KJPKG_TO_JPKG(fmd_vals[ROTH_FM_FMD_LHC]);
	/* fuel particle total silica content */
	rfm->stot[EnumD1HSizeClass] 	= ROTH_FM_STD_TOTAL_SILICA_UNITLESS; 
	rfm->stot[EnumD10HSizeClass] 	= ROTH_FM_STD_TOTAL_SILICA_UNITLESS; 
	rfm->stot[EnumD100HSizeClass] 	= ROTH_FM_STD_TOTAL_SILICA_UNITLESS; 
	rfm->stot[EnumLHSizeClass] 		= ROTH_FM_STD_TOTAL_SILICA_UNITLESS; 
	rfm->stot[EnumLWSizeClass] 		= ROTH_FM_STD_TOTAL_SILICA_UNITLESS; 
	/* fuel particle effective silica content */
	rfm->seff[EnumD1HSizeClass] 	= ROTH_FM_STD_EFF_SILICA_UNITLESS;
	rfm->seff[EnumD10HSizeClass] 	= ROTH_FM_STD_EFF_SILICA_UNITLESS;
	rfm->seff[EnumD100HSizeClass] 	= ROTH_FM_STD_EFF_SILICA_UNITLESS;
	rfm->seff[EnumLHSizeClass] 		= ROTH_FM_STD_EFF_SILICA_UNITLESS;
	rfm->seff[EnumLWSizeClass] 		= ROTH_FM_STD_EFF_SILICA_UNITLESS;
	
	/* count num fuel particles */
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		if ( UNITS_FP_GT_ZERO(rfm->load[i]) )	{
			rfm->num_fp = rfm->num_fp + 1;
			}
		}
	
	/* populate derived fuel particle properties */
	if ( RothFuelModelSetDerivedProperties(rfm) ) 	{
		ERR_ERROR_CONTINUE("Unable to initialize RothFuelModel from .FMD file. \n", ERR_EBADFUNC);
		FreeRothFuelModel(rfm);
		rfm = NULL;
		return rfm;
		}
				
	return rfm;
	}

RothFuelModel * InitRothFuelModelUnBurnable()	{
	RothFuelModel * rfm = NULL;
		
	if ( (rfm = InitRothFuelModelEmpty()) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for RothFuelModel. \n", ERR_ENOMEM);
		return rfm;
		}

	rfm->brntype = EnumRothUnBurnable;
	
	return rfm;
	}

/*
 * Description:
 * Takes a RothFuelModel structure consisting of multi-sized fuel particles
 * and calculates the derived fuel particle properties.
 *
 * Arguments:
 * rfm- RothFuelModel containing fuel particle attributes
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int RothFuelModelSetDerivedProperties(RothFuelModel * rfm)	{
	int i;
	float tarea, lsav, dsav;

	tarea = lsav = dsav = 0.0;
	
	/* check args */
	if ( rfm == NULL )	{
		ERR_ERROR("Unable to calculate derived properties in RothFuelModel. \n", ERR_EINVAL);
		}
		
	/* check if burnable fuel */
	if ( rfm->brntype == EnumRothUnBurnable )	{
		return ERR_SUCCESS;
		}
		 
	/* update fuel particle derived properties */   		
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		/* surface area */
		if ( UNITS_FP_GT_ZERO(rfm->dens[i]) )	{
			rfm->sarea[i] = (rfm->load[i] * rfm->sav[i]) / rfm->dens[i];
			}
		else	{
			rfm->sarea[i] = 0.0;
			}
		/* effective heating number */
		if ( rfm->units == EnumEnglishUnits )	{
			if ( UNITS_FP_GT_ZERO(rfm->sav[i]) )	{
				rfm->effhn[i] = exp(-138.0 / rfm->sav[i]);
				}
			else	{
				rfm->effhn[i] = 0.0;
				}
			}
		if ( rfm->units == EnumMetricUnits )	{
			if ( UNITS_FP_GT_ZERO(rfm->sav[i]) )	{
				rfm->effhn[i] = exp(-1.0 / (0.0022 * rfm->sav[i]));
				}
			else	{
				rfm->effhn[i] = 0.0;
				}
			}		
		/* zero wtg */
		rfm->awtg[i] = 0.0;
		}
	
	/* accumulate surface areas by life category for the entire fuel bed */
	rfm->larea = rfm->darea = 0.0;
    rfm->larea = rfm->sarea[EnumLHSizeClass] + rfm->sarea[EnumLWSizeClass];
    rfm->darea = rfm->sarea[EnumD1HSizeClass] + rfm->sarea[EnumD10HSizeClass] + rfm->sarea[EnumD100HSizeClass];

	/* surface area weighting factor for each particle */
	if ( UNITS_FP_GT_ZERO(rfm->larea) )	{
		rfm->awtg[EnumLHSizeClass] = rfm->sarea[EnumLHSizeClass] / rfm->larea;
		rfm->awtg[EnumLWSizeClass] = rfm->sarea[EnumLWSizeClass] / rfm->larea;
		}
	if ( UNITS_FP_GT_ZERO(rfm->darea) )	{
		rfm->awtg[EnumD1HSizeClass] = rfm->sarea[EnumD1HSizeClass] / rfm->darea;
		rfm->awtg[EnumD10HSizeClass] = rfm->sarea[EnumD10HSizeClass] / rfm->darea;
		rfm->awtg[EnumD100HSizeClass] = rfm->sarea[EnumD100HSizeClass] / rfm->darea;		
		}		
		
	/* surface area weighting contributed by live and dead fuels */
	tarea = rfm->larea + rfm->darea;
	rfm->larea = rfm->larea / tarea;
	rfm->darea = rfm->darea / tarea;
	
	/* fuel bed bulk density */		
	if ( UNITS_FP_GT_ZERO(rfm->fdepth) )	{
		rfm->fdens = (rfm->load[EnumLHSizeClass] + rfm->load[EnumLWSizeClass] + rfm->load[EnumD1HSizeClass] +
					rfm->load[EnumD10HSizeClass] + rfm->load[EnumD100HSizeClass]) / rfm->fdepth;
		}
		
	/* fuel bed characteristic sav */
	lsav =	(rfm->awtg[EnumLHSizeClass] * rfm->sav[EnumLHSizeClass]) +
			(rfm->awtg[EnumLWSizeClass] * rfm->sav[EnumLWSizeClass]);	
	dsav =	(rfm->awtg[EnumD1HSizeClass] * rfm->sav[EnumD1HSizeClass]) +
			(rfm->awtg[EnumD10HSizeClass] * rfm->sav[EnumD10HSizeClass]) +
			(rfm->awtg[EnumD100HSizeClass] * rfm->sav[EnumD100HSizeClass]);
	rfm->fsav = (rfm->larea * lsav) + (rfm->darea * dsav);

	/* fuel bed packing ratio */
	rfm->pkrat = 0.0;
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		if ( UNITS_FP_GT_ZERO(rfm->dens[i]) )	{
			rfm->pkrat += rfm->load[i] / rfm->dens[i];
			}
		}
	if ( UNITS_FP_GT_ZERO(rfm->fdepth) )	{
		rfm->pkrat /= rfm->fdepth;
		}
	
	return ERR_SUCCESS;
	}

int RothFuelModelMetricToEnglish(RothFuelModel * rfm)	{
	int i;
	 
	if ( rfm == NULL ) {
		ERR_ERROR("RothFuelModel values not initialized, conversion failed. \n", ERR_EINVAL);
		}
	else if ( rfm->brntype == EnumRothUnBurnable )	{
		ERR_ERROR("RothFuelModel  designated unburnable, conversion failed. \n", ERR_EINVAL);
		}
	else if ( rfm->units == EnumUnknownUnits )	{
		ERR_ERROR("RothFuelModel units are unknown, unable to convert to ENGLISH. \n", ERR_EINVAL);
		}
	else if ( rfm->units == EnumEnglishUnits )	{
		ERR_ERROR("RothFuelModel values already ENGLISH, no conversion necessary. \n", ERR_WARNING);
		}

	rfm->units = EnumEnglishUnits;
	
	/* convert each static fuel particle property */
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		if ( UNITS_FP_GT_ZERO(rfm->load[i]) )	{
			rfm->load[i] = UNITS_KGPMSQ_TO_LBPFTSQ(rfm->load[i]);
			}
		if ( UNITS_FP_GT_ZERO(rfm->sav[i]) )	{
			rfm->sav[i] = UNITS_MSAV_TO_FTSAV(rfm->sav[i]);
			}
		if ( UNITS_FP_GT_ZERO(rfm->dens[i]) )	{
			rfm->dens[i] = UNITS_KGPM3_TO_LBPFT3(rfm->dens[i]);
			}
		if ( UNITS_FP_GT_ZERO(rfm->hc[i]) )	{
			rfm->hc[i] = UNITS_JPKG_TO_BTUPLB(rfm->hc[i]);
			}
		}

	/* convert the static fuel bed properties */
	if ( UNITS_FP_GT_ZERO(rfm->fdepth) )	{
		rfm->fdepth = UNITS_M_TO_FT(rfm->fdepth);
		}
	if ( UNITS_FP_GT_ZERO(rfm->fdens) )		{
		rfm->fdens = UNITS_KGPM3_TO_LBPFT3(rfm->fdens);
		}
	if ( UNITS_FP_GT_ZERO(rfm->fsav) )		{
		rfm->fsav = UNITS_MSAV_TO_FTSAV(rfm->fsav);
		}	
		
	/* recalculate derived fuel particle properties */
	if ( RothFuelModelSetDerivedProperties(rfm) ) 	{
		ERR_ERROR("Calculation of derived properties failed during unit conversion. \n", ERR_EBADFUNC);
		}
		
	return ERR_SUCCESS;	
	}

int RothFuelModelEnglishToMetric(RothFuelModel * rfm)	{
	int i;
	 
	if ( rfm == NULL ) {
		ERR_ERROR("RothFuelModel values not initialized, conversion failed. \n", ERR_EINVAL);
		}
	else if ( rfm->brntype == EnumRothUnBurnable )	{
		ERR_ERROR("RothFuelModel  designated unburnable, conversion failed. \n", ERR_EINVAL);
		}
	else if ( rfm->units == EnumUnknownUnits )	{
		ERR_ERROR("RothFuelModel units are unknown, unable to convert to METRIC. \n", ERR_EINVAL);
		}
	else if ( rfm->units == EnumMetricUnits )	{
		ERR_ERROR("RothFuelModel values already METRIC, no conversion necessary. \n", ERR_WARNING);
		}

	rfm->units = EnumMetricUnits;
	
	/* convert each static fuel particle property */
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		if ( UNITS_FP_GT_ZERO(rfm->load[i]) )	{
			rfm->load[i] = UNITS_LBPFTSQ_TO_KGPMSQ(rfm->load[i]);
			}
		if ( UNITS_FP_GT_ZERO(rfm->sav[i]) )	{
			rfm->sav[i] = UNITS_FTSAV_TO_MSAV(rfm->sav[i]);
			}
		if ( UNITS_FP_GT_ZERO(rfm->dens[i]) )	{
			rfm->dens[i] = UNITS_LBPFT3_TO_KGPM3(rfm->dens[i]);
			}
		if ( UNITS_FP_GT_ZERO(rfm->hc[i]) )	{
			rfm->hc[i] = UNITS_BTUPLB_TO_JPKG(rfm->hc[i]);
			}
		}

	/* convert the static fuel bed properties */
	if ( UNITS_FP_GT_ZERO(rfm->fdepth) )	{
		rfm->fdepth = UNITS_FT_TO_M(rfm->fdepth);
		}
	if ( UNITS_FP_GT_ZERO(rfm->fdens) )		{
		rfm->fdens = UNITS_LBPFT3_TO_KGPM3(rfm->fdens);
		}
	if ( UNITS_FP_GT_ZERO(rfm->fsav) )		{
		rfm->fsav = UNITS_FTSAV_TO_MSAV(rfm->fsav);
		}	
		
	/* recalculate derived fuel particle properties */
	if ( RothFuelModelSetDerivedProperties(rfm) ) 	{
		ERR_ERROR("Calculation of derived properties failed during unit conversion. \n", ERR_EBADFUNC);
		}
		
	return ERR_SUCCESS;	
	}

void RothFuelModelDumpToStream(RothFuelModel * rfm, FILE * fstream)	{
	if ( rfm == NULL || fstream == NULL )	{
		ERR_ERROR_CONTINUE("Unable to dump RothFuelModel attributes. \n", ERR_EINVAL);
		return;
		}

	switch(rfm->units)	{
		case EnumUnknownUnits:
			fprintf(fstream, "UNITS: UNKNOWN \n");
			break;
		case EnumEnglishUnits:
			fprintf(fstream, "UNITS: ENGLISH \n");
			break;
		case EnumMetricUnits:
			fprintf(fstream, "UNITS: METRIC \n");
			break;
		default:
			fprintf(fstream, "UNITS: <WARNING! NOT RECOGNIZED> \n");
			break;
		}
	fprintf(fstream, "FUEL BED PROPERTIES \n");
	fprintf(fstream, "DEPTH: \t\t %f \n", rfm->fdepth);
	fprintf(fstream, "MEXT: \t\t %f \n", rfm->mex);
	fprintf(fstream, "ROS ADJ: \t %f \n", rfm->ros_adj);
	fprintf(fstream, "NUM FP: \t %d \n", rfm->num_fp);
	fprintf(fstream, "FUEL BED CHARACTERISTIC PROPERTIES \n");
	fprintf(fstream, "FDENS: \t\t %f \n", rfm->fdens);
	fprintf(fstream, "FSAV: \t\t %f \n", rfm->fsav);
	fprintf(fstream, "PACKR: \t\t %f \n", rfm->pkrat);
	fprintf(fstream, "FUEL PARTICLE PROPERTIES \n");	
	fprintf(fstream, "SIZE: \t D1H \t\t D10H \t\t D100H \t\t LH \t\t LW \n");
	fprintf(fstream, "LOAD: \t %.4f \t %.4f \t %.4f \t %.4f \t %.4f \n", 
		rfm->load[EnumD1HSizeClass], rfm->load[EnumD10HSizeClass], rfm->load[EnumD100HSizeClass], 
		rfm->load[EnumLHSizeClass], rfm->load[EnumLWSizeClass] );			
	fprintf(fstream, "SAV: \t %.4f \t %.4f \t %.4f \t %.4f \t %.4f \n", 
		rfm->sav[EnumD1HSizeClass], rfm->sav[EnumD10HSizeClass], rfm->sav[EnumD100HSizeClass], 
		rfm->sav[EnumLHSizeClass], rfm->sav[EnumLWSizeClass] );
	fprintf(fstream, "DENS: \t %.4f \t %.4f \t %.4f \t %.4f \t %.4f \n", 
		rfm->dens[EnumD1HSizeClass], rfm->dens[EnumD10HSizeClass], rfm->dens[EnumD100HSizeClass], 
		rfm->dens[EnumLHSizeClass], rfm->dens[EnumLWSizeClass] );
	fprintf(fstream, "HC: \t %.2f \t %.2f \t %.2f \t %.2f \t %.2f \n", 
		rfm->hc[EnumD1HSizeClass], rfm->hc[EnumD10HSizeClass], rfm->hc[EnumD100HSizeClass], 
		rfm->hc[EnumLHSizeClass], rfm->hc[EnumLWSizeClass] );
	fprintf(fstream, "STOT: \t %f \t %f \t %f \t %f \t %f \n", 
		rfm->stot[EnumD1HSizeClass], rfm->stot[EnumD10HSizeClass], rfm->stot[EnumD100HSizeClass], 
		rfm->stot[EnumLHSizeClass], rfm->stot[EnumLWSizeClass] );
	fprintf(fstream, "SEFF: \t %f \t %f \t %f \t %f \t %f \n", 
		rfm->seff[EnumD1HSizeClass], rfm->seff[EnumD10HSizeClass], rfm->seff[EnumD100HSizeClass], 
		rfm->seff[EnumLHSizeClass], rfm->seff[EnumLWSizeClass] );
	fprintf(fstream, "FUEL PARTICLE DERIVED PROPERTIES \n");
	fprintf(fstream, "SAREA: \t %f \t %f \t %f \t %f \t %f \n", 
		rfm->sarea[EnumD1HSizeClass], rfm->sarea[EnumD10HSizeClass], rfm->sarea[EnumD100HSizeClass], 
		rfm->sarea[EnumLHSizeClass], rfm->sarea[EnumLWSizeClass] );
	fprintf(fstream, "EFFHN: \t %f \t %f \t %f \t %f \t %f \n", 
		rfm->effhn[EnumD1HSizeClass], rfm->effhn[EnumD10HSizeClass], rfm->effhn[EnumD100HSizeClass], 
		rfm->effhn[EnumLHSizeClass], rfm->effhn[EnumLWSizeClass] );			
	fprintf(fstream, "FUEL BED WEIGHTING PROPERTIES \n");		
	fprintf(fstream, "AWTG: \t %f \t %f \t %f \t %f \t %f \n", 
		rfm->awtg[EnumD1HSizeClass], rfm->awtg[EnumD10HSizeClass], rfm->awtg[EnumD100HSizeClass], 
		rfm->awtg[EnumLHSizeClass], rfm->awtg[EnumLWSizeClass] );
	fprintf(fstream, "LDWTG: \t %f \t %f \t %f \t %f \t %f \n", 
		rfm->darea, rfm->darea, rfm->darea, rfm->larea, rfm->larea );	
	fprintf(fstream, "FUEL PARTICLE ENVIRONMENTAL PROPERTIES \n");		
	fprintf(fstream, "FM: \t %f \t %f \t %f \t %f \t %f \n", 
		rfm->fm[EnumD1HSizeClass], rfm->fm[EnumD10HSizeClass], rfm->fm[EnumD100HSizeClass], 
		rfm->fm[EnumLHSizeClass], rfm->fm[EnumLWSizeClass] );
	fprintf(fstream, "NOTE: some attributes not user-configurable, assigned default values \n");	
	
	return;
	}

void FreeRothFuelModel(void * vptr)	{
 	RothFuelModel * rfm = NULL;
 	if ( vptr != NULL )	{
 		rfm = (RothFuelModel *) vptr;
 		if ( rfm != NULL )	{
 			if ( rfm->rp != NULL )	{
 				FreeRothPipeline(rfm->rp);
 				}
 			free(rfm);
 			}
 		} 	
 	rfm = NULL;
 	return;	
 	}
	
/* end of RothFuelModel.c */
