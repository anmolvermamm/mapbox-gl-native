#ifndef _SKY_GUARD_WARNING_H_
#define _SKY_GUARD_WARNING_H_

#include <string>
#include <iosfwd>
//#include "json_fwd.hpp"
#include "json.hpp"
#include "polygonf.h"
#include <unordered_map>
#include <functional>
#include "geo_type.h"
#include "rectf.h"

class ByteArray;
class SkyGuardVBO;
class TargetTimer;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
class Criteria
{
   public:
   void FromJson(  nlohmann::json& json );
   friend std::ostream& operator<<( std::ostream& os , const Criteria& criteria );

   int id;
   int type_id;
   int severity_id;
   std::string severity;
   std::string event_type;
};
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
class Geometry
{
   public:

   GeoType GetType( const std::string& type_str );
   void GetPolygon( GeoType type , nlohmann::json& json );
   void FromJson(  GeoType type , nlohmann::json& json );
   friend std::ostream& operator<<( std::ostream& os , const Geometry& geometry );


   //PolygonF polygon;
   std::vector<PolygonF> polygon_list;
   //std::vector<PolygonF> m_holes;
   std::string type_str;
   GeoType type;

};
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
class Properties
{
   public:

   void FromJson( GeoType type, nlohmann::json& json );
   void FromJsonPoint( nlohmann::json& json );
   void FromJsonLine( nlohmann::json& json );
   void FromJsonPolygon( nlohmann::json& json );
   friend std::ostream& operator<<( std::ostream& os , const Properties& property );

   unsigned id = 0;
   GeoType type;
   std::string state;
   std::string locale;
   std::string name;
};
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
class Feature
{
public:
   void FromJson(  GeoType type , nlohmann::json& json );
   friend std::ostream& operator<<( std::ostream& os , const Feature& warning );
   unsigned GetId()const;

   Properties properties;
   Geometry geometry;
};
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
class Warning
{
   public:
   void FromJson(  nlohmann::json& json );
   friend std::ostream& operator<<( std::ostream& os , const Warning& warning );

   void AddGeoPoints( nlohmann::json& json );
   void AddGeoLines( nlohmann::json& json );
   void AddGeoPolygons( nlohmann::json& json );
   
   int id;
   int version;
   RectF bbox;
   std::string warning_type;
   std::string message;
   std::vector<Feature> feature_list;
};

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
class SkyGuardWarning
{
public:
   SkyGuardWarning();
   virtual ~SkyGuardWarning();

   void FromJson( const ByteArray& data );

   friend std::ostream& operator<<( std::ostream& os , const SkyGuardWarning& warning );

   int GetId() { return id; }

   PolygonF GetAllPoints();
   std::vector<PolygonF> GetAllLines();
   std::vector<PolygonF> GetAllPolygons();

   const SkyGuardVBO& GetVBO() const;

   bool IsInBBox( const PointF& point )const;
   float GetClosestDistance( const PointF& point )const;
   int GetHighestPriorty()const;
   std::string GetString()const;

   int GetHighestVersion();

   void SetExpireFunction( const std::function<void(void)>& fn );

private:
   void BuildSkyGuardVBO();
   void CalculateBBox();
   void CalculateHighestPriority();

private:
   int id = 0;
   int highest_version = 0;
   int highest_version_id = 0;
   int highest_priority = std::numeric_limits<int>::max();
   std::string client_name;
   std::string message;
   std::string expire_time;
   RectF total_bbox;
   //Criteria
   std::unordered_map<int,Criteria> m_criteria; 

   //Warnings
   std::unordered_map<int,Warning> m_warnings; 

   std::unique_ptr<SkyGuardVBO> m_skyguard_vbo;

   std::unique_ptr<TargetTimer> m_expired_timer;
   
};

#endif // end of include guard: _SKY_GUARD_WARNING_H_ 
