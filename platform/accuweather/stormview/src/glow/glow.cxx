#include "glow.h"

#include "gl_include.h"
#include "rectf.h"
#include "pointf.h"
#include "linef.h"
#include "polygonf.h"
#include <vector>
#include <iostream>
#include "raw_image_data.h"
#include "byte_array.h"


bool DEBUG_GL_LOAD = false;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool Glow::compileProgramFromShaderMem( 
unsigned int& program_out,
const std::string& vertex_mem,
const std::string& fragment_mem )
{
 
   //bool vert_success, frag_success, compile_success;
   GLuint return_program, vert_program, frag_program;
   bool vert_success = compileShaderFromMem( vert_program , vertex_mem , GL_VERTEX_SHADER  );
   bool frag_success = compileShaderFromMem( frag_program , fragment_mem , GL_FRAGMENT_SHADER ); 

   bool compile_success = compileProgramFromShaders( return_program , vert_program ,  frag_program);

   bool success = vert_success & frag_success & compile_success;
   program_out = return_program;
   return success;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool Glow::compileProgramFromShaders( 
unsigned int& program_out,
unsigned int vertex_id ,
unsigned int fragment_id ,
bool delete_shaders )
{
	int Result = GL_FALSE;

	// Link the program
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vertex_id);
	glAttachShader(ProgramID, fragment_id);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
   bool success = Result;

   if(delete_shaders) {
      glDeleteShader(vertex_id);
      glDeleteShader(fragment_id);
   }

   program_out = ProgramID;
   return success;
 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
std::string Glow::GetError( unsigned int program_id )
{
	int InfoLogLength;
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
   std::vector<char> ProgramErrorMessage(InfoLogLength+1);
   glGetProgramInfoLog(program_id, InfoLogLength, NULL, &ProgramErrorMessage[0]);
   std::string ret;
   ret.append( ProgramErrorMessage.begin() , ProgramErrorMessage.end());
   return ret;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool Glow::compileShaderFromMem( 
unsigned int& shader_out,
const std::string& shader_mem,
unsigned int shader_type )
{

   GLuint shaderID = glCreateShader(shader_type);
   shader_out = shaderID;
   if( shader_mem.empty() ) return false;

   char const * SourcePointer = shader_mem.c_str();
   glShaderSource(shaderID, 1, &SourcePointer , NULL);
      glCompileShader(shaderID);

	//int Result = GL_FALSE;
	//int InfoLogLength;
	//glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	//glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
   return true;

 

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int Glow::GetUniformLocation(unsigned int program , const std::string& name )
{
   return glGetUniformLocation(program, name.c_str());
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int Glow::GetAttributeLocation(unsigned int program , const std::string& name )
{
   return glGetAttribLocation(program, name.c_str());
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int Glow::CreateBuffer( )
{
   GLuint buffer;
   glGenBuffers(1, &buffer); 
   return buffer;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int Glow::CreateBuffer( const RectF& rectf, float z, BUFFER_TYPE type  )
{
   auto verts = std::vector<float>{   
         rectf.left()   , rectf.top()      , z ,
         rectf.left()   , rectf.bottom()   , z ,
         rectf.right()  , rectf.bottom()   , z ,
         rectf.right()  , rectf.top()      , z  
   };
   return CreateBuffer( verts , type );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int Glow::CreateBuffer( const RectF& rectf,  BUFFER_TYPE type )
{
   auto verts = std::vector<float>{   
         rectf.left()   , rectf.top()     ,
         rectf.left()   , rectf.bottom()  ,
         rectf.right()  , rectf.bottom()  ,
         rectf.right()  , rectf.top()      
   };
   return CreateBuffer( verts , type );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int Glow::CreateBuffer( const std::vector<float>& list, BUFFER_TYPE type )
{
   GLuint buffer;
   glGenBuffers(1, &buffer); 

   int calc = list.size()*sizeof(float);
   auto gl_type = type == STATIC ? GL_STATIC_DRAW : GL_STREAM_DRAW;

   glBindBuffer(GL_ARRAY_BUFFER , buffer );
   glBufferData(GL_ARRAY_BUFFER , calc ,
         &list[0] , gl_type );
   glBindBuffer(GL_ARRAY_BUFFER , 0 );

   return buffer;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int Glow::CreateBuffer( const RectF& rect,  const RectF& tex_coord )
{
   auto verts = std::vector<float>{   
         rect.left()  , rect.top()   , tex_coord.left() , tex_coord.top() ,
         rect.left()  , rect.bottom()   , tex_coord.left() , tex_coord.bottom() ,
         rect.right()  , rect.bottom()   , tex_coord.right() , tex_coord.bottom() ,
         rect.right()  , rect.top()   , tex_coord.right() , tex_coord.top()  
   };
   return CreateBuffer( verts  );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int Glow::CreateBuffer( const PolygonF& poly, BUFFER_TYPE type  )
{
   auto buffer = CreateBuffer();
   LoadBuffer( buffer , poly , type );
   return buffer;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Glow::LoadBuffer( unsigned int buffer, const PointF& point, BUFFER_TYPE type  )
{
   auto verts = std::vector<float>{   
         point.x()   , point.y() 
   };
   return LoadBuffer( buffer,  verts , type );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Glow::LoadBuffer( unsigned int buffer, const LineF& line,  BUFFER_TYPE type  )
{
  //this is inefficient
  //auto verts = std::vector<float>{}; 
  //verts.reserve( line.size() * 2 );
  // 
  //auto end = !line.empty() && line.IsClosed() ?
  //   (line.end()-1 ) : line.end();

  //for( auto i = line.begin() ; i != end ; ++i )
  //{
  //   verts.push_back( i->x() );
  //   verts.push_back( i->y() );
  //}
  //LoadBuffer( buffer , verts , type );
  
  //this works as long as we do not add memembers to pointf
  auto size = !line.empty() && line.IsClosed() ?
     (line.size()-1 ) : line.size();

   int calc = size*sizeof(float)*3;
   auto gl_type = type == STATIC ? GL_STATIC_DRAW : GL_STREAM_DRAW;

   glBindBuffer(GL_ARRAY_BUFFER , buffer );
   glBufferData(GL_ARRAY_BUFFER , calc ,
         &line[0] , gl_type );
   return;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Glow::LoadBuffer( unsigned int buffer, const PolygonF& poly, BUFFER_TYPE type  )
{

  auto size = !poly.empty() && poly.IsClosed() ?
     (poly.size()-1 ) : poly.size();

   int calc = size*sizeof(float)*3;
   auto gl_type = type == STATIC ? GL_STATIC_DRAW : GL_STREAM_DRAW;

   glBindBuffer(GL_ARRAY_BUFFER , buffer );
   glBufferData(GL_ARRAY_BUFFER , calc ,
         &poly[0] , gl_type );
   return;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Glow::LoadBuffer(unsigned int buffer,  const RectF& rectf, float z, BUFFER_TYPE type  )
{
   auto verts = std::vector<float>{   
         rectf.left()   , rectf.top()      , z ,
         rectf.left()   , rectf.bottom()   , z ,
         rectf.right()  , rectf.bottom()   , z ,
         rectf.right()  , rectf.top()      , z  
   };
   return LoadBuffer( buffer,  verts , type );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Glow::LoadBuffer( unsigned int buffer, const std::vector<float>& list, BUFFER_TYPE type )
{
   int calc = list.size()*sizeof(float);
   auto gl_type = type == STATIC ? GL_STATIC_DRAW : GL_STREAM_DRAW;

   glBindBuffer(GL_ARRAY_BUFFER , buffer );
   glBufferData(GL_ARRAY_BUFFER , calc ,
         &list[0] , gl_type );
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned int Glow::CreateElemBuffer( const std::vector<unsigned int>& list, BUFFER_TYPE type )
{
   auto gl_type = type == STATIC ? GL_STATIC_DRAW : GL_STREAM_DRAW;
   int calc = list.size()*sizeof(unsigned int);
   GLuint element_buffer;
   glGenBuffers(1, &element_buffer); 
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer );
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, calc , 
         &list[0] , gl_type );
   return element_buffer;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Glow::DeleteBuffer( unsigned int& buffer )
{
   GLuint buf = buffer;
   glDeleteBuffers( 1 , &buf );
   buffer = 0;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void Glow::DeleteTexture(unsigned int& texture )
{
   GLuint tex = texture;
   glDeleteTextures( 1 , &tex );
   texture = 0;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned Glow::Load2DImageToTextureFromRawData( const void* pixels , int width ,
      int height,  FILTER_TYPE filter, INTERNAL_FORMAT format )
{
   GLuint tex_ref;
   glGenTextures( 1 , &tex_ref);
   if( !pixels || !width || !height )
   {
      std::cerr << "Trying to load null size/data texture" << "\n";
      return tex_ref;
   }

   glBindTexture(GL_TEXTURE_2D, tex_ref);

   auto gl_filter = filter == Glow::NEAREST ? GL_NEAREST : GL_LINEAR;

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   auto gl_format = format == Glow::RGBA ? GL_RGBA : GL_RED;

   glTexImage2D(GL_TEXTURE_2D, 0, gl_format, width,
         height, 0, gl_format, GL_UNSIGNED_BYTE,
         pixels);

   glBindTexture( GL_TEXTURE_2D  , 0 );

   return tex_ref;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
unsigned Glow::Load2DImageToTextureFromMem(const ByteArray& data , bool &is_8_bit,
      FILTER_TYPE type, bool use_palette)
{

   RawImageData image;
   unsigned int format;
   image.LoadFromMem( data, format, use_palette );

   // Need to determine format for gl texture either GL_RGBA or GL_RED
   is_8_bit = INTERNAL_FORMAT(format) == Glow::RGBA ? false : true;
   return Glow::Load2DImageToTextureFromRawData( 
         image.m_data.data(),
         image.m_size.x(), 
         image.m_size.y(),
         type, INTERNAL_FORMAT(format) );
}
