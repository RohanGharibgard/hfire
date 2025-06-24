#ifndef	Units_H
#define Units_H

#include <stdlib.h>

#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/* floating point value defining threshold for equality */
#define UNITS_EPSILON							(1.0e-6)

/* UNIT CONVERSION CONSTANTS */

/* mass per unit area */
#define UNITS_MGPHA_TO_TNPAC_FACTOR				(4.46e-1)

#define UNITS_MGPHA_TO_LBPFTSQ_FACTOR			(2.04816e-2)

#define UNITS_MGPHA_TO_KGPMSQ_FACTOR			(1.0e-1)

#define UNITS_KGPMSQ_TO_LBPFTSQ_FACTOR			(2.04816e-1)

/* surface area to volume ratio */
#define UNITS_CMSAV_TO_FTSAV_FACTOR				(33.0250)

#define UNITS_CMSAV_TO_MSAV_FACTOR				(100.0)

#define UNITS_MSAV_TO_FTSAV_FACTOR				(3.30250e-1)

/* length/distance */
#define UNITS_CM_TO_FT_FACTOR					(3.28084e-2)

#define UNITS_CM_TO_M_FACTOR					(1.0e-2)

#define UNITS_M_TO_FT_FACTOR					(3.28084)

/* heat content */
#define UNITS_KJPKG_TO_BTUPLB_FACTOR			(4.299e-1)

#define UNITS_KJPKG_TO_JPKG_FACTOR				(1000.0)

#define UNITS_JPKG_TO_BTUPLB_FACTOR				(4.299e-4)

/* mass per unit area */
#define UNITS_KGPM3_TO_LBPFT3_FACTOR			(6.242796e-2)
 
 /* velocity */
#define UNITS_MPSEC_TO_FTPMIN_FACTOR			(196.8504)

#define UNITS_MPSEC_TO_MILEPHR_FACTOR			(2.236936)

#define UNITS_MPSEC_TO_KMPHR_FACTOR				(3.6)

#define UNITS_MILEPHR_TO_FTPMIN_FACTOR			(88.0)

/* enumeration constant used to distinguish units */
enum EnumUnitType_	{
	EnumUnknownUnits	= 0,
	EnumEnglishUnits 	= 1,
	EnumMetricUnits 	= 2
	};
	
/* enumeration constant used to distinguish units of velocity */
enum EnumUnitVelocity_	{
	EnumUnknownVelocity	= 0,
	EnumMpsVelocity		= 1,
	EnumFpmVeloctiy		= 2,
	EnumMphVelocity		= 3,
	EnumKmphVelocity	= 4,
	EnumEndVelocity		= 5
	};
	
/* enumeration constant used to distinguish units of distance */
enum EnumUnitDistance_	{
	EnumUnknownDistance	= 0,
	EnumMetersDistance	= 1,
	EnumFeetDistance	= 2,
	EnumMilesDistance	= 3,
	EnumKMetersDistance	= 4,
	EnumCMetersDistance	= 5,
	EnumEndDistance		= 6
	};

/* enumeration constant used to distinguish units of mass per unit area */	
enum EnumUnitMassparea_ {
	EnumUnknownMassparea= 0,
	EnumMgphaMassparea	= 1,
	EnumTnpacMassparea	= 2,
	EnumLbpftsqMassparea= 3,
	EnumKgpmsqMassparea = 4,
	EnumEndMassparea	= 5
	};
	
/* enumeration constant used to distinguish units of surface area to volume */	
enum EnumUnitSav_ {
	EnumUnknownSav 		= 0,
	EnumCm2pcm3Sav	 	= 1,
	EnumM2pm3Sav	 	= 2,
	EnumFt2pft3Sav 		= 3,
	EnumEndSav	 		= 4
	};	

/* enumeration constant used to distinguish units of heat content */
enum EnumUnitHeatContent_ {
	EnumUnknownHeatContent 	= 0,
	EnumKjpkgHeatContent 	= 1,
	EnumJpkgHeatContent		= 2,
	EnumBtuplbHeatContent	= 3,
	EnumEndHeatContent		= 4
	};
	
/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */

/* possible units fuel values stored in */
typedef enum EnumUnitType_ EnumUnitType;
 
/* possible units of velocity */
typedef enum EnumUnitVelocity_ EnumUnitVelocity;

/* conversions of EnumUnitVelocity to m/s 
 *	\code
 *		1 mps = 1 mps
 *		1 fpm = 0.00508 mps
 *		1 mph = 0.44704 mps
 *		1 kmph = 0.277777 mps
 *	\endcode
 */
static const double velocity_to_mps[EnumEndVelocity] = { 0.0, 1.0, 0.00508, 0.44704, 0.277777 };

/* possible units of distance */
typedef enum EnumUnitDistance_ EnumUnitDistance;

/* conversions of EnumUnitDistance to meters 
 *	\code
 *		1 m = 1m
 *		1 ft = 0.3048 m
 *		1 mile = 1609.344 m
 *		1 km = 1000.0 m
 *		1 cm = 0.01 m
 *	\endcode
 */
static const double distance_to_m[EnumEndDistance] = { 0.0, 1.0, 0.3048, 1609.344, 1000.0, 0.01 };

/* possible units of mass per unit area */
typedef enum EnumUnitMassparea_ EnumUnitMassparea;

/* conversions of EnumUnitMassparea to megagrams per hectare 
 *	\code
 * 		1 mg/ha = 1 mg/ha
 *		1 tn/ac = 2.241692 mg/ha
 *		1 lb/ft^2 = 48.825740 mg/ha
 *		1 kg/m^2 = 10.0 mg/ha
 *	\endcode
 */
static const double massparea_to_mgpha[EnumEndMassparea] = { 0.0, 1.0, 2.241692, 48.825740, 10.0 };

/* possible units of surface area to volume */
typedef enum EnumUnitSav_ EnumUnitSav;

/* conversions of EnumUnitSav to cm^2 per cm^3 
 *	\code
 * 		1 cm^2/cm^3 = 1 cm^2/cm^3
 *		1 m^2/m^3 = 0.01 cm^2/cm^3
 *		1 ft^2/ft^3 = 0.032527 cm^2/cm^3
 *	\endcode
 */
static const double sav_to_cm2pcm3[EnumEndSav] = { 0.0, 1.0, 0.01, 0.032527 };

/* possible units of heat content */
typedef enum EnumUnitHeatContent_ EnumUnitHeatContent;

/* conversions of EnumUnitHeatContent to kJ per kg 
 * 	\code
 *		1 kJ/kg = 1 kJ/kg
 *		1 J/kg = 0.001 kJ/kg
 *		1 BTU/lb = 2.326002 kJ/kg
 * 	\endcode
 */
static const double heatcontent_to_kjpkg[EnumEndHeatContent] = { 0.0, 1.0, 0.001, 2.326002 };

/*
 *********************************************************
 * MACROS
 *********************************************************
 */

/* macro used for floating point test of zero */
#define UNITS_FP_IS_ZERO(a)						(fabs((a)) < (UNITS_EPSILON))

/* macro used for floating point test less than zero */
#define UNITS_FP_LT_ZERO(a)						((a) < (UNITS_EPSILON))

/* macro used for floating point test greater than zero */
#define UNITS_FP_GT_ZERO(a)						((a) > (UNITS_EPSILON))

/* macro used for floating point test of equality */
#define UNITS_FP_ARE_EQUAL(a,b)					(fabs((a) - (b)) < (UNITS_EPSILON))

/* converts from units in Mg/ha to tons/acre  */
#define UNITS_MGPHA_TO_TNPAC(a)					((a) * (UNITS_MGPHA_TO_TNPAC_FACTOR))

/* converts from units in tons /acre to Mg/ha */
#define UNITS_TNPAC_TO_MGPHA(a)					((a) / (UNITS_MGPHA_TO_TNPAC_FACTOR))

/* converts from units in Mg/ha to lb/ft^2  */
#define UNITS_MGPHA_TO_LBPFTSQ(a)				((a) * (UNITS_MGPHA_TO_LBPFTSQ_FACTOR))

/* converts from units in lb/ft^2 to Mg/ha  */
#define UNITS_LBPFTSQ_TO_MGPHA(a)				((a) / (UNITS_MGPHA_TO_LBPFTSQ_FACTOR))

/* converts from units in Mg/ha to kg/m^2  */
#define UNITS_MGPHA_TO_KGPMSQ(a)				((a) * (UNITS_MGPHA_TO_KGPMSQ_FACTOR))

/* converts from units in kg/m^2 to Mg/ha */
#define UNITS_KGPMSQ_TO_MGPHA(a)				((a) / (UNITS_MGPHA_TO_KGPMSQ_FACTOR))

/* converts from units in kg/m^2 to lb/ft^2 */
#define UNITS_KGPMSQ_TO_LBPFTSQ(a)				((a) * (UNITS_KGPMSQ_TO_LBPFTSQ_FACTOR))

/* converts from units in kg/m^2 to lb/ft^2 */
#define UNITS_LBPFTSQ_TO_KGPMSQ(a)				((a) / (UNITS_KGPMSQ_TO_LBPFTSQ_FACTOR))

/* converts from units of cm^2/cm^3 to ft^2/ft^3 */
#define UNITS_CMSAV_TO_FTSAV(a)					((a) * (UNITS_CMSAV_TO_FTSAV_FACTOR))

/* converts from units of ft^2/ft^3 to cm^2/cm^3 */
#define UNITS_FTSAV_TO_CMSAV(a)					((a) / (UNITS_CMSAV_TO_FTSAV_FACTOR))

/* converts from units of cm^2/cm^3 to m^2/m^3 */
#define UNITS_CMSAV_TO_MSAV(a) 					((a) * (UNITS_CMSAV_TO_MSAV_FACTOR))

/* converts from units of m^2/m^3 to cm^2/cm^3 */
#define UNITS_MSAV_TO_CMSAV(a) 					((a) / (UNITS_CMSAV_TO_MSAV_FACTOR))

/* converts from units of m^2/m^3 to ft^2/ft^3 */
#define UNITS_MSAV_TO_FTSAV(a)					((a) * (UNITS_MSAV_TO_FTSAV_FACTOR))

/* converts from units of ft^2/ft^3 to m^2/m^3 */
#define UNITS_FTSAV_TO_MSAV(a)					((a) / (UNITS_MSAV_TO_FTSAV_FACTOR))

/* converts from units of cm to ft */
#define UNITS_CM_TO_FT(a)						((a) * (UNITS_CM_TO_FT_FACTOR))

/* converts from units of ft to cm */
#define UNITS_FT_TO_CM(a)						((a) / (UNITS_CM_TO_FT_FACTOR))

/* converts from units of cm to m */
#define UNITS_CM_TO_M(a)						((a) * (UNITS_CM_TO_M_FACTOR))

/* converts from units of m to cm */
#define UNITS_M_TO_CM(a)						((a) / (UNITS_CM_TO_M_FACTOR))

/* converts from units of m to ft */
#define UNITS_M_TO_FT(a)						((a) * (UNITS_M_TO_FT_FACTOR))

/* converts from units of ft to m */
#define UNITS_FT_TO_M(a)						((a) / (UNITS_M_TO_FT_FACTOR))

/* converts from units of kJ/kg to BTU/lb */
#define UNITS_KJPKG_TO_BTUPLB(a)				((a) * (UNITS_KJPKG_TO_BTUPLB_FACTOR))

/* converts from units of BTU/lb to kJ/kg */
#define UNITS_BTUPLB_TO_KJPKG(a)				((a) / (UNITS_KJPKG_TO_BTUPLB_FACTOR))

/* converts from units of kJ/kg to J/kg */ 
#define UNITS_KJPKG_TO_JPKG(a)	 				((a) * (UNITS_KJPKG_TO_JPKG_FACTOR))

/* converts from units of J/kg to kJ/kg */ 
#define UNITS_JPKG_TO_KJPKG(a) 					((a) / (UNITS_KJPKG_TO_JPKG_FACTOR))

/* converts from units of J/kg to BTU/lb */
#define UNITS_JPKG_TO_BTUPLB(a)					((a) * (UNITS_JPKG_TO_BTUPLB_FACTOR))

/* converts from units of BTU/lb to J/kg */
#define UNITS_BTUPLB_TO_JPKG(a)					((a) / (UNITS_JPKG_TO_BTUPLB_FACTOR))

/* converts from units of kg/m^3 to lb/ft^3  */
#define UNITS_KGPM3_TO_LBPFT3(a)				((a) * (UNITS_KGPM3_TO_LBPFT3_FACTOR))
 
/* converts from units of lb/ft^3 to kg/m^3  */
#define UNITS_LBPFT3_TO_KGPM3(a)				((a) / (UNITS_KGPM3_TO_LBPFT3_FACTOR))

/* converts from units of m/s to ft/min */
#define UNITS_MPSEC_TO_FTPMIN(a)				((a) * (UNITS_MPSEC_TO_FTPMIN_FACTOR))

/* converts from units of ft/min to m/s */
#define UNITS_FTPMIN_TO_MPSEC(a)				((a) / (UNITS_MPSEC_TO_FTPMIN_FACTOR))

/* converts from units of mph to ft/min */
#define UNITS_MILEPHR_TO_FTPMIN(a)				((a) * (UNITS_MILEPHR_TO_FTPMIN_FACTOR))

/* converts from units of ft/min to mph */
#define UNITS_FTPMIN_TO_MILEPHR(a)				((a) / (UNITS_MILEPHR_TO_FTPMIN_FACTOR))

/* convert from units of m/s to mph */
#define UNITS_MPSEC_TO_MILEPHR(a)				((a) * (UNITS_MPSEC_TO_MILEPHR_FACTOR))

/* convert from units of mph to m/s */
#define UNITS_MILEPHR_TO_MPSEC(a)				((a) / (UNITS_MPSEC_TO_MILEPHR_FACTOR))

/* convert from units of m/s to km/hr */
#define UNITS_MPSEC_TO_KMPHR(a)					((a) * (UNITS_MPSEC_TO_KMPHR_FACTOR))

/* convert from units of km/hr to m/s */
#define UNITS_KMPHR_TO_MPSEC(a)					((a) / (UNITS_MPSEC_TO_KMPHR_FACTOR))

/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

int ConvertVelocityUnits(EnumUnitVelocity from_units, double from_value, 
						 EnumUnitVelocity to_units, double * to_value);
						 
int ConvertDistanceUnits(EnumUnitDistance from_units, double from_value,
						 EnumUnitDistance to_units, double * to_value);
						 
int ConvertMasspareaUnits(EnumUnitMassparea from_units, double from_value,
						  EnumUnitMassparea to_units, double * to_value);
						  
int ConvertSavUnits(EnumUnitSav from_units, double from_value,
					EnumUnitSav to_units, double * to_value);
					
int ConvertHeatContentUnits(EnumUnitHeatContent from_units, double from_value,
							EnumUnitHeatContent to_units, double * to_value);
 
#endif Units_H		/* end of Units.h */
