#ifndef _GEO_TYPE_H_
#define _GEO_TYPE_H_

#include <string>

enum class GeoType { UNKNOWN = -1, POINT , LINE , POLYGON };


inline std::string GeoTypeToStr( GeoType type )
{
   return  type == GeoType::POINT  ? "Point" :
           type == GeoType::LINE  ? "Line" : 
           type == GeoType::POLYGON  ? "Polygon" : 
           "Unknown? ";
}

#endif // end of include guard: _GEO_TYPE_H_ 
