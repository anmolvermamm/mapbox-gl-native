#ifndef _SITE_INFO_H_
#define _SITE_INFO_H_

#include <string>
#include "pointf.h"

class SiteInfo
{
public:
   float angle;
   float bin_spacing;
   float first_bin_offset;
   float lat;
   float lon;
   PointF m_center_geo;
   PointF m_center_merc;
   float resolution_degree;
   int moment;
   int num_bins;
   int num_rays;
   int tilt;
   int time;
   std::string pid;
   std::string site_name;
};

#endif // end of include guard: _SITE_INFO_H_ 
