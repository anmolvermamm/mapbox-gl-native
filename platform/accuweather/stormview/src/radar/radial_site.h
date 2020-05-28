#ifndef _RADIAL_SITE_H_
#define _RADIAL_SITE_H_



#include <memory>
#include <vector>
#include "pointf.h"
#include "rectf.h"
#include "site_info.h"
#include "rolling_deque.h"
#include "../timer.h"

class ByteArray;

class RadialSite
{
private:
   class RadialItem
   {
   public:
      std::string m_time;
      std::string m_id;
      unsigned int m_time_epoch;
      unsigned int m_texture = 0;
      bool m_loaded = false;
      bool m_requested = false;
      bool m_is_8bit = true;
      bool operator<(const RadialItem& other ) const;
      ~RadialItem();
   };

public:
   RadialSite(const SiteInfo& info, unsigned int past_time, unsigned int current_time);
   virtual ~RadialSite();

   bool operator<( const RadialSite& other ) const;
   const std::string& GetId() const;

   bool ImgLoaded();
   bool ImgRequested();
   unsigned int GetCurrentTimeFrameEpoch();
   const char* GetCurrentTimeFrameString();
   int FrameCount();
   bool CheckLoaded(const std::string& time);
   std::string ActiveTime();

   void SetTimes( const std::vector<std::string>& times );
   void SetImage( const std::string& time , const ByteArray& data );

   //We will need to add the time in here
   void Update(const unsigned int past_time, const unsigned int current_time,
         const unsigned int current_loop_time);
   void Draw();

private:

   SiteInfo m_info;
   RectF m_bounds;
   unsigned int m_buffer;
   float m_radius;
   unsigned int m_past_time;

   std::vector<std::string> m_times;
   std::vector<unsigned int> m_times_epoch;
   RollingDeque<std::shared_ptr<RadialItem> > m_products;
   std::shared_ptr<RadialItem> m_active;
   
};




#endif // end of include guard: _RADIAL_SITE_H_ 
