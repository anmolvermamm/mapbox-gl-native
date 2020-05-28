#ifndef _GEO_JSON_PARSER_H_
#define _GEO_JSON_PARSER_H_

#include "pointf.h"
#include "polygonf.h"
#include "ext/json.hpp"
#include "string"
#include "projections.h"
#include <algorithm>
#include <set>
#include <map>
#include "glow/default_vbo.h"
#include "geo_type.h"
#include "logging.h"
#include <deque>

class DefaultVBO;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline nlohmann::json ConvertCharArrayToJson(const char* json_char, int size)
{
   std::vector<char> json_vec;
   json_vec.insert(json_vec.end(), json_char, json_char + size);

   // need to add null termination if not already at end of char array
   if(json_vec.at(json_vec.size() - 1) != '\0')
   {
      json_vec.push_back('\0');
   }

   nlohmann::json response;
   try {
      response = nlohmann::json::parse(json_vec.begin(), json_vec.end());
   }
   catch(std::exception& e)
   {
      printf("\nTrouble parsing char array to json object: %s\n", e.what());
   }

   return response;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline PointF ParseGeoJsonPoint(nlohmann::json point_data, bool &ok)
{
   PointF point;
   if(point_data.size() == 2)
   {
      ok = true;
      point = PointF(point_data[0].get<float>(),
            point_data[1].get<float>());
      point = GeoPointToMercatorPoint(point);
   }
   else
   {
      ok = false;
      point = PointF(0.0f, 0.0f);
   }

   return point;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline PolygonF ParseGeoJsonLine(nlohmann::json line_data, bool &ok)
{
   PolygonF poly_line;
   if(!line_data.empty() && line_data.size() > 0)
   {
      for(const auto& poly_line_data : line_data)
      {
         bool ok_point;
         auto point = ParseGeoJsonPoint(poly_line_data, ok_point);
         if(ok_point)
         {
            poly_line.emplace_back(point);
         }
      }
   }

   if(poly_line.empty() || poly_line.size() <= 1)
   {
      ok = false;
   }
   else
   {
      ok = true;
   }

   return poly_line;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline PolygonF ParseGeoPolygon(nlohmann::json poly_data, bool &ok)
{
   PolygonF polygon;

   if(!poly_data.empty() && poly_data.size() > 0)
   {
      unsigned int count = 0;
      for(const auto& polygon_data : poly_data)
      {
         PolygonF hole;
         float poly_area = 0;
         for(const auto& poly_points : polygon_data)
         {
            bool ok_point;
            auto point = ParseGeoJsonPoint(poly_points, ok_point);
            if(ok_point)
            {
               if(count == 0)
               {
                  polygon.emplace_back(point);
                  if(polygon.size() > 1)
                  {
                     poly_area += polygon.at(polygon.size() - 2).x() * polygon.at(polygon.size() - 1).y()
                           - polygon.at(polygon.size() - 1).x() * polygon.at(polygon.size() - 2).y();
                  }
               }
               else
               {
                  hole.emplace_back(point);
                  if(hole.size() > 1)
                  {
                     poly_area += hole.at(hole.size() - 2).x() * hole.at(hole.size() - 1).y()
                           - hole.at(hole.size() - 1).x() * hole.at(hole.size() - 2).y();
                  }
               }
            }
         }

         if(count == 0)
         {
            poly_area = poly_area / 2.0;
            if(poly_area > 0.0)
            {
               polygon.right_handed = true;
            }
            else
            {
               polygon.right_handed = false;
            }
         }
         else
         {
            poly_area = poly_area / 2.0;
            if(poly_area > 0.0)
            {
               hole.right_handed = true;
            }
            else
            {
               hole.right_handed = false;
            }
         }

         if(!hole.empty() && hole.size() > 2)
         {
            polygon.AddHoleToPolygon(hole);
         }

         count++;
      }
   }

   if(polygon.empty() || polygon.size() <= 0)
   {
      ok = false;
   }
   else
   {
      ok = true;
   }

   return polygon;
}


/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline std::vector<std::string> SplitWithCharacters(const std::string& str, int split_length)
{
   int num_substrings = str.length() / split_length;
   std::vector<std::string> ret;

   for(int idx = 0; idx < num_substrings; idx++)
   {
      ret.push_back(str.substr(idx * split_length, split_length));
   }

   if(str.length() % split_length != 0)
   {
      ret.push_back(str.substr(split_length * num_substrings));
   }

   return ret;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline Color HexToRGB(std::string hex)
{
   if(hex.at(0) == '#')
   {
      hex.erase(0,1);
   }

   while(hex.length() != 6)
   {
      hex += "0";
   }

   std::vector<std::string> color_int = SplitWithCharacters(hex, 2);
   short red = std::stoi(color_int[0], nullptr, 16);
   short green = std::stoi(color_int[1], nullptr, 16);
   short blue = std::stoi(color_int[2], nullptr, 16);

   Color color(red, green, blue);

   return color;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline Color ParseColor(nlohmann::json color_json, std::string key,
      Color default_color)
{
   bool ok;
   Color color = default_color;
   std::string color_str;

   (color_json[key] != nullptr) ?
         ok = true :
         ok = false;

   if(ok)
   {
      color_str = color_json[key];
      color = HexToRGB(color_str);
   }

   return color;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline PrimitiveStyle ParsePrimitiveStyle(nlohmann::json style, int icon_id)
{
   PrimitiveStyle primitive_style;

   primitive_style.color = ParseColor(style, "color", Color(1.0f, 0.0f,0.0f));

   (style["weight"] != nullptr) ?
         primitive_style.weight = style["weight"].get<float>() :
         primitive_style.weight = 1.0;

   (style["opacity"] != nullptr) ?
         primitive_style.opacity = style["opacity"].get<float>() :
         primitive_style.opacity = 1.0;

   (style["border"] != nullptr) ?
         primitive_style.border = style["border"].get<bool>() :
         primitive_style.border = true;

   (style["borderWidth"] != nullptr) ?
         primitive_style.border_width = style["borderWidth"].get<float>() :
         primitive_style.border_width = 1.0;

   primitive_style.border_color = ParseColor(style, "borderColor", Color(128, 128, 128));

   (style["radius"]  != nullptr) ?
         primitive_style.radius = style["radius"].get<float>() :
         primitive_style.radius = 7.0;

   primitive_style.fill_color = ParseColor(style, "fillColor", Color(1.0f, 0.0f, 0.0f));

   (style["fillOpacity"] != nullptr) ?
         primitive_style.fill_opacity = style["fillOpacity"].get<float>() :
         primitive_style.fill_opacity = 1.0;

   primitive_style.icon_id = icon_id;

   return primitive_style;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline void ParseFeatures(nlohmann::json features, std::map<int, size_t> &style_points,
      std::map<int, size_t> &style_lines, std::map<int, size_t> &style_polys,
      std::unique_ptr<DefaultVBO> &vbo_manager)
{
   try
   {
      std::string type_name = "";
      if(features["type"] != nullptr)
         type_name = features["type"].get<std::string>();

      auto geometry = features["geometry"];
      auto geometry_type = geometry["type"].get<std::string>();
      std::transform(geometry_type.begin(), geometry_type.end(),
            geometry_type.begin(), ::tolower);
      auto coords = geometry["coordinates"];

      unsigned int id = 0;
      if(features["id"] != nullptr)
      {
         id = features["id"].get<int>();
      }

      int style_id = -1;
      if(features["properties"] != nullptr)
      {
         auto properties = features["properties"];

         if(properties["id"] != nullptr)
         {
            id = properties["id"].get<int>();
         }

         (properties["styleId"] != nullptr) ?
               style_id = properties["styleId"].get<int>() :
               style_id = -1;
      }

      size_t point_hash = style_points.at(style_id);
      size_t line_hash = style_lines.at(style_id);
      size_t poly_hash = style_polys.at(style_id);

      bool ok;
      if(geometry_type.compare("point") == 0)
      {
         auto point = ParseGeoJsonPoint(coords, ok);
         if(ok)
         {
            PolygonF poly_point;
            poly_point.emplace_back(point);
            vbo_manager->PushPolygon(GeoType::POINT, id, 0.2, poly_point, point_hash);
         }
      }
      else if(geometry_type.compare("linestring") == 0)
      {
         auto line = ParseGeoJsonLine(coords, ok);
         if(ok)
         {
            vbo_manager->PushPolygon(GeoType::LINE, id, 0.2, line, line_hash);
         }
      }
      else if(geometry_type.compare("line") == 0)
      {
         auto line = ParseGeoJsonLine(coords, ok);
         if(ok)
         {
            vbo_manager->PushPolygon(GeoType::LINE, id, 0.2, line, line_hash);
         }
      }
      else if(geometry_type.compare("polygon") == 0)
      {
         auto poly = ParseGeoPolygon(coords, ok);
         if(ok)
         {
            vbo_manager->PushPolygon(GeoType::POLYGON, id, 0.2, poly, poly_hash);
         }
      }
      else if(geometry_type.compare("multipoint") == 0)
      {
         for(const auto& multi_points : coords)
         {
            auto point = ParseGeoJsonPoint(multi_points, ok);
            if(ok)
            {
               PolygonF poly_point;
               poly_point.emplace_back(point);
               vbo_manager->PushPolygon(GeoType::POINT, id, 0.2, poly_point, point_hash);
            }
         }
      }
      else if(geometry_type.compare("multilinestring") == 0)
      {
         for(const auto& multi_string : coords)
         {
            auto line = ParseGeoJsonLine(multi_string, ok);
            if(ok)
            {
               vbo_manager->PushPolygon(GeoType::LINE, id, 0.2, line, line_hash);
            }
         }
      }
      else if(geometry_type.compare("multipolygon") == 0)
      {
         for(const auto& multi_poly : coords)
         {
            auto poly = ParseGeoPolygon(multi_poly, ok);
            if(ok)
            {
               vbo_manager->PushPolygon(GeoType::POLYGON, id, 0.2, poly, poly_hash);
            }
         }
      }
      else
      {
         printf("\nUnsupported geometry type\n");
      }
   }
   catch(std::exception& e)
   {
      printf("\nGeoJSON Feature syntax incorrect: %s\n", e.what());
   }

   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline void StoreStylizationFromJson(int style_id, nlohmann::json style_type,
      std::map<int, size_t> &style_point_map, std::map<int, size_t> &style_line_map,
      std::map<int, size_t> &style_poly_map, std::unique_ptr<DefaultVBO> &vbo_manager)
{
   PrimitiveStyle default_point;
   PrimitiveStyle default_line;
   PrimitiveStyle default_poly;

   default_point.color = default_line.color = default_poly.color = Color(1.0f, 0.0f, 0.0f);
   default_point.weight = default_line.weight = default_poly.weight = 1.0;
   default_point.radius = default_line.radius = default_poly.radius = 7.0;
   default_point.opacity = default_line.opacity = default_poly.opacity = 1.0;
   default_point.border = true;
   default_line.border = default_poly.border = false;
   default_point.border_width = default_line.border_width = default_poly.border_width = 1.0;
   default_point.border_color = Color(128, 128, 128);
   default_line.border_color = default_poly.border_color = Color(0, 0, 0);
   default_poly.fill_color = Color(1.0f, 0.0f, 0.0f);
   default_line.fill_color = default_point.fill_color = Color(0.0f, 0.0f, 0.0f);
   default_poly.fill_opacity = 1.0;
   default_line.fill_opacity = default_point.fill_opacity = 0.0;
   default_point.icon_id = default_line.icon_id = default_poly.icon_id = 0;

   std::size_t style_hash = 0;
   if(style_point_map.find(style_id) == style_point_map.end())
   {
      PrimitiveStyle point_style = default_point;
      if(style_type["pointStyle"] != nullptr)
      {
         point_style = ParsePrimitiveStyle(style_type["pointStyle"], 0);
      }
      style_hash = std::hash<PrimitiveStyle>{}(point_style);
      vbo_manager->PushStyle(style_hash, point_style);
      style_point_map.insert(std::pair<int, size_t>(style_id, style_hash));
   }

   if(style_line_map.find(style_id) == style_line_map.end())
   {
      PrimitiveStyle line_style = default_line;
      if(style_type["lineStyle"] != nullptr)
      {
         line_style = ParsePrimitiveStyle(style_type["lineStyle"], 0);
      }
      style_hash = std::hash<PrimitiveStyle>{}(line_style);
      vbo_manager->PushStyle(style_hash, line_style);
      style_line_map.insert(std::pair<int, size_t>(style_id, style_hash));
   }

   if(style_poly_map.find(style_id) == style_poly_map.end())
   {
      PrimitiveStyle poly_style = default_poly;
      if(style_type["polyStyle"] != nullptr)
      {
         poly_style = ParsePrimitiveStyle(style_type["polyStyle"], 0);
      }
      style_hash = std::hash<PrimitiveStyle>{}(poly_style);
      vbo_manager->PushStyle(style_hash, poly_style);
      style_poly_map.insert(std::pair<int, size_t>(style_id, style_hash));
   }

   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline std::deque<nlohmann::json> ParseGeoJson(nlohmann::json json_data, nlohmann::json style_data,
      std::map<int, size_t> &style_point_map, std::map<int, size_t> &style_line_map,
      std::map<int, size_t> &style_poly_map, std::unique_ptr<DefaultVBO> &vbo_manager)
{
   int style_id = -1;

   try
   {
      if(style_data["styles"] != nullptr)
      {
         // list of styles to use using a style id
         for(const auto& style : style_data["styles"])
         {
            style_id = style["id"].get<int>();
            auto style_type = style["style"];

            StoreStylizationFromJson(style_id, style_type, style_point_map,
                  style_line_map, style_poly_map, vbo_manager);
         }
      }
      else if(style_data["style"] != nullptr)
      {
         // only using single style for all primitives
         auto style_type = style_data["style"];
         style_id = style_type["id"].get<int>();

         StoreStylizationFromJson(style_id, style_type, style_point_map,
               style_line_map, style_poly_map, vbo_manager);
      }
      else
      {
         printf("\nEmpty json style data");
      }
   }
   catch(std::exception& e)
   {
      printf("\nNot Supported stylization json data: %s", e.what());
   }

   std::deque<nlohmann::json> process_json_queue;
   try
   {
      std::string feature_collection_str = "";

      if(json_data["type"] != nullptr)
      {
         feature_collection_str = json_data["type"].get<std::string>();
         std::transform(feature_collection_str.begin(), feature_collection_str.end(),
               feature_collection_str.begin(), ::tolower);
      }

      if(feature_collection_str.compare("featurecollection") == 0)
      {
         // It is a feature collection
         auto features = json_data["features"];
         for(auto& feature : features)
         {
            //if(feature["type"] != nullptr)
            if(feature["properties"]["styleId"] == nullptr)
            {
               feature["properties"]["styleId"] = style_id;
            }

               process_json_queue.push_back(feature);
         }
      }
      else
      {
         if(json_data["properties"]["styleId"] == nullptr)
         {
            json_data["properties"]["styleId"] = style_id;
         }

         process_json_queue.push_back(json_data);
      }
   }
   catch(std::exception& e)
   {
      printf("\nNot Supported GeoJson (FeatureCollection): %s\n", e.what());
      return process_json_queue;
   }

   return process_json_queue;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline std::deque<nlohmann::json> ParseGeoJson(nlohmann::json json_data,
      nlohmann::json style_data, int icon_id, std::map<int, size_t> &style_point_map,
      std::unique_ptr<DefaultVBO> &vbo_manager)
{
   PrimitiveStyle default_point;

   default_point.color = Color(0.0f, 0.0f, 0.0f);
   default_point.weight = 1.0;
   default_point.radius = 8.0;
   default_point.opacity = 1.0;
   default_point.border = false;
   default_point.border_width = 1.0;
   default_point.border_color = Color(0.0f, 0.0f, 0.0f);
   default_point.fill_color = Color(0.0f, 0.0f, 0.0f);
   default_point.fill_opacity = 0.0;
   default_point.icon_id = icon_id;

   int style_id = -1;
   try
   {
      std::size_t style_hash = 0;
      if(style_data["styles"] != nullptr)
      {
         // list of styles to use using a style id within GeoJson Properties
         for(const auto& style : style_data["styles"])
         {
            style_id = style["id"].get<int>() * 1000 + icon_id;
            auto style_type = style["style"];

            if(style_point_map.find(style_id) == style_point_map.end())
            {
               PrimitiveStyle point_style = default_point;
               if(style_type["pointStyle"] != nullptr)
               {
                  point_style = ParsePrimitiveStyle(style_type["pointStyle"], icon_id);
               }
               style_hash = std::hash<PrimitiveStyle>{}(point_style);
               vbo_manager->PushStyle(style_hash, point_style);
               style_point_map.insert(std::pair<int, size_t>(style_id, style_hash));
            }
         }
      }
      else if(style_data["style"] != nullptr)
      {
         // only using single style for all primitives
         auto style_type = style_data["style"];
         style_id = style_type["id"].get<int>() * 1000 + icon_id;

         if(style_point_map.find(style_id) == style_point_map.end())
         {
            PrimitiveStyle point_style = default_point;
            if(style_type["pointStyle"] != nullptr)
            {
               point_style = ParsePrimitiveStyle(style_type["pointStyle"], icon_id);
            }
            style_hash = std::hash<PrimitiveStyle>{}(point_style);
            vbo_manager->PushStyle(style_hash, point_style);
            style_point_map.insert(std::pair<int, size_t>(style_id, style_hash));
         }
      }
      else
      {
         printf("\nEmpty json style data");
      }
   }
   catch(std::exception& e)
   {
      printf("\nNot Supported stylization json data: %s", e.what());
   }

   std::deque<nlohmann::json> process_json_queue;
   try
   {
      std::string feature_collection_str = "";

      if(json_data["type"] != nullptr)
      {
         feature_collection_str = json_data["type"].get<std::string>();
         std::transform(feature_collection_str.begin(), feature_collection_str.end(),
               feature_collection_str.begin(), ::tolower);
      }

      if(feature_collection_str.compare("featurecollection") == 0)
      {
         auto features = json_data["features"];
         for(auto& feature : features)
         {
            if(feature["properties"]["styleId"] == nullptr)
            {
               feature["properties"]["styleId"] = style_id;
            }

            process_json_queue.push_back(feature);
         }
      }
      else
      {
         if(json_data["properties"]["styleId"] == nullptr)
         {
            json_data["properties"]["styleId"] = style_id;
         }

         process_json_queue.push_back(json_data);
      }
   }
   catch(std::exception& e)
   {
      printf("\nNot Supported GeoJson (FeatureCollection): %s\n", e.what());
      return process_json_queue;
   }

   return process_json_queue;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
inline void ParseIconFeatures(nlohmann::json features,
      std::map<int, size_t> &style_points, std::unique_ptr<DefaultVBO> &vbo_manager)
{
   try
   {
      std::string type_name = "";
      if(features["type"] != nullptr)
         type_name = features["type"].get<std::string>();

      auto geometry = features["geometry"];
      auto geometry_type = geometry["type"].get<std::string>();
      std::transform(geometry_type.begin(), geometry_type.end(),
            geometry_type.begin(), ::tolower);
      auto coords = geometry["coordinates"];

      unsigned int id = 0;
      if(features["id"] != nullptr)
      {
         id = features["id"].get<int>();
      }

      int style_id = -1;
      if(features["properties"] != nullptr)
      {
         auto properties = features["properties"];

         if(properties["id"] != nullptr)
         {
            id = properties["id"].get<int>();
         }

         (properties["styleId"] != nullptr) ?
               style_id = properties["styleId"].get<int>() :
               style_id = -1;
      }

      size_t point_hash = style_points.at(style_id);
      int icon_id = vbo_manager->m_styles.at(point_hash).icon_id;

      bool ok;
      if(geometry_type.compare("point") == 0)
      {
         auto point = ParseGeoJsonPoint(coords, ok);
         if(ok)
         {
            PolygonF poly_point;
            poly_point.emplace_back(point);
            vbo_manager->AddIcon(id, icon_id, 0.2, poly_point,
                  point_hash);
         }
      }
      else if(geometry_type.compare("multipoint") == 0)
      {
         for(const auto& multi_points : coords)
         {
            auto point = ParseGeoJsonPoint(multi_points, ok);
            if(ok)
            {
               PolygonF poly_point;
               poly_point.emplace_back(point);
               vbo_manager->AddIcon(id, icon_id, 0.2, poly_point,
                     point_hash);
            }
         }
      }
      else
      {
         printf("\nUnsupported geometry type\n");
      }
   }
   catch(std::exception& e)
   {
      printf("\nGeoJSON Feature syntax incorrect: %s\n", e.what());
   }

   return;
}

#endif // end of include guard: _GEO_JSON_PARSER_H_
