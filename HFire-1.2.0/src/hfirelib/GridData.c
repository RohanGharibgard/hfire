#include "GridData.h"

/* INITIALIZATION */
GridData * InitGridDataFromBinaryRaster(char * main_fname, char * header_fname, EnumGridType grid_type)	{
	GridData * gd = NULL;

	/* check args */	
	if ( main_fname == NULL || header_fname == NULL )	{
		ERR_ERROR_CONTINUE("Must supply a main and header filename to initialize GridData. \n", ERR_EINVAL);
		return gd;
		}
			
	/* STEP 1: initilize the GridData and GridFileInfo structures */
	if ( (gd = (GridData *) malloc(sizeof(GridData))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize GridData, memory allocation failed. \n", ERR_ENOMEM);
		return gd;
		}		
	if ( (gd->gfinfo = (GridFileInfo *) malloc(sizeof(GridFileInfo))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize GridFileInfo, memory allocation failed. \n", ERR_ENOMEM);
		free(gd);
		gd = NULL;
		return gd;
		}
		
	/* STEP 2: store main_file and header file info */
	gd->gfinfo->gfmt = EnumBinaryGrid;
	if( (gd->gfinfo->main_file = (char *) malloc(sizeof(char) * (strlen(main_fname) + 1))) != NULL
		&& (gd->gfinfo->header_file = (char *) malloc(sizeof(char) * (strlen(header_fname) + 1))) != NULL )	{
		strcpy(gd->gfinfo->main_file, main_fname);
		strcpy(gd->gfinfo->header_file, header_fname);
		}
	else	{
		ERR_ERROR_CONTINUE("Unable to initialize grid file names, memory allocation failed. \n", ERR_ENOMEM);		
		free(gd->gfinfo);
		free(gd);
		gd = NULL;
		return gd;
		}
		
	/* STEP 3: set the data type */
	gd->gtype = grid_type;
	if ( (gd->arr = (GridArray *) malloc(sizeof(GridArray))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize grid array, memory allocation failed. \n", ERR_ENOMEM);
		free(gd->gfinfo->main_file);
		free(gd->gfinfo->header_file);
		free(gd->gfinfo);
		free(gd);
		gd = NULL;
		return gd;
		}

	/* STEP 4: get the header info */
	if ( (gd->ghdr = InitGridHeaderInfoFromFile(gd->gfinfo)) == NULL )	{
		ERR_ERROR_CONTINUE("Failed to retrieve grid header info. \n", ERR_EFAILED);
		free(gd->arr);
		free(gd->gfinfo->main_file);
		free(gd->gfinfo->header_file);
		free(gd->gfinfo);
		free(gd);
		gd = NULL;
		return gd;
		}

	/* STEP 5: get the data */
	switch (gd->gtype)	{
		case EnumDblGrid:
			if ( (gd->arr->da = ImportBinaryRasterAsDblTwoDArray(gd->gfinfo, gd->ghdr)) == NULL )	{
				ERR_ERROR_CONTINUE("Unable to load array for GridData. \n", ERR_EFAILED);
				free(gd);
				gd = NULL;
				}
			break;
		case EnumFltGrid:
			if ( (gd->arr->fa = ImportBinaryRasterAsFltTwoDArray(gd->gfinfo, gd->ghdr)) == NULL )	{
				ERR_ERROR_CONTINUE("Unable to load array for GridData. \n", ERR_EFAILED);
				free(gd);
				gd = NULL;
				}			
			break;
		case EnumLIntGrid:
			if ( (gd->arr->lia = ImportBinaryRasterAsLIntTwoDArray(gd->gfinfo, gd->ghdr)) == NULL )	{
				ERR_ERROR_CONTINUE("Unable to load array for GridData. \n", ERR_EFAILED);
				free(gd);
				gd = NULL;
				}			
			break;						
		case EnumIntGrid:
			if ( (gd->arr->ia = ImportBinaryRasterAsIntTwoDArray(gd->gfinfo, gd->ghdr))	== NULL )	{
				ERR_ERROR_CONTINUE("Unable to load array for GridData. \n", ERR_EFAILED);
				free(gd);
				gd = NULL;
				}			
			break;
		case EnumByteGrid:
			if ( (gd->arr->ba = ImportBinaryRasterAsByteTwoDArray(gd->gfinfo, gd->ghdr)) == NULL )	{
				ERR_ERROR_CONTINUE("Unable to load array for GridData. \n", ERR_EFAILED);
				free(gd);
				gd = NULL;
				}			
			break;
		case EnumBitGrid:
			ERR_ERROR_CONTINUE("Data type for bit GridData not yet implemented. \n", ERR_EUNIMPL);
			free(gd);
			gd = NULL;		
			break;
		default:
			ERR_ERROR_CONTINUE("Data type for GridIO not recognized. \n", ERR_EINVAL);
			free(gd);
			gd = NULL;
			return gd;		
		}
			
	return gd;	
	}
	
GridData * InitGridDataFromAsciiRaster(char * main_fname, EnumGridType grid_type)		{
	GridData * gd = NULL;

	/* check args */	
	if ( main_fname == NULL )	{
		ERR_ERROR_CONTINUE("Must supply a main filename to initialize GridData. \n", ERR_EINVAL);
		return gd;
		}

	/* STEP 1: initilize the GridData and GridFileInfo structures */
	if ( (gd = (GridData *) malloc(sizeof(GridData))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize GridData, memory allocation failed. \n", ERR_ENOMEM);
		return gd;
		}		
	if ( (gd->gfinfo = (GridFileInfo *) malloc(sizeof(GridFileInfo))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize GridFileInfo, memory allocation failed. \n", ERR_ENOMEM);
		free(gd);
		gd = NULL;
		return gd;
		}
		
	/* STEP 2: store main_file info */
	gd->gfinfo->gfmt = EnumAsciiGrid;
	if( (gd->gfinfo->main_file = (char *) malloc(sizeof(char) * (strlen(main_fname) + 1))) != NULL )	{
		strcpy(gd->gfinfo->main_file, main_fname);
		}
	else	{
		ERR_ERROR_CONTINUE("Unable to initialize grid file names, memory allocation failed. \n", ERR_ENOMEM);
		free(gd->gfinfo);		
		free(gd);
		gd = NULL;
		return gd;
		}
	gd->gfinfo->header_file = NULL;
		
	/* STEP 3: set the data type */
	gd->gtype = grid_type;
	if ( (gd->arr = (GridArray *) malloc(sizeof(GridArray))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to initialize grid array, memory allocation failed. \n", ERR_ENOMEM);
		free(gd->gfinfo->main_file);
		free(gd->gfinfo->header_file);
		free(gd->gfinfo);		
		free(gd);
		gd = NULL;
		return gd;
		}

	/* STEP 4: get the header info */
	if ( (gd->ghdr = InitGridHeaderInfoFromFile(gd->gfinfo)) == NULL )	{
		ERR_ERROR_CONTINUE("Failed to retrieve grid header info. \n", ERR_EFAILED);
		free(gd->gfinfo->main_file);
		free(gd->gfinfo->header_file);
		free(gd->gfinfo);		
		free(gd);
		gd = NULL;
		return gd;
		}

	/* STEP 5: get the data */
	switch (gd->gtype)	{
		case EnumDblGrid:
			if ( (gd->arr->da = ImportAsciiRasterAsDblTwoDArray(gd->gfinfo, gd->ghdr)) == NULL )	{
				ERR_ERROR_CONTINUE("Unable to load array for GridData. \n", ERR_EFAILED);
				free(gd);
				gd = NULL;
				}			
			break;
		case EnumFltGrid:
			if ( (gd->arr->fa = ImportAsciiRasterAsFltTwoDArray(gd->gfinfo, gd->ghdr)) == NULL )	{
				ERR_ERROR_CONTINUE("Unable to load array for GridData. \n", ERR_EFAILED);
				free(gd);
				gd = NULL;
				}			
			break;
		case EnumLIntGrid:
			if ( (gd->arr->lia = ImportAsciiRasterAsLIntTwoDArray(gd->gfinfo, gd->ghdr)) == NULL )	{
				ERR_ERROR_CONTINUE("Unable to load array for GridData. \n", ERR_EFAILED);
				free(gd);
				gd = NULL;
				}			
			break;						
		case EnumIntGrid:
			if ( (gd->arr->ia = ImportAsciiRasterAsIntTwoDArray(gd->gfinfo, gd->ghdr)) == NULL )	{
				ERR_ERROR_CONTINUE("Unable to load array for GridData. \n", ERR_EFAILED);
				free(gd);
				gd = NULL;
				}			
			break;
		case EnumByteGrid:
			if ( (gd->arr->ba = ImportAsciiRasterAsByteTwoDArray(gd->gfinfo, gd->ghdr)) == NULL )	{
				ERR_ERROR_CONTINUE("Unable to load array for GridData. \n", ERR_EFAILED);
				free(gd);
				gd = NULL;
				}			
			break;
		case EnumBitGrid:
			ERR_ERROR_CONTINUE("Data type for bit GridData not yet implemented. \n", ERR_EUNIMPL);
			free(gd);
			gd = NULL;		
			break;			
		default:
			ERR_ERROR_CONTINUE("Data type for GridIO not recognized. \n", ERR_EINVAL);
			free(gd);
			gd = NULL;
			return gd;		
		}
			
	return gd;
	}
	
GridData * InitGridDataFromDblTwoDArray(DblTwoDArray * arr, double xll, double yll, int cellsz, int nodata)	{
	GridData * gd			= NULL;
	int r, c, ismsb = 0;
	
	/* check args */
	if ( arr == NULL )	{
		ERR_ERROR_CONTINUE("Must provide an initialized array to construct GridData. \n", ERR_EINVAL);
		return gd;
		}
	
	/* initialize GridData */
	if ( (gd = (GridData *) malloc(sizeof(GridData))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		return gd;
		}
	gd->gfinfo = NULL;
	if ( (gd->arr = (GridArray *) malloc(sizeof(GridArray))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridArray. \n", ERR_ENOMEM);
		free(gd);
		gd = NULL;
		return gd;
		}
	/* platform dependent byte-ordering */
	#ifdef USING_UNIX
	ismsb = 1;
	#endif
	#ifdef USING_PC
	ismsb = 0;
	#endif
	if ( (gd->ghdr = InitGridHeaderInfoFromArgs(DBLTWODARRAY_SIZE_ROW(arr), DBLTWODARRAY_SIZE_COL(arr), 
												xll, yll, cellsz, nodata, ismsb)) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		free(gd->arr);
		free(gd);
		gd = NULL;		
		return gd;
		}
		
	/* set the data type and clone array */
	gd->gtype = EnumDblGrid;
	if ( (gd->arr->da = InitDblTwoDArraySizeEmpty(DBLTWODARRAY_SIZE_ROW(arr), 
													DBLTWODARRAY_SIZE_COL(arr))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		free(gd->ghdr);
		free(gd->arr);
		free(gd);
		gd = NULL;		
		}
	for(r = 0; r < DBLTWODARRAY_SIZE_ROW(arr); r++)	{
		for(c = 0; c < DBLTWODARRAY_SIZE_COL(arr); c++)	{
			DBLTWODARRAY_SET_DATA(gd->arr->da, r, c, DBLTWODARRAY_GET_DATA(arr, r, c));
			}
		}
	
	return gd;
	}

GridData * InitGridDataFromFltTwoDArray(FltTwoDArray * arr, double xll, double yll, int cellsz, int nodata)	{
	GridData * gd			= NULL;
	int r, c, ismsb = 0;
	
	/* check args */
	if ( arr == NULL )	{
		ERR_ERROR_CONTINUE("Must provide an initialized array to construct GridData. \n", ERR_EINVAL);
		return gd;
		}
	
	/* initialize GridData */
	if ( (gd = (GridData *) malloc(sizeof(GridData))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		return gd;
		}
	gd->gfinfo = NULL;
	if ( (gd->arr = (GridArray *) malloc(sizeof(GridArray))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridArray. \n", ERR_ENOMEM);
		free(gd);
		gd = NULL;		
		return gd;
		}
	/* platform dependent byte-ordering */
	#ifdef USING_UNIX
	ismsb = 1;
	#endif
	#ifdef USING_PC
	ismsb = 0;
	#endif
	if ( (gd->ghdr = InitGridHeaderInfoFromArgs(FLTTWODARRAY_SIZE_ROW(arr), FLTTWODARRAY_SIZE_COL(arr), 
												xll, yll, cellsz, nodata, ismsb)) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		free(gd->arr);
		free(gd);
		gd = NULL;		
		return gd;
		}
		
	/* set the data type and clone array */
	gd->gtype = EnumFltGrid;
	if ( (gd->arr->fa = InitFltTwoDArraySizeEmpty(FLTTWODARRAY_SIZE_ROW(arr), 
													FLTTWODARRAY_SIZE_COL(arr))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		free(gd->ghdr);
		free(gd->arr);
		free(gd);
		gd = NULL;		
		}
	for(r = 0; r < FLTTWODARRAY_SIZE_ROW(arr); r++)	{
		for(c = 0; c < FLTTWODARRAY_SIZE_COL(arr); c++)	{
			FLTTWODARRAY_SET_DATA(gd->arr->fa, r, c, FLTTWODARRAY_GET_DATA(arr, r, c));
			}
		}
	
	return gd;
	}
	
GridData * InitGridDataFromLIntTwoDArray(LIntTwoDArray * arr, double xll, double yll, int cellsz, int nodata)	{
	GridData * gd			= NULL;
	int r, c, ismsb = 0;
	
	/* check args */
	if ( arr == NULL )	{
		ERR_ERROR_CONTINUE("Must provide an initialized array to construct GridData. \n", ERR_EINVAL);
		return gd;
		}
	
	/* initialize GridData */
	if ( (gd = (GridData *) malloc(sizeof(GridData))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		return gd;
		}
	gd->gfinfo = NULL;
	if ( (gd->arr = (GridArray *) malloc(sizeof(GridArray))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridArray. \n", ERR_ENOMEM);
		free(gd);
		gd = NULL;		
		return gd;
		}
	/* platform dependent byte-ordering */
	#ifdef USING_UNIX
	ismsb = 1;
	#endif
	#ifdef USING_PC
	ismsb = 0;
	#endif
	if ( (gd->ghdr = InitGridHeaderInfoFromArgs(LINTTWODARRAY_SIZE_ROW(arr), LINTTWODARRAY_SIZE_COL(arr), 
												xll, yll, cellsz, nodata, ismsb)) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		free(gd->arr);
		free(gd);
		gd = NULL;		
		return gd;
		}
		
	/* set the data type and clone array */
	gd->gtype = EnumLIntGrid;
	if ( (gd->arr->lia = InitLIntTwoDArraySizeEmpty(LINTTWODARRAY_SIZE_ROW(arr), 
													LINTTWODARRAY_SIZE_COL(arr))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		free(gd->ghdr);
		free(gd->arr);
		free(gd);
		gd = NULL;		
		}
	for(r = 0; r < LINTTWODARRAY_SIZE_ROW(arr); r++)	{
		for(c = 0; c < LINTTWODARRAY_SIZE_COL(arr); c++)	{
			LINTTWODARRAY_SET_DATA(gd->arr->lia, r, c, LINTTWODARRAY_GET_DATA(arr, r, c));
			}
		}
	
	return gd;
	}

GridData * InitGridDataFromIntTwoDArray(IntTwoDArray * arr, double xll, double yll, int cellsz, int nodata)	{
	GridData * gd			= NULL;
	int r, c, ismsb = 0;
	
	/* check args */
	if ( arr == NULL )	{
		ERR_ERROR_CONTINUE("Must provide an initialized array to construct GridData. \n", ERR_EINVAL);
		return gd;
		}
	
	/* initialize GridData */
	if ( (gd = (GridData *) malloc(sizeof(GridData))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		return gd;
		}
	gd->gfinfo = NULL;
	if ( (gd->arr = (GridArray *) malloc(sizeof(GridArray))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridArray. \n", ERR_ENOMEM);
		free(gd);
		gd = NULL;		
		return gd;
		}
	/* platform dependent byte-ordering */
	#ifdef USING_UNIX
	ismsb = 1;
	#endif
	#ifdef USING_PC
	ismsb = 0;
	#endif
	if ( (gd->ghdr = InitGridHeaderInfoFromArgs(INTTWODARRAY_SIZE_ROW(arr), INTTWODARRAY_SIZE_COL(arr), 
												xll, yll, cellsz, nodata, ismsb)) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		free(gd->arr);
		free(gd);
		gd = NULL;		
		return gd;
		}
		
	/* set the data type and clone array */
	gd->gtype = EnumIntGrid;
	if ( (gd->arr->ia = InitIntTwoDArraySizeEmpty(INTTWODARRAY_SIZE_ROW(arr), 
													INTTWODARRAY_SIZE_COL(arr))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		free(gd->ghdr);
		free(gd->arr);
		free(gd);
		gd = NULL;		
		}
	for(r = 0; r < INTTWODARRAY_SIZE_ROW(arr); r++)	{
		for(c = 0; c < INTTWODARRAY_SIZE_COL(arr); c++)	{
			INTTWODARRAY_SET_DATA(gd->arr->ia, r, c, INTTWODARRAY_GET_DATA(arr, r, c));
			}
		}
	
	return gd;
	}
	
GridData * InitGridDataFromByteTwoDArray(ByteTwoDArray * arr, double xll, double yll, int cellsz, int nodata)	{
	GridData * gd			= NULL;
	int r, c, ismsb = 0;
	
	/* check args */
	if ( arr == NULL )	{
		ERR_ERROR_CONTINUE("Must provide an initialized array to construct GridData. \n", ERR_EINVAL);
		return gd;
		}
	
	/* initialize GridData */
	if ( (gd = (GridData *) malloc(sizeof(GridData))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		return gd;
		}
	gd->gfinfo = NULL;
	if ( (gd->arr = (GridArray *) malloc(sizeof(GridArray))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridArray. \n", ERR_ENOMEM);
		free(gd);
		gd = NULL;		
		return gd;
		}
	/* platform dependent byte-ordering */
	#ifdef USING_UNIX
	ismsb = 1;
	#endif
	#ifdef USING_PC
	ismsb = 0;
	#endif
	if ( (gd->ghdr = InitGridHeaderInfoFromArgs(BYTETWODARRAY_SIZE_ROW(arr), BYTETWODARRAY_SIZE_COL(arr), 
												xll, yll, cellsz, nodata, ismsb)) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		free(gd->arr);
		free(gd);
		gd = NULL;		
		return gd;
		}
		
	/* set the data type and clone array */
	gd->gtype = EnumByteGrid;
	if ( (gd->arr->ba = InitByteTwoDArraySizeEmpty(BYTETWODARRAY_SIZE_ROW(arr), 
													BYTETWODARRAY_SIZE_COL(arr))) == NULL )	{
		ERR_ERROR_CONTINUE("Unable to allocate memory for GridData. \n", ERR_ENOMEM);
		free(gd->ghdr);
		free(gd->arr);
		free(gd);
		gd = NULL;		
		}
	for(r = 0; r < BYTETWODARRAY_SIZE_ROW(arr); r++)	{
		for(c = 0; c < BYTETWODARRAY_SIZE_COL(arr); c++)	{
			BYTETWODARRAY_SET_DATA(gd->arr->ba, r, c, BYTETWODARRAY_GET_DATA(arr, r, c));
			}
		}
	
	return gd;
	}
		
GridHeaderInfo * InitGridHeaderInfoFromFile(GridFileInfo * gfinfo)	{
	GridHeaderInfo * ghdr 	= NULL;
	FILE * fstream 			= NULL;
	char * token 			= NULL;
	char * line 			= NULL;
	int last_line_header 	= 0;
	fpos_t last_file_pos;

	/* check args */
	if ( gfinfo == NULL )	{
		ERR_ERROR_CONTINUE("Must supply a GridFileInfo structure to initialize GridHeader. \n", ERR_EINVAL);
		return ghdr;
		}
	if ( (ghdr = (GridHeaderInfo *) malloc(sizeof(GridHeaderInfo))) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for GridHeaderInfo. \n", ERR_ENOMEM);
		return ghdr;
		}
	
	/* open header based upon data format */
	if ( gfinfo->gfmt == EnumAsciiGrid )	{
		fstream = fopen(gfinfo->main_file, "rb");
		}
	else if ( gfinfo->header_file != NULL )	{
		fstream = fopen(gfinfo->header_file, "rb");
		}
	if ( fstream == NULL )	{
		ERR_ERROR_CONTINUE("Unable to open GridHeader. \n", ERR_EIOFAIL);
		free(ghdr);
		ghdr = NULL;
		return ghdr;	
		}

	/* read in contents of header */
	while( last_line_header != 1 )	{
		fgetpos(fstream, &last_file_pos);
		line = GetLineFStreamIO(fstream);
		if ( line != NULL && (token = strtok(line, GRIDDATA_HEADER_SEP_CHARS)) != NULL )	{
			if ( token != NULL && isalpha(token[0]) == 0 )	{
				/* 	first line of data */
				last_line_header = 1;
				/* rewind file pointer */
				fsetpos(fstream, &last_file_pos);
				}
			/* process keyword token */
			else if ( strcmp(token, GRIDDATA_KEYWORD_NCOLS) == 0)	{
				if ( (token = strtok(NULL, GRIDDATA_HEADER_SEP_CHARS)) != NULL )	{
					ghdr->ncols = atoi(token);
					}
				}
			else if ( strcmp(token, GRIDDATA_KEYWORD_NROWS) == 0)	{			
				if ( (token = strtok(NULL, GRIDDATA_HEADER_SEP_CHARS)) != NULL )	{
					ghdr->nrows = atoi(token);
					}			
				}
			else if (strcmp(token, GRIDDATA_KEYWORD_XLLCORNER) == 0)	{
				if ( (token = strtok(NULL, GRIDDATA_HEADER_SEP_CHARS)) != NULL )	{
					ghdr->xllcorner = atof(token);
					}				
				}
			else if (strcmp(token, GRIDDATA_KEYWORD_YLLCORNER) == 0)	{			
				if ( (token = strtok(NULL, GRIDDATA_HEADER_SEP_CHARS)) != NULL )	{
					ghdr->yllcorner = atof(token);
					}
				}
			else if (strcmp(token, GRIDDATA_KEYWORD_CELLSIZE) == 0)	{			
				if ( (token = strtok(NULL, GRIDDATA_HEADER_SEP_CHARS)) != NULL )	{
					ghdr->cellsize = atoi(token);
					}			
				}
			else if (strcmp(token, GRIDDATA_KEYWORD_NODATA_value) == 0)	{
				if ( (token = strtok(NULL, GRIDDATA_HEADER_SEP_CHARS)) != NULL )	{
					ghdr->NODATA_value = atoi(token);
					}			
				}
			else if (strcmp(token, GRIDDATA_KEYWORD_BYTEORDER) == 0)	{
				if ( (token = strtok(NULL, GRIDDATA_HEADER_SEP_CHARS)) != NULL )	{
					if ( strcmp(token, GRIDDATA_KEYWORD_BYTEORDER_MSB) == 0 )	{
						ghdr->is_msbfirst = 1;
						}
					else	{
						ghdr->is_msbfirst = 0;
						}
					}			
				}				
			else	{
				/* don't know what we got */
				ERR_ERROR_CONTINUE("Unrecognized keyword in grid header. \n", ERR_ESANITY);
				}
			free(line);
			}
		else	{
			/* empty lines must mean header portion is over, no need to rewind when empty */
			last_line_header = 1;
			}
		}		/* end of while */
		
	if ( gfinfo->gfmt == EnumAsciiGrid ) 	{
		gfinfo->bytes_read = ftell(fstream);
		}
	fclose(fstream);
		
	return ghdr;
	}
	
GridHeaderInfo * InitGridHeaderInfoFromArgs (int nrows, int ncols,	double xll, double yll, int cellsz, int nodata,
 												int ismsb)	{
	GridHeaderInfo * ghdr = NULL;

	if ( (ghdr = (GridHeaderInfo *) malloc(sizeof(GridHeaderInfo))) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for GridHeaderInfo. \n", ERR_ENOMEM);
		}
	else	{
		ghdr->nrows 			= nrows;
		ghdr->ncols 			= ncols;
		ghdr->xllcorner 		= xll;
		ghdr->yllcorner 		= yll;
		ghdr->cellsize 			= cellsz;
		ghdr->NODATA_value 	= nodata;
		ghdr->is_msbfirst 	= ismsb;
		}
		
	return ghdr; 												
 	}

/* DATA IMPORT */	
DblTwoDArray * ImportBinaryRasterAsDblTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr)	{
	DblTwoDArray * arr2d 	= NULL;
	FILE * fstream 			= NULL;
	float * fbuff 			= NULL;
	size_t items_read 		= 0;	
	int r, c;	

	if ( gfinfo == NULL || ghdr == NULL || ((fstream = fopen(gfinfo->main_file, "rb")) == NULL) )	{
		ERR_ERROR_CONTINUE("Cannot retrieve file or header information to load GRID data. \n", ERR_EINVAL);	
		return arr2d;
		}

 	if ( (fbuff = (float *) malloc(sizeof(float) * ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for IO buffer. \n", ERR_EIOFAIL);
 		fclose(fstream);
 		return arr2d;
 		}
 		
	if ( (arr2d = InitDblTwoDArraySizeEmpty(ghdr->nrows, ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for 2D array. \n", ERR_ENOMEM);
 		fclose(fstream);
 		free(fbuff);
 		return arr2d;
 		}		
	
	for( r = 0; r < ghdr->nrows; r++ )	{
 		items_read = fread((float *)fbuff, sizeof(float), ghdr->ncols, fstream);
 		for( c = 0; c < items_read; c++ )	{
 			if ( SetDataDblTwoDArray(arr2d, r, c, (double) fbuff[c]) )	{ 			
				ERR_ERROR_CONTINUE("Attempt to insert into grid past valid index. \n", ERR_EDOM);
 				}
			}
		if ( items_read != ghdr->ncols )	{
			ERR_ERROR_CONTINUE("Full column of data not read from grid. \n", ERR_ERANGE);
			}
		} 

	if ( fbuff != NULL	)
		free(fbuff);			
	fclose(fstream);
	 	
 	return arr2d;
	}

DblTwoDArray * ImportAsciiRasterAsDblTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr)	{
	DblTwoDArray * arr2d 	= NULL;
	FILE * fstream 			= NULL;
	double buff;
	int r, c;	

	if ( gfinfo == NULL || ghdr == NULL || ((fstream = fopen(gfinfo->main_file, "rb")) == NULL) )	{
		ERR_ERROR_CONTINUE("Cannot retrieve file or header information to load GRID data. \n", ERR_EINVAL);	
		return arr2d;
		}
 		
	if ( (arr2d = InitDblTwoDArraySizeEmpty(ghdr->nrows, ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for 2D array. \n", ERR_ENOMEM);
 		fclose(fstream);
 		return arr2d;
 		}		
	
	/* move to stored offset */
	fseek(fstream, gfinfo->bytes_read, SEEK_SET);
 	/* start populating array and reading in more data as necessary */
	for( r = 0; r < ghdr->nrows; r++ )	{
 		for( c = 0; c < ghdr->ncols; c++ )	{
 			if ( fscanf(fstream, "%lf ", &buff) != EOF )	{
	 			if ( SetDataDblTwoDArray(arr2d, r, c, buff) )
					ERR_ERROR_CONTINUE("Attempt to insert into grid past valid index. \n", ERR_EDOM);
				}
			}
		}

	fclose(fstream);
	 	
 	return arr2d;
	}
		
FltTwoDArray * ImportBinaryRasterAsFltTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr)	{
	FltTwoDArray * arr2d 	= NULL;
	FILE * fstream 			= NULL;
	float * fbuff 			= NULL;
	size_t items_read 		= 0;	
	int r, c;	

	if ( gfinfo == NULL || ghdr == NULL || ((fstream = fopen(gfinfo->main_file, "rb")) == NULL) )	{
		ERR_ERROR_CONTINUE("Cannot retrieve file or header information to load GRID data. \n", ERR_EINVAL);	
		return arr2d;
		}

 	if ( (fbuff = (float *) malloc(sizeof(float) * ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for IO buffer. \n", ERR_EIOFAIL);
 		fclose(fstream);
 		return arr2d;
 		}
 		
	if ( (arr2d = InitFltTwoDArraySizeEmpty(ghdr->nrows, ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for 2D array. \n", ERR_ENOMEM);
 		fclose(fstream);
 		free(fbuff);
 		return arr2d;
 		}		
	
	for( r = 0; r < ghdr->nrows; r++ )	{
 		items_read = fread((float *)fbuff, sizeof(float), ghdr->ncols, fstream);
 		for( c = 0; c < items_read; c++ )	{
 			if ( SetDataFltTwoDArray(arr2d, r, c, (float) fbuff[c]) )	{ 			
				ERR_ERROR_CONTINUE("Attempt to insert into grid past valid index. \n", ERR_EDOM);
 				}
			}
		if ( items_read != ghdr->ncols )	{
			ERR_ERROR_CONTINUE("Full column of data not read from grid. \n", ERR_ERANGE);
			}
		} 

	if ( fbuff != NULL	)
		free(fbuff);			
	fclose(fstream);
	 	
 	return arr2d;
	}
	
FltTwoDArray * ImportAsciiRasterAsFltTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr)	{
	FltTwoDArray * arr2d 	= NULL;
	FILE * fstream 			= NULL;
	double buff;
	int r, c;	

	if ( gfinfo == NULL || ghdr == NULL || ((fstream = fopen(gfinfo->main_file, "rb")) == NULL) )	{
		ERR_ERROR_CONTINUE("Cannot retrieve file or header information to load GRID data. \n", ERR_EINVAL);	
		return arr2d;
		}
 		
	if ( (arr2d = InitFltTwoDArraySizeEmpty(ghdr->nrows, ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for 2D array. \n", ERR_ENOMEM);
 		fclose(fstream);
 		return arr2d;
 		}		
	
	/* move to stored offset */
	fseek(fstream, gfinfo->bytes_read, SEEK_SET);
 	/* start populating array and reading in more data as necessary */
	for( r = 0; r < ghdr->nrows; r++ )	{
 		for( c = 0; c < ghdr->ncols; c++ )	{
 			if ( fscanf(fstream, "%lf ", &buff) != EOF )	{
	 			if ( SetDataFltTwoDArray(arr2d, r, c, (float) buff) )
					ERR_ERROR_CONTINUE("Attempt to insert into grid past valid index. \n", ERR_EDOM);
				}
			}
		}

	fclose(fstream);
	 	
 	return arr2d;
	}
	
LIntTwoDArray * ImportBinaryRasterAsLIntTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr)	{
	LIntTwoDArray * arr2d 	= NULL;
	FILE * fstream 			= NULL;
	float * fbuff 			= NULL;
	size_t items_read 		= 0;	
	int r, c;	

	if ( gfinfo == NULL || ghdr == NULL || ((fstream = fopen(gfinfo->main_file, "rb")) == NULL) )	{
		ERR_ERROR_CONTINUE("Cannot retrieve file or header information to load GRID data. \n", ERR_EINVAL);	
		return arr2d;
		}

 	if ( (fbuff = (float *) malloc(sizeof(float) * ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for IO buffer. \n", ERR_EIOFAIL);
 		fclose(fstream);
 		return arr2d;
 		}
 		
	if ( (arr2d = InitLIntTwoDArraySizeEmpty(ghdr->nrows, ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for 2D array. \n", ERR_ENOMEM);
 		fclose(fstream);
 		free(fbuff);
 		return arr2d;
 		}		
	
	for( r = 0; r < ghdr->nrows; r++ )	{
 		items_read = fread((float *)fbuff, sizeof(float), ghdr->ncols, fstream);
 		for( c = 0; c < items_read; c++ )	{
 			if ( SetDataLIntTwoDArray(arr2d, r, c, (long int) fbuff[c]) )	{
				ERR_ERROR_CONTINUE("Attempt to insert into grid past valid index. \n", ERR_EDOM);
 				}
			}
		if ( items_read != ghdr->ncols )	{
			ERR_ERROR_CONTINUE("Full column of data not read from grid. \n", ERR_ERANGE);
			}
		} 

	if ( fbuff != NULL	)
		free(fbuff);			
	fclose(fstream);
	 	
 	return arr2d;
	}
	
LIntTwoDArray * ImportAsciiRasterAsLIntTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr)	{
	LIntTwoDArray * arr2d 	= NULL;
	FILE * fstream 			= NULL;
	double buff;
	int r, c;	

	if ( gfinfo == NULL || ghdr == NULL || ((fstream = fopen(gfinfo->main_file, "rb")) == NULL) )	{
		ERR_ERROR_CONTINUE("Cannot retrieve file or header information to load GRID data. \n", ERR_EINVAL);	
		return arr2d;
		}
 		
	if ( (arr2d = InitLIntTwoDArraySizeEmpty(ghdr->nrows, ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for 2D array. \n", ERR_ENOMEM);
 		fclose(fstream);
 		return arr2d;
 		}		
	
	/* move to stored offset */
	fseek(fstream, gfinfo->bytes_read, SEEK_SET);
 	/* start populating array and reading in more data as necessary */
	for( r = 0; r < ghdr->nrows; r++ )	{
 		for( c = 0; c < ghdr->ncols; c++ )	{
 			if ( fscanf(fstream, "%lf ", &buff) != EOF )	{
	 			if ( SetDataLIntTwoDArray(arr2d, r, c, (long int) buff) )
					ERR_ERROR_CONTINUE("Attempt to insert into grid past valid index. \n", ERR_EDOM);
				}
			}
		}

	fclose(fstream);
	 	
 	return arr2d;
	}

IntTwoDArray * ImportBinaryRasterAsIntTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr)	{
	IntTwoDArray * arr2d 	= NULL;
	FILE * fstream 			= NULL;
	float * fbuff 			= NULL;
	size_t items_read 		= 0;	
	int r, c;	

	if ( gfinfo == NULL || ghdr == NULL || ((fstream = fopen(gfinfo->main_file, "rb")) == NULL) )	{
		ERR_ERROR_CONTINUE("Cannot retrieve file or header information to load GRID data. \n", ERR_EINVAL);	
		return arr2d;
		}

 	if ( (fbuff = (float *) malloc(sizeof(float) * ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for IO buffer. \n", ERR_EIOFAIL);
 		fclose(fstream);
 		return arr2d;
 		}
 		
	if ( (arr2d = InitIntTwoDArraySizeEmpty(ghdr->nrows, ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for 2D array. \n", ERR_ENOMEM);
 		fclose(fstream);
 		free(fbuff);
 		return arr2d;
 		}		
	
	for( r = 0; r < ghdr->nrows; r++ )	{
 		items_read = fread((float *)fbuff, sizeof(float), ghdr->ncols, fstream);
 		for( c = 0; c < items_read; c++ )	{
 			if ( SetDataIntTwoDArray(arr2d, r, c, (int) fbuff[c]) )	{ 			
				ERR_ERROR_CONTINUE("Attempt to insert into grid past valid index. \n", ERR_EDOM);
 				}
			}
		if ( items_read != ghdr->ncols )	{
			ERR_ERROR_CONTINUE("Full column of data not read from grid. \n", ERR_ERANGE);
			}
		} 

	if ( fbuff != NULL	)
		free(fbuff);			
	fclose(fstream);
	 	
 	return arr2d;
	}
	
IntTwoDArray * ImportAsciiRasterAsIntTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr)	{
	IntTwoDArray * arr2d 	= NULL;
	FILE * fstream 			= NULL;
	double buff;
	int r, c;	

	if ( gfinfo == NULL || ghdr == NULL || ((fstream = fopen(gfinfo->main_file, "rb")) == NULL) )	{
		ERR_ERROR_CONTINUE("Cannot retrieve file or header information to load GRID data. \n", ERR_EINVAL);	
		return arr2d;
		}
 		
	if ( (arr2d = InitIntTwoDArraySizeEmpty(ghdr->nrows, ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for 2D array. \n", ERR_ENOMEM);
 		fclose(fstream);
 		return arr2d;
 		}		
	
	/* move to stored offset */
	fseek(fstream, gfinfo->bytes_read, SEEK_SET);
 	/* start populating array and reading in more data as necessary */
	for( r = 0; r < ghdr->nrows; r++ )	{
 		for( c = 0; c < ghdr->ncols; c++ )	{
 			if ( fscanf(fstream, "%lf ", &buff) != EOF )	{
	 			if ( SetDataIntTwoDArray(arr2d, r, c, (int) buff) )
					ERR_ERROR_CONTINUE("Attempt to insert into grid past valid index. \n", ERR_EDOM);
				}
			}
		}

	fclose(fstream);
	 	
 	return arr2d;
	}

ByteTwoDArray * ImportBinaryRasterAsByteTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr)	{
	ByteTwoDArray * arr2d 	= NULL;
	FILE * fstream 			= NULL;
	float * fbuff 			= NULL;
	size_t items_read 		= 0;	
	int r, c;	

	if ( gfinfo == NULL || ghdr == NULL || ((fstream = fopen(gfinfo->main_file, "rb")) == NULL) )	{
		ERR_ERROR_CONTINUE("Cannot retrieve file or header information to load GRID data. \n", ERR_EINVAL);	
		return arr2d;
		}

 	if ( (fbuff = (float *) malloc(sizeof(float) * ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for IO buffer. \n", ERR_EIOFAIL);
 		fclose(fstream);
 		return arr2d;
 		}
 		
	if ( (arr2d = InitByteTwoDArraySizeEmpty(ghdr->nrows, ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for 2D array. \n", ERR_ENOMEM);
 		fclose(fstream);
 		free(fbuff);
 		return arr2d;
 		}		
	
	for( r = 0; r < ghdr->nrows; r++ )	{
 		items_read = fread((float *)fbuff, sizeof(float), ghdr->ncols, fstream);
 		for( c = 0; c < items_read; c++ )	{
 			if ( SetDataByteTwoDArray(arr2d, r, c, (unsigned char) fbuff[c]) )	{ 			
				ERR_ERROR_CONTINUE("Attempt to insert into grid past valid index. \n", ERR_EDOM);
 				}
			}
		if ( items_read != ghdr->ncols )	{
			ERR_ERROR_CONTINUE("Full column of data not read from grid. \n", ERR_ERANGE);
			}
		} 

	if ( fbuff != NULL	)
		free(fbuff);			
	fclose(fstream);
	 	
 	return arr2d;
	}

ByteTwoDArray * ImportAsciiRasterAsByteTwoDArray(GridFileInfo * gfinfo, GridHeaderInfo * ghdr)	{
	ByteTwoDArray * arr2d 	= NULL;
	FILE * fstream 			= NULL;
	double buff;
	int r, c;	

	if ( gfinfo == NULL || ghdr == NULL || ((fstream = fopen(gfinfo->main_file, "rb")) == NULL) )	{
		ERR_ERROR_CONTINUE("Cannot retrieve file or header information to load GRID data. \n", ERR_EINVAL);	
		return arr2d;
		}
 		
	if ( (arr2d = InitByteTwoDArraySizeEmpty(ghdr->nrows, ghdr->ncols)) == NULL )	{
		ERR_ERROR_CONTINUE("Memory not allocated for 2D array. \n", ERR_ENOMEM);
 		fclose(fstream);
 		return arr2d;
 		}		
	
	/* move to stored offset */
	fseek(fstream, gfinfo->bytes_read, SEEK_SET);
 	/* start populating array and reading in more data as necessary */
	for( r = 0; r < ghdr->nrows; r++ )	{
 		for( c = 0; c < ghdr->ncols; c++ )	{
 			if ( fscanf(fstream, "%lf ", &buff) != EOF )	{
	 			if ( SetDataByteTwoDArray(arr2d, r, c, (unsigned char) buff) )
					ERR_ERROR_CONTINUE("Attempt to insert into grid past valid index. \n", ERR_EDOM);
				}
			}
		}

	fclose(fstream);
	 	
 	return arr2d;
	}
	
/* DATA EXPORT */
int ExportGridDataAsBinaryRaster(GridData * gd, char * main_fname)	{
	FILE * fstream 			= NULL;
	float * buff			= NULL;
	double ** dbuff 		= NULL;
	float ** fbuff			= NULL;
	long int ** libuff		= NULL;	
	int ** ibuff			= NULL;
	unsigned char ** bbuff	= NULL;
	char * header			= NULL;
	char * fname_ext		= NULL;
	int i, j, rows, cols;

	/* check args */
	if ( gd == NULL || gd->ghdr == NULL || main_fname == NULL )	{
		ERR_ERROR("Cannot retrieve file or header information to write GRID data. \n", ERR_EINVAL);
		}
	if ( (header = (char *) malloc( sizeof(char) * (strlen(main_fname) + 1))) == NULL )	{
		ERR_ERROR("Unable to write grid file names, memory allocation failed. \n", ERR_ENOMEM);
		}
		
	/* create a header file */
	header = strcpy(header, main_fname);
	if ( (fname_ext = strrchr(header, *(GRIDDATA_FILE_EXTENSION_SEPARATOR))) == NULL )	{
		ERR_ERROR("Main file must be terminated with a 3 digit character extension. \n", ERR_EDOM);
		}
	*(++fname_ext) = 'h';
	*(++fname_ext) = 'd';
	*(++fname_ext) = 'r';
	if ( (fstream = fopen(header, "w")) == NULL )	{
		ERR_ERROR("Cannot write output header filename to write GRID header. \n", ERR_EIOFAIL);
		}
	
	/* write contents of header file */
	fprintf(fstream, "%s %s %d \n", 		GRIDDATA_KEYWORD_NCOLS, GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->ncols);
	fprintf(fstream, "%s %s %d \n", 		GRIDDATA_KEYWORD_NROWS, GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->nrows);
	/* editted MM 05312001 appears to be CW formatting bug */
	fprintf(fstream, "%s %s %f \n",			GRIDDATA_KEYWORD_XLLCORNER, GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->xllcorner);
	fprintf(fstream, "%s %s %f \n", 		GRIDDATA_KEYWORD_YLLCORNER, GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->yllcorner);						
	fprintf(fstream, "%s %s %d \n", 		GRIDDATA_KEYWORD_CELLSIZE, GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->cellsize);
	fprintf(fstream, "%s %s %d \n", 	GRIDDATA_KEYWORD_NODATA_value, GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->NODATA_value);
	if ( gd->ghdr->is_msbfirst == 1 )	{
		fprintf(fstream, "%s %s %s \n", GRIDDATA_KEYWORD_BYTEORDER, GRIDDATA_HEADER_SEP_CHARS, GRIDDATA_KEYWORD_BYTEORDER_MSB);
		}
	else if ( gd->ghdr->is_msbfirst == 0 )	{
		fprintf(fstream, "%s %s %s \n", GRIDDATA_KEYWORD_BYTEORDER, GRIDDATA_HEADER_SEP_CHARS, GRIDDATA_KEYWORD_BYTEORDER_LSB);
		}
		
	/* close open stream */
	if ( fstream != NULL )
		fclose(fstream);
	
	/* free memory associated with header */
	if ( header != NULL )
		free(header);
			
	/* create a main file */
	if ( (fstream = fopen(main_fname, "wb")) == NULL )	{
		ERR_ERROR("Cannot write output main file to write GRID data. \n", ERR_EIOFAIL);
		}

	/* write contents of main file */
 	switch (gd->gtype)	{
	 	/* need to do something with the byte ordering here and convert contents of buffer if necessary */
 		case EnumDblGrid:
 			if ( gd->arr->da != NULL )	{
				dbuff 	= GetUnderlyingDblTwoDArray(gd->arr->da);
				rows 	= GetSizeRowDblTwoDArray(gd->arr->da);
				cols 	= GetSizeColDblTwoDArray(gd->arr->da);
				if ( (buff = (float *) malloc(sizeof(float) * cols)) == NULL )	{
					fclose(fstream);				
					ERR_ERROR("Memory not allocated for IO buffer. \n", ERR_EIOFAIL);
					}						
				for( i = 0; i < rows; i++)	{
					for (j = 0; j < cols; j++)	{
						buff[j] = (float) (dbuff[i][j]);
						}
					fwrite(buff, sizeof(float), cols, fstream);
					}
				if ( buff != NULL )
					free(buff);									 			
 				}
 			break;
 		case EnumFltGrid:
 			if ( gd->arr->fa != NULL )	{
				fbuff 	= GetUnderlyingFltTwoDArray(gd->arr->fa);
				rows 	= GetSizeRowFltTwoDArray(gd->arr->fa);
				cols 	= GetSizeColFltTwoDArray(gd->arr->fa);
				if ( (buff = (float *) malloc(sizeof(float) * cols)) == NULL )	{
					fclose(fstream);
					ERR_ERROR("Memory not allocated for IO buffer. \n", ERR_EIOFAIL);
					}						
				for( i = 0; i < rows; i++)	{
					for (j = 0; j < cols; j++)	{
						buff[j] = (float) (fbuff[i][j]);
						}
					fwrite(buff, sizeof(float), cols, fstream);
					}
				if ( buff != NULL )
					free(buff);									 			
 				}
 			break;
 		case EnumLIntGrid:
 			if ( gd->arr->lia != NULL )	{
				libuff 	= GetUnderlyingLIntTwoDArray(gd->arr->lia);
				rows 	= GetSizeRowLIntTwoDArray(gd->arr->lia);
				cols 	= GetSizeColLIntTwoDArray(gd->arr->lia);
				if ( (buff = (float *) malloc(sizeof(float) * cols)) == NULL )	{
					fclose(fstream);				
					ERR_ERROR("Memory not allocated for IO buffer. \n", ERR_EIOFAIL);
					}						
				for( i = 0; i < rows; i++)	{
					for (j = 0; j < cols; j++)	{
						buff[j] = (float) (libuff[i][j]);
						}
					fwrite(buff, sizeof(float), cols, fstream);
					}
				if ( buff != NULL )
					free(buff);									 			
 				}
 			break; 	 			 			
 		case EnumIntGrid:
 			if ( gd->arr->ia != NULL )	{ 		
				ibuff 	= GetUnderlyingIntTwoDArray(gd->arr->ia);
				rows 	= GetSizeRowIntTwoDArray(gd->arr->ia);
				cols 	= GetSizeColIntTwoDArray(gd->arr->ia);
				if ( (buff = (float *) malloc(sizeof(float) * cols)) == NULL )	{
					fclose(fstream);				
					ERR_ERROR("Memory not allocated for IO buffer. \n", ERR_EIOFAIL);
					}			
				for( i = 0; i < rows; i++)	{
					for (j = 0; j < cols; j++)	{
						buff[j] = (float) (ibuff[i][j]);
						}
					fwrite(buff, sizeof(float), cols, fstream);
					}
				if ( buff != NULL )
					free(buff);
				}	
 			break;
		case EnumByteGrid:
			 if ( gd->arr->ba != NULL)	{
				bbuff 	= GetUnderlyingByteTwoDArray(gd->arr->ba);
				rows 	= GetSizeRowByteTwoDArray(gd->arr->ba);
				cols 	= GetSizeColByteTwoDArray(gd->arr->ba);
				if ( (buff = (float *) malloc(sizeof(float) * cols)) == NULL )	{
					fclose(fstream);				
					ERR_ERROR("Memory not allocated for IO buffer. \n", ERR_EIOFAIL);
					}				
				for( i = 0; i < rows; i++)	{
					for (j = 0; j < cols; j++)	{
						buff[j] = (float) (bbuff[i][j]);
						}
					fwrite(buff, sizeof(float), cols, fstream);
					}
				if ( buff != NULL )
					free(buff);
				}
			break;
		case EnumBitGrid:
			fclose(fstream);		
			ERR_ERROR("Data type for bit GridData not yet implemented. \n", ERR_EUNIMPL);
			break;			
		default:
			fclose(fstream);		
			ERR_ERROR("Data type for GridIO not recognized. \n", ERR_EINVAL);
			break;
		}
	
	/* close open stream */
	fclose(fstream);		
	
	return ERR_SUCCESS;
	}
	
int ExportGridDataAsAsciiRaster(GridData * gd, char * main_fname)	{
	FILE * fstream 			= NULL;
	double ** dbuff 		= NULL;
	float ** fbuff			= NULL;
	long int ** libuff		= NULL;	
	int ** ibuff			= NULL;
	unsigned char ** bbuff	= NULL;
	int i, j, rows, cols;

	/* check args */
	if ( gd == NULL || gd->ghdr == NULL || main_fname == NULL )	{
		ERR_ERROR("Cannot retrieve file or header information to write GRID data. \n", ERR_EINVAL);
		}
			
	/* create a main file */
	if ( (fstream = fopen(main_fname, "w")) == NULL )	{
		ERR_ERROR("Cannot write output main file to write GRID data. \n", ERR_EIOFAIL);
		}
		
	/* write contents of header section in main file */
	fprintf(fstream, "%s %s %d \n", 	GRIDDATA_KEYWORD_NCOLS, GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->ncols);
	fprintf(fstream, "%s %s %d \n", 	GRIDDATA_KEYWORD_NROWS, GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->nrows);
	/* editted MM 05312001 appears to be CW formatting bug */
	fprintf(fstream, "%s %s %f \n",		GRIDDATA_KEYWORD_XLLCORNER, GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->xllcorner);
	fprintf(fstream, "%s %s %f \n", 	GRIDDATA_KEYWORD_YLLCORNER, GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->yllcorner);						
	fprintf(fstream, "%s %s %d \n", 	GRIDDATA_KEYWORD_CELLSIZE, GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->cellsize);
	fprintf(fstream, "%s %s %d \n", GRIDDATA_KEYWORD_NODATA_value,GRIDDATA_HEADER_SEP_CHARS, gd->ghdr->NODATA_value);	

	/* write contents of data section in main file */
 	switch (gd->gtype)	{
 		case EnumDblGrid:
 			if ( gd->arr->da != NULL )	{
				dbuff 	= GetUnderlyingDblTwoDArray(gd->arr->da);
				rows 	= GetSizeRowDblTwoDArray(gd->arr->da);
				cols 	= GetSizeColDblTwoDArray(gd->arr->da);	
				for( i = 0; i < rows; i++)	{
					for ( j = 0; j < cols; j++ )	{
						fprintf(fstream, "%.4lf%s", dbuff[i][j], GRIDDATA_HEADER_SEP_CHARS);
						}
					fprintf(fstream, "\n");
					}				 			
 				}
 			break;
 		case EnumFltGrid:
 			if ( gd->arr->fa != NULL )	{
				fbuff 	= GetUnderlyingFltTwoDArray(gd->arr->fa);
				rows 	= GetSizeRowFltTwoDArray(gd->arr->fa);
				cols 	= GetSizeColFltTwoDArray(gd->arr->fa);	
				for( i = 0; i < rows; i++)	{
					for ( j = 0; j < cols; j++ )	{
						fprintf(fstream, "%.4f%s", fbuff[i][j], GRIDDATA_HEADER_SEP_CHARS);
						}
					fprintf(fstream, "\n");
					}				 			
 				}
 			break; 
 		case EnumLIntGrid:
 			if ( gd->arr->lia != NULL )	{
				libuff 	= GetUnderlyingLIntTwoDArray(gd->arr->lia);
				rows 	= GetSizeRowLIntTwoDArray(gd->arr->lia);
				cols 	= GetSizeColLIntTwoDArray(gd->arr->lia);	
				for( i = 0; i < rows; i++)	{
					for ( j = 0; j < cols; j++ )	{
						fprintf(fstream, "%ld%s", libuff[i][j], GRIDDATA_HEADER_SEP_CHARS);
						}
					fprintf(fstream, "\n");
					}				 			
 				}
 			break; 						
 		case EnumIntGrid:
 			if ( gd->arr->ia != NULL )	{ 		
				ibuff 	= GetUnderlyingIntTwoDArray(gd->arr->ia);
				rows 	= GetSizeRowIntTwoDArray(gd->arr->ia);
				cols 	= GetSizeColIntTwoDArray(gd->arr->ia);	
				for( i = 0; i < rows; i++)	{
					for ( j = 0; j < cols; j++ )	{
						fprintf(fstream, "%d%s", ibuff[i][j], GRIDDATA_HEADER_SEP_CHARS);
						}
					fprintf(fstream, "\n");
					}
				}	
 			break;
		case EnumByteGrid:
			 if ( gd->arr->ba != NULL)	{
				bbuff 	= GetUnderlyingByteTwoDArray(gd->arr->ba);
				rows 	= GetSizeRowByteTwoDArray(gd->arr->ba);
				cols 	= GetSizeColByteTwoDArray(gd->arr->ba);
				for( i = 0; i < rows; i++)	{
					for ( j = 0; j < cols; j++ )	{
						fprintf(fstream, "%u%s", bbuff[i][j], GRIDDATA_HEADER_SEP_CHARS);
						}
					fprintf(fstream, "\n");
					}
				}
			break;
		case EnumBitGrid:
			fclose(fstream);
			ERR_ERROR("Data type for bit GridData not yet implemented. \n", ERR_EUNIMPL);
			break;			
		default:
			fclose(fstream);		
			ERR_ERROR("Data type for GridIO not recognized. \n", ERR_EINVAL);
			break;
		}
	
	/* close open stream */
	fclose(fstream);		
	
	return ERR_SUCCESS;
	}
	
/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of TwoDArray to file in Binary Raster format on disk.
 * Binary Raster format consists of two files:
 * a machine readable file containing the contents of the array
 * a human readable file containing associated header information
 *
 * Arguments:
 * arr- the TwoDArray object
 * xll- lower left x coord of TwoDArray
 * yll- lower left y coord of TwoDArray
 * cellsz- cellsz of each element in TwoDArray
 * nodata- value of nodata in TwoDArray
 * ismsb- flag value alerting to byte order of machine
 * fname- name of file to write TwoDArray to on disk
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 
int ExportDblTwoDArrayAsBinaryRaster(DblTwoDArray * arr, double xll, double yll, int cellsz, 
										int nodata, int ismsb, char * fname)	{
	GridData * gd = NULL;
	int success;
	
	/* check args */
	if ( arr == NULL || fname == NULL )	{
		ERR_ERROR("Unable to export two dimensional array to file. \n", ERR_EINVAL);
		}
	
	/* initialize and export GridData */
	if ( (gd = InitGridDataFromDblTwoDArray(arr, xll, yll, cellsz, nodata)) == NULL )	{
		ERR_ERROR("Unable to intialize GridData for exporting 2D array to file. \n", ERR_EBADFUNC);
		}	
	gd->ghdr->is_msbfirst = ismsb;
	success = ExportGridDataAsBinaryRaster(gd, fname);
	
	FreeGridData(gd);
	
	return success;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of TwoDArray to file in Ascii Raster format on disk.
 * Ascii Raster format consists of one file:
 * header information listed at top of file
 * contents of the array listed in space delimeted format below header
 *
 * Arguments:
 * arr- the TwoDArray object
 * xll- lower left x coord of TwoDArray
 * yll- lower left y coord of TwoDArray
 * cellsz- cellsz of each element in TwoDArray
 * nodata- value of nodata in TwoDArray
 * fname- name of file to write TwoDArray to on disk
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 	
int ExportDblTwoDArrayAsAsciiRaster(DblTwoDArray * arr, double xll, double yll, int cellsz,
 										int nodata, char * fname)	{
	GridData * gd = NULL;
	int success;
	
	/* check args */
	if ( arr == NULL || fname == NULL )	{
		ERR_ERROR("Unable to export two dimensional array to file. \n", ERR_EINVAL);
		}
	
	/* initialize and export GridData */
	if ( (gd = InitGridDataFromDblTwoDArray(arr, xll, yll, cellsz, nodata)) == NULL )	{
		ERR_ERROR("Unable to intialize GridData for exporting 2D array to file. \n", ERR_EBADFUNC);
		}
	success = ExportGridDataAsAsciiRaster(gd, fname);
	
	FreeGridData(gd);
	
	return success;
	}
	
/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of TwoDArray to file in Binary Raster format on disk.
 * Binary Raster format consists of two files:
 * a machine readable file containing the contents of the array
 * a human readable file containing associated header information
 *
 * Arguments:
 * arr- the TwoDArray object
 * xll- lower left x coord of TwoDArray
 * yll- lower left y coord of TwoDArray
 * cellsz- cellsz of each element in TwoDArray
 * nodata- value of nodata in TwoDArray
 * ismsb- flag value alerting to byte order of machine
 * fname- name of file to write TwoDArray to on disk
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 
int ExportFltTwoDArrayAsBinaryRaster(FltTwoDArray * arr, double xll, double yll, int cellsz, 
										int nodata, int ismsb, char * fname)	{
	GridData * gd = NULL;
	int success;
	
	/* check args */
	if ( arr == NULL || fname == NULL )	{
		ERR_ERROR("Unable to export two dimensional array to file. \n", ERR_EINVAL);
		}
	
	/* initialize and export GridData */
	if ( (gd = InitGridDataFromFltTwoDArray(arr, xll, yll, cellsz, nodata)) == NULL )	{
		ERR_ERROR("Unable to intialize GridData for exporting 2D array to file. \n", ERR_EBADFUNC);
		}	
	gd->ghdr->is_msbfirst = ismsb;
	success = ExportGridDataAsBinaryRaster(gd, fname);
	
	FreeGridData(gd);
	
	return success;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of TwoDArray to file in Ascii Raster format on disk.
 * Ascii Raster format consists of one file:
 * header information listed at top of file
 * contents of the array listed in space delimeted format below header
 *
 * Arguments:
 * arr- the TwoDArray object
 * xll- lower left x coord of TwoDArray
 * yll- lower left y coord of TwoDArray
 * cellsz- cellsz of each element in TwoDArray
 * nodata- value of nodata in TwoDArray
 * fname- name of file to write TwoDArray to on disk
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 	
int ExportFltTwoDArrayAsAsciiRaster(FltTwoDArray * arr, double xll, double yll, int cellsz,
 										int nodata, char * fname)	{
	GridData * gd = NULL;
	int success;
	
	/* check args */
	if ( arr == NULL || fname == NULL )	{
		ERR_ERROR("Unable to export two dimensional array to file. \n", ERR_EINVAL);
		}
	
	/* initialize and export GridData */
	if ( (gd = InitGridDataFromFltTwoDArray(arr, xll, yll, cellsz, nodata)) == NULL )	{
		ERR_ERROR("Unable to intialize GridData for exporting 2D array to file. \n", ERR_EBADFUNC);
		}
	success = ExportGridDataAsAsciiRaster(gd, fname);
	
	FreeGridData(gd);
	
	return success;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of TwoDArray to file in Binary Raster format on disk.
 * Binary Raster format consists of two files:
 * a machine readable file containing the contents of the array
 * a human readable file containing associated header information
 *
 * Arguments:
 * arr- the TwoDArray object
 * xll- lower left x coord of TwoDArray
 * yll- lower left y coord of TwoDArray
 * cellsz- cellsz of each element in TwoDArray
 * nodata- value of nodata in TwoDArray
 * ismsb- flag value alerting to byte order of machine
 * fname- name of file to write TwoDArray to on disk
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 
int ExportLIntTwoDArrayAsBinaryRaster(LIntTwoDArray * arr, double xll, double yll, int cellsz, 
										int nodata, int ismsb, char * fname)	{
	GridData * gd = NULL;
	int success;
	
	/* check args */
	if ( arr == NULL || fname == NULL )	{
		ERR_ERROR("Unable to export two dimensional array to file. \n", ERR_EINVAL);
		}
	
	/* initialize and export GridData */
	if ( (gd = InitGridDataFromLIntTwoDArray(arr, xll, yll, cellsz, nodata)) == NULL )	{
		ERR_ERROR("Unable to intialize GridData for exporting 2D array to file. \n", ERR_EBADFUNC);
		}	
	gd->ghdr->is_msbfirst = ismsb;
	success = ExportGridDataAsBinaryRaster(gd, fname);
	
	FreeGridData(gd);
	
	return success;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of TwoDArray to file in Ascii Raster format on disk.
 * Ascii Raster format consists of one file:
 * header information listed at top of file
 * contents of the array listed in space delimeted format below header
 *
 * Arguments:
 * arr- the TwoDArray object
 * xll- lower left x coord of TwoDArray
 * yll- lower left y coord of TwoDArray
 * cellsz- cellsz of each element in TwoDArray
 * nodata- value of nodata in TwoDArray
 * fname- name of file to write TwoDArray to on disk
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 	
int ExportLIntTwoDArrayAsAsciiRaster(LIntTwoDArray * arr, double xll, double yll, int cellsz,
 										int nodata, char * fname)	{
	GridData * gd = NULL;
	int success;
	
	/* check args */
	if ( arr == NULL || fname == NULL )	{
		ERR_ERROR("Unable to export two dimensional array to file. \n", ERR_EINVAL);
		}
	
	/* initialize and export GridData */
	if ( (gd = InitGridDataFromLIntTwoDArray(arr, xll, yll, cellsz, nodata)) == NULL )	{
		ERR_ERROR("Unable to intialize GridData for exporting 2D array to file. \n", ERR_EBADFUNC);
		}
	success = ExportGridDataAsAsciiRaster(gd, fname);
	
	FreeGridData(gd);
	
	return success;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of TwoDArray to file in Binary Raster format on disk.
 * Binary Raster format consists of two files:
 * a machine readable file containing the contents of the array
 * a human readable file containing associated header information
 *
 * Arguments:
 * arr- the TwoDArray object
 * xll- lower left x coord of TwoDArray
 * yll- lower left y coord of TwoDArray
 * cellsz- cellsz of each element in TwoDArray
 * nodata- value of nodata in TwoDArray
 * ismsb- flag value alerting to byte order of machine
 * fname- name of file to write TwoDArray to on disk
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 
int ExportIntTwoDArrayAsBinaryRaster(IntTwoDArray * arr, double xll, double yll, int cellsz, 
										int nodata, int ismsb, char * fname)	{
	GridData * gd = NULL;
	int success;
	
	/* check args */
	if ( arr == NULL || fname == NULL )	{
		ERR_ERROR("Unable to export two dimensional array to file. \n", ERR_EINVAL);
		}
	
	/* initialize and export GridData */
	if ( (gd = InitGridDataFromIntTwoDArray(arr, xll, yll, cellsz, nodata)) == NULL )	{
		ERR_ERROR("Unable to intialize GridData for exporting 2D array to file. \n", ERR_EBADFUNC);
		}	
	gd->ghdr->is_msbfirst = ismsb;
	success = ExportGridDataAsBinaryRaster(gd, fname);
	
	FreeGridData(gd);
	
	return success;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of TwoDArray to file in Ascii Raster format on disk.
 * Ascii Raster format consists of one file:
 * header information listed at top of file
 * contents of the array listed in space delimeted format below header
 *
 * Arguments:
 * arr- the TwoDArray object
 * xll- lower left x coord of TwoDArray
 * yll- lower left y coord of TwoDArray
 * cellsz- cellsz of each element in TwoDArray
 * nodata- value of nodata in TwoDArray
 * fname- name of file to write TwoDArray to on disk
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 	
int ExportIntTwoDArrayAsAsciiRaster(IntTwoDArray * arr, double xll, double yll, int cellsz,
 										int nodata, char * fname)	{
	GridData * gd = NULL;
	int success;
	
	/* check args */
	if ( arr == NULL || fname == NULL )	{
		ERR_ERROR("Unable to export two dimensional array to file. \n", ERR_EINVAL);
		}
	
	/* initialize and export GridData */
	if ( (gd = InitGridDataFromIntTwoDArray(arr, xll, yll, cellsz, nodata)) == NULL )	{
		ERR_ERROR("Unable to intialize GridData for exporting 2D array to file. \n", ERR_EBADFUNC);
		}
	success = ExportGridDataAsAsciiRaster(gd, fname);
	
	FreeGridData(gd);
	
	return success;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of TwoDArray to file in Binary Raster format on disk.
 * Binary Raster format consists of two files:
 * a machine readable file containing the contents of the array
 * a human readable file containing associated header information
 *
 * Arguments:
 * arr- the TwoDArray object
 * xll- lower left x coord of TwoDArray
 * yll- lower left y coord of TwoDArray
 * cellsz- cellsz of each element in TwoDArray
 * nodata- value of nodata in TwoDArray
 * ismsb- flag value alerting to byte order of machine
 * fname- name of file to write TwoDArray to on disk
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 
int ExportByteTwoDArrayAsBinaryRaster(ByteTwoDArray * arr, double xll, double yll, int cellsz, 
										int nodata, int ismsb, char * fname)	{
	GridData * gd = NULL;
	int success;
	
	/* check args */
	if ( arr == NULL || fname == NULL )	{
		ERR_ERROR("Unable to export two dimensional array to file. \n", ERR_EINVAL);
		}
	
	/* initialize and export GridData */
	if ( (gd = InitGridDataFromByteTwoDArray(arr, xll, yll, cellsz, nodata)) == NULL )	{
		ERR_ERROR("Unable to intialize GridData for exporting 2D array to file. \n", ERR_EBADFUNC);
		}	
	gd->ghdr->is_msbfirst = ismsb;
	success = ExportGridDataAsBinaryRaster(gd, fname);
	
	FreeGridData(gd);
	
	return success;
	}

/*
 * Visibility:
 * global
 *
 * Description:
 * Exports the contents of TwoDArray to file in Ascii Raster format on disk.
 * Ascii Raster format consists of one file:
 * header information listed at top of file
 * contents of the array listed in space delimeted format below header
 *
 * Arguments:
 * arr- the TwoDArray object
 * xll- lower left x coord of TwoDArray
 * yll- lower left y coord of TwoDArray
 * cellsz- cellsz of each element in TwoDArray
 * nodata- value of nodata in TwoDArray
 * fname- name of file to write TwoDArray to on disk
 *
 * Returns:
 * ERR_SUCCESS (0) if operation successful, an error code otherwise.
 * Best use of this facility is as follows...
 * int error_status = CallFunctionXXX();
 * if ( error_status)  something bad happened
 */	 	
int ExportByteTwoDArrayAsAsciiRaster(ByteTwoDArray * arr, double xll, double yll, int cellsz,
 										int nodata, char * fname)	{
	GridData * gd = NULL;
	int success;
	
	/* check args */
	if ( arr == NULL || fname == NULL )	{
		ERR_ERROR("Unable to export two dimensional array to file. \n", ERR_EINVAL);
		}
	
	/* initialize and export GridData */
	if ( (gd = InitGridDataFromByteTwoDArray(arr, xll, yll, cellsz, nodata)) == NULL )	{
		ERR_ERROR("Unable to intialize GridData for exporting 2D array to file. \n", ERR_EBADFUNC);
		}
	success = ExportGridDataAsAsciiRaster(gd, fname);
	
	FreeGridData(gd);
	
	return success;
	}
				
/* MEMORY MGT */	
void FreeGridData(void * vptr)	{
	GridData * gd = NULL;
	if ( vptr != NULL )	{
		gd = (GridData *) vptr;
		if ( gd->arr != NULL )
			FreeGridArray(gd->arr, gd->gtype);
		if ( gd->ghdr != NULL )
			FreeGridHeaderInfo(gd->ghdr);
		if ( gd->gfinfo != NULL )
			FreeGridFileInfo(gd->gfinfo);
		free(gd);
		}
	gd = NULL;
	return;
	}

void FreeGridArray(void * vptr, EnumGridType grid_type)	{
	GridArray * ga = NULL;
	if ( vptr != NULL )	{
		ga = (GridArray *) vptr;
		switch(grid_type)	{
			case EnumDblGrid:
				if ( ga->da != NULL )
					FreeDblTwoDArray(ga->da);
				break;
			case EnumFltGrid:
				if ( ga->fa != NULL )
					FreeFltTwoDArray(ga->fa);
				break;
			case EnumLIntGrid:
				if ( ga->lia != NULL )
					FreeLIntTwoDArray(ga->lia);
				break;
			case EnumIntGrid:
				if ( ga->ia != NULL )
					FreeIntTwoDArray(ga->ia);
				break;
			case EnumByteGrid:	
				if ( ga->ba != NULL )
					FreeByteTwoDArray(ga->ba);
				break;
			case EnumBitGrid:
				ERR_ERROR_CONTINUE("Data type for bit GridData not implemented, memory not freed. \n", ERR_EUNIMPL);
				break;
			default:
				ERR_ERROR_CONTINUE("Data type not recognized, GridData memory not freed. \n", ERR_EINVAL);
				break;
			}
		free(ga);
		}
	ga = NULL;
	return;
	}
	
void FreeGridHeaderInfo(void * vptr)	{
	GridHeaderInfo * ghi = NULL;
	if ( vptr != NULL )	{
		ghi = (GridHeaderInfo *) vptr;
		free(ghi);
		}
	ghi = NULL;
	return;
	}

void FreeGridFileInfo(void * vptr)	{
	GridFileInfo * gfi = NULL;
	if ( vptr != NULL )	{
		gfi = (GridFileInfo *) vptr;
		if ( gfi->main_file != NULL )
			free(gfi->main_file);
		if ( gfi->header_file != NULL )
			free(gfi->header_file);
		free(gfi);
		}
	gfi = NULL;
	return;
	}
					
/* end of GridData.c */
