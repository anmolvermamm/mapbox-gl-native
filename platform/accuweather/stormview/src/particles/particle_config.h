#ifndef _PARTICLE_CONFIG_H_
#define _PARTICLE_CONFIG_H_


#include "rectf.h"
#include "pointf.h"
#include "point.h"

class ParticleConfig
{
public:
   
   static void CalculateParameters();

   static const RectF& GetExtendedRect();
   static const PointF& GetResolution();
   static const Point& GetImageSize();
   static const RectF& GetImageRect();

   static RectF ExtendRect( const RectF& rect );
   static RectF LimitRect( const RectF& rect );

   static PointF GetRandomPosInRect();

   static Point GetDimFromPoint( const PointF& point );

   static float GetWidth();


private:
   RectF m_extended_rect;
   PointF m_viewport_size;
   PointF m_res;

   float m_default_width;
   Point m_image_size;

   static ParticleConfig& GetInstance();
   ParticleConfig();
   ParticleConfig(const ParticleConfig& ) = delete;
   ParticleConfig& operator=(ParticleConfig&) = delete;
   
};


#endif // end of include guard: _PARTICLE_CONFIG_H_ 
