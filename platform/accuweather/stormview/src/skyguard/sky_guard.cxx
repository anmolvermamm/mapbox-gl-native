#include "sky_guard.h"

#include "glow_point_sprite.h"
#include "glow_lines.h"
#include <glm/mat4x4.hpp>
#include "config.h"
#include "http_network_request.h"
#include "pointf.h"
#include <iostream>
#include "json.hpp"
#include <set>
#include "sky_guard_warning.h"
#include "projections.h"
#include "sky_guard_vbo.h"

#include "date_time.h"
#include "timer.h"

#include "error_code.h"
#include "error_handler.h"
#include "logging.h"


bool SKYGUARD_LOG = false;


/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
SkyGuard::SkyGuard()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
SkyGuard::~SkyGuard()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuard::Init()
{
   //m_base_url = "https://enterpriseportal-dev.accuweather.com";
   m_base_url = "https://enterpriseportal-v2.accuweather.com";
   m_auth_key = "Authorization";
   m_auth_value = "Basic a2hscmp4Sko2OGJFMmRTR25JbU03ZlRWcGdFdGNKUENPY3pzN3dMbjpUbVpmczdLNVQ5bmlYSThFaEtVa1Q2U0JBTVI1WkJZbnVxM01FdVBY";

   GlowPointSprite::InitGlowPointSprite();
   GlowLines::InitGlowLines();

   MakeSkyguardListRequest();

   m_update_timer = std::make_unique<CountDownTimer>();
   m_update_timer->setCallback(
         [this](){ this->MakeSkyguardListRequest(); }
         );
   m_update_timer->SetCountDownTimeMs( 60 * 1000 , true );

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuard::Update()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuard::Draw()
{
   auto mat = Config::GetMatrix();

   static int count = -1;
   
   //We are looping through all our warnings and drawing them
   //this isn't the most efficient way of setting up the opengl calls
   //but it is fast enough for now
   for( auto& pair : m_map )
   {
      auto& warning = pair.second;
      const auto& vbo = warning->GetVBO();
      for( auto& vbo_geometry : vbo.geometry )
      {
         if( vbo_geometry.type == GeoType::POINT )
         {
            //GlowPointSprite::Draw(
            //   mat ,
            //   vbo_geometry.buffer ,
            //   vbo_geometry.count
            //);
         } else if( vbo_geometry.type == GeoType::LINE )
         {
            //GlowLines::Draw(
            //   mat ,
            //   vbo_geometry.buffer ,
            //   vbo_geometry.count,
            //   false
            //);
         } else if( vbo_geometry.type == GeoType::POLYGON )
         {
            //GlowLines::Draw(
            //   mat ,
            //   vbo_geometry.buffer ,
            //   vbo_geometry.count,
            //   true
            //);
         }
      }
   }

   ++count;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuard::MakeSkyguardListRequest()
{

   std::string url = m_base_url + "/api/skyguard-list";

   NetworkHeaders headers;
   headers[m_auth_key] = m_auth_value;

   auto success = [this]( auto& data )
   {
      this->ReceiveSkyGuardList( data );
   };

   auto failure = [this]( auto code, auto& error )
   {
      this->ReceiveSkyGuardListError( code, error );
   };

   HttpNetworkRequest::RequestData(
         url,
         headers,
         success,
         failure
   );

   if( SKYGUARD_LOG )
      printf("Make Skyguard List Request Url: %s\n", url.c_str());
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuard::ReceiveSkyGuardList(const ByteArray& data )
{
   auto str = data.to_string();
   using nlohmann::json;

   bool skip_client_check = true;
   
   auto j =  json::parse( str );

   auto clients = std::set<int>{ 1 , 7 , 50 };
   if( j.empty() )
   {
      if( SKYGUARD_LOG )
         printf("No warnings to display\n");
      ErrorHandler::PushError( SKYGUARDERROR + NODATAERROR , "No Skyguard Warnigns to Display" );
      return;
   }

   for( auto item : j )
   {
      auto id = item["id"].get<int>();
      auto client = item["clientId"].get<int>();
      auto version = item["latestVersion"].get<int>();
      //auto completed = item["completed"].get<bool>();

      if( clients.find( client ) != clients.end() || skip_client_check )
      {
         auto i = m_map.find( id );
         if( i == m_map.end() )
         {
            MakeSkyguardWarningRequest( { client , id , version } );
         } else if( version > i->second->GetHighestVersion() )
         {
            MakeSkyguardWarningRequest( { client , id , version } );
         } else
         {
         }

      }
   }
   
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuard::MakeSkyguardWarningRequest(
         const WarningRequest& request
)
{

   auto url = m_base_url 
      + "/api/client/" 
      + std::to_string(request.client)
      + "/skyguard/"
      + std::to_string(request.id)
      + "?version="
      + std::to_string(request.version);

   if( SKYGUARD_LOG )
   {
      printf("Make Skyguard Warning Request Url: %s\n", url.c_str());
   }

   NetworkHeaders headers;
   headers[m_auth_key] = m_auth_value;

   auto success = [this]( auto& data )
   {
      this->ReceiveSkyGuardWarning( data );
   };

   auto failure = [request,this]( auto code, auto& error )
   {
      this->ReceiveSkyGuardWarningError( code, request,  error );
   };

   HttpNetworkRequest::RequestData(
         url,
         headers,
         success,
         failure
   );

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuard::ReceiveSkyGuardWarning( const ByteArray& data )
{
   auto warning = std::make_shared<SkyGuardWarning>();
   warning->FromJson( data );

   auto id = warning->GetId();
   bool has = false;
   if( m_map.find( warning->GetId() ) != m_map.end()  )
   {
      has = true;
   }

   auto fn = [id,this](){
      this->RemoveSkyGuardWarning( id );
   };
   warning->SetExpireFunction( fn );
   m_map[warning->GetId()] = warning;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuard::ReceiveSkyGuardListError( int code, const std::string& err)
{
   printf("SkyGuard List Error is: %s\n", err.c_str());
   auto msg = std::string( "Error receving SkyGuard List Data. ");
   msg + err;
   ErrorHandler::PushError( SKYGUARDERROR + http_error_type( code )  , msg );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuard::ReceiveSkyGuardWarningError(int code, const WarningRequest& request, const std::string& err)
{
   auto msg = std::string( "Error receving SkyGuard Warning Data. ")  
      + "Id: " + std::to_string( request.id ) 
      + " Client: " + std::to_string( request.client )
      + " Version: " + std::to_string( request.version)
      + " Code: " + std::to_string(code);
   msg + err;
   ErrorHandler::PushError( SKYGUARDERROR + http_error_type( code ) , msg );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::string SkyGuard::Query( const PointF& pos )
{
   std::vector<std::shared_ptr<SkyGuardWarning>> found;

   for( const auto& wpair : m_map )
   {
      const auto& warning = wpair.second;
      if( warning->IsInBBox( pos ) )
      {
         found.push_back( warning );
      }
   }


   if( found.empty() )
   {
      return "No skyguard warning found";
   }


   auto highest_priority = found.begin();
   for(auto iter = found.begin()+1 ; iter != found.end() ; ++iter )
   {
      if( (*iter)->GetHighestPriorty()  <
          (*highest_priority)->GetHighestPriorty() )
      {
         highest_priority = iter;
      }
   }

   return (*highest_priority)->GetString();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuard::RemoveSkyGuardWarning( int id )
{
   auto i = m_map.find( id );
   if( i == m_map.end() )
      return;

   if( SKYGUARD_LOG )
      printf("Remove skyguard warning id: %d\n", id);
   m_map.erase( i );
}
