#include "config.h"
#include "particle_config.h"
#include "randomf.h"

#include <algorithm>
#include <iostream>

#ifdef __APPLE__
#include <math.h>
#else
// Android
#include <math.h>
#endif
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ParticleConfig& ParticleConfig::GetInstance()
{
   static ParticleConfig the_instance;
   return the_instance;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ParticleConfig::ParticleConfig() :
   m_default_width { 2.0 },
   m_image_size{ 1440 , 1440 }
//   m_image_size{ 1024 , 1024 }
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleConfig::CalculateParameters()
{
   auto& me = GetInstance();
   auto rect = Config::GetBox( Config::TARGET::WORLD );

   auto t_rect= ParticleConfig::ExtendRect( rect );
   me.m_extended_rect= ParticleConfig::LimitRect( t_rect );

   me.m_viewport_size = Config::GetViewport();

   me.m_res.setX( rect.width() / me.m_viewport_size.x() );
   me.m_res.setY( rect.height() / me.m_viewport_size.y() );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const RectF& ParticleConfig::GetExtendedRect()
{
   auto& me = GetInstance();
   return me.m_extended_rect;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const PointF& ParticleConfig::GetResolution()
{
   auto& me = GetInstance();
   return me.m_res;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const Point& ParticleConfig::GetImageSize()
{
   auto& me = GetInstance();
   return me.m_image_size;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const RectF& ParticleConfig::GetImageRect()
{
   static RectF rect
   { -3.1415 ,  3.1415 , -3.1415 , 3.1415 };
   return rect;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
RectF ParticleConfig::ExtendRect( const RectF& rect )
{
   auto& me = GetInstance();
   const float pixel_extend = 200;
   RectF ret {
      rect.left()   - ( me.m_res.x() * pixel_extend ),
      rect.right()  + ( me.m_res.x() * pixel_extend ),
      rect.bottom() - ( me.m_res.y() * pixel_extend ),
      rect.top()    + ( me.m_res.y() * pixel_extend )
   };
   return ret;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
RectF ParticleConfig::LimitRect( const RectF& rect )
{
   //static auto max_box = TileCalc::GetMaxNativeBox(); 
   static auto max_box = GetImageRect(); 
   RectF ret {
      std::max( rect.left()   , max_box.left() ),
      std::min( rect.right()  , max_box.right() ),
      std::max( rect.bottom() , max_box.bottom() ),
      std::min( rect.top()    , max_box.top() )
   };
   return ret;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
PointF ParticleConfig::GetRandomPosInRect()
{
   auto& me = GetInstance();
   PointF ret {
      RandomF::randomF() * me.m_extended_rect.width()  + me.m_extended_rect.left(),
      RandomF::randomF() * me.m_extended_rect.height() + me.m_extended_rect.bottom()
   };

   return ret;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
float ParticleConfig::GetWidth()
{
   auto& me = GetInstance();
   return me.m_default_width;
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
Point ParticleConfig::GetDimFromPoint( const PointF& pos )
{
   //static auto& me = GetInstance();
   static auto image_rect = GetImageRect();
   static auto image_size = GetImageSize();
   
   PointF per;
   bool invert_y = false;
   per.x() = ( pos.x() - image_rect.left() ) /   image_rect.width();
   per.y() = ( pos.y() - image_rect.bottom() ) / image_rect.height();
   per.y() = invert_y ?  
      1.0 - per.y() : per.y() ;

   Point ret;

#ifdef __APPLE__
   ret.x() = round(float(image_size.x())*per.x());   
   ret.y() = round(float(image_size.y())*per.y());
#elif __ANDROID__
   ret.x() = round(float(image_size.x())*per.x());
   ret.y() = round(float(image_size.y())*per.y());
#else
   ret.x() = std::round(float(image_size.x())*per.x());
   ret.y() = std::round(float(image_size.y())*per.y());
#endif

   return ret;
}
