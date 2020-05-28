#ifndef _DEFAULT_VBO_CXX_
#define _DEFAULT_VBO_CXX_

#include "default_vbo.h"
#include "../polygonf.h"
#include <iostream>
#include "../geo_type.h"
#include "../logging.h"

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DefaultVBO::~DefaultVBO()
{
   ClearVBOManager();
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DefaultVBOGeometry::DefaultVBOGeometry(GeoType type, int id, float z_order,
      const PolygonF &poly, size_t style) :
      type(type), id(id), z_order(z_order), poly(poly), style_key(style)
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DefaultVBOGeometry::~DefaultVBOGeometry()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DefaultVBOIcon::DefaultVBOIcon(int id, int icon_id, float z_order,
      const PolygonF &poly, size_t style) :
      id(id), icon_id(icon_id), z_order(z_order), poly(poly), style_key(style)
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DefaultVBOIcon::~DefaultVBOIcon()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultVBO::PushPolygon(GeoType type, int id, float z_order, const PolygonF &poly,
      size_t style)
{
   if(type == GeoType::POINT)
   {
      m_point_geometry.emplace_back(type, id, z_order, poly, style);
      std::sort(m_point_geometry.begin(), m_point_geometry.end());
      //UpdateBufferData(type, style);
   }
   else if(type == GeoType::LINE)
   {
      m_line_geometry.emplace_back(type, id, z_order, poly, style);
      std::sort(m_line_geometry.begin(), m_line_geometry.end());
      //UpdateBufferData(type, style);
   }
   else if(type == GeoType::POLYGON)
   {
      m_poly_geometry.emplace_back(type, id, z_order, poly, style);
      std::sort(m_poly_geometry.begin(), m_poly_geometry.end());
      //UpdateBufferData(type, style);
   }
   else
   {
      printf("Unknown Geometry type");
   }
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultVBO::RemovePolygon(int id)
{
   auto iter_point = std::find_if(m_point_geometry.begin(), m_point_geometry.end(), [id](
         auto& item) {return item.id == id; });

   if(iter_point != m_point_geometry.end())
   {
      m_point_geometry.erase(iter_point);
   }

   auto iter_line = std::find_if(m_line_geometry.begin(), m_line_geometry.end(), [id](
         auto& item) {return item.id == id; });

   if(iter_line != m_line_geometry.end())
   {
      m_line_geometry.erase(iter_line);
   }

   auto iter_poly = std::find_if(m_poly_geometry.begin(), m_poly_geometry.end(), [id](
         auto& item) {return item.id == id; });

   if(iter_poly != m_poly_geometry.end())
   {
      m_poly_geometry.erase(iter_poly);
   }

   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultVBO::PushStyle(size_t style_key, PrimitiveStyle style)
{
   if(m_styles.find(style_key) == m_styles.end())
   {
      m_styles.insert(std::pair<size_t, PrimitiveStyle>(style_key, style));
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultVBO::UpdateBufferData(GeoType type, size_t style)
{
   if(type == GeoType::POINT)
   {
      std::vector<float> buffer_data;
      PointBufferDraw point_draw;
      for(auto const& point : m_point_geometry)
      {
         if(point.style_key == style)
         {
            for(auto const& coordinates : point.poly)
            {
               buffer_data.push_back(coordinates.x());
               buffer_data.push_back(coordinates.y());
               buffer_data.push_back(point.z_order);
            }
         }
      }

      point_draw.vertex_buffer_points = Glow::CreateBuffer(buffer_data);
      point_draw.points_size = buffer_data.size() / 3;

      if(m_point_buffer.find(style) == m_point_buffer.end())
      {
         m_point_buffer.insert(std::pair<size_t, PointBufferDraw>(style, point_draw));
      }
      else
      {
         unsigned int buffer_destory = m_point_buffer[style].vertex_buffer_points;
         m_point_buffer[style] = point_draw;
         Glow::DeleteBuffer(buffer_destory);
      }
   }
   else if(type == GeoType::LINE)
   {
      std::vector<unsigned int> elements;
      std::vector<float> buffer_data;
      LineBufferDraw line_draw;

      int count = 0;
      for(auto const& line : m_line_geometry)
      {
         if(line.style_key == style)
         {
            for(auto const& coordinates : line.poly)
            {
               buffer_data.push_back(coordinates.x());
               buffer_data.push_back(coordinates.y());
               buffer_data.push_back(line.z_order);
               elements.push_back(count);
               count++;
            }

            elements.push_back(4294967295);
         }
      }

      line_draw.buffer = Glow::CreateBuffer(buffer_data);
      line_draw.element_buffer = Glow::CreateElemBuffer(elements);
      line_draw.element_buffer_size = elements.size();

      if(m_line_buffer.find(style) == m_line_buffer.end())
      {
         m_line_buffer.insert(std::pair<size_t, LineBufferDraw>(style, line_draw));
      }
      else
      {
         unsigned int buffer_destroy = m_line_buffer[style].buffer;
         unsigned int buffer_ele_destroy = m_line_buffer[style].element_buffer;
         m_line_buffer[style] = line_draw;
         Glow::DeleteBuffer(buffer_destroy);
         Glow::DeleteBuffer(buffer_ele_destroy);
      }
   }
   else if(type == GeoType::POLYGON)
   {
      std::vector<unsigned int> elements;
      std::vector<float> buffer_data;
      std::vector<PolyBufferDraw> poly_draw_list;

      int count = 0;
      for(auto const& poly : m_poly_geometry)
      {
         if(poly.style_key == style)
         {
            PolyBufferDraw poly_draw;
            if(poly.poly.right_handed)
            {
               for(auto const& coordinates : poly.poly)
               {
                  buffer_data.push_back(coordinates.x());
                  buffer_data.push_back(coordinates.y());
                  buffer_data.push_back(poly.z_order);
                  elements.push_back(count);
                  count++;
               }
            }
            else
            {
               for(int idx = poly.poly.size() - 1; idx > 0; idx--)
               {
                  buffer_data.push_back(poly.poly.at(idx).x());
                  buffer_data.push_back(poly.poly.at(idx).y());
                  buffer_data.push_back(poly.z_order);
                  elements.push_back(count);
                  count++;
               }
            }

            elements.push_back(4294967295);

            for(auto const& hole : poly.poly.holes)
            {
               if(hole.right_handed)
               {
                  for(int idx = hole.size() - 1; idx > 0; idx--)
                  {
                     buffer_data.push_back(hole.at(idx).x());
                     buffer_data.push_back(hole.at(idx).y());
                     buffer_data.push_back(poly.z_order);
                     elements.push_back(count);
                     count++;
                  }
               }
               else
               {
                  for(auto const& coordinates : hole)
                  {
                     buffer_data.push_back(coordinates.x());
                     buffer_data.push_back(coordinates.y());
                     buffer_data.push_back(poly.z_order);
                     elements.push_back(count);
                     count++;
                  }
               }

               elements.push_back(4294967295);
            }

            poly_draw.buffer = Glow::CreateBuffer(buffer_data);
            poly_draw.element_buffer = Glow::CreateElemBuffer(elements);
            poly_draw.element_buffer_size = elements.size();
            poly_draw_list.push_back(poly_draw);
         }
      }

      if(m_poly_buffer.find(style) == m_poly_buffer.end())
      {
         m_poly_buffer.insert(std::pair<size_t,
               std::vector<PolyBufferDraw>>(style, poly_draw_list));
      }
      else
      {
         std::vector<PolyBufferDraw> destroy_list = m_poly_buffer[style];
         m_poly_buffer[style] = poly_draw_list;
         for(auto buffer_item : destroy_list)
         {
            Glow::DeleteBuffer(buffer_item.buffer);
            Glow::DeleteBuffer(buffer_item.element_buffer);
         }
         destroy_list.clear();
      }
   }
   else
   {
      printf("Unknown Geometry type");
   }
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultVBO::RegisterIcon(int icon_id, const ByteArray& png_image)
{
   if(m_icons.find(icon_id) == m_icons.end())
   {
      bool is_8_bit;
      unsigned int texture_ref = Glow::Load2DImageToTextureFromMem(png_image, is_8_bit);
      m_icons.insert(std::pair<int, unsigned int>(icon_id, texture_ref));
   }

   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultVBO::AddIcon(int item_id, int icon_id, float z_order,
      const PolygonF &poly, size_t style)
{
   m_icons_geometry.emplace_back(item_id, icon_id, z_order, poly, style);
   std::sort(m_icons_geometry.begin(), m_icons_geometry.end());
   UpdateBufferData(style, icon_id);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultVBO::RemoveIconItem(int item_id)
{
   auto iter_point = std::find_if(m_icons_geometry.begin(), m_icons_geometry.end(), [item_id](
         auto& item) {return item.id == item_id; });

   if(iter_point != m_icons_geometry.end())
   {
      m_icons_geometry.erase(iter_point);
      UpdateBufferData((*iter_point).style_key, (*iter_point).icon_id);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultVBO::UpdateBufferData(size_t style, int icon_id)
{
   IconDraw icon_draw;
   std::vector<float> buffer_data;
   for(auto const& icon : m_icons_geometry)
   {
      if(icon.style_key == style && icon.icon_id == icon_id)
      {
         for(auto const& coordinates : icon.poly)
         {
            buffer_data.push_back(coordinates.x());
            buffer_data.push_back(coordinates.y());
            buffer_data.push_back(icon.z_order);
         }
      }
   }

   icon_draw.vertex_buffer_points = Glow::CreateBuffer(buffer_data);
   icon_draw.points_size = buffer_data.size() / 3;
   icon_draw.texture_ref = m_icons.at(icon_id);

   if(m_icons_buffer.find(style) == m_icons_buffer.end())
   {
      m_icons_buffer.insert(std::pair<size_t, IconDraw>(style, icon_draw));
   }
   else
   {
      unsigned int buffer_destory = m_icons_buffer[style].vertex_buffer_points;
      m_icons_buffer[style] = icon_draw;
      Glow::DeleteBuffer(buffer_destory);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void DefaultVBO::ClearVBOManager()
{
   m_point_geometry.clear();
   m_line_geometry.clear();
   m_poly_geometry.clear();

   m_icons_geometry.clear();
   //m_styles.clear();

   for(auto buffer_item : m_icons_buffer)
   {
      Glow::DeleteBuffer(buffer_item.second.vertex_buffer_points);
   }
   m_icons_buffer.clear();

   for(auto buffer_item : m_point_buffer)
   {
      Glow::DeleteBuffer(buffer_item.second.vertex_buffer_points);
   }
   m_point_buffer.clear();

   for(auto buffer_item : m_line_buffer)
   {
      Glow::DeleteBuffer(buffer_item.second.buffer);
      Glow::DeleteBuffer(buffer_item.second.element_buffer);
   }
   m_line_buffer.clear();

   for(auto poly_item : m_poly_buffer)
   {
      for(auto buffer_item : poly_item.second)
      {
         Glow::DeleteBuffer(buffer_item.buffer);
         Glow::DeleteBuffer(buffer_item.element_buffer);
      }
   }
   m_poly_buffer.clear();

   //m_icons.clear();
}

#endif // end of include guard _DEFAULT_VBO_CXX_
