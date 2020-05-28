#ifndef _GLOW_H_
#define _GLOW_H_

#include <string>
#include <vector>
class RectF;
class PointF;
class ByteArray;
class LineF;
class PolygonF;

class Glow
{
public:

   enum BUFFER_TYPE { STATIC , STREAM };
   enum FILTER_TYPE { NEAREST , LINEAR };
   enum INTERNAL_FORMAT { RGBA , RED };

   //Everything in this class will be static

   static bool compileProgramFromShaderMem( 
         unsigned int& program_out,
         const std::string& vertex_mem,
         const std::string& fragment_mem );
   static bool compileProgramFromShaders( 
         unsigned int& program_out,
         unsigned int vertex_id ,
         unsigned int fragment_id ,
         bool delete_shaders = true );
   static bool compileShaderFromMem( 
         unsigned int& shader_out,
         const std::string& shader_mem,
         unsigned int shader_type );
   static std::string GetError( 
         unsigned int program_id );

   static int GetUniformLocation(unsigned int program , const std::string& name );
   static int GetAttributeLocation(unsigned int program , const std::string& name );

   //Array buffers
   static unsigned int CreateBuffer();
   static unsigned int CreateBuffer( const RectF& , float z, BUFFER_TYPE type = STATIC );
   static unsigned int CreateBuffer( const RectF& , BUFFER_TYPE type = STATIC );
   static unsigned int CreateBuffer( const RectF& rect,  const RectF& tex_coord );
   static unsigned int CreateBuffer( const std::vector<float>& list, BUFFER_TYPE type = STATIC  );
   static unsigned int CreateBuffer( const PolygonF& poly, BUFFER_TYPE type = STATIC  );

   static void LoadBuffer( unsigned int buffer, const PointF& , BUFFER_TYPE type = STREAM );
   static void LoadBuffer( unsigned int buffer, const LineF& , BUFFER_TYPE type = STREAM );
   static void LoadBuffer( unsigned int buffer, const PolygonF& , BUFFER_TYPE type = STREAM );
   static void LoadBuffer( unsigned int buffer, const RectF& , float z, BUFFER_TYPE type = STREAM );
   static void LoadBuffer( unsigned int buffer, const std::vector<float>& list, BUFFER_TYPE type = STREAM  );

   //Elem buffers
   static unsigned int CreateElemBuffer( const std::vector<unsigned int>& list, BUFFER_TYPE type = STATIC  );

   //static LoadTextureFromFile(const std::string& file);
   static unsigned Load2DImageToTextureFromRawData( const void* raw_image , int width , int height , FILTER_TYPE filter = NEAREST,
         INTERNAL_FORMAT = RGBA);
   //static unsigned Load2DImageToTextureFromMem(const std::string& data );
   static unsigned Load2DImageToTextureFromMem(const ByteArray& data , bool &is_8_bit, FILTER_TYPE filter = NEAREST,
         bool use_palette = false );

   //Delete buffers
   static void DeleteBuffer( unsigned int& buffer );
   static void DeleteTexture(unsigned int& texture );

   
};


#endif // end of include guard: _GLOW_H_ 
