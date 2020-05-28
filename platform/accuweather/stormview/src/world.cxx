#include "world.h"
#include "camera.h"
#include "controls.h"
#include "layer.h"
#include "world_layer.h"
#include "particle_layer.h"
#include "radial_layer.h"
#include "sky_guard_layer.h"
#include "config.h"
#include "http_network_request.h"
#include <algorithm>
#include "logging.h"

#include <iostream>
#include "timer.h"

bool AscendingCompare(const std::unique_ptr<DefaultLayer>& left, const std::unique_ptr<DefaultLayer>& right)
{
   return left->GetLayerId() < right->GetLayerId();
}

bool DecendingCompare(const std::unique_ptr<DefaultLayer>& left, const std::unique_ptr<DefaultLayer>& right)
{
   return left->GetLayerId() > right->GetLayerId();
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
World::World() : m_layers(AscendingCompare)
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
World::World(bool ordering) : m_layers(ordering ? AscendingCompare : DecendingCompare)
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
World::~World()
{
   HttpNetworkRequest::DeleteHttpNetworkRequest();
   for(const auto& layer : m_layers)
   {
      layer->ClearCustomLayer();
   }

   m_layers.clear();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::init()
{
   m_camera = std::make_unique<Camera>();
   m_controls = std::make_unique<Controls>();

#ifdef DESKTOP
   printf("Desktop is Enabled\n");
#else
   printf("Mobile is Enabled\n");
#endif

   Config::SetCamera( m_camera.get() );
   Config::UpdateConfig();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::setViewport( float width , float height )
{
   m_camera->SetViewPort( { width , height } );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::setExtents( float left , float right , float bottom, float top )
{
   static float ltop = 0.0;

   if(left > right)
   {
      if(right < 0.0)
      {
         right = right + 360.0;
      }
      else
      {
         left = left - 180.0;
      }

      Config::SetOverIDL(true);
   }
   else
   {
      Config::SetOverIDL(false);
   }

   PointF top_left(GeoPointToMercatorPoint(PointF(left, top)));
   PointF bottom_right(GeoPointToMercatorPoint(PointF(right, bottom)));
   m_camera->SetExtents( { top_left.x() , bottom_right.x() , bottom_right.y() , top_left.y() } );

   ltop = top;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::setMousePos( float x , float y )
{
   m_controls->SetMousePos( { x , y } ); 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::setMouseState(bool left_down , bool right_down )
{
   m_controls->SetMouseState( left_down , right_down ); 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::setFrameTimeMs( float frame_time )
{
   m_frame_time = frame_time; 
   Config::GetInstance().m_frame_time = frame_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::update()
{
   ++m_update_count;
   HttpNetworkRequest::StartHttpNetworkRequest();
   HttpNetworkRequest::PollNetwork();

   Config::UpdateConfig();
   Timer::UpdateAllTimers();

   auto point = Config::GetCenter();
   if(point.x() != m_center_point.x()
         || point.y() != m_center_point.y())
   {
      Config::SetNeedToDraw(true);
      m_center_point = point;
   }

   m_animated = false;
   for( const auto& layer : m_layers )
   {
      if(!m_animated)
      {
         if((layer->IsAnimatedLayer() && layer->GetLayerEnabled()))
         {
            m_animated = true;
         }
      }

      layer->Update(this);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::needToDraw()
{
   Config::SetNeedToDraw(true);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::draw()
{
   ++m_draw_count;

//#ifdef __ANDROID__
//   int draw_count = Config::GetDrawCount();
//   if(draw_count < 3 || m_animated)
//   {
//      glClear(GL_COLOR_BUFFER_BIT);
//      for( const auto& layer : m_layers )
//         layer->Draw(this);
//      Config::SetDrawCount(draw_count++);
//   }
//   else
//   {
//      Config::SetNeedToDraw(false);
//   }
//#else
   if(Config::GetNeedToDraw() || m_animated)
   {
      glClear(GL_COLOR_BUFFER_BIT);
      for( const auto& layer : m_layers )
         layer->Draw(this);
      Config::SetNeedToDraw(false);
   }
//#endif
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::drawWithinMapboxLayer()
{
   ++m_draw_count;
   if(Config::GetNeedToDraw() || m_animated)
   {
      for( const auto& layer : m_layers )
         layer->Draw(this);
      Config::SetNeedToDraw(false);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::debug()
{
   bool log_frame_time = false;
   if( log_frame_time )
   {
      printf("Frametime: %f ms\n", m_frame_time);
      printf("Extents\n");
      printf("Left: %f, Right: %f, Top: %f, Bottom: %f\n",
            m_camera->GetExtents().left(),
            m_camera->GetExtents().right(),
            m_camera->GetExtents().top(),
            m_camera->GetExtents().bottom());
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
Camera& World::GetCamera()
{
   return *m_camera; 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
Controls& World::GetControls()
{
   return *m_controls; 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
float World::GetFrameTime()
{
   return m_frame_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::enableMutualExclusion( bool enable )
{
   m_mutual_exlusion = enable;
}




/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::EnableLayer(int layer_id, bool enable)
{
   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) {return item->GetLayerId() == layer_id; });

   if(iter != m_layers.end())
   {
      (*iter)->SetEnabled(enable);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool World::AddSkyguardLayer(int layer_id)
{
   if(layer_id >= m_unique_id)
   {
      m_unique_id = layer_id + 1;
   }

   if(CheckUniqueLayerId(layer_id))
   {
      m_layers.insert(std::make_unique<SkyGuardLayer>(layer_id));
      for( const auto& layer : m_layers )
      {
         if(!layer->IsInitialized())
            layer->Init();
      }

      return true;
   }
   else
   {
      return false;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool World::AddParticleLayer(int layer_id)
{
   if(layer_id >= m_unique_id)
   {
      m_unique_id = layer_id + 1;
   }

   if(CheckUniqueLayerId(layer_id))
   {
      m_layers.insert(std::make_unique<ParticleLayer>(layer_id));
      for( const auto& layer : m_layers )
      {
         if(!layer->IsInitialized())
            layer->Init();
      }

      return true;
   }
   else
   {
      return false;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool World::AddRadarLayer(int layer_id)
{
   if(layer_id >= m_unique_id)
   {
      m_unique_id = layer_id + 1;
   }

   if(CheckUniqueLayerId(layer_id))
   {
      m_layers.insert(std::make_unique<RadialLayer>(layer_id));
      for( const auto& layer : m_layers )
      {
         if(!layer->IsInitialized())
            layer->Init();
      }

      return true;
   }
   else
   {
      return false;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool World::AddCustomLayer(int layer_id)
{
   if(layer_id >= m_unique_id)
   {
      m_unique_id = layer_id + 1;
   }

   if(CheckUniqueLayerId(layer_id))
   {
      m_layers.insert(std::make_unique<DefaultLayer>(layer_id));
      for( const auto& layer : m_layers )
      {
         if(!layer->IsInitialized())
            layer->Init();
      }

      return true;
   }
   else
   {
      return false;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool World::AddAnimationLayer(int layer_id, const char* animation_stylization,
      int stylization_size)
{
   if(layer_id >= m_unique_id)
   {
      m_unique_id = layer_id + 1;
   }

   if(CheckUniqueLayerId(layer_id))
   {
      m_layers.insert(std::make_unique<DefaultLayer>(layer_id));
      for( const auto& layer : m_layers )
      {
         if(!layer->IsInitialized())
            layer->Init(animation_stylization, stylization_size);
      }

      return true;
   }
   else
   {
      return false;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::ClearCustomLayer(int layer_id)
{
   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) {return item->GetLayerId() == layer_id; });

   if(iter != m_layers.end())
   {
      (*iter)->ClearCustomLayer();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::RemoveCustomLayer(int layer_id)
{
   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) {return item->GetLayerId() == layer_id; });

   if(iter != m_layers.end())
      m_layers.erase(iter);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*void World::AddRenderItemToLayer(int layer_id, int item_id,
      float z_order, float *vertices, int vertices_len, float *color,
      int color_len)
{
   if(item_id >= m_unique_id)
   {
      m_unique_id = item_id + 1;
   }

   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) {return item->GetLayerId() == layer_id; });

   if(iter != m_layers.end())
   {
      std::vector<float> vertices_vec(vertices, vertices + vertices_len);
      std::vector<float> color_vec(color, color + color_len);
      (*iter)->AddRenderItem(item_id, z_order, vertices_vec, color_vec);
   }
}*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::AddToCustomLayer(int layer_id, const char* geo_json_data, int geo_size,
      const char* stylization_json_data, int stylization_size)
{
   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) {return item->GetLayerId() == layer_id; });

   if(iter != m_layers.end())
   {
      (*iter)->AddToCustomLayer(geo_json_data, geo_size, stylization_json_data,
            stylization_size);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::RemoveRenderItemFromLayer(int layer_id, int item_id)
{
   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) {return item->GetLayerId() == layer_id; });

   if(iter != m_layers.end())
   {
      (*iter)->RemoveRenderItem(item_id);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool World::CheckUniqueLayerId(int layer_id)
{
   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) { return item->GetLayerId() == layer_id; });
   if(iter != m_layers.end())
   {
      return false;
   }
   else
   {
      return true;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int World::GetUniqueId()
{
   return m_unique_id;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::UpdateTimes(unsigned int past_time, unsigned int now_time,
      unsigned int current_loop_time)
{
   Config::SetPastTime(past_time);
   Config::SetCurrentTime(now_time);
   Config::SetCurrentLoopTime(current_loop_time);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::SetScreenDensity(float density)
{
   Config::SetScreenDensity(density);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::RegisterIconToCustomLayer(int layer_id, int icon_id,
      const char *icon_png, unsigned int size)
{
   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) {return item->GetLayerId() == layer_id; });

   if(iter != m_layers.end())
   {
      (*iter)->RegisterIconToCustomLayer(icon_id, icon_png, size);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::AddIconToCustomLayer(int layer_id, int icon_id,
      const char *geo_json_data, int geo_size, const char *stylization_json_data,
      int stylization_size)
{
   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) {return item->GetLayerId() == layer_id; });

   if(iter != m_layers.end())
   {
      (*iter)->AddIconToCustomLayer(icon_id, geo_json_data, geo_size,
            stylization_json_data, stylization_size);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::RemoveIconFromCustomLayer(int layer_id, int item_id)
{
   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) {return item->GetLayerId() == layer_id; });

   if(iter != m_layers.end())
   {
      (*iter)->RemoveIconFromCustomLayer(item_id);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::ChangeRadarLocation(float lat, float lon)
{
   Config::SetLatLon(lat, lon);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::ChangeRadarProduct(int product_num, int tilt_num)
{
   Config::SetRadarProduct(product_num, tilt_num);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::ChangeRadarPalette(int palette_type)
{
   Config::SetPaletteType(palette_type);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int World::GetCurrentRadarTimeFrameEpoch(int layer_id)
{
   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) {return item->GetLayerId() == layer_id; });

   if((*iter)->IsLayer(LayerTypes::RADAR))
   {
      return (*iter)->GetCurrentTimeFrameEpoch();
   }
   else
   {
      return 0;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const char* World::GetCurrentRadarTimeFrameString(int layer_id)
{
   auto iter = std::find_if(m_layers.begin(), m_layers.end(), [layer_id](
         auto& item) {return item->GetLayerId() == layer_id; });

   if((*iter)->IsLayer(LayerTypes::RADAR))
   {
      return (*iter)->GetCurrentTimeFrameString();
   }
   else
   {
      return "";
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::setAPIKey(const char* apikey)
{
   Config::SetAPIKey(apikey);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void World::SetSSRadarMaxFrameCount(int max_frame_count)
{
   Config::SetSSRadarMaxFrameCount(max_frame_count);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int World::GetSSPastSpanTime()
{
   return Config::GetSSPastSpanTime();
}
