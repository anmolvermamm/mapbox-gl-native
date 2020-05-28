#ifndef _RAW_IMAGE_DATA_CXX_
#define _RAW_IMAGE_DATA_CXX_


#include "raw_image_data.h"
#include "point.h"

#include <iostream>
#include <fstream>

#include "png_image.h"
#include "byte_array.h"
//#include "image_debug.h"

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
RawImageData::RawImageData()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
RawImageData::~RawImageData()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
Color RawImageData::GetColorFromPoint( const Point& point )
{
   auto index = GetIndexFromPoint( point );
   return GetColorFromIndex( index );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int RawImageData::GetIndexFromPoint( const Point& point )
{
   unsigned int index =  point.y()  * this->m_size.x() * 4 + point.x() * 4;
   unsigned int ret =  point.x() < m_size.x() && point.y() < m_size.y() ? index : -1;
   return ret;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool RawImageData::DimInImage( const Point& dim )
{
   return dim.x() < m_size.x() && dim.y() < m_size.y() 
         && dim.x() >= 0 && dim.y() >= 0
         ? true : false;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
Color RawImageData::GetColorFromIndex( int index )
{
   if( (size_t)index+3 > m_data.size() )
   {
      Color ret {0, 0, 0, 255 };
      return ret;
   }
   
   Color ret {
      m_data[index+0]  ,
      m_data[index+1]  ,
      m_data[index+2]  ,
      255 
   };
   

   return ret;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool RawImageData::SaveDebug(const std::string& filename )
{
   // Received Particle data
   std::ofstream out(filename);
   out << "int debug_width = ";
   out << m_size.x();
   out << ";\n";

   out << "int debug_height = ";
   out << m_size.y();
   out << ";\n";

   out << "std::vector<unsigned char> debug_data = { ";

   bool first = true;
   for( auto c : m_data ){
      if( first )
      {
         first = false;
      } else {
         out << ",";
      }
      out << (int)c;
      //out << R"(')";
      //out << c;
      //out << R"(')";
      
   }
   
   out << " }; ";
   //out << data;
   out.close();
   return true;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RawImageData::LoadFromDebug()
{
   //m_size.x() = debug_width;
   //m_size.y() = debug_height;

   //m_data = debug_data;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
#include <fstream> 
void RawImageData::LoadFromFile(const std::string& filename, const bool use_palette )
{
   std::vector<char> data;
   std::ifstream ifs; 

   ifs.open(filename , std::ifstream::in);
   char c;
   while( ifs.get(c) )
   {
      data.push_back( c );
   }
   printf("Data size: %lu\n", data.size());
   

   //PngImage png( filename );
   PngImage png( data, use_palette );

   m_size.x() = png.width;
   m_size.y() = png.height;

   m_data = png.data();

   printf("Width: %d, Height: %d, Data Size: %lu\n", m_size.x(), m_size.y(),
         m_data.size());
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void RawImageData::LoadFromMem(const ByteArray& data, unsigned int &format, const bool use_palette)
{
   PngImage png( data.data, use_palette );
   m_size.x() = png.width;
   m_size.y() = png.height;
   m_data = png.data();
   format = png.format;
}

#endif // end of include guard: _RAW_IMAGE_DATA_CXX_ 
