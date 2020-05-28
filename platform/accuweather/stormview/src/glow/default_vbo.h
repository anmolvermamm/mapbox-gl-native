#ifndef _DEFAULT_VBO_H_
#define _DEFAULT_VBO_H_

#include "glow.h"
#include "../geo_type.h"
#include "../color.h"
#include <vector>
#include <map>
//#include "../geo_json_parser.h"
#include "../rectf.h"
#include "../polygonf.h"
#include "../png_image.h"

//class PolygonF;
class DefaultVBO;

struct PrimitiveStyle {
   Color color;
   float weight;
   float radius;
   float opacity;
   bool border;
   float border_width;
   Color border_color;
   Color fill_color;
   float fill_opacity;
   int icon_id;
};

template <typename T>
inline void hash_combine(T& seed, T value)
{
   seed ^= value + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

namespace std {
   template<>
   struct hash<Color>
   {
         size_t operator()(const Color& color) const {
            std::size_t hash_key = 0;
            std::hash<float> color_hash;

            hash_combine(hash_key, color_hash(color.red()));
            hash_combine(hash_key, color_hash(color.green()));
            hash_combine(hash_key, color_hash(color.blue()));
            hash_combine(hash_key, color_hash(color.alpha()));
            return hash_key;
         }
   };
}

namespace std {
   template<>
   struct hash<PrimitiveStyle>
   {
         size_t operator()(const PrimitiveStyle& style) const {

            std::size_t hash_key = 0;
            std::hash<Color> color_hash;
            std::hash<bool> bool_hash;
            std::hash<float> float_hash;
            std::hash<int> int_hash;

            hash_combine(hash_key, color_hash(style.color));
            hash_combine(hash_key, float_hash(style.weight));
            hash_combine(hash_key, float_hash(style.radius));
            hash_combine(hash_key, float_hash(style.opacity));
            hash_combine(hash_key, bool_hash(style.border));
            hash_combine(hash_key, float_hash(style.border_width));
            hash_combine(hash_key, color_hash(style.border_color));
            hash_combine(hash_key, color_hash(style.fill_color));
            hash_combine(hash_key, float_hash(style.fill_opacity));
            hash_combine(hash_key, int_hash(style.icon_id));

            return hash_key;
         }
   };
}



class DefaultVBOGeometry
{
   public:
      GeoType type;
      int id = 0;
      float z_order = 0.2;
      PolygonF poly;
      size_t style_key = 0;

      ~DefaultVBOGeometry();
      DefaultVBOGeometry(GeoType type, int id, float z_order, const PolygonF &poly,
            size_t style);

      inline bool operator< (const DefaultVBOGeometry& rhs) const
      {
         return this->id < rhs.id;
      }
};

class DefaultVBOIcon
{
   public:
      int id = 0;
      float z_order = 0.2;
      PolygonF poly;
      size_t style_key = 0;
      int icon_id = 0;

      ~DefaultVBOIcon();
      DefaultVBOIcon(int id, int icon_id, float z_order, const PolygonF &poly,
            size_t style);

      inline bool operator< (const DefaultVBOIcon& rhs) const
      {
         return this->id < rhs.id;
      }
};

struct PointBufferDraw {
   unsigned int vertex_buffer_points = 0;
   unsigned int points_size = 0;
};

struct LineBufferDraw {
   unsigned int buffer = 0;
   unsigned int element_buffer = 0;
   unsigned int element_buffer_size = 0;
};

struct PolyBufferDraw {
   unsigned int buffer = 0;
   unsigned int element_buffer = 0;
   unsigned int element_buffer_size = 0;
};

struct IconDraw {
   unsigned int vertex_buffer_points = 0;
   unsigned int points_size = 0;
   unsigned int texture_ref = 0;
};

class DefaultVBO
{
   public:
      ~DefaultVBO();

      void PushPolygon(GeoType type, int id, float z_order, const PolygonF &poly,
            size_t style);
      void PushStyle(size_t style_key, PrimitiveStyle style);
      void RegisterIcon(int icon_id, const ByteArray& png_image);
      void AddIcon(int item_id, int icon_id, float z_order, const PolygonF &poly,
            size_t style);
      void RemovePolygon(int id);
      void RemoveIconItem(int item_id);
      void ClearVBOManager();

      void UpdateBufferData(GeoType type, size_t style);
      void UpdateBufferData(size_t style, int icon_id);

      std::vector<DefaultVBOGeometry> m_point_geometry;
      std::vector<DefaultVBOGeometry> m_line_geometry;
      std::vector<DefaultVBOGeometry> m_poly_geometry;

      std::vector<DefaultVBOIcon> m_icons_geometry;

      std::map<size_t, PrimitiveStyle> m_styles;

      std::map<size_t, IconDraw> m_icons_buffer;
      std::map<size_t, PointBufferDraw> m_point_buffer;
      std::map<size_t, LineBufferDraw> m_line_buffer;
      std::map<size_t, std::vector<PolyBufferDraw>> m_poly_buffer;

      std::map<int, unsigned int> m_icons;
};

#endif // end of include guard: _DEFAULT_VBO_H_
