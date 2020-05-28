#include "viewer_api.h"
#include "world.h"
#include "error_handler.h"
#include <cstring>
#include <memory>
//#include "glow.h" //#include "glow_shader_mem.h" #include <iostream>

std::unique_ptr<World> world;
struct options_struct options_default_struct = {true};

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_init()
{
   if(!world)
   {
      world = std::make_unique<World>();
      world->init();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_init_params(default_options options)
{
   options_default_struct = options;
   if(!world)
   {
      world = std::make_unique<World>(options.ordering);
      world->init();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_destroy()
{
   if( world )
   {
      world.reset();
      world.release();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool viewer_exists()
{
   return world != nullptr;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_set_viewport( float width , float height )
{
   world->setViewport( width , height );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_set_extents( float left , float right , float bottom, float top )
{

   world->setExtents( left ,right , bottom , top );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_set_mos_pos( float x , float y , bool left_down , bool right_down)
{
   world->setMousePos( x , y );
   world->setMouseState( left_down , right_down );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_set_frame_time_ms( float frame_time )
{
   world->setFrameTimeMs( frame_time );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_update()
{
   world->update();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_draw()
{
   world->draw();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_draw_within_mapbox_layer()
{
   world->drawWithinMapboxLayer();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_need_to_draw()
{
   world->needToDraw();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_debug()
{
   world->debug();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_enable_mutual_exclusion( bool enable )
{
   world->enableMutualExclusion( enable );
}




/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int get_viewer_error_code()
{
   return ErrorHandler::GetTopErrorCode();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const char * get_viewer_error_msg()
{
   return ErrorHandler::GetTopErrorMessage();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int get_viewer_error_size()
{
   return ErrorHandler::GetTopErrorSize();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int get_viewer_error_count()
{
   return ErrorHandler::GetErrorCount();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void clear_viewer_error()
{
   ErrorHandler::ClearTopError();
}


/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool viewer_add_skyguard_layer(int layer_id)
{
   return world->AddSkyguardLayer(layer_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool viewer_add_wind_layer(int layer_id)
{
   return world->AddParticleLayer(layer_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool viewer_add_radar_layer(int layer_id)
{
   return world->AddRadarLayer(layer_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_enable_layer(int layer_id, bool enable)
{
   world->EnableLayer(layer_id, enable);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool viewer_add_custom_layer(int layer_id)
{
   if(!world)
   {
      world = std::make_unique<World>();
      world->init();
   }

   return world->AddCustomLayer(layer_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool viewer_add_animation_layer(int layer_id, const char *animation_stylization,
      int stylization_size)
{
   if(!world)
   {
      world = std::make_unique<World>();
      world->init();
   }

   return world->AddAnimationLayer(layer_id, animation_stylization,
         stylization_size);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_clear_custom_layer(int layer_id)
{
   world->ClearCustomLayer(layer_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_remove_custom_layer(int layer_id)
{
   world->RemoveCustomLayer(layer_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
//void viewer_add_render_item_to_layer(int layer_id, int item_id,
//      float z_order, float *vertices, int vertices_len, float *color,
//      int color_len)
//{
//   world->AddRenderItemToLayer(layer_id, item_id, z_order, vertices, vertices_len,
//         color, color_len);
//}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_remove_render_item_from_layer(int layer_id, int item_id)
{
   world->RemoveRenderItemFromLayer(layer_id, item_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_add_to_custom_layer(int layer_id, const char* geo_json_data, int geo_size,
      const char* stylization_json_data, int stylization_size)
{
   world->AddToCustomLayer(layer_id, geo_json_data, geo_size,
         stylization_json_data, stylization_size);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int get_unique_id()
{
   return world->GetUniqueId();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_update_times(unsigned int past_time, unsigned int now_time,
      unsigned int current_loop_time)
{
   world->UpdateTimes(past_time, now_time, current_loop_time);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_set_screen_density(float density)
{
   world->SetScreenDensity(density);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_register_icon_to_custom_layer(int layer_id, int icon_id,
      const char *icon_png, unsigned int size)
{
   world->RegisterIconToCustomLayer(layer_id, icon_id, icon_png, size);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_add_icon_to_custom_layer(int layer_id, int icon_id,
      const char* geo_json_data, int geo_size, const char *stylization_json_data,
      int stylization_size)
{
   world->AddIconToCustomLayer(layer_id, icon_id, geo_json_data, geo_size,
         stylization_json_data, stylization_size);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_remove_icon_from_custom_layer(int layer_id, int item_id)
{
   world->RemoveIconFromCustomLayer(layer_id, item_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_change_radar_location(float lat, float lon)
{
   world->ChangeRadarLocation(lat, lon);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_set_ss_radar_max_frame_count(int max_frame_count)
{
   world->SetSSRadarMaxFrameCount(max_frame_count);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_change_radar_product(SS_RADAR_PRODUCT product_num,
      SS_RADAR_PRODUCT_TILT tilt_num)
{
   world->ChangeRadarProduct(product_num, tilt_num);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_change_radar_palette(PALETTE_TYPE palette_type)
{
   world->ChangeRadarPalette(palette_type);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int get_current_radar_time_frame_epoch(int layer_id)
{
   return world->GetCurrentRadarTimeFrameEpoch(layer_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const char* get_current_radar_time_frame_string(int layer_id)
{
   return world->GetCurrentRadarTimeFrameString(layer_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void viewer_update_apikey(const char* apikey)
{
   world->setAPIKey(apikey);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int viewer_get_ss_past_span_time()
{
   return world->GetSSPastSpanTime();
}

