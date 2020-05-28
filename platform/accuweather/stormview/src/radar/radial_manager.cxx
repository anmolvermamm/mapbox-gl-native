#include "radial_manager.h"

#include <algorithm>
#include <limits>
#include "site_info.h"

#include "../config.h"
#include "radial_site.h"

#include "../timer.h"

#include "http_network_request.h"
#include "json.hpp"
#include "logging.h"

extern bool RADIAL_LOG;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
RadialManager::RadialManager(unsigned int past_time, unsigned int current_time,
      unsigned int current_loop_time)
   : m_queued_sites( 3 )
{
   m_update_timer = std::make_unique<CountDownTimer>();
   m_update_timer->setCallback(
            [this](){this->RequestTimes(m_current_id);});
   m_update_timer->SetCountDownTimeMs(60 * 1000, true);

   m_past_time = past_time;
   m_current_time = current_time;
   m_current_loop_time = current_loop_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
RadialManager::~RadialManager()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::SetSiteInfoList( const std::vector<SiteInfo>& info_list )
{
   m_info_list = info_list;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::PrintClosestSite( const PointF&  )
{
   printf("TODO");
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::ToggleEnable( bool toggle )
{
   m_enable = toggle ? !m_enable : m_enable;
   SetEnabled( m_enable );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::SetEnabled( bool enable )
{
   m_enable = enable;

   if( !m_enable )
   {
      m_active_site.reset();
      m_queued_sites.clear();
      m_current_id = "";
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::Update(const PointF& point, const unsigned int past_time,
      const unsigned int current_time, const unsigned int current_loop_time)
{
   if( !m_enable ) return;

   m_past_time = Config::GetPastTime();
   m_current_time = Config::GetCurrentTime();
   m_current_loop_time = Config::GetCurrentLoopTime();

   auto found = m_info_list.cend();
   float distance = std::numeric_limits<float>::max();
   for( auto i = m_info_list.cbegin() ; i != m_info_list.cend() ; ++i )
   {
      float distanceS = point.distanceSquared( i->m_center_merc );
      if( distanceS < distance )
      {
         found = i;
         distance = distanceS;
      }
   }

   if( found != m_info_list.end() )
   {
      m_current_id = found->pid ;
      auto& id = m_current_id;

      auto iter = std::find_if( m_queued_sites.begin() , m_queued_sites.end() , 
         [&id](auto& radial_site) {
            return radial_site->GetId() == id;
         }
      );
      if( iter == m_queued_sites.end() )
      {
         AddSite( *found );
      } else
      {
         m_active_site = *iter;
      }
   } 

   if( m_active_site )
   {
      m_active_site->Update(past_time, current_time, current_loop_time);
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::Draw()
{
   if( m_active_site )
   {
      m_active_site->Draw();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::AddSite( const SiteInfo& info )
{
   auto& id = info.pid;
   auto iter = std::find_if( m_queued_sites.begin() , m_queued_sites.end() , [id]( 
            auto& item ) { return item->GetId() == id; } );

   if( iter == m_queued_sites.end() )
   {
      auto item = std::make_shared<RadialSite>(info, m_past_time, m_current_time);
      m_active_site = item;
      this->RequestTimes( m_active_site->GetId() );
      m_queued_sites.push_stack( item );
   }

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::RequestTimes( const std::string& id )
{
   if( !m_enable ) return;

   if(!m_active_site || m_active_site->GetId().compare(id) != 0)
      return;

   auto url = Config::GetRadarHost();
   url += "/get_site_times?";
   url += "id=";
   url += id;

   if(!Config::GetAPIKey().empty())
   {
      url += "&apikey=" + Config::GetAPIKey();
   }

   if( RADIAL_LOG )
      printf("Request URL: %s\n", url.c_str());

   auto success = [this,id](const ByteArray& data) {
      this->ReceiveTimes(id,data);
   };

   auto error = [this,id]( int code, const std::string& error ) {
      this->ReceiveTimesError( code, id, error );
   };

   HttpNetworkRequest::RequestData( url ,
         success , error );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
//void RadialManager::ReceiveTimes( const std::string& id  , const std::vector<std::string>& times )
//{
//   for( auto& site : m_queued_sites )
//   {
//      if( site->GetId() == id )
//      {
//         site->SetTimes( times );
//         this->RequestImage( id , site->ActiveTime() ); 
//      }
//   }
//}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::ReceiveTimes(const std::string& id, const ByteArray& data )
{
   if(!m_active_site || m_active_site->GetId().compare(id) != 0)
      return;

   using nlohmann::json;
   auto j =  json::parse( data.to_string() );
   std::vector<std::string> times = j["times"];

   if( RADIAL_LOG )
      printf("Received time list size: %lu\n", times.size());

   if(Config::GetSSRadarMaxFrameCount() > 0)
   {
      while(times.size() > Config::GetSSRadarMaxFrameCount())
      {
         times.erase(times.begin());
      }
   }

   for( auto& site : m_queued_sites )
   {
      if( site->GetId().compare(id) == 0 )
      {
         site->SetTimes( times );
         for(auto& time : times)
         {
            this->RequestImage( id , time );
         }
      }
   }

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::ReceiveTimesError(int code, const std::string& id, 
      const std::string& error  )
{
   printf("Received network error for Radar Times! id: %s, err: %s\n",
         id.c_str(), error.c_str());
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::RequestImage( const std::string& id , const std::string& time )
{
   if( !m_enable ) return;

   if(!m_active_site || m_active_site->CheckLoaded(time))
      return;

   auto url = Config::GetRadarHost();
   url += "/get_site_data?";
   url += "id=";
   url += id;
   url += "&frametime=";
   url += time;

   if(Config::GetSMIRActive())
   {
      url += "&8bit_smir=true";
   }
   else
   {
      url += "&8bit=true";
   }

   if(!Config::GetAPIKey().empty())
   {
      url += "&apikey=" + Config::GetAPIKey();
   }

   if( RADIAL_LOG )
      printf("Request URL: %s\n", url.c_str());

   auto success = [this,id,time](const ByteArray& data) {
      this->ReceiveImage(id,time,data);
   };

   auto error = [this,id,time]( int code, const std::string& error ) {
      this->ReceiveImageError( code, id, time, error );
   };

   HttpNetworkRequest::RequestData( url ,
         success , error );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::ReceiveImage( const std::string& id  , const std::string& time, const ByteArray& data )
{
   for( auto& site : m_queued_sites )
   {
      if( site->GetId() == id )
      {
         if( RADIAL_LOG )
            printf("Received image size: %lu, time: %s\n", data.data.size(), time.c_str());
         site->SetImage( time , data );
      }
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::ReceiveImageError(int code, const std::string& id, const std::string& time, 
      const std::string& error  )
{
   printf("Received network error for Radar Image! id: %s, time: %s, error: %s\n",
         id.c_str(), time.c_str(), error.c_str());
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialManager::ClearSiteList()
{
   m_queued_sites.clear();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int RadialManager::GetCurrentTimeFrameEpoch()
{
   if(m_active_site)
   {
      return m_active_site->GetCurrentTimeFrameEpoch();
   }
   else
   {
      return 0;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const char* RadialManager::GetCurrentTimeFrameString()
{
   if(m_active_site)
   {
      return m_active_site->GetCurrentTimeFrameString();
   }
   else
   {
      return "";
   }
}
