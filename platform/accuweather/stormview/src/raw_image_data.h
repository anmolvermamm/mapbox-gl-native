#ifndef _RAW_IMAGE_DATA_H_
#define _RAW_IMAGE_DATA_H_

#include <vector>
#include <string>
#include "point.h"
#include "color.h"

class ByteArray;

class RawImageData
{
public:
   enum FileType{ PNG };

   RawImageData();

   Color GetColorFromPoint( const Point& point );
   unsigned int GetIndexFromPoint( const Point& point );
   Color GetColorFromIndex(int index );

   bool DimInImage( const Point& size );
   
   bool SaveDebug(const std::string& filename );
   void LoadFromDebug();
   void LoadFromFile(const std::string& filename, const bool use_palette = false );
   void LoadFromMem(const ByteArray& data, unsigned int &format, const bool use_palette = false);

   //Cannot copy
   RawImageData(const RawImageData&)=delete;
   RawImageData& operator=(const RawImageData&)=delete;

   //We can move it though
   RawImageData(RawImageData&&) = default;
   RawImageData& operator=(RawImageData&&) = default;
   virtual ~RawImageData();

   Point m_size;
//private:

   //operator const char*() { return &m_data[0]; }

   std::vector<unsigned char> m_data; 
   //std::vector<int> m_data; 
private:
   
};


#endif // end of include guard: _RAW_IMAGE_DATA_H_ 
