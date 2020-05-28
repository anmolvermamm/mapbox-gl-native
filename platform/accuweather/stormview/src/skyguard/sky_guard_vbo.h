#ifndef _SKYGUARD_VBO_H_
#define _SKYGUARD_VBO_H_

#include "geo_type.h"
#include "color.h"
#include <vector>
#include "rectf.h"

class PolygonF;
class SkyGuardVBO;

class SkyGuardVBOGeometry
{
public:
   unsigned int buffer = 0;
   unsigned int prev_buffer = 0;
   unsigned int id = 0;
   unsigned int count = 0;
   GeoType type;
   Color color;

   ~SkyGuardVBOGeometry();
   SkyGuardVBOGeometry( GeoType type , unsigned id , const PolygonF& poly );

   SkyGuardVBOGeometry(const SkyGuardVBOGeometry&) = delete;
   SkyGuardVBOGeometry& operator=(const SkyGuardVBOGeometry&) = delete;

   SkyGuardVBOGeometry(SkyGuardVBOGeometry&&);
   SkyGuardVBOGeometry& operator=(SkyGuardVBOGeometry&&) = delete;


};

class SkyGuardVBO
{
   public:
   ~SkyGuardVBO(); 
   
   void PushPolygon( GeoType type,  int id ,const PolygonF& poly );

   std::vector<SkyGuardVBOGeometry> geometry;
};


#endif // end of include guard: _SKYGUARD_VBO_H_ 
