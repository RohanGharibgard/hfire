/*! 
 * \file PhysFuelModel.c
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
 
#include "PhysFuelModel.h"

static int PhysFuelModelSetDerivedProperties(PhysFuelModel * pfm);

PhysFuelModel * InitPhysFuelModelEmpty()	{
	PhysFuelModel * pfm = NULL;
	int i;
	
	if ( (pfm = (PhysFuelModel *) malloc(sizeof(PhysFuelModel))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for PhysFuelModel. \n", ERR_ENOMEM);
		return pfm;
		}
		
	pfm->brntype	= EnumPhysUnBurnable;
	pfm->units		= EnumUnknownUnits;
	pfm->fdepth = 0.0;
	pfm->num_fp = 0;
	pfm->fdens = pfm->fsav = pfm->pkrat = 0.0;
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		pfm->load[i] = pfm->sav[i] = pfm->dens[i] = 0.0;
		pfm->sarea[i] = pfm->effhn[i] = 0.0;
		pfm->awtg[i] = 0.0;
		pfm->larea = pfm->darea = 0.0;
		pfm->fm[i] = 0.0;
		}
		
	return pfm;
	}

PhysFuelModel * InitPhysFuelModelFMDFile(FILE * fstream, int model_num)	{
	PhysFuelModel * pfm = NULL;
	int c, i = 0;
	char * line = NULL, * token = NULL;
	float fmd_vals[PHYS_FM_FMD_NUM_VALUES];
	
	if ( fstream == NULL )	{
		ERR_ERROR_CONTINUE("Stream to FMD file not open, PhysFuelModel not retrieved. \n", ERR_EIOFAIL);
		return pfm;
		}
		
	if ( (pfm = InitPhysFuelModelEmpty()) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for PhysFuelModel. \n", ERR_ENOMEM);
		return pfm;
		}

	pfm->brntype = EnumPhysBurnable;
	pfm->units = EnumEnglishUnits;
	
	/* retrieve fmd contents */
	while ( (c = fgetc(fstream)) != EOF )	{
		ungetc(c, fstream);
		/* test line is not empty */
		if ( (line = GetLineFStreamIO(fstream)) != NULL )		{
			/* test line is not comment and parse into tokens */
			if ( line[0] != *PHYS_FM_FMD_COMMENT_CHAR 
				 && (token = strtok(line, PHYS_FM_FMD_SEPARATOR_CHARS)) != NULL )		{
				/* ENGLISH UNITS */				
				if ( strcmp(token, PHYS_FM_FMD_KEYWORD_ENGLISH) == 0)	{
					pfm->units = EnumEnglishUnits;
					}
				/* METRIC UNITS */				
				else if ( strcmp(token, PHYS_FM_FMD_KEYWORD_METRIC) == 0)	{
					pfm->units = EnumMetricUnits;
					}
				/* FUEL VALUES */
				else if ( model_num == atoi(token) )	{
					/* model num matches, populate the attributes of the fuel model */						
					while ( (token = strtok(NULL, PHYS_FM_FMD_SEPARATOR_CHARS)) != NULL )	{
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
	if ( i == 0 && pfm != NULL )	{
		ERR_ERROR_CONTINUE("Attribute values for model num not retrieved from .FMD file. \n", ERR_ERANGE);
		FreePhysFuelModel(pfm);
		pfm = NULL;
		return pfm;
		}

	/* populate static fuel bed properties */
	pfm->fdepth 					= (pfm->units == EnumEnglishUnits) ?
			fmd_vals[PHYS_FM_FMD_FBED_DEPTH] : UNITS_CM_TO_M(fmd_vals[PHYS_FM_FMD_FBED_DEPTH]);
	/* fuel particle load */
	pfm->load[EnumD1HSizeClass] 	= (pfm->units == EnumEnglishUnits) ?
			fmd_vals[PHYS_FM_FMD_D1H_LOAD] 		: UNITS_MGPHA_TO_KGPMSQ(fmd_vals[PHYS_FM_FMD_D1H_LOAD]);
	pfm->load[EnumD10HSizeClass] 	= (pfm->units == EnumEnglishUnits) ?
			fmd_vals[PHYS_FM_FMD_D10H_LOAD] 	: UNITS_MGPHA_TO_KGPMSQ(fmd_vals[PHYS_FM_FMD_D10H_LOAD]);	
	pfm->load[EnumD100HSizeClass] 	= (pfm->units == EnumEnglishUnits) ?
			fmd_vals[PHYS_FM_FMD_D100H_LOAD] 	: UNITS_MGPHA_TO_KGPMSQ(fmd_vals[PHYS_FM_FMD_D100H_LOAD]);	
	pfm->load[EnumLHSizeClass] 		= (pfm->units == EnumEnglishUnits) ?
			fmd_vals[PHYS_FM_FMD_LH_LOAD] 		: UNITS_MGPHA_TO_KGPMSQ(fmd_vals[PHYS_FM_FMD_LH_LOAD]);	
	pfm->load[EnumLWSizeClass] 		= (pfm->units == EnumEnglishUnits) ?
			fmd_vals[PHYS_FM_FMD_LW_LOAD] 		: UNITS_MGPHA_TO_KGPMSQ(fmd_vals[PHYS_FM_FMD_LW_LOAD]);
	/* fuel particle sav */			
	pfm->sav[EnumD1HSizeClass]		= (pfm->units == EnumEnglishUnits) ?
			fmd_vals[PHYS_FM_FMD_1H_SAV] 		: UNITS_CMSAV_TO_MSAV(fmd_vals[PHYS_FM_FMD_1H_SAV]);
	pfm->sav[EnumD10HSizeClass]		= (pfm->units == EnumEnglishUnits) ? 
			PHYS_FM_D10H_STD_SAV_ENGLISH 		: UNITS_FTSAV_TO_MSAV(PHYS_FM_D10H_STD_SAV_ENGLISH);
	pfm->sav[EnumD100HSizeClass]	= (pfm->units == EnumEnglishUnits) ? 
			PHYS_FM_D100H_STD_SAV_ENGLISH 		: UNITS_FTSAV_TO_MSAV(PHYS_FM_D100H_STD_SAV_ENGLISH);
	pfm->sav[EnumLHSizeClass]		= (pfm->units == EnumEnglishUnits) ? 
			fmd_vals[PHYS_FM_FMD_LH_SAV] 		: UNITS_CMSAV_TO_MSAV(fmd_vals[PHYS_FM_FMD_LH_SAV]);
	pfm->sav[EnumLWSizeClass]		= (pfm->units == EnumEnglishUnits) ? 
			fmd_vals[PHYS_FM_FMD_LW_SAV] 		: UNITS_CMSAV_TO_MSAV(fmd_vals[PHYS_FM_FMD_LW_SAV]);
	/* fuel particle dens */
	pfm->dens[EnumD1HSizeClass] 	= (pfm->units == EnumEnglishUnits) ? 
			PHYS_FM_STD_FP_DENSITY_ENGLISH 		: UNITS_LBPFT3_TO_KGPM3(PHYS_FM_STD_FP_DENSITY_ENGLISH);
	pfm->dens[EnumD10HSizeClass] 	= (pfm->units == EnumEnglishUnits) ? 
			PHYS_FM_STD_FP_DENSITY_ENGLISH 		: UNITS_LBPFT3_TO_KGPM3(PHYS_FM_STD_FP_DENSITY_ENGLISH);
	pfm->dens[EnumD100HSizeClass] 	= (pfm->units == EnumEnglishUnits) ? 
			PHYS_FM_STD_FP_DENSITY_ENGLISH 		: UNITS_LBPFT3_TO_KGPM3(PHYS_FM_STD_FP_DENSITY_ENGLISH);
	pfm->dens[EnumLHSizeClass]		= (pfm->units == EnumEnglishUnits) ? 
			PHYS_FM_STD_FP_DENSITY_ENGLISH 		: UNITS_LBPFT3_TO_KGPM3(PHYS_FM_STD_FP_DENSITY_ENGLISH);
	pfm->dens[EnumLWSizeClass]		= (pfm->units == EnumEnglishUnits) ? 
			PHYS_FM_STD_FP_DENSITY_ENGLISH 		: UNITS_LBPFT3_TO_KGPM3(PHYS_FM_STD_FP_DENSITY_ENGLISH);
	
	/* count num fuel particles */
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		if ( UNITS_FP_GT_ZERO(pfm->load[i]) )	{
			pfm->num_fp = pfm->num_fp + 1;
			}
		}
	
	/* populate derived fuel particle properties */
	if ( PhysFuelModelSetDerivedProperties(pfm) ) 	{
		ERR_ERROR_CONTINUE("Unable to initialize PhysFuelModel from .FMD file. \n", ERR_EBADFUNC);
		FreePhysFuelModel(pfm);
		pfm = NULL;
		return pfm;
		}
				
	return pfm;
	}
	
PhysFuelModel * InitPhysFuelModelUnBurnable()	{
	PhysFuelModel * pfm = NULL;
		
	if ( (pfm = InitPhysFuelModelEmpty()) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for PhysFuelModel. \n", ERR_ENOMEM);
		return pfm;
		}

	pfm->brntype = EnumPhysUnBurnable;
	
	return pfm;
	}

/*
 * Description:
 * Takes a PhysFuelModel structure consisting of multi-sized fuel particles
 * and calculates the derived fuel particle properties.
 *
 * Arguments:
 * pfm- PhysFuelModel containing fuel particle attributes
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */
int PhysFuelModelSetDerivedProperties(PhysFuelModel * pfm)	{
	int i;
	float tarea, lsav, dsav;

	tarea = lsav = dsav = 0.0;
	
	/* check args */
	if ( pfm == NULL )	{
		ERR_ERROR("Unable to calculate derived properties in PhysFuelModel. \n", ERR_EINVAL);
		}
		
	/* check if burnable fuel */
	if ( pfm->brntype == EnumPhysUnBurnable )	{
		return ERR_SUCCESS;
		}
		 
	/* update fuel particle derived properties */   		
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		/* surface area */
		if ( UNITS_FP_GT_ZERO(pfm->dens[i]) )	{
			pfm->sarea[i] = (pfm->load[i] * pfm->sav[i]) / pfm->dens[i];
			}
		else	{
			pfm->sarea[i] = 0.0;
			}
		/* effective heating number */
		if ( pfm->units == EnumEnglishUnits )	{
			if ( UNITS_FP_GT_ZERO(pfm->sav[i]) )	{
				pfm->effhn[i] = exp(-138.0 / pfm->sav[i]);
				}
			else	{
				pfm->effhn[i] = 0.0;
				}
			}
		if ( pfm->units == EnumMetricUnits )	{
			if ( UNITS_FP_GT_ZERO(pfm->sav[i]) )	{
				pfm->effhn[i] = exp(-1.0 / (0.0022 * pfm->sav[i]));
				}
			else	{
				pfm->effhn[i] = 0.0;
				}
			}		
		/* zero wtg */
		pfm->awtg[i] = 0.0;
		}
	
	/* accumulate surface areas by life category for the entire fuel bed */
	pfm->larea = pfm->darea = 0.0;
    pfm->larea = pfm->sarea[EnumLHSizeClass] + pfm->sarea[EnumLWSizeClass];
    pfm->darea = pfm->sarea[EnumD1HSizeClass] + pfm->sarea[EnumD10HSizeClass] + pfm->sarea[EnumD100HSizeClass];

	/* surface area weighting factor for each particle */
	if ( UNITS_FP_GT_ZERO(pfm->larea) )	{
		pfm->awtg[EnumLHSizeClass] = pfm->sarea[EnumLHSizeClass] / pfm->larea;
		pfm->awtg[EnumLWSizeClass] = pfm->sarea[EnumLWSizeClass] / pfm->larea;
		}
	if ( UNITS_FP_GT_ZERO(pfm->darea) )	{
		pfm->awtg[EnumD1HSizeClass] = pfm->sarea[EnumD1HSizeClass] / pfm->darea;
		pfm->awtg[EnumD10HSizeClass] = pfm->sarea[EnumD10HSizeClass] / pfm->darea;
		pfm->awtg[EnumD100HSizeClass] = pfm->sarea[EnumD100HSizeClass] / pfm->darea;		
		}		
		
	/* surface area weighting contributed by live and dead fuels */
	tarea = pfm->larea + pfm->darea;
	pfm->larea = pfm->larea / tarea;
	pfm->darea = pfm->darea / tarea;
	
	/* fuel bed bulk density */		
	if ( UNITS_FP_GT_ZERO(pfm->fdepth) )	{
		pfm->fdens = (pfm->load[EnumLHSizeClass] + pfm->load[EnumLWSizeClass] + pfm->load[EnumD1HSizeClass] +
					pfm->load[EnumD10HSizeClass] + pfm->load[EnumD100HSizeClass]) / pfm->fdepth;
		}
		
	/* fuel bed characteristic sav */
	lsav =	(pfm->awtg[EnumLHSizeClass] * pfm->sav[EnumLHSizeClass]) +
			(pfm->awtg[EnumLWSizeClass] * pfm->sav[EnumLWSizeClass]);	
	dsav =	(pfm->awtg[EnumD1HSizeClass] * pfm->sav[EnumD1HSizeClass]) +
			(pfm->awtg[EnumD10HSizeClass] * pfm->sav[EnumD10HSizeClass]) +
			(pfm->awtg[EnumD100HSizeClass] * pfm->sav[EnumD100HSizeClass]);
	pfm->fsav = (pfm->larea * lsav) + (pfm->darea * dsav);

	/* fuel bed packing ratio */
	pfm->pkrat = 0.0;
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		if ( UNITS_FP_GT_ZERO(pfm->dens[i]) )	{
			pfm->pkrat += pfm->load[i] / pfm->dens[i];
			}
		}
	if ( UNITS_FP_GT_ZERO(pfm->fdepth) )	{
		pfm->pkrat /= pfm->fdepth;
		}
	
	return ERR_SUCCESS;
	}

int PhysFuelModelMetricToEnglish(PhysFuelModel * pfm)	{
	int i;
	 
	if ( pfm == NULL ) {
		ERR_ERROR("PhysFuelModel values not initialized, conversion failed. \n", ERR_EINVAL);
		}
	else if ( pfm->brntype == EnumPhysUnBurnable )	{
		ERR_ERROR("PhysFuelModel  designated unburnable, conversion failed. \n", ERR_EINVAL);
		}
	else if ( pfm->units == EnumUnknownUnits )	{
		ERR_ERROR("PhysFuelModel units are unknown, unable to convert to ENGLISH. \n", ERR_EINVAL);
		}
	else if ( pfm->units == EnumEnglishUnits )	{
		ERR_ERROR("PhysFuelModel values already ENGLISH, no conversion necessary. \n", ERR_WARNING);
		}

	pfm->units = EnumEnglishUnits;
	
	/* convert each static fuel particle property */
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		if ( UNITS_FP_GT_ZERO(pfm->load[i]) )	{
			pfm->load[i] = UNITS_KGPMSQ_TO_LBPFTSQ(pfm->load[i]);
			}
		if ( UNITS_FP_GT_ZERO(pfm->sav[i]) )	{
			pfm->sav[i] = UNITS_MSAV_TO_FTSAV(pfm->sav[i]);
			}
		if ( UNITS_FP_GT_ZERO(pfm->dens[i]) )	{
			pfm->dens[i] = UNITS_KGPM3_TO_LBPFT3(pfm->dens[i]);
			}
		}

	/* convert the static fuel bed properties */
	if ( UNITS_FP_GT_ZERO(pfm->fdepth) )	{
		pfm->fdepth = UNITS_M_TO_FT(pfm->fdepth);
		}
	if ( UNITS_FP_GT_ZERO(pfm->fdens) )		{
		pfm->fdens = UNITS_KGPM3_TO_LBPFT3(pfm->fdens);
		}
	if ( UNITS_FP_GT_ZERO(pfm->fsav) )		{
		pfm->fsav = UNITS_MSAV_TO_FTSAV(pfm->fsav);
		}	
		
	/* recalculate derived fuel particle properties */
	if ( PhysFuelModelSetDerivedProperties(pfm) ) 	{
		ERR_ERROR("Calculation of derived properties failed during unit conversion. \n", ERR_EBADFUNC);
		}
		
	return ERR_SUCCESS;	
	}

int PhysFuelModelEnglishToMetric(PhysFuelModel * pfm)	{
	int i;
	 
	if ( pfm == NULL ) {
		ERR_ERROR("PhysFuelModel values not initialized, conversion failed. \n", ERR_EINVAL);
		}
	else if ( pfm->brntype == EnumPhysUnBurnable )	{
		ERR_ERROR("PhysFuelModel  designated unburnable, conversion failed. \n", ERR_EINVAL);
		}
	else if ( pfm->units == EnumUnknownUnits )	{
		ERR_ERROR("PhysFuelModel units are unknown, unable to convert to METRIC. \n", ERR_EINVAL);
		}
	else if ( pfm->units == EnumMetricUnits )	{
		ERR_ERROR("PhysFuelModel values already METRIC, no conversion necessary. \n", ERR_WARNING);
		}

	pfm->units = EnumMetricUnits;
	
	/* convert each static fuel particle property */
	for(i = 0; i < EnumNumSizeClasses; i++) 	{
		if ( UNITS_FP_GT_ZERO(pfm->load[i]) )	{
			pfm->load[i] = UNITS_LBPFTSQ_TO_KGPMSQ(pfm->load[i]);
			}
		if ( UNITS_FP_GT_ZERO(pfm->sav[i]) )	{
			pfm->sav[i] = UNITS_FTSAV_TO_MSAV(pfm->sav[i]);
			}
		if ( UNITS_FP_GT_ZERO(pfm->dens[i]) )	{
			pfm->dens[i] = UNITS_LBPFT3_TO_KGPM3(pfm->dens[i]);
			}
		}

	/* convert the static fuel bed properties */
	if ( UNITS_FP_GT_ZERO(pfm->fdepth) )	{
		pfm->fdepth = UNITS_FT_TO_M(pfm->fdepth);
		}
	if ( UNITS_FP_GT_ZERO(pfm->fdens) )		{
		pfm->fdens = UNITS_LBPFT3_TO_KGPM3(pfm->fdens);
		}
	if ( UNITS_FP_GT_ZERO(pfm->fsav) )		{
		pfm->fsav = UNITS_FTSAV_TO_MSAV(pfm->fsav);
		}	
		
	/* recalculate derived fuel particle properties */
	if ( PhysFuelModelSetDerivedProperties(pfm) ) 	{
		ERR_ERROR("Calculation of derived properties failed during unit conversion. \n", ERR_EBADFUNC);
		}
		
	return ERR_SUCCESS;	
	}

void PhysFuelModelDumpToStream(PhysFuelModel * pfm, FILE * fstream)	{
	if ( pfm == NULL || fstream == NULL )	{
		ERR_ERROR_CONTINUE("Unable to dump PhysFuelModel attributes. \n", ERR_EINVAL);
		return;
		}

	switch(pfm->units)	{
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
	fprintf(fstream, "DEPTH: \t\t %f \n", pfm->fdepth);
	fprintf(fstream, "NUM FP: \t %d \n", pfm->num_fp);
	fprintf(fstream, "FUEL BED CHARACTERISTIC PROPERTIES \n");
	fprintf(fstream, "FDENS: \t\t %f \n", pfm->fdens);
	fprintf(fstream, "FSAV: \t\t %f \n", pfm->fsav);
	fprintf(fstream, "PACKR: \t\t %f \n", pfm->pkrat);
	fprintf(fstream, "FUEL PARTICLE PROPERTIES \n");	
	fprintf(fstream, "SIZE: \t D1H \t\t D10H \t\t D100H \t\t LH \t\t LW \n");
	fprintf(fstream, "LOAD: \t %.4f \t %.4f \t %.4f \t %.4f \t %.4f \n", 
		pfm->load[EnumD1HSizeClass], pfm->load[EnumD10HSizeClass], pfm->load[EnumD100HSizeClass], 
		pfm->load[EnumLHSizeClass], pfm->load[EnumLWSizeClass] );			
	fprintf(fstream, "SAV: \t %.4f \t %.4f \t %.4f \t %.4f \t %.4f \n", 
		pfm->sav[EnumD1HSizeClass], pfm->sav[EnumD10HSizeClass], pfm->sav[EnumD100HSizeClass], 
		pfm->sav[EnumLHSizeClass], pfm->sav[EnumLWSizeClass] );
	fprintf(fstream, "DENS: \t %.4f \t %.4f \t %.4f \t %.4f \t %.4f \n", 
		pfm->dens[EnumD1HSizeClass], pfm->dens[EnumD10HSizeClass], pfm->dens[EnumD100HSizeClass], 
		pfm->dens[EnumLHSizeClass], pfm->dens[EnumLWSizeClass] );
	fprintf(fstream, "FUEL PARTICLE DERIVED PROPERTIES \n");
	fprintf(fstream, "SAREA: \t %f \t %f \t %f \t %f \t %f \n", 
		pfm->sarea[EnumD1HSizeClass], pfm->sarea[EnumD10HSizeClass], pfm->sarea[EnumD100HSizeClass], 
		pfm->sarea[EnumLHSizeClass], pfm->sarea[EnumLWSizeClass] );
	fprintf(fstream, "EFFHN: \t %f \t %f \t %f \t %f \t %f \n", 
		pfm->effhn[EnumD1HSizeClass], pfm->effhn[EnumD10HSizeClass], pfm->effhn[EnumD100HSizeClass], 
		pfm->effhn[EnumLHSizeClass], pfm->effhn[EnumLWSizeClass] );			
	fprintf(fstream, "FUEL BED WEIGHTING PROPERTIES \n");		
	fprintf(fstream, "AWTG: \t %f \t %f \t %f \t %f \t %f \n", 
		pfm->awtg[EnumD1HSizeClass], pfm->awtg[EnumD10HSizeClass], pfm->awtg[EnumD100HSizeClass], 
		pfm->awtg[EnumLHSizeClass], pfm->awtg[EnumLWSizeClass] );
	fprintf(fstream, "LDWTG: \t %f \t %f \t %f \t %f \t %f \n", 
		pfm->darea, pfm->darea, pfm->darea, pfm->larea, pfm->larea );	
	fprintf(fstream, "FUEL PARTICLE ENVIRONMENTAL PROPERTIES \n");		
	fprintf(fstream, "FM: \t %f \t %f \t %f \t %f \t %f \n", 
		pfm->fm[EnumD1HSizeClass], pfm->fm[EnumD10HSizeClass], pfm->fm[EnumD100HSizeClass], 
		pfm->fm[EnumLHSizeClass], pfm->fm[EnumLWSizeClass] );
	fprintf(fstream, "NOTE: some attributes not user-configurable, assigned default values \n");	
	
	return;
	}

void FreePhysFuelModel(void * vptr)	{
 	PhysFuelModel * pfm = NULL;
 	if ( vptr != NULL )	{
 		pfm = (PhysFuelModel *) vptr;
 		if ( pfm != NULL )	{
 			free(pfm);
 			}
 		} 	
 	pfm = NULL;
 	return;	
 	}
	
/* end of PhysFuelModel.c */
