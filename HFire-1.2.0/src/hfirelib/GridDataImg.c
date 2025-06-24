#include "GridDataImg.h"

/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of the GridData as a png image file
 * Available gdFontPtr fonts:
 *	{gdFontTiny, gdFontSmall, gdFontMediumBold, gdFontLarge, gdFontGiant}
 * any can be used as argument to title_font
 * Output image can be resized relative to input GridData by supplying
 * image width and height parameters different from GridData row and columns.
 *
 * Arguments:
 * gd- GridData with underlying two-dimensional array of values
 * png_fname- filename of exported png file
 * imgW- width of output image, in pixels
 * imgH- height of output image, in pixels
 * title_text- text to display on image (nothing if NULL)
 * title_font- font for title text
 * title_pos- enumeration of text positions
 * title_r- red component of RGB value of title text
 * title_g- green component of RGB value of title text
 * title_b- blue component of RGB value of title text 
 * nodata_r- red compoenent of RGB value of nodata cells in GridData
 * nodata_g- green compoenent of RGB value of nodata cells in GridData
 * nodata_b- blue compoenent of RGB value of nodata cells in GridData 
 * red- index colormap of red values to assign to each gd attribute
 * green- index colormap of green values to assign to each gd attribute 
 * blue- index colormap of blue values to assign to each gd attribute 
 * icm_size- index colormap size (size of red, green, blue arrays)
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 
int GridDataImgExportIndexColorModelPng(GridData * gd, char * png_fname, int imgW, int imgH,
		char * title_text, gdFontPtr title_font, EnumPosition title_pos, int title_r, int title_g, int title_b,
		int nodata_r, int nodata_g, int nodata_b, int * red, int * green, int * blue, int icm_size)	{
	gdImagePtr im_org 	= NULL;
	gdImagePtr im_resz	= NULL;
	FILE * out		= NULL;
	int i, j, tx, ty, data, title_clr, nodata_clr;
	int * icm;
	
	/* check args */
	if ( gd == NULL || png_fname == NULL ) 	{
		ERR_ERROR("Arguments supplied to GridDataImgExportIndexColorModelPng invalid. \n", ERR_EINVAL);
		}	
	
	/* create gd format image */
	if ( (im_org = gdImageCreate(gd->ghdr->ncols, gd->ghdr->nrows)) == NULL )	{
		ERR_ERROR("Unable to create gdImagePtr. GridDataImgExportIndexColorModelPng failed. \n", ERR_ENOMEM);
		}
	/* allocate memory for index color model */
	if ( (icm = (int *) malloc(sizeof(int) * icm_size)) == NULL ) 	{
		gdImageDestroy(im_org);
		ERR_ERROR("Color model table memory not allocated. GridDataImgExportIndexColorModelPng failed. \n", ERR_ENOMEM);
		}
	
	/* allocate colors */
	nodata_clr 	= gdImageColorAllocate(im_org, nodata_r, nodata_g, nodata_b);
	for(i = 0; i < icm_size; i++)	{
		icm[i] = gdImageColorAllocate(im_org, red[i], green[i], blue[i]);
		}

	/* set color inside gd image */
   	for(i = 0; i < gd->ghdr->nrows; i++)	{
   		for(j = 0; j < gd->ghdr->ncols; j++)	{
   			GRID_DATA_GET_DATA(gd, i, j, data);   			
   			if ( data == gd->ghdr->NODATA_value || data < 0 )	{
   				gdImageSetPixel(im_org, j, i, nodata_clr);
   				}
   			else	{
	   			gdImageSetPixel(im_org, j, i, icm[data]);
	   			}
   			}
   		}

   	/* assign title location */
   	switch(title_pos)	{
   		case EnumULPosition:
   			tx = imgW - (imgW * 0.9);
   			ty = imgH * 0.1;
   			break;
   		case EnumLLPosition:
   			tx = imgW - (imgW * 0.9);
   			ty = imgH * 0.9;
   			break;
   		case EnumLRPosition:
   			tx = imgW - (imgW * 0.35);
   			ty = imgH * 0.9;
   			break;
   		case EnumURPosition:
   			tx = imgW - (imgW * 0.35);
   			ty = imgH * 0.1; 		
   			break;
   		case EnumCTRPostion:
   		default:
   			tx = imgW - (imgW * 0.65);
   			ty = imgH * 0.5;
   			break;
   		}
		   				
	/* resize if necessary */
	if ( gd->ghdr->ncols != imgW || gd->ghdr->nrows != imgH )	{
		/* create image at new size and copy pixels */
		if ( (im_resz = gdImageCreate(imgW, imgH)) == NULL )	{
			free(icm);
			gdImageDestroy(im_org);
			ERR_ERROR("Unable to create gdImagePtr. GridDataImgExportIndexColorModelPng failed. \n", ERR_ENOMEM);
			}
		gdImageCopyResized(im_resz, im_org, 0, 0, 0, 0, imgW, imgH, gd->ghdr->ncols, gd->ghdr->nrows);
		
		/* add title in resized dimensions */
		if ( title_text != NULL )	{
		   	title_clr = gdImageColorAllocate(im_resz, title_r, title_g, title_b);   	
		   	gdImageString(im_resz, title_font, tx, ty, (unsigned char *) title_text, title_clr);
			}
				
		/* write resized output image */
   		if ( (out = fopen(png_fname, "wb")) == NULL )	{
			free(icm);
 			gdImageDestroy(im_org);
 			gdImageDestroy(im_resz);   		
   			ERR_ERROR("Unable to open output png file. GridDataImgExportIndexColorModelPng failed. \n", ERR_EIOFAIL);
   			}
 		gdImagePng(im_resz, out);
		fclose(out);
		gdImageDestroy(im_resz);		
		}
	else	{
		/* add title in unresized dimensions */
		if ( title_text != NULL )	{  
			title_clr = gdImageColorAllocate(im_org, title_r, title_g, title_b);		 	
		   	gdImageString(im_org, title_font, tx, ty, (unsigned char *) title_text, title_clr);
			}		

		/* write output image */
	   	if ( (out = fopen(png_fname, "wb")) == NULL )	{
			free(icm);
	 		gdImageDestroy(im_org);   		
	   		ERR_ERROR("Unable to open output png file. GridDataImgExportIndexColorModelPng failed. \n", ERR_EIOFAIL);
	   		}
	 	gdImagePng(im_org, out);
		fclose(out);
		}
		
	/* free memory */
	free(icm);
 	gdImageDestroy(im_org); 
	
	return ERR_SUCCESS;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Retrieves the rgb values to be used in generating an index color model
 * for GridData.  The format of the icm file is as follows:
 * 	numeric_val=red:green:blue
 * 	eg
 * 	12=255:255:255
 * All cells in the GridData structure with an attribute value of 12 will
 * be assign a color of 255,255,255 in the output image.
 * Negative value attributes cannot be specified inside of an icm file.
 *
 * Arguments:
 * icm_file- C-style string ('\0' terminated array of characters) pointing to icm file
 * red- index colormap of red values to assign to each gd attribute
 * green- index colormap of green values to assign to each gd attribute 
 * blue- index colormap of blue values to assign to each gd attribute 
 * icm_size- index colormap size (size of red, green, blue arrays)
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	
int GridDataImgGetIndexColorModelFromFile(char * icm_file, int ** red, int ** green, int ** blue, int * icm_size)	{
	FILE * fstream			= NULL;
	IntTwoDArray * rgb_arr	= NULL;
	int i;
	
	/* check args */
	if ( icm_file == NULL )	{
		ERR_ERROR("Unable to determine icm_file filname. \n", ERR_EINVAL);
		}
	
	/* open stream to icm_file */
	if ( (fstream = fopen(icm_file, "r")) == NULL )	{
		ERR_ERROR("Unable to open icm_file. \n", ERR_EIOFAIL);
		}
		
	/* read indexes from file */
	if ( (rgb_arr = GetIntTwoDArrayTableFStreamIO(fstream, GRIDDATA_IMG_ICM_SEP_CHAR,
	 									GRIDDATA_IMG_ICM_COMMENT_CHAR)) == NULL )	{
	 	fclose(fstream);
		ERR_ERROR("Unable to retrieve two dimensional array of rgb values from icm_file. \n", ERR_EBADFUNC);
		}
	fclose(fstream);
	
	/* find the max attribute value in the index */
	*icm_size = 0;
	for(i = 0; i < INTTWODARRAY_SIZE_ROW(rgb_arr); i++)	{
		if ( INTTWODARRAY_GET_DATA(rgb_arr, i, 0) > *icm_size )	{
			*icm_size = INTTWODARRAY_GET_DATA(rgb_arr, i, 0);
			}
		}
	*icm_size = *icm_size + 1;
	
	/* allocate memory for indexes */	
	if ( (*red = (int *) calloc((*icm_size), sizeof(int))) == NULL )	{
		ERR_ERROR("Unable to allocate memory for red index. \n", ERR_ENOMEM);
		}		
	if ( (*green = (int *) calloc((*icm_size), sizeof(int))) == NULL )	{
		free(*red);
		ERR_ERROR("Unable to allocate memory for green index. \n", ERR_ENOMEM);
		}
	if ( (*blue = (int *) calloc((*icm_size), sizeof(int))) == NULL )	{
		free(*red);
		free(*green);
		ERR_ERROR("Unable to allocate memory for blue index. \n", ERR_ENOMEM);
		}		
		
	/* insert indexes into arrays */
	for(i = 0; i < INTTWODARRAY_SIZE_ROW(rgb_arr); i++)	{
		(*red)[INTTWODARRAY_GET_DATA(rgb_arr, i, 0)] 	= INTTWODARRAY_GET_DATA(rgb_arr, i, 1);
		(*green)[INTTWODARRAY_GET_DATA(rgb_arr, i, 0)] 	= INTTWODARRAY_GET_DATA(rgb_arr, i, 2);
		(*blue)[INTTWODARRAY_GET_DATA(rgb_arr, i, 0)] 	= INTTWODARRAY_GET_DATA(rgb_arr, i, 3);
		}
	
	FreeIntTwoDArray(rgb_arr);
			
	return ERR_SUCCESS;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of the GridData as a jpeg image file.
 * Available gdFontPtr fonts:
 *	{gdFontTiny, gdFontSmall, gdFontMediumBold, gdFontLarge, gdFontGiant}
 * any can be used as argument to title_font
 * Output image can be resized relative to input GridData by supplying
 * image width and height parameters different from GridData row and columns.
 *
 * Arguments:
 * gd- GridData with underlying two-dimensional array of values
 * jpg_fname- filename of exported jpeg file
 * imgW- width of output image, in pixels
 * imgH- height of output image, in pixels
 * title_text- text to display on image (nothing if NULL)
 * title_font- font for title text
 * title_pos- enumeration of text positions
 * title_r- red component of RGB value of title text
 * title_g- green component of RGB value of title text
 * title_b- blue component of RGB value of title text 
 * nodata_r- red compoenent of RGB value of nodata cells in GridData
 * nodata_g- green compoenent of RGB value of nodata cells in GridData
 * nodata_b- blue compoenent of RGB value of nodata cells in GridData
 * intervals- cell value attributes (potentially floats) defining breakpoints between colors 
 * red- index colormap of red values to assign to each gd attribute
 * green- index colormap of green values to assign to each gd attribute 
 * blue- index colormap of blue values to assign to each gd attribute 
 * dcm_size- direct color model size (size of red, green, blue arrays)
 * quality- integer specifying image quality (typically 0-95), if value < 0 default is used
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 
int GridDataImgExportDirectColorModelJpeg(GridData * gd, char * jpg_fname, int imgW, int imgH,
		char * title_text, gdFontPtr title_font, EnumPosition title_pos, int title_r, int title_g, int title_b,
		int nodata_r, int nodata_g, int nodata_b, 
		double * intervals, int * red, int * green, int * blue, int dcm_size, int quality)	{
	gdImagePtr im_org 	= NULL;
	gdImagePtr im_resz	= NULL;
	FILE * out		= NULL;
	int i, j, c, tx, ty, title_clr, nodata_clr;
	double data;
	int * dcm;
	
	/* check args */
	if ( gd == NULL || jpg_fname == NULL ) 	{
		ERR_ERROR("Arguments supplied to GridDataImgExportDirectColorModelJpeg invalid. \n", ERR_EINVAL);
		}	
	
	/* create gd format image */
	if ( (im_org = gdImageCreate(gd->ghdr->ncols, gd->ghdr->nrows)) == NULL )	{
		ERR_ERROR("Unable to create gdImagePtr. GridDataImgExportDirectColorModelJpeg failed. \n", ERR_ENOMEM);
		}
	/* allocate memory for direct color model */
	if ( (dcm = (int *) malloc(sizeof(int) * dcm_size)) == NULL ) 	{
		gdImageDestroy(im_org);
		ERR_ERROR("Color model table memory not allocated. GridDataImgExportDirectColorModelJpeg failed. \n", ERR_ENOMEM);
		}
	
	/* allocate colors */
	nodata_clr 	= gdImageColorAllocate(im_org, nodata_r, nodata_g, nodata_b);
	for(i = 0; i < dcm_size; i++)	{
		dcm[i] = gdImageColorAllocate(im_org, red[i], green[i], blue[i]);
		}

	/* set color inside gd image */
   	for(i = 0; i < gd->ghdr->nrows; i++)	{
   		for(j = 0; j < gd->ghdr->ncols; j++)	{
   			/* retrieve attribute of pixel */
   			GRID_DATA_GET_DATA(gd, i, j, data);
   			/* set NO DATA */
   			if ( GRIDDATA_IMG_FP_ARE_EQUAL(data,gd->ghdr->NODATA_value) )	{
   				gdImageSetPixel(im_org, j, i, nodata_clr);
   				}
   			/* find the attributes position in intervals array */
   			else	{
	   			for(c = dcm_size - 1; c >= 0; c--)	{
   					if ( data > intervals[c] )	{
   						break;
   						}
   					}
   				/* attribute value not found in intervals, assign default */
   				if ( c < 0 )	{
   					gdImageSetPixel(im_org, j, i, nodata_clr);
   					}
   				/* else assign color */
   				else	{
	   				gdImageSetPixel(im_org, j, i, dcm[c]);
	   				}
   				}
   			}
   		}

   	/* assign title location */
   	switch(title_pos)	{
   		case EnumULPosition:
   			tx = imgW - (imgW * 0.9);
   			ty = imgH * 0.1;
   			break;
   		case EnumLLPosition:
   			tx = imgW - (imgW * 0.9);
   			ty = imgH * 0.9;
   			break;
   		case EnumLRPosition:
   			tx = imgW - (imgW * 0.35);
   			ty = imgH * 0.9;
   			break;
   		case EnumURPosition:
   			tx = imgW - (imgW * 0.35);
   			ty = imgH * 0.1; 		
   			break;
   		case EnumCTRPostion:
   		default:
   			tx = imgW - (imgW * 0.65);
   			ty = imgH * 0.5;
   			break;
   		}
		   				
	/* resize if necessary */
	if ( gd->ghdr->ncols != imgW || gd->ghdr->nrows != imgH )	{
		/* create image at new size and copy pixels */
		if ( (im_resz = gdImageCreate(imgW, imgH)) == NULL )	{
			free(dcm);
			gdImageDestroy(im_org);
			ERR_ERROR("Unable to create gdImagePtr. GridDataImgExportDirectColorModelJpeg failed. \n", ERR_ENOMEM);
			}
		gdImageCopyResized(im_resz, im_org, 0, 0, 0, 0, imgW, imgH, gd->ghdr->ncols, gd->ghdr->nrows);
		
		/* add title in resized dimensions */
		if ( title_text != NULL )	{
		   	title_clr = gdImageColorAllocate(im_resz, title_r, title_g, title_b);   	
		   	gdImageString(im_resz, title_font, tx, ty, (unsigned char *) title_text, title_clr);
			}
				
		/* write resized output image */
   		if ( (out = fopen(jpg_fname, "wb")) == NULL )	{
			free(dcm);
 			gdImageDestroy(im_org);
 			gdImageDestroy(im_resz);   		
   			ERR_ERROR("Unable to open output jpg file. GridDataImgExportDirectColorModelJpeg failed. \n", ERR_EIOFAIL);
   			}
 		gdImageJpeg(im_resz, out, quality);
		fclose(out);
		gdImageDestroy(im_resz);		
		}
	else	{
		/* add title in unresized dimensions */
		if ( title_text != NULL )	{  
			title_clr = gdImageColorAllocate(im_org, title_r, title_g, title_b);		 	
		   	gdImageString(im_org, title_font, tx, ty, (unsigned char *) title_text, title_clr);
			}		

		/* write output image */
	   	if ( (out = fopen(jpg_fname, "wb")) == NULL )	{
			free(dcm);
	 		gdImageDestroy(im_org);   		
	   		ERR_ERROR("Unable to open output jpg file. GridDataImgExportDirectColorModelJpeg failed. \n", ERR_EIOFAIL);
	   		}
	 	gdImageJpeg(im_org, out, quality);
		fclose(out);
		}
		
	/* free memory */
	free(dcm);
 	gdImageDestroy(im_org); 
	
	return ERR_SUCCESS;		
	}
		
/*
 * Visibility:
 * global
 *
 * Description:
 * Retrieves the rgb values to be used in generating a direct color model image
 * from GridData.  The format of the dcm file is as follows:
 * 	float_val=red:green:blue
 * 	eg
 * 	0.1=255:255:255
 *	0.4=225:118:225
 * All values must be listed in increasing order.
 * Assuming 0.1 is the first record in the dcm file, all cells in the GridData 
 * structure with:
 * 		0.1 <= cell_value < 0.4
 * will be assigned a color of 255,255,255
 * 		0.4 <= cell_value 
 * will be assigned a color of 225,118,225.
 * Unlike the index color model, where attribute values are used to index directly
 * into the array of rgb colors, the assignment of colors in a direct color model
 * requires that each GridData cell value be compared to values in the intervals array.
 * As a result the efficiency of the indexing process for direct color model images
 * decreases as the number of values in the intervals array increases.
 *
 * Arguments:
 * dcm_file- C-style string ('\0' terminated array of characters) pointing to dcm file
 * intervals- cell value attributes (potentially floats) defining breakpoints between colors
 * red- red values to assign to each gd attribute
 * green- green values to assign to each gd attribute 
 * blue- blue values to assign to each gd attribute 
 * dcm_size- index colormap size (size of red, green, blue arrays)
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	
int GridDataImgGetDirectColorModelFromFile(char * dcm_file, double ** intervals, 
													int ** red, int ** green, int ** blue, int * dcm_size)	{
	FILE * fstream			= NULL;
	DblTwoDArray * rgb_arr	= NULL;
	int i;
	
	/* check args */
	if ( dcm_file == NULL )	{
		ERR_ERROR("Unable to determine dcm_file filname. \n", ERR_EINVAL);
		}
	
	/* open stream to dcm_file */
	if ( (fstream = fopen(dcm_file, "r")) == NULL )	{
		ERR_ERROR("Unable to open dcm_file. \n", ERR_EIOFAIL);
		}
		
	/* read indexes from file */
	if ( (rgb_arr = GetDblTwoDArrayTableFStreamIO(fstream, GRIDDATA_IMG_DCM_SEP_CHAR,
	 									GRIDDATA_IMG_DCM_COMMENT_CHAR)) == NULL )	{
	 	fclose(fstream);
		ERR_ERROR("Unable to retrieve two dimensional array of rgb values from dcm_file. \n", ERR_EBADFUNC);
		}
	fclose(fstream);
	
	/* allocate memory for attribute breakpoints defined in file */
	*dcm_size = DBLTWODARRAY_SIZE_ROW(rgb_arr);
	if ( (*intervals = (double *) calloc((*dcm_size), sizeof(double))) == NULL )	{
		ERR_ERROR("Unable to allocate memory for attribute intervals. \n", ERR_ENOMEM);
		}	
			
	/* allocate memory for indexes */	
	if ( (*red = (int *) calloc((*dcm_size), sizeof(int))) == NULL )	{
		free(*intervals);
		ERR_ERROR("Unable to allocate memory for red index. \n", ERR_ENOMEM);
		}		
	if ( (*green = (int *) calloc((*dcm_size), sizeof(int))) == NULL )	{
		free(*intervals);	
		free(*red);
		ERR_ERROR("Unable to allocate memory for green index. \n", ERR_ENOMEM);
		}
	if ( (*blue = (int *) calloc((*dcm_size), sizeof(int))) == NULL )	{
		free(*intervals);	
		free(*red);
		free(*green);
		ERR_ERROR("Unable to allocate memory for blue index. \n", ERR_ENOMEM);
		}		
		
	/* insert indexes into arrays */
	for(i = 0; i < DBLTWODARRAY_SIZE_ROW(rgb_arr); i++)	{
		(*intervals)[i]		= DBLTWODARRAY_GET_DATA(rgb_arr, i, 0);
		(*red)		[i] 	= (int) DBLTWODARRAY_GET_DATA(rgb_arr, i, 1);
		(*green)	[i] 	= (int) DBLTWODARRAY_GET_DATA(rgb_arr, i, 2);
		(*blue)		[i] 	= (int) DBLTWODARRAY_GET_DATA(rgb_arr, i, 3);
		}
	
	FreeDblTwoDArray(rgb_arr);
			
	return ERR_SUCCESS;	
	}

/* end of GridDataImg.c */
