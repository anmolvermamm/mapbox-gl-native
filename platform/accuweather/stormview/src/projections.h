#ifndef _PROJECTIONS_H_
#define _PROJECTIONS_H_


#include "pointf.h"
#include "rectf.h"
#include <cmath>

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline float LonToMercator( float lon )
{
   return (M_PI / 180.0) * lon;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline float LatToMercator( float lat )
{
 
   if (lat > 85.05)
      lat = 85.05;

   if (lat < -85.05)
      lat = -85.05;

   float rad_lat = lat * (M_PI / 180.0);
   return log((1 + sin(rad_lat)) / cos(rad_lat));
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline float MercXToLon( float x )
{
   return (180.0 / M_PI) * x;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline float MercYToLat( float y )
{
   return atan(sinh(y)) * (180.0/M_PI);
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline PointF GeoPointToMercatorPoint( const PointF& geo )
{
   PointF merc;
   merc.setX( LonToMercator( geo.x() ) );
   merc.setY( LatToMercator( geo.y() ) );
   return merc;
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline RectF GeoRectToMercatorRect( const RectF& geo )
{
   //X is lon
   //Y is lat
   return RectF(
         LonToMercator( geo.left() ),
         LonToMercator( geo.right() ),
         LatToMercator( geo.bottom() ),
         LatToMercator( geo.top() )
        );
}



#endif // end of include guard: _PROJECTIONS_H_ 
