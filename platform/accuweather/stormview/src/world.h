#ifndef _WORLD_H_
#define _WORLD_H_

#include <memory>
#include <vector>
#include <set>
#include <string>
#include "layer_types.h"
#include "default_layer.h"
#include "glow/gl_include.h"


class Camera;
class Controls;
class Layer;

class World
{
public:


   World();
   World(bool ordering);
   virtual ~World();

   void init();
   void setViewport( float width , float height );
   void setExtents( float left , float right , float bottom, float top );
   void SetScreenDensity(float density);
   void setMousePos( float x , float y );
   void setMouseState(bool left_down , bool right_down );
   void setFrameTimeMs( float frame_time );
   void setAPIKey(const char* apikey);
   void update();
   void draw();
   void drawWithinMapboxLayer();
   void needToDraw();

   bool AddSkyguardLayer(int layer_id);
   bool AddParticleLayer(int layer_id);
   bool AddRadarLayer(int layer_id);
   bool AddCustomLayer(int layer_id);
   bool AddAnimationLayer(int layer_id, const char *animation_stylization,
         int styliztion_size);
   void ClearCustomLayer(int layer_id);
   bool CheckUniqueLayerId(int layer_id);
   void RemoveCustomLayer(int layer_id);
   //void AddRenderItemToLayer(int layer_id, int item_id,
   //      float z_order, float *vertices, int vertices_len, float *color,
   //      int color_len);
   void RemoveRenderItemFromLayer(int layer_id, int item_id);
   void AddToCustomLayer(int layer_id, const char* geo_json_data, int geo_size,
         const char* stylization_json_data, int stylization_size);
   void RegisterIconToCustomLayer(int layer_id, int icon_id,
         const char *icon_png, unsigned int size);
   void AddIconToCustomLayer(int layer_id, int icon_id,
         const char *geo_json_data, int geo_size,
         const char *stylization_json_data, int stylization_size);
   void RemoveIconFromCustomLayer(int layer_id, int item_id);
   void ChangeRadarLocation(float lat, float lon);
   void ChangeRadarProduct(int product_num, int tilt_num);
   void ChangeRadarPalette(int palette_type);
   void SetSSRadarMaxFrameCount(int max_frame_count);
   unsigned int GetCurrentRadarTimeFrameEpoch(int layer_id);
   const char* GetCurrentRadarTimeFrameString(int layer_id);
   unsigned int GetSSPastSpanTime();

   void UpdateTimes(unsigned int past_time, unsigned int now_time,
         unsigned int current_loop_time);

   int GetUniqueId();


   void enableMutualExclusion( bool enable );

   void EnableLayer(int layer_id, bool enable);

   void debug();

   Camera& GetCamera();
   Controls& GetControls();
   float GetFrameTime();

private:
   std::unique_ptr<Camera> m_camera;
   std::unique_ptr<Controls> m_controls;
   std::set<std::unique_ptr<DefaultLayer>, bool (*)(const std::unique_ptr<DefaultLayer>&,
         const std::unique_ptr<DefaultLayer>&)> m_layers;
   float m_frame_time = 0;

   int m_update_count = 0;
   int m_draw_count = 0;

   bool m_mutual_exlusion = false;
   int m_unique_id = 0;
   PointF m_center_point = PointF(0.0f, 0.0f);
   bool m_animated = false;
   
};


#endif // end of include guard: _WORLD_H_ 
