#include <stdlib.h>
#include <stdio.h>

#include "FuelModel.h"
#include "RothFuelModel.h"
#include "Roth1972.h"
#include "WindSpd.h"

/* stores field names output to textfile */
#define NUM_WND_FLDS_CSV						(7)
#define NUM_WND_VALS_CSV_LINE				(7)
static char * wind_field_names[NUM_WND_FLDS_CSV] = 
  {"WindMag", "ModFMNum1", "ModFMNum2", "ExtFMNum1", "ExtFMNum2", "MidFlameWndFMNum1", "MidFlameWndFMNum2"};

#define NUM_SLP_FLDS_CSV						(5)
#define NUM_SLP_VALS_CSV_LINE				(5)
static char * slp_field_names [NUM_SLP_FLDS_CSV] = 
  {"SlpPcnt", "ModFMNum1", "ModFMNum2", "ExtFMNum1", "ExtFMNum2"};

/* environmental variables: wind and slope */
#define MIN_WIND_MAG 						(0.0)					/* all wind magnitudes in ft/min */
#define MAX_WIND_MAG 						(1968.504)
#define STEP_WIND_MAG 					(196.8504)

#define STATIC_WIND_MAG					(0.0)
#define STATIC_WIND_AZ					(0.0)

#define MIN_SLP_PCNT 						(0.0)					/* all slopes in pcnt eg rise/run */		
#define MAX_SLP_PCNT 						(1.01)
#define STEP_SLP_PCNT 					(0.1)

#define STATIC_ASPECT						(180.0)

#define STATIC_SLOPE						(0.0)

#define STATIC_ELL_ADJ          (1.0)

void QuitFatal(char * emessage)	{
	if ( emessage == 0 )	{
		fprintf(stderr, "FATAL ERROR. Program Aborting.\n");
		}
	else	{
		fprintf(stderr, "%s", emessage);
		}
		
	exit(1);
	}	
	
void WriteOutFieldNamesCSV(char * fname, char ** field_names, int num_field_names)	{
	FILE * csvFile = 0;
	int single_val;
	
	if ( fname != 0 && field_names != 0 && (csvFile = fopen(fname, "a")) != NULL)	{
		for(single_val = 0; single_val < num_field_names; single_val++)	{
			fprintf(csvFile, "%s", field_names[single_val]);
			if ( single_val != (num_field_names - 1) )
				fprintf(csvFile, ",");
			}
		fprintf(csvFile, "\n");
		fclose(csvFile);
		}
	return;
	}
	
void WriteOutLineCSV(char * fname, float * line_vals, int num_vals_line)	{
	FILE * csvFile = 0;
	int single_val;
	
	if ( fname != 0 && line_vals != 0 && (csvFile = fopen(fname, "a")) != NULL)	{
		for(single_val = 0; single_val < num_vals_line; single_val++)	{
			fprintf(csvFile, "%f", line_vals[single_val]);
			if ( single_val != (num_vals_line - 1) )
				fprintf(csvFile, ",");
			}
		fprintf(csvFile, "\n");
		fclose(csvFile);
		}
	return;
	}	
		
int main(int argc, char * argv[]) {
	/* argc		  = number of arguments (should be 7) */
	/* argv[0] 	= name of executable */
	/* argv[1] 	= fmd file fname */
	/* argv[2]	= fm1 num from fmd file */
	/* argv[3]	= fm2 num from fmd file */
	/* argv[4] 	= output wnd dir */
	/* argv[5] 	= output slp dir */		
  /* argv[6]  = windspeed at ref */
	/* argv[7]	= NULL */

	/* VARIABLE DECLARATION */	
	FuelModel * fm1	= NULL;
	FuelModel * fm2	= NULL;
	float currStep;
  double midflameWndMps1, midflameWndFpm1, fbedhgtm1,
         midflameWndMps2, midflameWndFpm2, fbedhgtm2;
	
	double modFMoist[EnumNumSizeClasses], extFMoist[EnumNumSizeClasses];
	
	float outModFM1, outModFM2, outExtFM1, outExtFM2;
	float csv_line_vals[256];
		
	/*VARIABLE INITIALIZATION*/
	if ( argc != 7 )	{
		printf("Usage: %s <fmd filename> <fm1 num> <fm2 num> <output wind fname> <output slope fname> <wind at ref> \n", argv[0]);
		return 0;
		}	
	
	/* initialize FuelModels to process */
	fm1 = InitFuelModelFMDFile(atoi(argv[2]), NULL, NULL, argv[1], EnumRoth);
	fm2 = InitFuelModelFMDFile(atoi(argv[3]), NULL, NULL, argv[1], EnumRoth);
	
	if ( fm1 == NULL || fm2 == NULL )	{
		QuitFatal("FuelModels not initialized.");
		}
		
	/* initialize RothFuelModels for calls to Roth1972 */
	if ( Roth1972FireSpreadSetFuelBed(fm1->rfm) || Roth1972FireSpreadSetFuelBed(fm2->rfm) )	{
		QuitFatal("RothFuelModel not initialized.");
		}
		
	/* intialize fmoisture to constant values */
	modFMoist[EnumD1HSizeClass] 	= 0.08;		
	modFMoist[EnumD10HSizeClass] 	= 0.09;
	modFMoist[EnumD100HSizeClass] 	= 0.11;	
	modFMoist[EnumLHSizeClass] 		= 1.5;		
	modFMoist[EnumLWSizeClass] 		= 1.1;
	
	extFMoist[EnumD1HSizeClass] 	= 0.02;		
	extFMoist[EnumD10HSizeClass] 	= 0.04;
	extFMoist[EnumD100HSizeClass] 	= 0.05;
	extFMoist[EnumLHSizeClass] 		= 0.9;		
	extFMoist[EnumLWSizeClass] 		= 0.7;
		
	/* calculate the rates of spread across range of wind and slope conditions*/	
	/* write headers into files */
	WriteOutFieldNamesCSV(argv[4], wind_field_names, NUM_WND_FLDS_CSV);		
	WriteOutFieldNamesCSV(argv[5], slp_field_names, NUM_SLP_FLDS_CSV);
				
	/* increment wind magnitude */
	for( currStep = MIN_WIND_MAG; currStep <= MAX_WIND_MAG; currStep+=STEP_WIND_MAG)
  {	
    /* Fuel Model # 1 : windspeed */
    midflameWndFpm1 = currStep;
    if ( atoi(argv[6]) )
    {
      midflameWndMps1 = UNITS_FTPMIN_TO_MPSEC(midflameWndFpm1);
      fbedhgtm1 = fm1->rfm->fdepth;
      if ( fm1->rfm->units == EnumEnglishUnits ) { fbedhgtm1 = UNITS_FT_TO_M(fbedhgtm1); }
      midflameWndMps1 = ConvertWindSpeedAtRefHgtToArbitraryHgtBHP(midflameWndMps1, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm1);
      midflameWndFpm1 = UNITS_MPSEC_TO_FTPMIN(midflameWndMps1);
    }

    /* Fuel Model # 2 : windspeed */
    midflameWndFpm2 = currStep;
    if ( atoi(argv[6]) )
    {
      midflameWndMps2 = UNITS_FTPMIN_TO_MPSEC(midflameWndFpm2);
      fbedhgtm2 = fm2->rfm->fdepth;
      if ( fm2->rfm->units == EnumEnglishUnits ) { fbedhgtm2 = UNITS_FT_TO_M(fbedhgtm2); }
      midflameWndMps2 = ConvertWindSpeedAtRefHgtToArbitraryHgtBHP(midflameWndMps2, WIND_SPD_RAWS_REF_HGT_METERS, 2.0 * fbedhgtm2);
      midflameWndFpm2 = UNITS_MPSEC_TO_FTPMIN(midflameWndMps2);
    }

		/* Fuel Model # 1: moderate fuel mositure conditions */	
		Roth1972FireSpreadNoWindNoSlope(fm1->rfm, modFMoist[EnumD1HSizeClass], modFMoist[EnumD10HSizeClass],
		 			modFMoist[EnumD100HSizeClass], modFMoist[EnumLHSizeClass], modFMoist[EnumLWSizeClass]);			
		Roth1972FireSpreadWindSlopeMax(fm1->rfm, midflameWndFpm1, (((int)(STATIC_WIND_AZ + 180.0)) % 360), 
					(STATIC_SLOPE * 100.0), STATIC_ASPECT, STATIC_ELL_ADJ);
		outModFM1 = fm1->rfm->rp->ros_max;

		/* Fuel Model # 2: moderate fuel mositure conditions */	
		Roth1972FireSpreadNoWindNoSlope(fm2->rfm, modFMoist[EnumD1HSizeClass], modFMoist[EnumD10HSizeClass],
		 			modFMoist[EnumD100HSizeClass], modFMoist[EnumLHSizeClass], modFMoist[EnumLWSizeClass]);			
		Roth1972FireSpreadWindSlopeMax(fm2->rfm, midflameWndFpm2, (((int)(STATIC_WIND_AZ + 180.0)) % 360), 
					(STATIC_SLOPE * 100.0), STATIC_ASPECT, STATIC_ELL_ADJ);
		outModFM2 = fm2->rfm->rp->ros_max;		

		/* Fuel Model # 1: extreme fuel mositure conditions */	
		Roth1972FireSpreadNoWindNoSlope(fm1->rfm, extFMoist[EnumD1HSizeClass], extFMoist[EnumD10HSizeClass],
		 			extFMoist[EnumD100HSizeClass], extFMoist[EnumLHSizeClass], extFMoist[EnumLWSizeClass]);			
		Roth1972FireSpreadWindSlopeMax(fm1->rfm, midflameWndFpm1, (((int)(STATIC_WIND_AZ + 180.0)) % 360), 
					(STATIC_SLOPE * 100.0), STATIC_ASPECT, STATIC_ELL_ADJ);
		outExtFM1 = fm1->rfm->rp->ros_max;

		/* Fuel Model # 2: extreme fuel mositure conditions */			
		Roth1972FireSpreadNoWindNoSlope(fm2->rfm, extFMoist[EnumD1HSizeClass], extFMoist[EnumD10HSizeClass],
		 			extFMoist[EnumD100HSizeClass], extFMoist[EnumLHSizeClass], extFMoist[EnumLWSizeClass]);			
		Roth1972FireSpreadWindSlopeMax(fm2->rfm, midflameWndFpm2, (((int)(STATIC_WIND_AZ + 180.0)) % 360), 
					(STATIC_SLOPE * 100.0), STATIC_ASPECT, STATIC_ELL_ADJ);
		outExtFM2 = fm2->rfm->rp->ros_max;			
			
		/* write out line CSV: WindMag ModFM1 ModFM2 ExtFM1 ExtFM2 */
		csv_line_vals[0] = UNITS_FTPMIN_TO_MPSEC(currStep);
		csv_line_vals[1] = UNITS_FTPMIN_TO_MPSEC(outModFM1);
		csv_line_vals[2] = UNITS_FTPMIN_TO_MPSEC(outModFM2);
		csv_line_vals[3] = UNITS_FTPMIN_TO_MPSEC(outExtFM1);			
		csv_line_vals[4] = UNITS_FTPMIN_TO_MPSEC(outExtFM2);
    csv_line_vals[5] = UNITS_FTPMIN_TO_MPSEC(midflameWndFpm1);
    csv_line_vals[6] = UNITS_FTPMIN_TO_MPSEC(midflameWndFpm2);
		WriteOutLineCSV(argv[4], csv_line_vals, NUM_WND_VALS_CSV_LINE);
		}
		
	/* increment slope percent */
	for( currStep = MIN_SLP_PCNT; currStep <= MAX_SLP_PCNT; currStep+=STEP_SLP_PCNT)	{
	/* WARNING: the maximum ROS occurs when the aspect is 180 from the wind direction, hence a
				wind with an azimuth of 0 (coming from north) and azimuth of 180 (south-facing)
				results in maximum  ROS */
		/* moderate fuel mositure conditions */						
		Roth1972FireSpreadNoWindNoSlope(fm1->rfm, modFMoist[EnumD1HSizeClass], modFMoist[EnumD10HSizeClass],
		 			modFMoist[EnumD100HSizeClass], modFMoist[EnumLHSizeClass], modFMoist[EnumLWSizeClass]);			
		Roth1972FireSpreadWindSlopeMax(fm1->rfm, STATIC_WIND_MAG, (((int)(STATIC_WIND_AZ + 180.0)) % 360), 
					(currStep * 100.0), STATIC_ASPECT, STATIC_ELL_ADJ);
		outModFM1 = fm1->rfm->rp->ros_max;
		
		Roth1972FireSpreadNoWindNoSlope(fm2->rfm, modFMoist[EnumD1HSizeClass], modFMoist[EnumD10HSizeClass],
		 			modFMoist[EnumD100HSizeClass], modFMoist[EnumLHSizeClass], modFMoist[EnumLWSizeClass]);			
		Roth1972FireSpreadWindSlopeMax(fm2->rfm, STATIC_WIND_MAG, (((int)(STATIC_WIND_AZ + 180.0)) % 360), 
					(currStep * 100.0), STATIC_ASPECT, STATIC_ELL_ADJ);
		outModFM2 = fm2->rfm->rp->ros_max;			
		
		/* extreme fuel mositure conditions */							
		Roth1972FireSpreadNoWindNoSlope(fm1->rfm, extFMoist[EnumD1HSizeClass], extFMoist[EnumD10HSizeClass],
		 			extFMoist[EnumD100HSizeClass], extFMoist[EnumLHSizeClass], extFMoist[EnumLWSizeClass]);			
		Roth1972FireSpreadWindSlopeMax(fm1->rfm, STATIC_WIND_MAG, (((int)(STATIC_WIND_AZ + 180.0)) % 360), 
					(currStep * 100.0), STATIC_ASPECT, STATIC_ELL_ADJ);
		outExtFM1 = fm1->rfm->rp->ros_max;	
		
		Roth1972FireSpreadNoWindNoSlope(fm2->rfm, extFMoist[EnumD1HSizeClass], extFMoist[EnumD10HSizeClass],
		 			extFMoist[EnumD100HSizeClass], extFMoist[EnumLHSizeClass], extFMoist[EnumLWSizeClass]);			
		Roth1972FireSpreadWindSlopeMax(fm2->rfm, STATIC_WIND_MAG, (((int)(STATIC_WIND_AZ + 180.0)) % 360), 
					(currStep * 100.0), STATIC_ASPECT, STATIC_ELL_ADJ);
		outExtFM2 = fm2->rfm->rp->ros_max;				
		
		/* write out one line of CSV: SlpPcnt ModFM1 ModFM2 ExtFM1 ExtFM2 */
		csv_line_vals[0] = currStep;
		csv_line_vals[1] = UNITS_FTPMIN_TO_MPSEC(outModFM1);
		csv_line_vals[2] = UNITS_FTPMIN_TO_MPSEC(outModFM2);
		csv_line_vals[3] = UNITS_FTPMIN_TO_MPSEC(outExtFM1);			
		csv_line_vals[4] = UNITS_FTPMIN_TO_MPSEC(outExtFM2);
		WriteOutLineCSV(argv[5], csv_line_vals, NUM_SLP_VALS_CSV_LINE);			
		}
	
	/* cleanup */
	FreeFuelModel(fm1);
	FreeFuelModel(fm2);	
	
	return 0;
	}
					
/* end of roth1972cmp.c */