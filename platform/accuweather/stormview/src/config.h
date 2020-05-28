#ifndef _CONFIG_H_
#define _CONFIG_H_


#include <string>
#include "rectf.h"
#include "pointf.h"
#include "glm/fwd.hpp"

class Camera;

//Need a better name for this class
class Config
{
public:

   enum TARGET { WORLD };
   enum PaletteProduct {LV2_REFLECTIVITY, LV2_VELOCITY};
   enum PaletteType {ACCU_PALETTE, NWS_PALETTE};
   //enum PaletteType
   friend class World;
   
   static const RectF& GetBox( TARGET target = WORLD );
   static glm::mat4x4 GetMatrix( TARGET target = WORLD );
   static PointF GetViewport();
   static PointF GetCenter();
   static const PointF GetLatLon();
   static void SetLatLon(float lat, float lon);
   static const int GetRadarProduct();
   static const int GetTilt();
   static void SetRadarProduct(int product_num, int tilt_num);
   static const bool GetNeedToDraw();
   static void SetNeedToDraw(bool need_to_draw);
   static const bool GetDrawCount();
   static void SetDrawCount(bool draw_count);
   static const bool GetSMIRActive();
   static const PaletteProduct GetPaletteProduct();
   static const PaletteType GetPaletteType();
   static void SetPaletteType(int palette_type);
   static void SetOverIDL(bool over_idl);
   static const bool IsOverIDL();

   static PointF WorldToViewport();
   static int GetPointSpriteSize();

   static void SetScreenDensity(float density);
   static void SetAPIKey(std::string apikey);
   static const float GetScreenDensity();
   static const std::string GetAPIKey();
   static void SetSSRadarMaxFrameCount(int max_frame_count);
   static const int GetSSRadarMaxFrameCount();

   static void SetPastTime(int past_time);
   static const unsigned int GetPastTime();
   static void SetCurrentTime(int current_time);
   static const unsigned int GetCurrentTime();
   static void SetCurrentLoopTime(int current_loop_time);
   static const unsigned int GetCurrentLoopTime();

   static const std::string& GetRadarHost();
   static const std::string& GetGFSHost();

   static float GetFrameTime();
   static int GetTimeT();
   static unsigned int ConvertTimeStringEpochTime(const std::string& time);

   static const unsigned int GetSSPastSpanTime();
   static void SetSSPastSpanTime(unsigned int span_time);
   
protected:
   static void SetCamera( Camera const * camera );

private:
   Config();
   static void UpdateConfig();

   static Config& GetInstance();
   Config(const Config& ) = delete;
   Config& operator=(Config&) = delete;

   Camera const * camera = nullptr;

   std::string m_radial_host;
   std::string m_gfs_host;

   int point_sprite_size = 10;

   int m_time_t;
   float m_frame_time;
   float m_screen_density = 1.0;
   PointF m_center_point = PointF(0.0f, 0.0f);
   bool m_need_to_draw = true;
   int m_draw_count = 0;
   int m_radar_product = 1000;
   int m_radar_tilt = 1;
   bool m_smir_active = true;
   PaletteProduct m_radar_palette_product = LV2_REFLECTIVITY;
   PaletteType m_radar_palette_type = ACCU_PALETTE;
   bool m_over_idl = false;
   int m_max_frame_count = 0;
   unsigned int m_ss_past_span_time = 3600;
   std::string m_api_key = "ekhA5PxPCm3KgNImGcXtjUJqRd4Rt3Cb";

   unsigned int m_current_loop_time = 0;
   unsigned int m_current_time = 0;
   unsigned int m_past_time = 0;
};


#endif // end of include guard: _CONFIG_H_ 
