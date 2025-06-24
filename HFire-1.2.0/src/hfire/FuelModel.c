/*! 
 * \file FuelModel.c
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
 
#include "FuelModel.h"

/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */

int FuelModelInitRothFuelModel(FuelModel * fm, char * fmd_fname);

int FuelModelInitRothFuelModelUnBurnable(FuelModel * fm);

int FuelModelInitPhysFuelModel(FuelModel * fm, char * fmd_fname);

int FuelModelInitPhysFuelModelUnBurnable(FuelModel * fm);
	
FuelModel * InitFuelModelEmpty(int fm_num, char * fm_name, char * fm_desc)	{
	FuelModel * fm = NULL;
	
	if ( (fm = (FuelModel *) malloc(sizeof(FuelModel))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for FuelModel. \n", ERR_ENOMEM);
		return fm;
		}

	/* assign model number */
	fm->model_num = fm_num;

	/* assign model name */
	if ( fm_name != NULL )	{
		if ( (fm->model_name = (char *) malloc(sizeof(char) * (strlen(fm_name) + 1))) != NULL )	{
			strcpy(fm->model_name, fm_name);
			}
		else	{
			ERR_ERROR_CONTINUE("Unable to allocate memory for FuelModel name. \n", ERR_ENOMEM);
			fm->model_name = NULL;		
			}
		}
	else	{
		if ( (fm->model_name = (char *) malloc(sizeof(char) * (strlen(FUELMODEL_UNNAMED_MODEL) + 1))) != NULL )	{
			strcpy(fm->model_name, FUELMODEL_UNNAMED_MODEL);
			}
		else	{
			ERR_ERROR_CONTINUE("Unable to allocate memory for FuelModel name. \n", ERR_ENOMEM);
			fm->model_name = NULL;		
			}
		}

	/* assign model description */
	if ( fm_desc != NULL )	{
		if ( (fm->model_desc = (char *) malloc(sizeof(char) * (strlen(fm_desc) + 1))) != NULL )	{
			strcpy(fm->model_desc, fm_desc);
			}
		else	{
			ERR_ERROR_CONTINUE("Unable to allocate memory for FuelModel description. \n", ERR_ENOMEM);
			fm->model_desc = NULL;
			}
		}
	else	{
		fm->model_desc = NULL;
		}
	
	/* assign unknown fuel model type */
	fm->type = EnumUnknownFuelModelType;
	
	/* NULL any other members */
	fm->rfm = NULL;
	fm->pfm = NULL;
	
	return fm;
	}
	
FuelModel * InitFuelModelFMDFile(int fm_num, char * fm_name, char * fm_desc, char * fmd_fname, FuelModelType type)	{
	FuelModel * fm = NULL;
	if ( (fm = InitFuelModelEmpty(fm_num, fm_name, fm_desc)) != NULL )	{
		switch(type)	{
			case EnumRoth:	
				if ( FuelModelInitRothFuelModel(fm, fmd_fname) )	{
					ERR_ERROR_CONTINUE("Unable to initialize RothFuelModel from FMD File. \n", ERR_EFAILED);
					FreeFuelModel(fm);
					fm = NULL;
					}
				break;
			case EnumPhys:
				if ( FuelModelInitPhysFuelModel(fm, fmd_fname) )	{
					ERR_ERROR_CONTINUE("Unable to initialize PhysFuelModel from FMD File. \n", ERR_EFAILED);
					FreeFuelModel(fm);
					fm = NULL;
					}			
				break;
			default:
				ERR_ERROR_CONTINUE("Must specify FuelModelType to initialize FuelModel. \n", ERR_EFAILED);
				FreeFuelModel(fm);
				fm = NULL;			
				break;
			}
		}
	return fm;
	}

FuelModel * InitFuelModelUnBurnable(int fm_num, char * fm_name, char * fm_desc, FuelModelType type)	{
	FuelModel * fm = NULL;
	if ( (fm = InitFuelModelEmpty(fm_num, fm_name, fm_desc)) != NULL )	{
		switch(type)	{
			case EnumRoth:
				if ( FuelModelInitRothFuelModelUnBurnable(fm) )	{
					ERR_ERROR_CONTINUE("Unable to initialize RothFuelModel as unburnable. \n", ERR_EFAILED);
					FreeFuelModel(fm);
					fm = NULL;			
					}
				break;
			case EnumPhys:
				if ( FuelModelInitPhysFuelModelUnBurnable(fm) )	{
					ERR_ERROR_CONTINUE("Unable to initialize PhysFuelModel as unburnable. \n", ERR_EFAILED);
					FreeFuelModel(fm);
					fm = NULL;			
					}			
				break;
			default:
				ERR_ERROR_CONTINUE("Must specify FuelModelType to initialize FuelModel. \n", ERR_EFAILED);
				FreeFuelModel(fm);
				fm = NULL;				
				break;
			}
		}
	return fm;
	}

/*
 * Description:
 * Initializes the RothFuelModel member of the FuelModel structure with attributes from an .FMD file.
 *
 * Arguments:
 * fm- ptr to initialized FuelModel
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	
int FuelModelInitRothFuelModel(FuelModel * fm, char * fmd_fname)	{
	FILE * fstream;
	
	/* check args */
	if ( fm == NULL )	{
		ERR_ERROR("FuelModel must be initialized before RothFuelModel attributes. \n", ERR_EINVAL);
		}
	if ( (fmd_fname != NULL ) && ((fstream = fopen(fmd_fname, "r")) != NULL ) )	{
		if ( (fm->rfm = InitRothFuelModelFMDFile(fstream, fm->model_num)) == NULL )		{
			fclose(fstream);
			ERR_ERROR("Unable to intialize RothFuelModel attributes in FuelModel. \n", ERR_EBADFUNC);
			}
		}
	else	{
		ERR_ERROR("Unable to open stream to fmd file for RothFuelModel attributes. \n", ERR_EINVAL);
		}
	
	fclose(fstream);
	fm->type = EnumRoth;
	
	return ERR_SUCCESS;
	}

/*
 * Description:
 * Initializes the RothFuelModel member of the FuelModel structure as unburnable fuel.
 *
 * Arguments:
 * fm- ptr to initialized FuelModel
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	
int FuelModelInitRothFuelModelUnBurnable(FuelModel * fm)	{
	/* check args */
	if ( fm == NULL )	{
		ERR_ERROR("FuelModel must be initialized before RothFuelModel attributes. \n", ERR_EINVAL);
		}
		
	fm->rfm = InitRothFuelModelUnBurnable();
	fm->type = EnumRoth;

	return ERR_SUCCESS;
	}

/*
 * Description:
 * Initializes the PhysFuelModel member of the FuelModel structure with attributes from an .FMD file.
 *
 * Arguments:
 * fm- ptr to initialized FuelModel
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	
int FuelModelInitPhysFuelModel(FuelModel * fm, char * fmd_fname)	{
	FILE * fstream;
	
	/* check args */
	if ( fm == NULL )	{
		ERR_ERROR("FuelModel must be initialized before PhysFuelModel attributes. \n", ERR_EINVAL);
		}
	if ( (fmd_fname != NULL ) && ((fstream = fopen(fmd_fname, "r")) != NULL ) )	{
		if ( (fm->pfm = InitPhysFuelModelFMDFile(fstream, fm->model_num)) == NULL )		{
			fclose(fstream);
			ERR_ERROR("Unable to intialize PhysFuelModel attributes in FuelModel. \n", ERR_EBADFUNC);
			}
		}
	else	{
		ERR_ERROR("Unable to open stream to fmd file for PhysFuelModel attributes. \n", ERR_EINVAL);
		}
	
	fclose(fstream);
	fm->type = EnumPhys;
	
	return ERR_SUCCESS;
	}

/*
 * Description:
 * Initializes the PhysFuelModel member of the FuelModel structure as unburnable fuel.
 *
 * Arguments:
 * fm- ptr to initialized FuelModel
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	
int FuelModelInitPhysFuelModelUnBurnable(FuelModel * fm)	{
	/* check args */
	if ( fm == NULL )	{
		ERR_ERROR("FuelModel must be initialized before PhysFuelModel attributes. \n", ERR_EINVAL);
		}
		
	fm->pfm = InitPhysFuelModelUnBurnable();
	fm->type = EnumPhys;

	return ERR_SUCCESS;
	}

int CmpNumToFuelModelNum(const void * fm_num, const void * fm)	{
	return (*(const int *)fm_num == ((FuelModel *)fm)->model_num);
	}
	
void FuelModelDumpToStream(FuelModel * fm, FILE * fstream)	{
	if ( fm == NULL || fstream == NULL )	{
		ERR_ERROR_CONTINUE("Unable to dump FuelModel. \n", ERR_EINVAL);
		return;
		}

	fprintf(fstream, 	"\n...Fuel Model Dump...\n");
	fprintf(fstream, 	"MODEL NUM: %d \n", fm->model_num);
	if ( fm->model_name != NULL )
		fprintf(fstream,"MODEL NAME: %s \n", fm->model_name);
	else
		fprintf(fstream,"MODEL NAME: <NOT INITIALIZED> \n");
	if ( fm->model_desc != NULL )
		fprintf(fstream,"MODEL DESCRIPTION: %s \n", fm->model_desc);
	else
		fprintf(fstream,"MODEL DESCRIPTION: <NOT INITIALIZED> \n");
	switch(fm->type)	{
		case EnumRoth:			
			if ( fm->rfm != NULL )
				RothFuelModelDumpToStream(fm->rfm, fstream);
			break;
		case EnumPhys:
			if ( fm->pfm != NULL )
				PhysFuelModelDumpToStream(fm->pfm, fstream);
			break;
		default:
			fprintf(fstream, "FUEL MODEL ATTRIBUTES NOT INITIALIZED \n");
			break;
		}
	fprintf(fstream, 	"\n");

	return;
	}

 void FreeFuelModel(void * vptr)	{
 	FuelModel * fm = NULL;
 	if ( vptr != NULL )	{
 		fm = (FuelModel *) vptr;
 		if ( fm->model_name != NULL )	{
 			free(fm->model_name);
 			} 			
 		if ( fm->model_desc != NULL )	{
 			free(fm->model_desc);
 			} 
  		if ( fm->rfm != NULL )	{
 			FreeRothFuelModel(fm->rfm);
 			} 
 		if ( fm->pfm != NULL )	{
 			FreePhysFuelModel(fm->pfm);
 			}
 		free(fm);
 		} 	
 	fm = NULL;
 	return;	
 	}

/* end of FuelModel.c */
