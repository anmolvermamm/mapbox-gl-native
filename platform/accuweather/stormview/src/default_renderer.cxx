#include "default_renderer.h"

#include "glow/glow_point_sprite.h"
#include "glow/glow_lines.h"
#include "glow/glow.h"
#include "polygonf.h"
#include "glm/mat4x4.hpp"
#include "color.h"
#include "config.h"
#include "default_vbo.h"
#include "glow/basic_sprite.h"
#include "glow/basic_icon.h"
#include "geo_type.h"
#include "ext/json.hpp"
#include "projections.h"
#include "string"
#include <vector>
#include "byte_array.h"

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DefaultRenderer::DefaultRenderer()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DefaultRenderer::~DefaultRenderer()
{
   if(m_vbo_manager)
   {
      m_vbo_manager.reset();
      m_vbo_manager.release();
   }

   BasicSprite::DeleteBasicSprite();
   GlowLines::DeleteGlowLines();
   GlowPointSprite::DeleteGlowPointSprite();
   BasicIcon::DeleteBasicIcon();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultRenderer::Init(const char* animation_stylization, int stylization_size)
{
   if(stylization_size > 0)
   {
      auto style_data = ConvertCharArrayToJson(animation_stylization, stylization_size);
      if(style_data != nullptr)
      {
         m_animation_manager = std::make_unique<AnimationManager>(style_data);
      }
   }
   else
   {
      printf("No animation stylization for custom layer");
   }


   BasicSprite::InitBasicSprite();
   GlowLines::InitGlowLines();
   GlowPointSprite::InitGlowPointSprite();
   BasicIcon::InitBasicIcon();
   m_vbo_manager = std::make_unique<DefaultVBO>();
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultRenderer::Update(float milliseconds)
{
   if(m_animation_manager != nullptr)
   {
      m_animation_manager->UpdateTime(milliseconds);
   }

   // parse a number of feature geojson items and update buffer data here
   int max_size = 50;
   int current_size = m_process_json_queue.size();
   for(int count = 0; (count < max_size) && (count < m_process_json_queue.size()); count++)
   {
      // parse and add draw items to list
      ParseFeatures(m_process_json_queue.at(0), m_style_point_map, m_style_line_map,
            m_style_poly_map, m_vbo_manager);
      m_process_json_queue.pop_front();
   }

   if(current_size > 0)
   {
      // Update buffers to draw items
      for(const auto& point_styles : m_style_point_map)
      {
         m_vbo_manager->UpdateBufferData(GeoType::POINT, point_styles.second);
      }

      for(const auto& line_styles : m_style_line_map)
      {
         m_vbo_manager->UpdateBufferData(GeoType::LINE, line_styles.second);
      }

      for(const auto& poly_styles : m_style_poly_map)
      {
         m_vbo_manager->UpdateBufferData(GeoType::POLYGON, poly_styles.second);
      }

      Config::SetNeedToDraw(true);
   }

   int max_size_icon = 50;
   int current_size_icons = m_process_json_icon_queue.size();
   for(int count = 0; (count < max_size_icon) && (count < m_process_json_icon_queue.size()); count++)
   {
      // parse and add draw items to list
      ParseIconFeatures(m_process_json_icon_queue.at(0), m_style_icon_map, m_vbo_manager);
      m_process_json_icon_queue.pop_front();
   }

   if(current_size_icons > 0)
   {
      // Update buffers to draw items
      for(const auto& icon_styles : m_style_icon_map)
      {
         m_vbo_manager->UpdateBufferData(icon_styles.second,
               m_vbo_manager->m_styles.at(icon_styles.second).icon_id);
      }

      Config::SetNeedToDraw(true);
   }

   //printf("%f", m_animation_manager->GetCurrentOpacity());

   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultRenderer::Draw()
{
   auto mat = Config::GetMatrix();

   for(const auto& render_poly_list : m_vbo_manager->m_poly_buffer)
   {
      for(const auto& render_polys : render_poly_list.second)
      {
         if(m_vbo_manager->m_styles.at(render_poly_list.first).weight > 0.0)
         {
            GlowLines::Draw(mat, GeoType::POLYGON, render_polys.buffer,
               render_polys.element_buffer, render_polys.element_buffer_size,
               m_vbo_manager->m_styles.at(render_poly_list.first), true, m_animation_manager);
         }

         BasicSprite::Draw(mat, render_polys.buffer,
               render_polys.element_buffer, render_polys.element_buffer_size,
               m_vbo_manager->m_styles.at(render_poly_list.first), m_animation_manager);
      }
   }

   for(const auto& render_lines : m_vbo_manager->m_line_buffer)
   {
      GlowLines::Draw(mat, GeoType::LINE, render_lines.second.buffer, render_lines.second.element_buffer,
            render_lines.second.element_buffer_size, m_vbo_manager->m_styles.at(render_lines.first),
            false, m_animation_manager);
   }

   for(const auto& render_points : m_vbo_manager->m_point_buffer)
   {
      GlowPointSprite::Draw(mat, render_points.second.vertex_buffer_points,
            render_points.second.points_size, m_vbo_manager->m_styles.at(render_points.first),
            m_animation_manager);
   }

   for(const auto& render_icon : m_vbo_manager->m_icons_buffer)
   {
      BasicIcon::Draw(mat, render_icon.second.vertex_buffer_points, render_icon.second.points_size,
            render_icon.second.texture_ref, m_vbo_manager->m_styles.at(render_icon.first));
   }

   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*void DefaultRenderer::AddRenderItem(int item_id, float z_order,
      std::vector<float> vectors, std::vector<float> color)
{
   Color fill_color(color.at(0), color.at(1), color.at(2));
   PolygonF new_poly;

   for(int idx = 0; idx < vectors.size(); idx+=2)
   {
      new_poly.emplace_back(vectors.at(idx), vectors.at(idx + 1));
   }

   m_vbo_manager->PushPolygon(GeoType::POINT, item_id, fill_color, z_order,
         new_poly);
}*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultRenderer::RemoveRenderItem(int item_id)
{
   m_vbo_manager->RemovePolygon(item_id);
   Config::SetNeedToDraw(true);
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultRenderer::AddToCustomLayer(const char* geo_json_data, int geo_size,
      const char* stylization_json_data, int stylization_size)
{
   auto json_data = ConvertCharArrayToJson(geo_json_data, geo_size);
   auto style_data = ConvertCharArrayToJson(stylization_json_data, stylization_size);

   if(json_data == nullptr)
   {
      printf("Unable to parse json response");
      return;
   }

   std::deque<nlohmann::json> process_queue = ParseGeoJson(json_data, style_data,
         m_style_point_map, m_style_line_map, m_style_poly_map, m_vbo_manager);
   m_process_json_queue.insert(std::end(m_process_json_queue),
         std::begin(process_queue), std::end(process_queue));

   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultRenderer::RegisterIconToCustomLayer(int icon_id, const char *icon_png,
      unsigned int size)
{
   ByteArray byte_array(icon_png, size);
   m_vbo_manager->RegisterIcon(icon_id, byte_array);
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultRenderer::AddIconToCustomLayer(int icon_id, const char *geo_json_data,
      int geo_size, const char *stylization_json_data, int stylization_size)
{
   auto json_data = ConvertCharArrayToJson(geo_json_data, geo_size);
   auto style_data = ConvertCharArrayToJson(stylization_json_data, stylization_size);

   if(json_data == nullptr)
   {
      printf("Unable to parse json response");
      return;
   }

   std::deque<nlohmann::json> process_queue = ParseGeoJson(json_data, style_data,
         icon_id, m_style_icon_map, m_vbo_manager);
   m_process_json_icon_queue.insert(std::end(m_process_json_icon_queue),
         std::begin(process_queue), std::end(process_queue));

   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultRenderer::RemoveIconFromCustomLayer(int item_id)
{
   m_vbo_manager->RemoveIconItem(item_id);
   Config::SetNeedToDraw(true);
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultRenderer::ClearCustomLayer()
{
   m_style_line_map.clear();
   m_style_point_map.clear();
   m_style_poly_map.clear();
   m_style_icon_map.clear();

   m_process_json_queue.clear();
   m_process_json_icon_queue.clear();
   m_vbo_manager->ClearVBOManager();
   Config::SetNeedToDraw(true);
}
