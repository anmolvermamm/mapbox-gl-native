#include "sky_guard_warning.h"

#include <iostream>
#include "byte_array.h"
#include "json.hpp"
#include "polygonf.h"
#include <algorithm>
#include "string_helper.h"
#include "sky_guard_vbo.h"
#include "projections.h"
#include "config.h"
#include "date_time.h"
#include "timer.h"


/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Criteria::FromJson( nlohmann::json& j )
{
   try{
      id = j["id"].get<int>();
      severity_id = j["severityId"].get<int>();
      type_id = j["eventTypeId"].get<int>();
      severity = j["severity"].get<std::string>();
      event_type = j["eventType"].get<std::string>();
   }catch(std::exception& e )
   {
      std::cerr << "\n!! Criteria What: " << e.what() << "\n"; 
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::ostream& operator<<( std::ostream& os , const Criteria& warning )
{
   os 
      << "id " << warning.id
      << " type_id " << warning.type_id
      << " severity_id " << warning.severity_id
      << " severity " << warning.severity
      << " event_type " << warning.event_type
      ;

   return os;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GeoType Geometry::GetType( const std::string& type_str )
{
   return type_str == "polygon" ? GeoType::POLYGON :
      type_str == "line" ? GeoType::LINE : GeoType::POINT;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Geometry::GetPolygon( GeoType type , nlohmann::json& coords )
{
   switch( type )
   {
      case GeoType::POINT:
      {
         if( polygon_list.empty() )
         {
            polygon_list.emplace_back();
         }
         PolygonF polygon;
         if( coords.size() == 2 )
         {
            auto point = PointF( 
                  coords[0].get<float>(),
                  coords[1].get<float>() );
            //Reproject
            point = GeoPointToMercatorPoint( point );
            polygon.emplace_back( point );
         }
         //polygon_list.push_back( std::move( polygon ) );
         polygon_list.front().Append( polygon );
      }
      break;
      case GeoType::LINE:
      {
         if( !coords.empty() )
         {
            for( const auto& polyline : coords )
            {
               PolygonF new_polygon;
               for( const auto& polypoint : polyline )
               {
                  if( polypoint.size() == 2 )
                  {
                     auto point = PointF( 
                           polypoint[0].get<float>(),
                           polypoint[1].get<float>() );
                     //Reproject
                     point = GeoPointToMercatorPoint( point );
                     new_polygon.emplace_back( point );
                  }
               }
               polygon_list.push_back( std::move( new_polygon ) );
            }
         }
      }
      break;
      case GeoType::POLYGON:
      {
         if( !coords.empty() )
         {
            bool first = true;
            for( const auto& polygon : coords )
            {
               PolygonF new_polygon;
               for( const auto& polypoint : polygon )
               {
                  if( polypoint.size() == 2 )
                  {
                     auto point = PointF( 
                           polypoint[0].get<float>(),
                           polypoint[1].get<float>() );
                     //Reproject
                     point = GeoPointToMercatorPoint( point );
                     new_polygon.emplace_back( point );
                  }
               }
               if( first ) 
               {
                  polygon_list.push_back( std::move( new_polygon ) );
               } else
               {
                  //Polygon is a hole probably
               }
               first = false;
            }
         }
      }
      break;
      case GeoType::UNKNOWN:
      {
         printf("Unsupported geo type");
      }
      break;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Geometry::FromJson( GeoType type,  nlohmann::json& json )
{
   try{
      type_str = str_to_lower( json["type"].get<std::string>() );

      //type = GetType( type_str );
      this->type = type;
      auto coords = json["coordinates"];

      GetPolygon( type , coords );


   }catch(std::exception& e )
   {
      std::cerr << "\n!! Geometry What: " << e.what() << "\n"; 
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::ostream& operator<<( std::ostream& os , const Geometry& geometry )
{
   os 
      << "typestr " << geometry.type_str
      //<< " geometry [ " 
      << " " 
      ;

   os << ( geometry.type == GeoType::POINT  ? "Point" :
           geometry.type == GeoType::LINE  ? "Line" : 
           geometry.type == GeoType::POLYGON  ? "Polygon" : 
           "Unknown? ");

   os << " Geometry [";

   switch( geometry.type )
   {
      case GeoType::POINT:
         if( !geometry.polygon_list.empty() )
         {
            auto point_list = geometry.polygon_list.front();
            for( const auto& point : point_list )
            {
               os << point;
               os << ",";
            }
         }
         break;
      case GeoType::LINE:
      case GeoType::POLYGON:

         for( const auto& poly : geometry.polygon_list )
         { 
            os << "[ ";
            for( const auto& point : poly )
            {
               os << point;
               os << ",";
            }
            os << "] ,";
         }

         break;

      case GeoType::UNKNOWN:
      {
         printf("Geo type unknown");
      }
   }
   os << " ]";

   return os;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Properties::FromJson(GeoType type,  nlohmann::json& json )
{

   try
   {
      id = json["id"].get<unsigned>();
   }catch(std::exception& e )
   {
      std::cerr << "\n!! Properties What: " << e.what() << "\n"; 
   }


   this->type = type;
   switch( type )
   {
      case GeoType::POINT:
         FromJsonPoint( json );
         break;
      case GeoType::LINE:
         FromJsonLine( json );
         break;
      case GeoType::POLYGON:
         FromJsonPolygon( json );
         break;
      case GeoType::UNKNOWN:
         printf("Geo type unknown");
         break;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Properties::FromJsonPoint( nlohmann::json& json )
{
   try
   {
      name = json["Name"].get<std::string>();
      state = json["Locale"].get<std::string>();
      //state = json["State"].get<std::string>();
   }catch(std::exception& e )
   {
      //TODO fix this
      //std::cerr << "\n!! Properties Point What: " << e.what() << "\n"; 
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Properties::FromJsonLine(  nlohmann::json& json )
{
   try
   {
      name = json["name"].get<std::string>();
      state = json["Locale"].get<std::string>();
   }catch(std::exception& e )
   {
      //TODO fix this
      //std::cerr << "\n!! Properties Line What: " << e.what() << "\n"; 
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Properties::FromJsonPolygon(  nlohmann::json& json )
{
   try
   {
      name = json["name"].get<std::string>();
      state = json["Locale"].get<std::string>();
   }catch(std::exception& e )
   {
      //TODO fix this
      //std::cerr << "\n!! Properties Polygon What: " << e.what() << "\n"; 
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::ostream& operator<<( std::ostream& os , const Properties& property )
{
   os 
      << "propery_id " << property.id
      << " state " << property.state
      << " name " << property.name
   ;
   return os;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Feature::FromJson(  GeoType type , nlohmann::json& json )
{
   auto jprop = json["properties"];
   this->properties.FromJson( type , jprop );

   auto jgeo = json["geometry"];
   this->geometry.FromJson( type , jgeo );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned Feature::GetId() const
{
   return properties.id;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::ostream& operator<<( std::ostream& os , const Feature& feature )
{
   os << " [[[Geometry]]] "   << feature.geometry;
   os << " [[[Properties]]] " << feature.properties;
   return os;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Warning::FromJson(  nlohmann::json& json )
{
   try
   {
      id = json["id"].get<int>();
      version = json["warningVersion"].get<int>();
      warning_type = json["warningType"].get<std::string>();
      auto rect_string = json["bbox"].get<std::string>();
      message = json["comments"].get<std::string>();

      bbox = RectF::FromString( rect_string );

      //Reproject
      bbox = GeoRectToMercatorRect( bbox );

      AddGeoPoints( json );
      AddGeoLines( json );
      AddGeoPolygons( json );

   }catch(std::exception& e )
   {
      std::cerr << "\n!! Warning What: " << e.what() << "\n"; 
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Warning::AddGeoPoints( nlohmann::json& json )
{
   auto i = json.find( "geoPoints" );
   if( i == json.end() || i->empty()  )
     return; 

   auto& list = *i;

   for( auto& feature_item : list )
   {
      Feature feature;
      feature.FromJson( GeoType::POINT , feature_item );
      feature_list.push_back( std::move( feature ) );
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Warning::AddGeoLines(   nlohmann::json& json )
{
   auto i = json.find( "geoLines" );
   if( i == json.end() || i->empty() )
     return; 

   auto& list = *i;

   for( auto& feature_item : list )
   {
      Feature feature;
      feature.FromJson( GeoType::LINE, feature_item );
      feature_list.push_back( std::move( feature ) );
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Warning::AddGeoPolygons( nlohmann::json& json )
{
   auto i = json.find( "geoPolygons" );
   if( i == json.end() || i->empty() )
   {
     return; 
   }

   auto& list = *i;

   for( auto& feature_item : list )
   {
      Feature feature;
      feature.FromJson( GeoType::POLYGON, feature_item );
      feature_list.push_back( std::move( feature ) );
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::ostream& operator<<( std::ostream& os , const Warning& warning )
{
   os 
      << "id " << warning.id
      << " warning_type " << warning.warning_type
      << " BBoxRect: " << warning.bbox
      ;

   os << "Feature size : " << warning.feature_list.size();
   os << " Features { "  ;
   for( const auto& feature : warning.feature_list )
   {
      os << feature; 
      os << ",";
   }
   os << " } ";
   return os;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuardWarning::BuildSkyGuardVBO()
{
   //for( auto& warn_pair : m_warnings )
   //{
   auto i = m_warnings.find( highest_version_id );

   if( i != m_warnings.end() )
   {
      auto& warning = i->second;

      for( auto& feature : warning.feature_list )
      {
         int id = feature.GetId();
         auto type = feature.geometry.type;
         auto& poly_list = feature.geometry.polygon_list;
         for( auto& poly : poly_list )
         {
            m_skyguard_vbo->PushPolygon( type , id , poly ); 
         }
      }
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuardWarning::CalculateBBox()
{
   for( auto& wpair : m_warnings )
   {
      total_bbox = wpair.second.bbox;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuardWarning::CalculateHighestPriority()
{
   for( auto& cpair : m_criteria )
   {
      highest_priority = cpair.second.severity_id < highest_priority ?
         cpair.second.severity_id : highest_priority;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
SkyGuardWarning::SkyGuardWarning()
{
   m_skyguard_vbo = std::make_unique<SkyGuardVBO>();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
SkyGuardWarning::~SkyGuardWarning()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuardWarning::FromJson( const ByteArray& data )
{
   auto str = data.to_string();
   using nlohmann::json;

   auto j =  json::parse( str );
   
   try
   {
      id =  j["id"].get<int>();
      client_name =  j["clientName"].get<std::string>();
      expire_time =  j["expireTime"].get<std::string>();
   }
   catch(std::exception& e )
   {
      std::cerr << "\n!! Skyguard What: " << e.what() << "\n"; 
   }

   auto criteria_list = j["criteria"];
   for( auto jc : criteria_list )
   {
      Criteria criteria;
      criteria.FromJson( jc );
      m_criteria[criteria.id] = criteria;
   }

   auto warning_list = j["warnings"];
   for( auto jw : warning_list )
   {
      Warning warning;
      warning.FromJson( jw );
      m_warnings[warning.id] = warning;
      if( warning.version >= highest_version )
      {
         highest_version = warning.version;
         highest_version_id = warning.id;
      }
   }

   DateTime expire_dt( expire_time );
   auto this_id = this->id;

   auto fn = [this_id,expire_dt]() {
      printf("Warning expired id: %d, iso: %s, time_t: %ld\n",
            this_id, expire_dt.GetISODate().c_str(), expire_dt.GetTimeT());
   };

   m_expired_timer = std::make_unique<TargetTimer>();
   m_expired_timer->SetTargetTime( expire_dt );

   BuildSkyGuardVBO();
   CalculateBBox();
   CalculateHighestPriority();

   //message =  j["message"].get<std::string>();

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuardWarning::SetExpireFunction( const std::function<void(void)>& fn )
{
   if( m_expired_timer )
   {
      m_expired_timer->setCallback( fn );
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::ostream& operator<<( std::ostream& os , const SkyGuardWarning& skywarning )
{
   os
     << " [Skyguard]: "
     << " id " << skywarning.id
     << " highest_version " << skywarning.highest_version
     << " highest_version_id" << skywarning.highest_version_id
     << " client name " << skywarning.client_name
     << " expire time " << skywarning.expire_time
     //<< " meassage " << skywarning.message
   ;

   os  << " [[Criteria]] {  ";
   for( const auto criteria : skywarning.m_criteria )
   {
      os << criteria.second; 
      os << ",";
   }

   os  << " } [[Warnings]] { ";
   for( const auto warning : skywarning.m_warnings )
   {
      os << warning.second; 
      os << ",";
   }
   os << " } ";

   return os;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
PolygonF SkyGuardWarning::GetAllPoints()
{
   
   PolygonF points;
   for( const auto warning : m_warnings )
   {
      auto& feature_list = warning.second.feature_list;
      for( const auto& feature : feature_list )
      {
         if( feature.geometry.type != GeoType::POINT )
            continue;

         if( feature.geometry.polygon_list.size() == 1 )
         {
            const auto& ref = feature.geometry.polygon_list.front();
            points.Append( ref );
         }
      }
   }
   return points;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::vector<PolygonF> SkyGuardWarning::GetAllLines()
{
   std::vector<PolygonF> lines;
   for( const auto warning : m_warnings )
   {
      auto& feature_list = warning.second.feature_list;
      for( const auto& feature : feature_list )
      {
         if( feature.geometry.type != GeoType::LINE )
            continue;

         const auto& ref = feature.geometry.polygon_list;
         std::copy( ref.begin() , ref.end() , std::back_inserter( lines ) );
      }
   }
   return lines;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::vector<PolygonF> SkyGuardWarning::GetAllPolygons()
{
   std::vector<PolygonF> lines;
   for( const auto warning : m_warnings )
   {
      auto& feature_list = warning.second.feature_list;
      for( const auto& feature : feature_list )
      {
         if( feature.geometry.type != GeoType::POLYGON )
            continue;

         const auto& ref = feature.geometry.polygon_list;
         std::copy( ref.begin() , ref.end() , std::back_inserter( lines ) );
      }
   }
   return lines;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const SkyGuardVBO& SkyGuardWarning::GetVBO() const
{
   return *m_skyguard_vbo;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool SkyGuardWarning::IsInBBox( const PointF& point )const
{
   auto this_box = total_bbox;
   auto diff = Config::WorldToViewport();
   this_box.ExpandSides( diff.x() * ( Config::GetPointSpriteSize() / 2 ) );
   bool success = this_box.ContainsPoint( point );
   return success; 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
float SkyGuardWarning::GetClosestDistance( const PointF& point )const
{
   return 1;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int SkyGuardWarning::GetHighestPriorty()const
{
   return highest_priority;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::string SkyGuardWarning::GetString()const
{
   if( m_warnings.empty() ) 
      return "";

   return m_warnings.find(highest_version_id)->second.message;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int SkyGuardWarning::GetHighestVersion()
{
   return highest_version;
}


