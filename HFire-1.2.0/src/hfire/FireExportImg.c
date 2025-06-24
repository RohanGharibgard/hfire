/*! 
 * \file FireExportImg.c
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
 
#include "FireExportImg.h"

int FireExportFireIDPng(ChHashTable * proptbl, FireYear * fyr, FireTimer * ft)	{
	KeyVal * png_dir, * icm_fname, * img_w, * img_h, * titl_txt, * titl_fnt, * titl_pos;	/* key/val props */
	GridData * gd 										= NULL;								/* fire year wrapper */
	gdFontPtr tfont										= gdFontTiny;						/* title font */	
	EnumPosition tpos									= EnumURPosition;					/* title position */
	char png_fname[FIRE_EXPORT_IMG_DEFAULT_FILENAME_SIZE]	= {'\0'};
	char title[FIRE_EXPORT_IMG_DEFAULT_TITLE_SIZE]			= {'\0'};
	int * red 											= NULL;
	int * green 										= NULL;
	int * blue 											= NULL;
	int icm_size, mt = 0;	
		
	/* check args */
	if ( proptbl == NULL  || ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFPDIR), (void *)&png_dir) )	{
		ERR_ERROR("Unable to retrieve EXPORT_FIRE_ID_PNG_DIRECTORY property. \n", ERR_EINVAL);
		}

	/* return if no export options specified */
	if	( strcmp(png_dir->val, GetFireVal(VAL_NULL)) == 0)	{
		return ERR_SUCCESS;
		}
		
	/* retrieve remaining export args */
	if ( ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFPICMF), (void *)&icm_fname)	
		|| 	ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFPWID), (void *)&img_w)
		|| 	ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFPHGT), (void *)&img_h)
		||  ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFPTTXT), (void *)&titl_txt)
		||  ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFPTF), (void *)&titl_fnt)
		||  ChHashTableRetrieve(proptbl, GetFireProp(PROP_EXPFPTP), (void *)&titl_pos) )	{
		ERR_ERROR("All EXPORT_FIRE_ID_PNG* properties must be specified when exporting. \n", ERR_EINVAL);
		}
		
	/* generate name for output png file of format fidYYYYMMDDHHHH.png */
	mt = FIRE_TIMER_GET_MILITARY_TIME(ft);
	#ifdef USING_PC
	sprintf(png_fname, "%s\\fid%d%02d%02d%04d.png", png_dir->val, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt);
	#endif
	#ifdef USING_UNIX
	sprintf(png_fname, "%s//fid%d%02d%02d%04d.png", png_dir->val, ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt);
	#endif
	
	/* wrap FireYear as GridData to export */
	if ( (gd = InitGridDataFromIntTwoDArray(fyr->id, fyr->xllcorner, fyr->yllcorner,
	 											fyr->cellsize, FIRE_YEAR_ID_UNBURNABLE)) == NULL )	{
		ERR_ERROR("Unable to load FireYear as GridData. \n", ERR_EINVAL);
		}
		
	/* retrieve index color model */
	if ( GridDataImgGetIndexColorModelFromFile((char *) icm_fname->val, &red, &green, &blue, &icm_size) )	{
		ERR_ERROR("Unable to load index color model from file. \n", ERR_EINVAL);
		}
	
	/* retrieve title text */
	if ( strcmp(titl_txt->val, GetFireVal(VAL_NULL)) == 0 )	{
		sprintf(title, "%d%02d%02d%04d", ft->sim_cur_yr, ft->sim_cur_mo, ft->sim_cur_dy, mt);
		}
	else	{
		strcpy(title, titl_txt->val);
		}
			
	/* retrieve title font */
	if ( strcmp(titl_fnt->val, GetFireVal(VAL_GIANT)) == 0 )	{
		tfont = gdFontGiant;
		}
	else if ( strcmp(titl_fnt->val, GetFireVal(VAL_LARGE)) == 0 )	{
		tfont = gdFontLarge;
		}
	else if ( strcmp(titl_fnt->val, GetFireVal(VAL_MEDBOLD)) == 0 )	{
		tfont = gdFontMediumBold;
		}
	else if ( strcmp(titl_fnt->val, GetFireVal(VAL_SMALL)) == 0 )	{
		tfont = gdFontSmall;
		}	
	else	{		/* default */
		tfont = gdFontTiny;
		}

	/* retrieve title position */
	if ( strcmp(titl_pos->val, GetFireVal(VAL_UL)) == 0 )	{
		tpos = EnumULPosition;
		}
	else if ( strcmp(titl_pos->val, GetFireVal(VAL_LL)) == 0 )	{
		tpos = EnumLLPosition;
		}
	else if ( strcmp(titl_pos->val, GetFireVal(VAL_LR)) == 0 )	{	
		tpos = EnumLRPosition;
		}
	else	{		/* default */
		tpos = EnumURPosition;
		}
	
	/* export png file  (default title color and no data colors) */
	if ( GridDataImgExportIndexColorModelPng(gd, png_fname, atoi(img_w->val), atoi(img_h->val),
			title, tfont, tpos, 			
			RGBColorsGetBasicColorRed(FIRE_EXPORT_IMG_DEFAULT_TITLE_COLOR),
			RGBColorsGetBasicColorGreen(FIRE_EXPORT_IMG_DEFAULT_TITLE_COLOR), 
			RGBColorsGetBasicColorBlue(FIRE_EXPORT_IMG_DEFAULT_TITLE_COLOR),
			RGBColorsGetBasicColorRed(FIRE_EXPORT_IMG_DEFAULT_NODATA_COLOR),
			RGBColorsGetBasicColorGreen(FIRE_EXPORT_IMG_DEFAULT_NODATA_COLOR), 
			RGBColorsGetBasicColorBlue(FIRE_EXPORT_IMG_DEFAULT_NODATA_COLOR),								 
			red, green, blue, icm_size) )	{
		FreeGridData(gd);
		free(red);
		free(green);
		free(blue);			
		ERR_ERROR("Unable to export PNG file of Fire IDs in function FireExportFireIDPng. \n", ERR_EBADFUNC);
		}
					
	/* free memory */
	FreeGridData(gd);
	free(red);
	free(green);
	free(blue);
		
	return ERR_SUCCESS;
	}
 
/* end of FireExportImg.c */
