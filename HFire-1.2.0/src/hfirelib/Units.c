#include "Units.h"

int ConvertVelocityUnits(EnumUnitVelocity from_units, double from_value, 
						 EnumUnitVelocity to_units, double * to_value)	{
	double mps_value = velocity_to_mps[from_units] * from_value;
	*to_value = mps_value / velocity_to_mps[to_units];
	return ERR_SUCCESS;
	}
	
int ConvertDistanceUnits(EnumUnitDistance from_units, double from_value,
						 EnumUnitDistance to_units, double * to_value)	{
	double m_value = distance_to_m[from_units] * from_value;
	*to_value = m_value / distance_to_m[to_units];
	return ERR_SUCCESS;
	}
	
int ConvertMasspareaUnits(EnumUnitMassparea from_units, double from_value,
						  EnumUnitMassparea to_units, double * to_value) {
	double mgpha_value = massparea_to_mgpha[from_units] * from_value;
	*to_value = mgpha_value / massparea_to_mgpha[to_units];
	return ERR_SUCCESS;
	}
	
int ConvertSavUnits(EnumUnitSav from_units, double from_value,
					EnumUnitSav to_units, double * to_value)	{
	double cm2pcm3_value = sav_to_cm2pcm3[from_units] * from_value;
	*to_value = cm2pcm3_value / sav_to_cm2pcm3[to_units];
	return ERR_SUCCESS;
	}
	
int ConvertHeatContentUnits(EnumUnitHeatContent from_units, double from_value,
							EnumUnitHeatContent to_units, double * to_value)	{
	double kjpkg_value = heatcontent_to_kjpkg[from_units] * from_value;
	*to_value = kjpkg_value / heatcontent_to_kjpkg[to_units];
	return ERR_SUCCESS;
	}

/* end of Units.c */
