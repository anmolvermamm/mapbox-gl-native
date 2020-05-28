#include "png_image.h"

#include <cstdlib>
#include <cstdio>
#include <png.h>

#include <memory>
#include <iostream> 
#include <fstream> 
#include <iostream>
#include <istream>
#include <cstring>
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
PngImage::PngImage(const std::string& filename, const bool use_palette )
{
   Load( filename, use_palette );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
PngImage::PngImage(const std::vector<char>& data, const bool use_palette)
{
   Load(data, use_palette);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::vector<unsigned char>& PngImage::data()
{
   return m_data;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void PngImage::Load(const std::string& filename, const bool use_palette)
{

   //int width, height;
   png_byte color_type;
   png_byte bit_depth;
   png_byte num_channels;
   //png_bytep *row_pointers;

   FILE *fp = std::fopen(filename.c_str(), "rb");

   png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if(!png) 
   {
      png_destroy_read_struct(&png, NULL, NULL);
      return;
   }

   png_infop info = png_create_info_struct(png);
   if(!info) 
   {
      png_destroy_read_struct(&png, &info, NULL);
      return;
   }

   if(setjmp(png_jmpbuf(png))) 
   {
      png_destroy_read_struct(&png, &info, NULL);
      return;
   }

   png_init_io(png, fp);

   //Instead of from a file, we can use a data source
   //png_set_read_fn(png_ptr, src, png_read_data);

   png_read_info(png, info);

   width      = png_get_image_width(png, info);
   height     = png_get_image_height(png, info);
   color_type = png_get_color_type(png, info);
   bit_depth  = png_get_bit_depth(png, info);
   

   // Read any color_type into 8bit depth, RGBA format.
   // See http://www.libpng.org/pub/png/libpng-manual.txt

   if(bit_depth == 16)
     png_set_strip_16(png);

   if(color_type == PNG_COLOR_TYPE_PALETTE)
     png_set_palette_to_rgb(png);

   // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
   if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
     png_set_expand_gray_1_2_4_to_8(png);

   if(png_get_valid(png, info, PNG_INFO_tRNS))
     png_set_tRNS_to_alpha(png);

   // These color_type don't have an alpha channel then fill it with 0xff.
   if(color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_PALETTE)
     png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

   if(color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
     png_set_gray_to_rgb(png);

   png_read_update_info(png, info);
   num_channels = png_get_channels(png, info);

   std::vector<png_bytep> row_pointers_vec;
   row_pointers_vec.resize(height);
   png_bytep *row_pointers = row_pointers_vec.data();

   m_data.resize( width * height * num_channels );
   
   //row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
   for(int y = 0; y < height; y++) {
     //row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
     row_pointers[y] = m_data.data() + (( height - y - 1 )* width * 4 );
     //row_pointers[y] = m_data.data() + (( y )* width * 4 );
     
   }

   png_read_image(png, row_pointers);
   png_destroy_read_struct(&png, &info, NULL);

   fclose(fp);
   png=NULL;
   info=NULL;

   //for(int y = 0; y < height; y++) {
   //   png_bytep row = row_pointers[y];
   //   for(int x = 0; x < width; x++) {
   //      png_bytep px = &(row[x * 4]);

   //      int index = (y * width * 4 ) + (x*4);
   //      m_data[ index + 0 ] = px[0];
   //      m_data[ index + 1 ] = px[1];
   //      m_data[ index + 2 ] = px[2];
   //      m_data[ index + 3 ] = px[3];
   //   }
   //}


}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
static void png_read_data(png_structp ctx, png_bytep dest, png_size_t size)
{
   auto ptr = (const char**)png_get_io_ptr(ctx);
   auto src = (const char*)(*ptr);

   if( src == nullptr )
   {
      printf("Error loading png from memory");
      return;
   }

   std::memcpy( dest , src , size );
   (*ptr) +=size;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void PngImage::Load(const std::vector<char>& data, const bool use_palette)
{
   png_byte color_type;
   png_byte bit_depth;
   png_byte num_channels;

   png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if(!png) 
   {
      png_destroy_read_struct(&png, NULL, NULL);
      return;
   }

   png_infop info = png_create_info_struct(png);
   if(!info) 
   {
      png_destroy_read_struct(&png, &info, NULL);
      return;
   }

   if(setjmp(png_jmpbuf(png)))
   {
      png_destroy_read_struct(&png, &info, NULL);
      return;
   }

   void* src = (void*)data.data();

   ////Instead of from a file, we can use a data source
   png_set_read_fn(png, &src, png_read_data);
   png_read_info(png, info);

   width      = png_get_image_width(png, info);
   height     = png_get_image_height(png, info);
   color_type = png_get_color_type(png, info);
   bit_depth  = png_get_bit_depth(png, info);

   // if 8 bit and using 8 bit palette within shaders need to set format to GL_RED
   // 1 for GL_RED and 0 for GL_RGBA
   if(color_type == PNG_COLOR_TYPE_PALETTE && use_palette)
   {
      format = 1;
   }

   if(bit_depth == 16)
     png_set_strip_16(png);

   // Only expand PNG image if we need to add color palette without shaders
   if(!use_palette)
   {
      if(color_type == PNG_COLOR_TYPE_PALETTE)
         png_set_palette_to_rgb(png);
   }

   // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
   if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
     png_set_expand_gray_1_2_4_to_8(png);

   // Only expand PNG image if we need to add color palette without shaders
   if(!use_palette)
   {
      if(png_get_valid(png, info, PNG_INFO_tRNS))
         png_set_tRNS_to_alpha(png);
   }

   // These color_type don't have an alpha channel then fill it with 0xff.
   // Only expand PNG image if we need to add color palette without shaders
   if(!use_palette)
   {
      if(color_type == PNG_COLOR_TYPE_RGB ||
         color_type == PNG_COLOR_TYPE_GRAY ||
         color_type == PNG_COLOR_TYPE_PALETTE)
            png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
   }


   if(color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
     png_set_gray_to_rgb(png);

   png_read_update_info(png, info);
   num_channels = png_get_channels(png, info);

   std::vector<png_bytep> row_pointers_vec;
   row_pointers_vec.resize(height);
   png_bytep *row_pointers = row_pointers_vec.data();

   m_data.resize( width * height * num_channels );
   for(int y = 0; y < height; y++) {
     row_pointers[y] = m_data.data() + (( height - y - 1 )* width * num_channels );
   }

   png_read_image(png, row_pointers);
   png_destroy_read_struct(&png, &info, NULL);

   png=NULL;
   info=NULL;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned char& PngImage::GetValue( int x , int y  , int channel )
{
   return m_data[ (y * width * 4 ) + (x*4) + channel ]; 
}
