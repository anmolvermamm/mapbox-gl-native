#include "radial_layer.h"

#include "radial_manager.h"
#include "config.h"

#include "gl_radial_2d.h"
#include "controls.h"

#include "gl_radial_2d.h"
#include "../config.h"

#include "http_network_request.h"
#include "json.hpp"

#include "projections.h"
#include "logging.h"


bool RADIAL_LOG = false;


/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
RadialLayer::RadialLayer(unsigned int layer_id) : DefaultLayer(layer_id)
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
RadialLayer::~RadialLayer()
{
   if(m_radial_manager)
   {
      m_radial_manager.reset();
      m_radial_manager.release();
   }

   GLRadial2D::DeleteRadial2D();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialLayer::Init(const char* flashing_stylization, int stylization_size)
{
   if( RADIAL_LOG )
      printf("Init Radial Layer\n");

   m_radial_manager = std::make_unique<RadialManager>(0, 0, 0);
   //m_radial_manager->SetEnabled( true );

   GLRadial2D::GLInitRadial2D();

   //this->RequestSites();
   this->m_initialized = true;
   this->m_is_animated = true;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialLayer::Update(World *world)
{
   if( !m_enabled ) return;

   if(m_product_num != Config::GetRadarProduct() || m_tilt_num != Config::GetTilt())
      SetRadarProduct();

   unsigned int current_time = Config::GetCurrentTime();
   unsigned int past_time = Config::GetPastTime();
   unsigned int current_loop_time = Config::GetCurrentLoopTime();

   auto center = Config::GetLatLon();
   m_radial_manager->Update(center, past_time, current_time, current_loop_time);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialLayer::Draw(World*) 
{
   if( m_enabled ) m_radial_manager->Draw();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialLayer::RequestSites()
{
   if( !m_enabled ) return;

   auto url = Config::GetRadarHost();
   url += "/get_sites?";
   url += "tilt=";
   url += std::to_string(m_radar_product);

   if(!Config::GetAPIKey().empty())
   {
      url += "&apikey=" + Config::GetAPIKey();
   }

   if( RADIAL_LOG )
      printf("Request URL: %s\n", url.c_str());

   auto success = [this]( const ByteArray& data ) {
      this->ReceiveSites( data );
   };

   auto failure = [this](int code, const std::string& error ) {
      this->ReceiveError( code, error );
   };

   HttpNetworkRequest::RequestData( url , success , failure );

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialLayer::RequestSiteInfo(const std::string& site_list)
{
   if( !m_enabled ) return;

   auto url = Config::GetRadarHost();
   url += "/get_sites_info?";
   url += "id=";
   url += site_list;

   if(!Config::GetAPIKey().empty())
   {
      url += "&apikey=" + Config::GetAPIKey();
   }

   if( RADIAL_LOG )
      printf("Request URL: %s\n", url.c_str());

   auto success = [this]( const ByteArray& data ) {
      this->ReceiveSitesInfo( data );
   };

   auto failure = [this](int code, const std::string& error ) {
      this->ReceiveError( code, error );
   };

   HttpNetworkRequest::RequestData( url , success , failure );

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialLayer::ReceiveSites(const ByteArray& data )
{
   using nlohmann::json;
   auto j =  json::parse( data.to_string() );
   auto sites = j["products"];
   //std::vector<std::string> frame_list;
   std::string sites_list;
   bool first = true;
   for( auto site : sites )
   {
      sites_list +=  
         first ? (first = false , site.get<std::string>() ) : std::string(",")
         += site.get<std::string>() ;
   }

   //std::cout << "sites list: " << sites_list << "\n";
   RequestSiteInfo( sites_list );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialLayer::ReceiveSitesInfo(const ByteArray& data )
{

   using nlohmann::json;
   auto j =  json::parse( data.to_string() );
   auto sites_info = j["sites_info"];
   std::vector<SiteInfo> info_list;
   for( auto site_info : sites_info )
   {
      try
      {
         SiteInfo info;
         info.angle = site_info.at("angle");
         info.angle = site_info["angle"].get<float>();
         info.bin_spacing = site_info["bin_spacing"].get<float>();
         info.first_bin_offset = site_info["first_bin_offset"].get<float>();
         info.lat = site_info["lat"].get<float>();
         info.lon = site_info["lon"].get<float>();

         info.m_center_geo =  PointF{info.lon ,info.lat };
         info.m_center_merc = GeoPointToMercatorPoint( info.m_center_geo );
         info.resolution_degree = site_info["resolution_degree"].get<float>();

         info.moment = site_info["moment"].get<int>();
         info.num_bins = site_info["num_bins"].get<int>();
         info.num_rays = site_info["num_rays"].get<int>();
         info.tilt = site_info["tilt"].get<int>();
         info.time = site_info["time"].get<int>();

         info.pid = site_info["pid"].get<std::string>();
         info.site_name = site_info["site_name"].get<std::string>();
         info_list.push_back( info );
      } catch(std::exception& e)
      {
         SiteInfo info;
         info.pid = site_info["pid"].get<std::string>();

         if(RADIAL_LOG)
            printf("Site id info missing");
      }
   }

   m_radial_manager->SetSiteInfoList( info_list );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialLayer::ReceiveError(int code, const std::string& error )
{
   printf("A network error occured requesting Radar Data %s\n",
         error.c_str());
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool RadialLayer::IsLayer(LayerTypes type)
{
   return type == LayerTypes::RADAR;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialLayer::SetEnabled(bool enable)
{
   m_enabled = enable;
   m_radial_manager->SetEnabled(enable);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int RadialLayer::GetCurrentTimeFrameEpoch()
{
   return m_radial_manager->GetCurrentTimeFrameEpoch();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const char* RadialLayer::GetCurrentTimeFrameString()
{
   return m_radial_manager->GetCurrentTimeFrameString();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialLayer::SetRadarProduct()
{
   m_product_num = Config::GetRadarProduct();
   m_tilt_num = Config::GetTilt();

   if(m_product_num >= 1000 && m_product_num <= 8000)
   {
      m_radar_product = 10000 + m_product_num + m_tilt_num;
   }
   else if(m_product_num <= 308)
   {
      if(m_tilt_num == 1 || m_tilt_num == 0)
      {
         m_radar_product = m_product_num;
      }
      else
      {
         m_radar_product = 11000 + m_radar_product + 100 * m_tilt_num;
      }
   }

   //m_radial_manager->ClearSiteList();

   RequestSites();
}
