#include "default_layer.h"

#include "world.h"
#include <iostream>
#include "pointf.h"

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DefaultLayer::DefaultLayer(int layer_id)
{
   m_layer_id = layer_id;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DefaultLayer::~DefaultLayer()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultLayer::Init(const char* animation_stylization, int stylization_size)
{
   if(stylization_size > 0)
   {
      this->m_is_animated = true;
   }

   m_default_renderer = std::make_unique<DefaultRenderer>();
   std::cout << "Init Custom Layer Id: " << m_layer_id << std::endl;
   m_default_renderer->Init(animation_stylization, stylization_size);
   m_initialized = true;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultLayer::Update(World *world)
{
   if(m_enabled)
   {
      float frametime = world->GetFrameTime();
      m_default_renderer->Update(frametime);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultLayer::Draw(World *world)
{
   if(m_enabled)
   {
      m_default_renderer->Draw();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool DefaultLayer::IsLayer(LayerTypes type)
{
   return type == LayerTypes::CUSTOMLAYER;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultLayer::SetEnabled(bool enabled)
{
   m_enabled = enabled;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool DefaultLayer::GetLayerEnabled()
{
   return m_enabled;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int DefaultLayer::GetLayerId() const
{
   return m_layer_id;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool DefaultLayer::IsInitialized()
{
   return m_initialized;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultLayer::SetLayerId(int layer_id)
{
   m_layer_id = layer_id;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*bool DefaultLayer::AddRenderItem(int item_id, float z_order,
      std::vector<float> vectors, std::vector<float> color)
{
   m_default_renderer->AddRenderItem(item_id, z_order, vectors, color);
}*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultLayer::RemoveRenderItem(int item_id)
{
   m_default_renderer->RemoveRenderItem(item_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultLayer::AddToCustomLayer(const char* geo_json_data, int geo_size,
      const char* stylization_json_data, int stylization_size)
{
   m_default_renderer->AddToCustomLayer(geo_json_data, geo_size,
         stylization_json_data, stylization_size);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultLayer::RegisterIconToCustomLayer(int icon_id, const char *icon_png,
      unsigned int size)
{
   m_default_renderer->RegisterIconToCustomLayer(icon_id, icon_png, size);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultLayer::AddIconToCustomLayer(int icon_id,
      const char* geo_json_data, int geo_size, const char *stylization_json_data,
      int stylization_size)
{
   m_default_renderer->AddIconToCustomLayer(icon_id, geo_json_data, geo_size,
         stylization_json_data, stylization_size);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultLayer::RemoveIconFromCustomLayer(int item_id)
{
   m_default_renderer->RemoveIconFromCustomLayer(item_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultLayer::ClearCustomLayer()
{
   if(m_default_renderer)
      m_default_renderer->ClearCustomLayer();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int DefaultLayer::GetCurrentTimeFrameEpoch()
{
   return 0;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const char* DefaultLayer::GetCurrentTimeFrameString()
{
   return "";
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool DefaultLayer::IsAnimatedLayer()
{
   return m_is_animated;
}
