#include "date_time.h"
#include <ctime>
#include <regex>
#include <iostream>
#include <sstream>



inline time_t my_timegm(register struct tm * t)
/* struct tm to seconds since Unix epoch */
{
    long year;
    time_t result;
#define MONTHSPERYEAR   12      /* months per calendar year */
    static const int cumdays[MONTHSPERYEAR] =
        { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

    /*@ +matchanyintegral @*/
    year = 1900 + t->tm_year + t->tm_mon / MONTHSPERYEAR;
    result = (year - 1970) * 365 + cumdays[t->tm_mon % MONTHSPERYEAR];
    result += (year - 1968) / 4;
    result -= (year - 1900) / 100;
    result += (year - 1600) / 400;
    if ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0) &&
        (t->tm_mon % MONTHSPERYEAR) < 2)
        result--;
    result += t->tm_mday - 1;
    result *= 24;
    result += t->tm_hour;
    result *= 60;
    result += t->tm_min;
    result *= 60;
    result += t->tm_sec;
    //if (t->tm_isdst == 1)
    //    result -= 3600;
    /*@ -matchanyintegral @*/
    return (result);
}


/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DateTime::DateTime( const std::string& isotime )
{
   m_time_t = ConvertISOTimeToTimeT( isotime );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DateTime::DateTime(std::time_t epoch_time )
{
   m_time_t = epoch_time;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
DateTime::~DateTime()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
time_t DateTime::GetTimeT()const
{
   return m_time_t;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::string DateTime::GetISODate()const
{
   return ConvertTimeTToISOTime(m_time_t);
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
time_t DateTime::NowTimeT()
{
   return time(NULL);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::string DateTime::NowUTCStr()
{
   return ConvertTimeTToISOTime( NowTimeT() );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
time_t DateTime::ConvertISOTimeToTimeT( const std::string& isotime )
{
   try{
   std::string regex_str = R"((\d{4})[-](\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})([Z+-])(\d{2})?[:]?(\d{2})?)";
   std::regex isoregex( regex_str , std::regex_constants::ECMAScript );
   std::smatch matches;

   std::regex_match( isotime , matches, isoregex);
   if( matches.size() == 10 )
   {
      // No Error
   }
   else
   {
      printf("Error processing ISOTime: %s", isotime.c_str());
      return 0;
   }

   bool has_offset = ! (matches[8]).str().empty();
   auto hour_offset =  has_offset ? std::stoi(matches[8]) : 0;
   hour_offset *= matches[7] == "-" ? 1 : -1;

   if( has_offset )
   {
      printf("has offset");
   }

   struct tm timeinfo;
   timeinfo.tm_year = std::stoi( matches[1] ) - 1900;
   timeinfo.tm_mon = std::stoi( matches[2] ) - 1;
   timeinfo.tm_mday = std::stoi( matches[3] );

   timeinfo.tm_hour = std::stoi( matches[4]  ) + hour_offset;
   timeinfo.tm_min = std::stoi( matches[5] );
   timeinfo.tm_sec = std::stoi( matches[6] );
   time_t res = my_timegm( &timeinfo );

   return res;
   } catch( std::exception& e )
   {
      std::cerr<< "Regex error: " << e.what() << "\n";
   }
  
   return 0;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::string DateTime::ConvertTimeTToISOTime(time_t time)
{

   struct tm* ptm;
   ptm = gmtime( &time ); 

   std::stringstream ss;
   ss << 
   (ptm->tm_year+1900)
   << "-" << (ptm->tm_mon+1)
   << "-" << ptm->tm_mday
   << "T" << ptm->tm_hour
   << ":" << ptm->tm_min
   << ":" << ptm->tm_sec
   << "Z";

   return ss.str();

}

