#include "radial_site.h"
#include "../logging.h"

#include "../networking/http_network_request.h"
#include "../glow/glow.h"
#include <glm/mat4x4.hpp>

#include "../config.h"
#include "gl_radial_2d.h"
#include "radial_buffer.h"
#include <algorithm>
#include "../date_time.h"


extern bool RADIAL_LOG;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool RadialSite::RadialItem::operator<(const RadialItem& other )const
{
   return (m_time_epoch < other.m_time_epoch);
   //return ( m_time.compare( other.m_time ) < 0 );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
RadialSite::RadialItem::~RadialItem()
{
   if( m_loaded )
   {
      if( RADIAL_LOG )
         printf("Unload Radial item for id: %s, time: %s, ref: %d\n",
               m_id.c_str(), m_time.c_str(), m_texture);

      Glow::DeleteTexture( m_texture );
   } else 
   {
      if(RADIAL_LOG)
         printf("Not loaded, delete item for time: %s", m_time.c_str());
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
RadialSite::RadialSite(const SiteInfo& info, unsigned int past_time,
      unsigned int current_time)
   : 
      m_info(info),
      m_products( 25 )
{
   static auto coords = RectF{ 0.0 , 1.0 , 0.0 , 1.0 };
   m_bounds = RectF{ 
      info.m_center_merc.x() - 0.1f , 
      info.m_center_merc.x() + 0.1f ,
      info.m_center_merc.y() - 0.1f , 
      info.m_center_merc.y() + 0.1f 
   };   
   m_buffer = Glow::CreateBuffer( m_bounds , coords );

   m_radius = info.num_bins* 0.001 * info.bin_spacing;
   m_past_time = current_time - past_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
RadialSite::~RadialSite()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
//void RadialSite::RequestTimes()
//{
//
//   FrameCallback call_back = [this](const std::vector<std::string>& times ) {
//      this->ReceiveTimes( times ); 
//   };
//
//}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool RadialSite::operator<( const RadialSite& other ) const
{
   return ( m_info.pid.compare( other.m_info.pid ) < 0 );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const std::string& RadialSite::GetId() const
{
   return m_info.pid;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool RadialSite::ImgLoaded()
{
   return m_active ?  m_active->m_loaded : false;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool RadialSite::ImgRequested()
{
   return m_active ?  m_active->m_requested : false;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int RadialSite::FrameCount()
{
   return m_times.size();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::string RadialSite::ActiveTime()
{
   return m_active ? m_active->m_time : "none";
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialSite::SetTimes( const std::vector<std::string>& times )
{
   m_times = times;

   for( auto& prod_time : m_times )
   {
      auto iter = std::find_if( m_products.begin() , m_products.end() , [prod_time](
               auto& item ) { return item->m_time.compare(prod_time) == 0; } );

      DateTime date_time(prod_time);
      time_t utc_epoch_time = date_time.GetTimeT();
      if( iter == m_products.end() && utc_epoch_time >= m_past_time)
      {
         if(RADIAL_LOG)
            printf("Epoch Time: %lu, Date String: %s", utc_epoch_time, prod_time.c_str());
         auto item = std::make_shared<RadialItem>();
         item->m_time = prod_time;
         item->m_time_epoch = utc_epoch_time;
         item->m_id = this->m_info.pid;
         m_products.push_stack( item );
         if(std::find(m_times_epoch.begin(), m_times_epoch.end(),
               item->m_time_epoch) == m_times_epoch.end())
         {
            m_times_epoch.push_back(item->m_time_epoch);
         }
      }
   }

   if(!m_active && !m_products.empty())
   {
      m_active = m_products.back();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool RadialSite::CheckLoaded(const std::string& time)
{
   auto iter = std::find_if( m_products.begin() , m_products.end() , [time](
            auto& item ) { return item->m_time.compare(time) == 0;});

   DateTime date_time(time);
   time_t utc_time_epoch = date_time.GetTimeT();
   if(utc_time_epoch < m_past_time)
      return true;

   if(iter != m_products.end())
      return (*iter)->m_loaded;
   else
      return false;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialSite::SetImage( const std::string& time , const ByteArray& data )
{
   // load product into texture if not already
   for( auto& product : m_products  )
   {
      if( (product->m_time.compare(time) == 0) && (data.data.size() > 0) && (!product->m_loaded))
      {
         if(RADIAL_LOG)
         {
            printf("\nRecieved image for id: %s, time: %s, data size: %lu\n",
                  m_info.pid.c_str(), time.c_str(), data.data.size());
         }

         product->m_texture = Glow::Load2DImageToTextureFromMem( data, product->m_is_8bit,
               Glow::NEAREST, true );
         product->m_loaded = true;
      }
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialSite::Update(const unsigned int past_time, const unsigned int current_time,
      const unsigned int current_loop_time)
{
   time_t epoch_time = 0;
   if( !m_products.empty() )
   {
      if(Config::GetSSRadarMaxFrameCount() > 0)
      {
         while(m_products.size() > Config::GetSSRadarMaxFrameCount())
         {
            m_products.erase(m_products.begin());
         }

         while(m_times_epoch.size() > Config::GetSSRadarMaxFrameCount())
         {
            m_times_epoch.erase(m_times_epoch.begin());
         }

         m_past_time = *(m_times_epoch.begin());
         Config::SetSSPastSpanTime(current_time - *(m_times_epoch.begin()));
      }
      else
      {
         m_past_time = current_time - past_time;
         Config::SetSSPastSpanTime(past_time);
         std::vector<unsigned int> times_epoch = m_times_epoch;
         for(auto& prod_time : m_times_epoch)
         {
            // delete old time and product
            if(prod_time < m_past_time)
            {
               if(RADIAL_LOG)
                  printf("Delete %d", prod_time);

               times_epoch.erase(std::remove(times_epoch.begin(), times_epoch.end(), prod_time),
                     times_epoch.end());
               auto iter = std::find_if(m_products.begin(), m_products.end(), [prod_time](
                            auto& item) {return item->m_time_epoch == prod_time; });
               if(iter != m_products.end())
                  m_products.erase(iter);
            }
         }

         m_times_epoch.clear();
         m_times_epoch = times_epoch;
      }

      int now_diff = -60000;
      if(!m_times_epoch.empty())
      {
         for(auto& prod_time : m_times_epoch)
         {
            // find the closest time that is before current time
            int diff = prod_time - current_loop_time;
            if(diff < 0 && (std::max(now_diff, diff) == diff))
            {
               now_diff = diff;
               epoch_time = prod_time;
            }
         }
      }

      // set the active product
      auto iter = std::find_if( m_products.begin() , m_products.end() , [epoch_time](
            auto& item ) { return item->m_time_epoch == epoch_time; } );
      if( iter != m_products.end())
      {
         m_active = *iter;
      }

      if(!m_active->m_loaded && !m_active->m_requested )
      {
         if(RADIAL_LOG)
            printf("Need to request image");

         m_active->m_requested = true;
      }
   }
   else
   {
      m_active.reset();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RadialSite::Draw()
{
   if(m_active && m_active->m_loaded )
   {
      const auto& mat = Config::GetMatrix( );
      RadialBuffer buffer;
      buffer.buffer = m_buffer;
      buffer.texture = m_active->m_texture;
      buffer.loaded = m_active->m_loaded;
      buffer.is_8bit = m_active->m_is_8bit;
      GLRadial2D::DrawGL( mat , buffer , m_radius , m_info.m_center_geo , 0.9 );
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int RadialSite::GetCurrentTimeFrameEpoch()
{
   if(m_active)
   {
      return m_active->m_time_epoch;
   }
   else
   {
      return 0;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const char* RadialSite::GetCurrentTimeFrameString()
{
   if(m_active)
   {
      return m_active->m_time.c_str();
   }
   else
   {
      return "";
   }
}
