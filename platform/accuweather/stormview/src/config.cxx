#include "config.h"
#include "camera.h"
#include "date_time.h"
#include "glm/mat4x4.hpp"

#include <sstream>
#include <iomanip>
#include <iostream>
#include "logging.h"
#include "projections.h"

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
Config::Config()
{
   m_radial_host = "http://api.accuweather.com/maps/v1/radar/usLocalRad/";
   m_gfs_host = "http://tilergrp.accuweather.com/relay_nexrad/";
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
Config& Config::GetInstance()
{
   static Config the_instance;
   return the_instance;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const RectF& Config::GetBox( TARGET target)
{
   const auto& me = GetInstance(); 
   switch( target )
   {
      case WORLD:
      default:
         return me.camera->GetExtents();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
glm::mat4x4 Config::GetMatrix( TARGET target )
{
   const auto& me = GetInstance(); 
   switch( target )
   {
      case WORLD:
      default:
         return me.camera->GetWorldMatrix();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
PointF Config::GetViewport()
{
   const auto& me = GetInstance();
   return me.camera->GetViewport();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetCamera( Camera const * camera )
{
   auto& me = GetInstance();
   me.camera = camera;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
PointF Config::GetCenter()
{
   auto& me = GetInstance();
   return me.camera->GetCenter();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
PointF Config::WorldToViewport()
{
   auto& me = GetInstance();
   return me.camera->GetWorldToViewport();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int Config::GetPointSpriteSize()
{
   auto& me = GetInstance();
   return me.point_sprite_size;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const std::string& Config::GetRadarHost()
{
   auto& me = GetInstance();
   return me.m_radial_host;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const std::string& Config::GetGFSHost()
{
   auto& me = GetInstance();
   return me.m_gfs_host;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
float Config::GetFrameTime()
{
   auto& me = GetInstance();
   return me.m_frame_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int Config::GetTimeT()
{
   auto& me = GetInstance();
   return me.m_time_t;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::UpdateConfig()
{
   auto& me = GetInstance();
   me.m_time_t = DateTime::NowTimeT();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int Config::ConvertTimeStringEpochTime(const std::string& time)
{
   auto& me = GetInstance();

   // Convert string time to epoch time (will be local time not UTC)
/*   tm time_struct;
   std::istringstream ss(time);
   ss >> std::get_time(&time_struct, "%Y-%m-%dT%H:%M:%SZ");

   unsigned int ret;
   char* tz;
   tz = getenv("TZ");
   if (tz)
       tz = strdup(tz);
   setenv("TZ", "", 1);
   tzset();
   ret = mktime(&time_struct);
   if (tz) {
       setenv("TZ", tz, 1);
       free(tz);
   } else
       unsetenv("TZ");
   tzset();*/

   return 0;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetAPIKey(std::string apikey)
{
   auto& me = GetInstance();
   me.m_api_key = apikey;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetScreenDensity(float density)
{
   auto& me = GetInstance();
   me.m_screen_density = density;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const float Config::GetScreenDensity()
{
   auto& me = GetInstance();
   return me.m_screen_density;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const std::string Config::GetAPIKey()
{
   auto& me = GetInstance();
   return me.m_api_key;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetLatLon(float lat, float lon)
{
   auto& me = GetInstance();
   PointF merc_point = PointF(lon, lat);
   me.m_center_point = merc_point;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const PointF Config::GetLatLon()
{
   auto& me = GetInstance();
   return me.m_center_point;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetNeedToDraw(bool need_to_draw)
{
   auto& me = GetInstance();
   me.m_need_to_draw = need_to_draw;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const bool Config::GetNeedToDraw()
{
   auto& me = GetInstance();
   return me.m_need_to_draw;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetDrawCount(bool draw_count)
{
   auto& me = GetInstance();
   me.m_draw_count = draw_count;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const bool Config::GetDrawCount()
{
   auto& me = GetInstance();
   return me.m_need_to_draw;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetRadarProduct(int product_num, int tilt_num)
{
   auto& me = GetInstance();

   switch(product_num)
   {
      case -1:
         me.m_radar_product = 1000;
         me.m_radar_palette_product = LV2_REFLECTIVITY;
         break;
      case 1000:
         me.m_radar_palette_product = LV2_REFLECTIVITY;
         break;
      case 2000:
         me.m_radar_palette_product = LV2_VELOCITY;
         break;
      default:
         me.m_radar_palette_product = LV2_REFLECTIVITY;
         break;
   }

   if(product_num == -1)
   {
      me.m_smir_active = true;
   }
   else
   {
      me.m_smir_active = false;
      me.m_radar_product = product_num;
   }

   me.m_radar_tilt = tilt_num;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const int Config::GetRadarProduct()
{
   auto& me = GetInstance();
   return me.m_radar_product;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const int Config::GetTilt()
{
   auto& me = GetInstance();
   return me.m_radar_tilt;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const bool Config::GetSMIRActive()
{
   auto& me = GetInstance();
   return me.m_smir_active;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetPaletteType(int palette_type)
{
   auto& me = GetInstance();
   me.m_radar_palette_type = (PaletteType) palette_type;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const Config::PaletteType Config::GetPaletteType()
{
   auto& me = GetInstance();
   return me.m_radar_palette_type;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const Config::PaletteProduct Config::GetPaletteProduct()
{
   auto& me = GetInstance();
   return me.m_radar_palette_product;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetOverIDL(bool over_idl)
{
   auto& me = GetInstance();
   me.m_over_idl = over_idl;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const bool Config::IsOverIDL()
{
   auto& me = GetInstance();
   return me.m_over_idl;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetSSRadarMaxFrameCount(int max_frame_count)
{
   auto& me = GetInstance();
   me.m_max_frame_count = max_frame_count;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const int Config::GetSSRadarMaxFrameCount()
{
   auto& me = GetInstance();
   return me.m_max_frame_count;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetSSPastSpanTime(unsigned int past_span_time)
{
   auto& me = GetInstance();
   me.m_ss_past_span_time = past_span_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const unsigned int Config::GetSSPastSpanTime()
{
   auto& me = GetInstance();
   return me.m_ss_past_span_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetPastTime(int past_time)
{
   auto& me = GetInstance();
   me.m_past_time = past_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const unsigned int Config::GetPastTime()
{
   auto& me = GetInstance();
   return me.m_past_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetCurrentTime(int current_time)
{
   auto& me = GetInstance();
   me.m_current_time = current_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const unsigned int Config::GetCurrentTime()
{
   auto& me = GetInstance();
   return me.m_current_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Config::SetCurrentLoopTime(int current_loop_time)
{
   auto& me = GetInstance();
   me.m_current_loop_time = current_loop_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const unsigned int Config::GetCurrentLoopTime()
{
   auto& me = GetInstance();
   return me.m_current_loop_time;
}
