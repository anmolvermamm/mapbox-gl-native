#ifndef _BYTE_ARRAY_H_
#define _BYTE_ARRAY_H_

#include <vector>
#include <string>

class ByteArray
{
public:
   ByteArray();
   ByteArray(const char* char_array, unsigned int size);
   ByteArray(const ByteArray&) = delete;
   ByteArray(ByteArray&&) = default;
   ByteArray& operator=(const ByteArray&) = delete;
   ByteArray& operator=(ByteArray&&) = default;

   //Make some way of streaming?
   std::vector<char> data;

   std::string to_string()const;
   
};




#endif // end of include guard: _BYTE_ARRAY_H_ 
