#include <stdlib.h>
#include <stdio.h>

#include "opt.h"
#include "opt_util.h"

#include "RothFuelModel.h"
#include "Roth1972.h"
#include "WindSpd.h"

/*
** @brief application metadata information
*/

static opt_app_meta_t APP_META =
{
  "calcros",
  "Compute the maximum rate of fire spread using Rothermel (1972).",
  NULL,
  "1.0.0",
  "(c) 2007 Marco Morais"
};

/*
** @brief application command-line options
*/

static opt_param_meta_t HELP =
{
  "h",
  "help",
  "display this help and exit",
  OPT_ARG_FALSE
};
static opt_param_meta_t FUEL_MODEL_FNAME =
{
  "f",
  "fuel_model_fname",
  "fuel model filename",
  OPT_ARG_IS_REQUIRED
};
static opt_param_meta_t FUEL_MODEL_NUM =
{
  "n",
  "fuel_model_num",
  "fuel model number",
  "1"
};
static opt_param_meta_t D1HFM =
{
  NULL,
  "d1hfm",
  "dead 1 hour fuel moisture",
  "0.02"
};
static opt_param_meta_t D10HFM =
{
  NULL,
  "d10hfm",
  "dead 10 hour fuel moisture",
  "0.04"
};
static opt_param_meta_t D100HFM =
{
  NULL,
  "d100hfm",
  "dead 100 hour fuel moisture",
  "0.05"
};
static opt_param_meta_t LHFM =
{
  NULL,
  "lhfm",
  "live herbaceous fuel moisture",
  "0.90"
};
static opt_param_meta_t LWFM =
{
  NULL,
  "lwfm",
  "live woody fuel moisture",
  "0.70"
};
static opt_param_meta_t WIND_SPD_MPS =
{
  NULL,
  "wind_spd_mps",
  "wind speed, in m/s",
  "0.0"
};
static opt_param_meta_t WIND_AZ_DEG =
{
  NULL,
  "wind_az_deg",
  "azimuth from which wind is blowing, in degrees",
  "0.0"
};
static opt_param_meta_t SLP_PCNT =
{
  NULL,
  "slp_pcnt",
  "slope percent\n"
  "                                    "
  "eg 100 means 100% slope = 1 unit rise / 1 unit run",
  "0.0"
};
static opt_param_meta_t ASP_DEG =
{
  NULL,
  "asp_deg",
  "terrain aspect\n"
  "                                    "
  "aspect is measured in 0-360 degrees\n"
  "                                    "
  "a value of -1 corresponds to perfectly level terrain",
  "-1.0"
};
static opt_param_meta_t EAF =
{
  NULL,
  "eaf",
  "ellipse adjustment factor\n"
  "                                    "
  "values are relative to standard model of fire spread\n"
  "                                    "
  "value < 1.0 = more circular\n"
  "                                    "
  "value > 1.0 = more elliptical",
  "1.0"
};
static opt_param_meta_t WAF =
{
  NULL,
  "waf",
  "windspeed adjustment factor\n"
  "                                    "
  "choose one of the following methods:\n"
  "                                    "
  "value of 'NOWAF' assumes windspeed at midflame, no adjustment\n"
  "                                    "
  "value of 'AB79' uses method of Albini and Baughman, 1979\n"
  "                                    "
  "value of 'BHP' uses method of BEHAVEPLUS",
  "BHP"
};
static opt_param_meta_t VERBOSE =
{
  "v",
  "verbose",
  "run program with verbose output",
  OPT_ARG_FALSE
};

static opt_param_meta_t * PARAM_META[] =
{
  &HELP,
  &FUEL_MODEL_FNAME,
  &FUEL_MODEL_NUM,
  &D1HFM,
  &D10HFM,
  &D100HFM,
  &LHFM,
  &LWFM,
  &WIND_SPD_MPS,
  &WIND_AZ_DEG,
  &SLP_PCNT,
  &ASP_DEG,
  &EAF,
  &WAF,
  &VERBOSE
};
static const size_t PARAM_META_SIZE =
  sizeof(PARAM_META) / sizeof(PARAM_META[0]);

int main(int argc, char * argv[])
{
  char *          fuel_model_fname = NULL;
  FILE *          fuel_model_file = NULL;
  long            fuel_model_num;
  RothFuelModel * fuel_model;
  double          d1hfm; 
  double          d10hfm;
  double          d100hfm;
  double          lhfm;
  double          lwfm;
  double          wind_spd_mps, wind_spd_fpm, fbedhgtm;
  double          wind_az_deg;
  double          slp_pcnt;
  double          asp_deg;
  double          eaf;
  char *          waf;
  long            verbose;

  size_t          opt_idx;

  /* process application options */
  if ( opt_getopt(argc, argv, PARAM_META, PARAM_META_SIZE) != EXIT_SUCCESS )
  {
      opt_fprintf_help(stdout, &APP_META, PARAM_META, PARAM_META_SIZE);
      exit(EXIT_FAILURE);
  }
  if ( opt_is_param_name_set(PARAM_META, PARAM_META_SIZE, "help") )
  {
      opt_fprintf_help(stdout, &APP_META, PARAM_META, PARAM_META_SIZE);
      exit(EXIT_SUCCESS);
  }
  if ( opt_is_missing_req_arg(PARAM_META, PARAM_META_SIZE) )
  {
      opt_fprintf_missing_req_arg(stderr, &APP_META, PARAM_META, PARAM_META_SIZE);
      exit(EXIT_FAILURE);
  }
  GET_STRING_ARG(fuel_model_fname, PARAM_META, PARAM_META_SIZE, "fuel_model_fname", opt_idx);
  GET_LONG_ARG(fuel_model_num, PARAM_META, PARAM_META_SIZE, "fuel_model_num", opt_idx);
  GET_DOUBLE_ARG(d1hfm, PARAM_META, PARAM_META_SIZE, "d1hfm", opt_idx);
  GET_DOUBLE_ARG(d10hfm, PARAM_META, PARAM_META_SIZE, "d10hfm", opt_idx);
  GET_DOUBLE_ARG(d100hfm, PARAM_META, PARAM_META_SIZE, "d100hfm", opt_idx);
  GET_DOUBLE_ARG(lhfm, PARAM_META, PARAM_META_SIZE, "lhfm", opt_idx);
  GET_DOUBLE_ARG(lwfm, PARAM_META, PARAM_META_SIZE, "lwfm", opt_idx);
  GET_DOUBLE_ARG(wind_spd_mps, PARAM_META, PARAM_META_SIZE, "wind_spd_mps", opt_idx);
  GET_DOUBLE_ARG(wind_az_deg, PARAM_META, PARAM_META_SIZE, "wind_az_deg", opt_idx);
  GET_DOUBLE_ARG(slp_pcnt, PARAM_META, PARAM_META_SIZE, "slp_pcnt", opt_idx);
  GET_DOUBLE_ARG(asp_deg, PARAM_META, PARAM_META_SIZE, "asp_deg", opt_idx);
  GET_DOUBLE_ARG(eaf, PARAM_META, PARAM_META_SIZE, "eaf", opt_idx);
  GET_STRING_ARG(waf, PARAM_META, PARAM_META_SIZE, "waf", opt_idx);
  GET_BOOL_ARG(verbose, PARAM_META, PARAM_META_SIZE, "verbose", opt_idx);

  /* open the fuel model file and initialize the fuel model */
  if ( (fuel_model_file = fopen(fuel_model_fname, "rb")) == NULL )
  {
    fprintf(stderr, "unable to open fuel model file %s\n", fuel_model_fname);
    exit(EXIT_FAILURE);
  }
  if ( (fuel_model = InitRothFuelModelFMDFile(fuel_model_file, fuel_model_num)) == NULL )
  {
    fprintf(stderr, "unable to read model number %d from fuel model file %s\n", fuel_model_num, fuel_model_fname);
    exit(EXIT_FAILURE);
  }
  if ( fuel_model_file != NULL ) { fclose(fuel_model_file); }

  /* display the fuel model if we are running in verbose mode */
  if ( verbose )
  {
    if ( RothFuelModelMetricToEnglish(fuel_model) )
    {
      fprintf(stderr, "unable to convert model from metric to english units");
      exit(EXIT_FAILURE);
    }
    RothFuelModelDumpToStream(fuel_model, stdout);
  }

  /* compute the windspeed adjustment factor and convert units */
  if ( strcmp("NOWAF", waf) == 0 )
  {
    /* no adjustment */
  }
  else if ( strcmp("AB79", waf) == 0 )
  {
    fbedhgtm = fuel_model->fdepth;
    if ( fuel_model->units == EnumEnglishUnits )
    {
      fbedhgtm = UNITS_FT_TO_M(fbedhgtm);
    }
    fbedhgtm *= 2.0;
    wind_spd_mps = ConvertWindSpeedAtRefHgtToArbitraryHgtAB79(
        wind_spd_mps, WIND_SPD_RAWS_REF_HGT_METERS, fbedhgtm);
  }
  else if ( strcmp("BHP", waf) == 0 )
  {
    fbedhgtm = fuel_model->fdepth;
    if ( fuel_model->units == EnumEnglishUnits )
    {
      fbedhgtm = UNITS_FT_TO_M(fbedhgtm);
    }
    fbedhgtm *= 2.0;
    wind_spd_mps = ConvertWindSpeedAtRefHgtToArbitraryHgtBHP(
        wind_spd_mps, WIND_SPD_RAWS_REF_HGT_METERS, fbedhgtm);
  }
  wind_spd_fpm = UNITS_MPSEC_TO_FTPMIN(wind_spd_mps);

  /* initialize the fuel bed to the fuel model */
  if ( Roth1972FireSpreadSetFuelBed(fuel_model) )
  {
    fprintf(stderr, "unable to create fuel bed from fuel particles\n");
    exit(EXIT_FAILURE);
  }

  /* compute the no-wind and no-slope rate of spread */
  if ( Roth1972FireSpreadNoWindNoSlope(fuel_model, d1hfm, d10hfm, d100hfm, lhfm, lwfm) )
  {
    fprintf(stderr, "unable to compute no-wind no-slope rate of spread\n");
    exit(EXIT_FAILURE);
  }

  if ( Roth1972FireSpreadWindSlopeMax(fuel_model, wind_spd_fpm, wind_az_deg, slp_pcnt, asp_deg, eaf) )
  {
    fprintf(stderr, "unable to compute maximum rate of spread\n");
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, 
    "# %3s %6s %6s %6s %6s %6s %8s %3s %3s %3s %3s %4s %9s %9s\n", 
    "NUM", "D1H", "D10H", "D100H", "LH", "LW", "WSP_MPS", "WAZ", "SLP", "ASP", "EAF", "WAF", "ROS_0", "ROS_MAX");
  fprintf(stdout, 
    "  %3d %6.2f %6.2f %6.2f %6.2f %6.2f %8.4f %3.0f %3.0f %3.0f %3.1f %4s %9.4f %9.4f\n", 
    fuel_model_num,
    d1hfm,
    d10hfm,
    d100hfm,
    lhfm,
    lwfm,
    wind_spd_mps,
    wind_az_deg,
    slp_pcnt,
    asp_deg,
    eaf,
    waf,
    UNITS_FTPMIN_TO_MPSEC(fuel_model->rp->ros_0), 
    UNITS_FTPMIN_TO_MPSEC(fuel_model->rp->ros_max));

  /* cleanup */
  FreeRothFuelModel(fuel_model);

  exit(EXIT_SUCCESS);
}

/* calcros.c */