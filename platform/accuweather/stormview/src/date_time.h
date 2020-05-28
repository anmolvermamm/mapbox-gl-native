#ifndef _DATE_TIME_H_
#define _DATE_TIME_H_


#include <string>
#include <chrono>
#include <ctime>

class DateTime
{
public:
   DateTime( const std::string& isotime );
   DateTime(time_t epoch_time );
   virtual ~DateTime();

   time_t GetTimeT()const;
   std::string GetISODate()const;

   static time_t NowTimeT();
   static std::string NowUTCStr();

private:
   static time_t ConvertISOTimeToTimeT( const std::string& isotime );
   static std::string ConvertTimeTToISOTime(time_t time);

   time_t m_time_t = 1;
   
};


#endif // end of include guard: _Z_TIME_H_ 
