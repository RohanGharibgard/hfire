/*! 
 * \file WindSpd.h
 * \brief Routines for calculating the windspeed during the simulation.
 *  
 * \sa Check the \htmlonly <a href="config_file_doc.html#WIND_SPEED">config file documentation</a> \endhtmlonly 
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
 
#ifndef	WindSpd_H
#define WindSpd_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "CoordTrans.h"
#include "Units.h"
#include "NLIBRand.h"
#include "FireProp.h"
#include "ChHashTable.h"
#include "KeyVal.h"
#include "DblTwoDArray.h"
#include "StrTwoDArray.h"
#include "GridData.h"
#include "FStreamIO.h"
#include "Err.h"

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
 
/* used in tokenizing WSP file */
#define WIND_SPD_WSP_COMMENT_CHAR								("#")
#define WIND_SPD_WSP_SEP_CHARS									(" =,;\t")

/* returns corresponding field index in WSP file */
#define WIND_SPD_YR_WSP_TBL_INDEX								(0)
#define WIND_SPD_MO_WSP_TBL_INDEX								(1)
#define WIND_SPD_DY_WSP_TBL_INDEX								(2)
 
/* value of NO DATA used in WSP files */
#define WIND_SPD_WSP_NO_DATA_VALUE								(-1)

/* keyword indicating units of windspeed in WSP */
#define WIND_SPD_WSP_KEYWORD_UNITS								("UNITS")
#define WIND_SPD_WSP_KEYWORD_MILEPHR							("MILEPHR")
#define WIND_SPD_WSP_KEYWORD_KMPHR								("KMPHR")

/* used in tokenizing ATM file */
#define WIND_SPD_ATM_COMMENT_CHAR								("#")
#define WIND_SPD_ATM_SEP_CHARS									(" =,;\t")

/* returns corresponding field index in ATM file */
#define WIND_SPD_MO_ATM_TBL_INDEX								(0)
#define WIND_SPD_DY_ATM_TBL_INDEX								(1)
#define WIND_SPD_HR_ATM_TBL_INDEX								(2)
#define WIND_SPD_FILE_ATM_TBL_INDEX								(6)

/* keyword indicating units of windspeed in ATM */
#define WIND_SPD_ATM_KEYWORD_ENGLISH							("ENGLISH")		/* ENGLISH == MILEPHR */
#define WIND_SPD_ATM_KEYWORD_METRIC								("METRIC")		/* METRIC == KMPHR */

/* used in tokenizing list associated with uniform range property */
#define WIND_SPD_RNG_SEP_CHARS									(" ;")

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/*!	\def WIND_SPD_RAWS_REF_HGT_METERS  
 *	\brief standard reference height of RAWS (Remote Automated Weather Station) stations 
 */
#define WIND_SPD_RAWS_REF_HGT_METERS							(UNITS_FT_TO_M(20.0))

/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* takes an hour 0-23 and returns corresponding field index in WSP file */
#define WIND_SPD_HR_TO_WSP_TBL_INDEX(hour)						((hour) + 3)

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/*! \def WIND_SPD_REF_HGT_TO_MIDFLAME
 *  \brief Given a windspeed at reference height, returns windspeed at midflame.
 *
 * Given a windspeed argument assumed to be taken at a reference height (usu. 20 feet, height of RAWS-
 * Remote Automated Weather Station) above the fuel bed, reduces the windspeed using a logarithmic
 * profile according to a value typical of what is experienced halfway between the top of the fuel bed
 * and the top of the flame above the fuel bed.  The formula used approximates a log profile and is 
 * described in (Albini and Baughman, 1979).  Rather than explicitly specify a value for flame height
 * and fuel bed height (the former is usually unavailable until after a prediction for the rate of spread
 * has been made), windspeed in nonforested areas (chaparral and grass fuels) is reduced from 20 ft
 * to a height of 2 * the fuel bed height.  This procedure is described and used in
 * (Finney, 1998).  An alternative approach to explicitly calculating the midflame windspeed is to
 * apply a windspeed adjustment factor (WAF) which corrects the windspeed observed at a height of 20 ft.
 * above the vegetation to that experienced at midflame.  A table of suggested WAF factors for each NFFL
 * fuel model appears in Rothermel (1983). 
 * An example: a windspeed of 10 m/s taken at 6.09 m above the fuel bed of height 1.8288 m is reduced
 * to 3.6 m/s at midflame height using the macro below.  For comparison, Rothermel's (1986) suggested
 * WAF for NFFL 4 (same height as our example) is 0.6 (current standard practice in the field is to use 0.5).
 * \code
 * WindSpeed At 6.09m (m/s)		Fuel Bed Height	(m)		Midflame Windspeed (m/s)
 * 10.0							1.8288					3.6408
 * 10.0							0.9144					2.9879
 * 10.0							0.3048					2.2840
 * 10.0							0.1524					1.9787
 * \endcode
 * Finney, 1998 has superficially different equation:
 * \code
 *		wsmps / log(((20 + (1.18*(2*fbedhgtm))) / (0.43*(2*fbedhgtm))))
 * \endcode
 * This equation produced identical values as implementation used here.  
 * The log function as referenced here is the 
 * the natural logarithm (ln) because in the ANSI C library the natural log function is named log.
 * \param wsmps value of windspeed in meters/sec at reference height above fuel bed
 * \param refhgtm reference height above fuel bed windspeed has been measured at, in meters
 * \param fbedhgt height of the fuel bed, in meters
 * \retval double Windspeed corrected to midflame height, in meters/sec
 */
#define WIND_SPD_REF_HGT_TO_MIDFLAME(wsmps, refhgtm, fbedhgtm)	(ConvertWindSpeedAtRefHgtToArbitraryHgt(wsmps, refhgtm, (2 *(fbedhgtm))))
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

/*! \fn int GetWindSpeedMpsFIXEDFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour,
												double rwx, double rwy, double * wspmps)
 *	\brief retrieves time and space dependent windspeed, in m/s, at a cell
 *
 *	Using this option each cell in the simulation domain is assigned the same windspeed
 * 	based upon index into a table of historical values
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#WIND_SPEED">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of simulation properties
 *	\param fbedhgtm height of the fuel bed in meters
 *	\param month date to retreive windspeed for  
 *	\param day date to retreive windspeed for 
 *	\param hour date to retreive windspeed for 
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate 
 *	\param wspmps if function returns successfully, windspeed in m/s
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int GetWindSpeedMpsFIXEDFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour,
										double rwx, double rwy, double * wspmps);

/*! \fn int GetWindSpeedMpsRANDUFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour,
												double rwx, double rwy, double * wspmps)
 *	\brief retrieves time and space dependent windspeed, in m/s, at a cell
 *
 *	Using this option each cell in the simulation domain is assigned the same windspeed
 * 	based upon a constrained uniform random number generating function
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#WIND_SPEED">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of simulation properties
 *	\param fbedhgtm height of the fuel bed in meters
 *	\param month date to retreive windspeed for  
 *	\param day date to retreive windspeed for 
 *	\param hour date to retreive windspeed for 
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 *	\param wspmps if function returns successfully, windspeed in m/s
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int GetWindSpeedMpsRANDUFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour,
										double rwx, double rwy, double * wspmps);

/*! \fn int GetWindSpeedMpsRANDHFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour,
												double rwx, double rwy, double * wspmps)
 *	\brief retrieves time and space dependent windspeed, in m/s, at a cell
 *
 *	Using this option each cell in the simulation domain is assigned the same windspeed
 * 	based upon random index into a table of historical values
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#WIND_SPEED">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of simulation properties
 *	\param fbedhgtm height of the fuel bed in meters
 *	\param month date to retreive windspeed for  
 *	\param day date to retreive windspeed for 
 *	\param hour date to retreive windspeed for 
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 *	\param wspmps if function returns successfully, windspeed in m/s
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */
int GetWindSpeedMpsRANDHFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour,
										double rwx, double rwy, double * wspmps);

/*! \fn int GetWindSpeedMpsSPATIALFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour, 
												double rwx, double rwy, double * wspmps)
 *	\brief retrieves time and space dependent windspeed, in m/s, at a cell
 *
 *	Using this option each cell in the simulation domain is assigned a unique windspeed
 * 	based upon index into a raster of windspeeds
 *	\sa ChHashTable
 *	\sa Check the \htmlonly <a href="config_file_doc.html#WIND_SPEED">config file documentation</a> \endhtmlonly
 *	\param proptbl ChHashTable of simulation properties
 *	\param fbedhgtm height of the fuel bed in meters
 *	\param month date to retreive windspeed for  
 *	\param day date to retreive windspeed for 
 *	\param hour date to retreive windspeed for 
 *	\param rwx real-world x coordinate
 *	\param rwy real-world y coordinate
 *	\param wspmps if function returns successfully, windspeed in m/s
 *	\retval ERR_SUCCESS(0) if operation successful, an error code otherwise
 *	\note Best use of this facility is as follows:
 *	\code	
 *			int error_status = SomeFunctionXXX();
 *			if ( error_status )
 *				// something bad happened
 *	\endcode
 */	
int GetWindSpeedMpsSPATIALFromProps(ChHashTable * proptbl, double fbedhgtm, int month, int day, int hour, 
											double rwx, double rwy, double * wspmps);

/*!	\fn double ConvertWindSpeedAtRefHgtToArbitraryHgtAB79(double wsmps, double refhgtm, double hgtm)
 * 	\brief Given a windspeed at a reference height in m/s returns an adjusted windspeed at arbitrary height.
 *
 * Given a windspeed argument assumed to be taken at a reference height (usu. 20 feet, height of RAWS-
 * Remote Automated Weather Station), returns the windspeed experienced at the height supplied as an 
 * argument. The formula used approximates a log profile and is described in (Albini and Baughman, 1979).
 * The approach used here assumes the same logarithmic windspeed profile is valid for both daytime and
 * nighttime. When correctinng windspeed for calculating dead fuel moisture, Rothermel (1986) suggests
 * that the log profile is innapropriate during nighttime conditions. In addition, Beer (1990) suggests
 * that the assumption of a logarithmic profile may be innapropriate when local atmospheric conditions
 * are unstable.
 * The macro WIND_SPD_REF_HGT_TO_MIDFLAME calls this function to retrieve the windspeed experienced
 * in the distance halfway between the top of the fuel bed and the top of the flame above the fuel bed.
 * \param wsmps value of windspeed in meters/sec at reference height above fuel bed
 * \param refhgtm reference height above fuel bed windspeed has been measured at, in meters
 * \param hgtm height at which the value for windspeed will be returned, in meters
 * \retval double Windspeed corrected to an arbitrary height, in meters/sec
 */
double ConvertWindSpeedAtRefHgtToArbitraryHgtAB79(double wsmps, double refhgtm, double hgtm);

/*!	\fn double ConvertWindSpeedAtRefHgtToArbitraryHgtBHP(double wsmps, double refhgtm, double hgtm)
 * 	\brief Given a windspeed at a reference height in m/s returns an adjusted windspeed at arbitrary height.
 *
 * This function is like ConvertWindSpeedAtRefHgtToArbitraryHgtAB79 except the windspeed adjustment
 * factor is rescaled to be consistent with values for the windspeed adjustment factor returned
 * from BEHAVEPLUS (BHP).
 * \param wsmps value of windspeed in meters/sec at reference height above fuel bed
 * \param refhgtm reference height above fuel bed windspeed has been measured at, in meters
 * \param hgtm height at which the value for windspeed will be returned, in meters
 * \retval double Windspeed corrected to an arbitrary height, in meters/sec
 */
double ConvertWindSpeedAtRefHgtToArbitraryHgtBHP(double wsmps, double refhgtm, double hgtm);
 												 
#endif WindSpd_H		/* end of WindSpd.h */
