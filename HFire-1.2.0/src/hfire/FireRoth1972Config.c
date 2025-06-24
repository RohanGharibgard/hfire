/*! 
 * \file FireRoth1972Config.c
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
 
#include "FireRoth1972Config.h"

int InitRothFuelModelListFromProps(ChHashTable * proptbl, List ** fmlist)	{
	FuelModel * fm			= NULL;				/* instance of a FuelModel */
	char * fmd_fname		= NULL;				/* name of fmd fuel values file */
	List * brn_list 		= NULL;				/* list of burnable model numbers */
	List * unbrn_list		= NULL;				/* list of unburnable model numbers */ 
	ListElmt * lel			= NULL;				/* single list element */
	KeyVal * entry			= NULL;				/* key/val instances from table */
	int * fmnum				= NULL;				/* fuel model number */

	if ( proptbl == NULL || (*fmlist = InitListEmpty(FreeFuelModel)) == NULL )	{
		ERR_ERROR("Unable to allocate memory for fuel model nums list. \n", ERR_ENOMEM);
		}
	
	/* ensure user specified RothFuelModel */	
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_FPTYP), (void *)&entry) ||
			(strcmp(entry->val, GetFireVal(VAL_ROTH)) != 0) )	{
		ERR_ERROR("FUEL_PROPS_TYPE property must be ROTH to create RothFuelModel. \n", ERR_EFAILED);
		}

	/* determine FMD_FILE */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_FPFMD), (void *)&entry) ||
	 		strcmp(entry->val, GetFireVal(VAL_NULL)) == 0 )	{
		FreeList(*fmlist);
		ERR_ERROR("Unable to retrieve FMD_FILE property for fuels data. \n", ERR_EFAILED);
		}
	fmd_fname = (char *) entry->val;	
										
	/* determine NUMS_IMPORT */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_FPIMP), (void *)&entry) )	{
		ERR_ERROR_CONTINUE("Unable to retrieve NUMS_IMPORT property for fuels data. \n", ERR_EFAILED);
		}
	brn_list = GetIntListFromLineFStreamIO((char *)entry->val, FIRE_ROTH1972_CONFIG_FUEL_SEP_CHARS);

	/* construct instances of BURNABLE FuelModels and place in fm list */
	lel = LIST_HEAD(brn_list);
	while (lel != NULL)	{
		fmnum = LIST_GET_DATA(lel);		
		if ( fmnum != NULL && (fm = InitFuelModelFMDFile(*fmnum, NULL, NULL, fmd_fname, EnumRoth)) != NULL )	{
			/* set the fuel bed properties */
			if ( Roth1972FireSpreadSetFuelBed(fm->rfm) )	{
				ERR_ERROR_CONTINUE("Fuels data not initialized. \n", ERR_EBADFUNC);
				}
			else	{
				ListInsertNext(*fmlist, NULL, fm);
				}
			}					
		lel = LIST_GET_NEXT_ELMT(lel);
		}
		
	/* must have at least one burnable fuel model to run simulation */
	if ( LIST_SIZE(*fmlist) < 1 )	{
		FreeList(*fmlist);
		ERR_ERROR("At least one BURNABLE FuelModel must be initialized to start simulation. \n", ERR_ESANITY);
		}
						
	/* determine NUMS_UNBURNABLE */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_FPUNB), (void *)&entry) )	{
		ERR_ERROR_CONTINUE("Unable to retrieve NUMS_UNBURNABLE property for fuels data. \n", ERR_EFAILED);
		}
	unbrn_list = GetIntListFromLineFStreamIO((char *)entry->val, FIRE_ROTH1972_CONFIG_FUEL_SEP_CHARS);

	/* construct instances of UNBURNABLE FuelModels and place in fm list */
	lel = LIST_HEAD(unbrn_list);
	while (lel != NULL)	{
		fmnum = LIST_GET_DATA(lel);		
		if ( fmnum != NULL && (fm = InitFuelModelUnBurnable(*fmnum, NULL, NULL, EnumRoth)) != NULL ) {
			/* set the fuel bed properties */
			if ( Roth1972FireSpreadSetFuelBed(fm->rfm) )	{
				ERR_ERROR_CONTINUE("Fuels data not initialized. \n", ERR_EBADFUNC);
				}
			else	{		
				ListInsertNext(*fmlist, NULL, fm);
				}
			}
		lel = LIST_GET_NEXT_ELMT(lel);
		}
	
	/* free memory */
	FreeList(brn_list);
	FreeList(unbrn_list);	
	 			
	return ERR_SUCCESS;
	}
	 
/* end of FireRothFuelModel.c */
