#ifndef _RADIAL_MANAGER_H_
#define _RADIAL_MANAGER_H_

#include <vector>
#include <memory>

#include "site_info.h"
#include "rolling_deque.h"
#include "pointf.h"

class RadialSite;
class ByteArray;
class CountDownTimer;

class RadialManager
{
public:
   RadialManager(unsigned int past_time, unsigned int current_time,
         unsigned int current_loop_time);
   virtual ~RadialManager();

   void SetSiteInfoList( const std::vector<SiteInfo>& info_list );
   void ClearSiteList();

   void PrintClosestSite( const PointF& point );
   unsigned int GetCurrentTimeFrameEpoch();
   const char* GetCurrentTimeFrameString();

   void ToggleEnable( bool toggle );
   void SetEnabled( bool enable );
   
   void Update(const PointF& point, const unsigned int past_time,
         const unsigned int current_time, const unsigned int current_loop_time);
   void Draw();

private:

   void AddSite( const SiteInfo& info );

   void RequestTimes( const std::string& id );
   void ReceiveTimes( const std::string& id, const ByteArray& data );
   void ReceiveTimesError(int code, const std::string& id , const std::string& error  );

   void RequestImage( const std::string& id, const std::string& time );
   void ReceiveImage( const std::string& id, const std::string& time, const ByteArray& data );
   void ReceiveImageError(int code, const std::string& id, const std::string& time, const std::string& error  );

   bool m_enable = false;

   std::vector<SiteInfo> m_info_list;

   RollingDeque<std::shared_ptr<RadialSite> > m_queued_sites;
   std::shared_ptr<RadialSite> m_active_site;

   std::string m_current_id;
   std::unique_ptr<CountDownTimer> m_update_timer;

   unsigned int m_current_time;
   unsigned int m_past_time;
   unsigned int m_current_loop_time;
};


#endif // end of include guard: _RADIAL_MANAGER_H_ 
