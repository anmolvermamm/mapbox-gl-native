#ifndef _PNG_IMAGE_H_
#define _PNG_IMAGE_H_

#include <vector>
#include <string>
#include <png.h>


class PngImage
{
public:
   PngImage(const std::string& filename, const bool use_palette = false );
   PngImage(const std::vector<char>& data, const bool use_palette = false);

   std::vector<unsigned char>& data();

   //Should be inline
   unsigned char& GetValue( int x , int y , int channel );



private:
   void Load(const std::string& filename, const bool use_palette = false );
   void Load(const std::vector<char>& data, const bool use_palette = false);

   std::vector<unsigned char> m_data; 

public:
   int width = 0;
   int height = 0;
   unsigned int format = 0;
};


#endif // end of include guard: _PNG_IMAGE_H_ 
