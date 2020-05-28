#ifndef _STRING_HELPER_H_
#define _STRING_HELPER_H_


#include <algorithm>
#include <string>

#ifdef __APPLE__
#include <vector>
#else
// Android
#include <vector>
#endif

inline std::string str_to_lower( const std::string& str )
{
   auto data = str;
   std::transform(data.begin(), data.end(), data.begin(), ::tolower);
   return str;
   
};

// for string delimiter
inline std::vector<std::string> split(std::string s, std::string delimiter) 
{
   size_t pos_start = 0, pos_end, delim_len = delimiter.length();
   std::string token;
   std::vector<std::string> res;
   while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
       token = s.substr(pos_start, pos_end - pos_start);
       pos_start = pos_end + delim_len;
       res.push_back(token);
   }
   res.push_back(s.substr(pos_start));
   return res;
}


#endif // end of include guard: _STRING_HELPER_H_ 
