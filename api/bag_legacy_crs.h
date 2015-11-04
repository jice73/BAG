#ifndef BAG_LEGACY_CRS_H
#define BAG_LEGACY_CRS_H

#include "bag_stdint.h"
#include "bag_c_types.h"

/* Legacy BAG datum definition */
typedef enum bagDatums
{
        wgs84,
        wgs72,
        nad83,
        unknown
} bagDatum;

/* Legacy Coordinate Type Enumeration */
typedef enum Coordinate_Types
{
  Geodetic,
  GEOREF,
  Geocentric,
  Local_Cartesian,    
  MGRS,
  UTM,
  UPS,
  Albers_Equal_Area_Conic,
  Azimuthal_Equidistant,
  BNG,
  Bonne,
  Cassini,
  Cylindrical_Equal_Area,
  Eckert4,
  Eckert6,
  Equidistant_Cylindrical,
  Gnomonic,
  Lambert_Conformal_Conic,
  Mercator,
  Miller_Cylindrical,
  Mollweide,
  Neys,
  NZMG,
  Oblique_Mercator,
  Orthographic,
  Polar_Stereo,
  Polyconic,
  Sinusoidal,
  Stereographic,
  Transverse_Cylindrical_Equal_Area,
  Transverse_Mercator,
  Van_der_Grinten,
  Unknown
} Coordinate_Type;

/* structure for parameters of all bag supported horizontal coord. sys.
        mercator, tm, utm, ps, ups, lambert, & geodetic. */
typedef struct t_bagProjectionParameters
{
        bagDatum datum;                               /* wgs84, wgs72, nad83, ...   */
        char  ellipsoid[256];                           /* ellipsoid                  */
        char  vertical_datum[256];                      /* vertical datum             */
        double origin_latitude;                          /* degrees                    */
        double central_meridian;                         /* degrees                    */
        double std_parallel_1;                           /* degrees                    */
        double std_parallel_2;                           /* degrees                    */
        double false_easting;                            /* meters                     */
        double false_northing;                           /* meters                     */
        double scale_factor;                             /* unitless                   */
        double latitude_of_true_scale;                   /* degrees                    */
        double longitude_down_from_pole;                 /* degrees                    */
        double latitude_of_centre;                       /* degrees                    */
        double longitude_of_centre;                      /* degrees                    */
        int32_t zone;                                     /* utm zone 1-60              */
        int32_t utm_override;                             /* utm: 0=autozone,1=use zone */
} bagProjectionParameters;

/* Legacy Reference system definition */
typedef struct t_bagLegacyReferenceSystem
{
    Coordinate_Type coordSys;                         /* either Geodetic or Mercator Transvers_Mercator,etc */
    bagProjectionParameters geoParameters;            /* Parameters for projection information                        */
} bagLegacyReferenceSystem;

Coordinate_Type bagCoordsys(const char *str);
bagDatum bagDatumID(const char *str);

/*! \brief  bagLegacyToWkt
* Description:
*     Utility function used to convert the old reference system definition structures
*     into a WKT (Well Known Text) string.
*
*  \param    system		The legacy reference system to convert.
*  \param	  hBuffer   	Modified to contain the horizontal reference system definition as WKT.
*	\param	  hBuffer_size	The size of the	horizontal definition buffer passed in.
*  \param	  vBuffer   	Modified to contain the vertical reference system definition as WKT.
*	\param	  vBuffer_size	The size of the	vertical definition buffer passed in.
*
* \return On success, a value of zero is returned.  On failure a value of -1 is returned.
*/
bagError bagLegacyToWkt(const bagLegacyReferenceSystem system,
    char *hBuffer, uint32_t hBuffer_size, char *vBuffer, uint32_t vBuffer_size);

/*! \brief  bagWktToLegacy
* Description:
*     Utility function used to convert a WKT (Well Known Text) reference system definition
*     into the old reference system defiition structures.
*
*     Some WKT definitions can not be converted into the old structures.
*
*  \param    horiz_wkt     String buffer containing the horizontal WKT reference system definition.
*  \param    vert_wkt      String buffer containing the vertical WKT reference system definition.
*  \param	  system	    Modified to contain the legacy reference system type.
*
* \return On success, a value of zero is returned.  On failure a value of -1 is returned.
*/
bagError bagWktToLegacy(const char *horiz_wkt, const char *vert_wkt, bagLegacyReferenceSystem *system);

#endif  //BAG_LEGACY_CRS_H