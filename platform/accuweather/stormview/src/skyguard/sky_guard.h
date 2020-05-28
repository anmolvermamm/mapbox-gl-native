#ifndef _SKY_GUARD_H_
#define _SKY_GUARD_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "linef.h"
#include "polygonf.h"

class PointF;
class ByteArray;
class SkyGuardWarning;
class CountDownTimer;

class SkyGuard
{
public:

   struct WarningRequest {
      int client;
      int id;
      int version;
   };

   SkyGuard();
   virtual ~SkyGuard();

   void Init();
   void Update();
   void Draw();
   virtual std::string Query( const PointF& );

protected:
   

   
   void MakeSkyguardListRequest();
   void MakeSkyguardWarningRequest(
         const WarningRequest& request
         //int client,
         //int id,
         //int version 
   );

   void ReceiveSkyGuardList(const ByteArray& data );
   void ReceiveSkyGuardWarning( const ByteArray& data );

   void ReceiveSkyGuardListError( int code, const std::string& error );
   void ReceiveSkyGuardWarningError( int code, const WarningRequest& request , const std::string& error );

   void RemoveSkyGuardWarning( int id );


private:
   //bool points_loaded = false;
   //bool lines_loaded = false;

   std::string m_base_url;
   std::string m_auth_key;
   std::string m_auth_value;

   //std::vector<PointF> m_points;
   //PolygonF line;
   //PolygonF draw_poly;

   std::unordered_map<int,std::shared_ptr<SkyGuardWarning>> m_map;

   std::unique_ptr<CountDownTimer> m_update_timer;
   
};


#endif // end of include guard: _SKY_GUARD_H_ 
