#include "glow_lines.h"

#include "glow.h"
#include "glow_shader_mem.h"
#include "gl_debug.h"
#include "gl_include.h"


#include "glm/mat4x4.hpp"
#include "pointf.h"
//#include "linef.h"
#include "polygonf.h"
#include "color.h"
#include "../config.h"

#include <iostream>
#include <string>

extern bool DEBUG_GL_LOAD;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GlowLines::DeleteGlowLines()
{
   auto& me = Get();
   Glow::DeleteBuffer(me.m_vertex_buffer);
   glDeleteProgram(me.m_program);
   me.m_init = false;
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool GlowLines::InitGlowLines()
{
   auto& me = Get();
   //We have been initialized already
   if( me.m_init ) return me.m_init;
 
   GL_DEBUG_MACRO();

   bool success  = Glow::compileProgramFromShaderMem(
         me.m_program,
         GlowShaderMem::m_line_2d_vert, 
         GlowShaderMem::m_line_2d_frag  );

   if( !success )
   {
      printf("Failed to glow_line_load\n");
      printf("Error %s\n", Glow::GetError(me.m_program).c_str());
      me.m_init = false;
   }

   GL_DEBUG_MACRO();

   
   me.m_vertex_buffer = Glow::CreateBuffer();
   GL_DEBUG_MACRO();

   me.m_vert_aloc = Glow::GetAttributeLocation( me.m_program , "a_position" );

   me.m_mat_uloc = Glow::GetUniformLocation( me.m_program , "u_matrix" );
   me.m_line_color_uloc = Glow::GetUniformLocation( me.m_program , "u_line_color");
   me.m_translate_uloc = Glow::GetUniformLocation(me.m_program, "u_translate");

   GL_DEBUG_MACRO();

   if( DEBUG_GL_LOAD )
   {
      printf("GlowLines\n");
      printf("program: %d, vert_buffer: %d, ver_aloc: %d\n",
            me.m_program, me.m_vertex_buffer, me.m_vert_aloc);
      printf("mat_uloc: %d, lcolor_uloc: %d\n",
            me.m_mat_uloc, me.m_line_color_uloc);
   }

   me.m_init = success;

   return me.m_init;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool GlowLines::Draw(
const glm::mat4x4& mat, 
const PolygonF& line
)
{
   static int count = -1;
   ++count;
   count = count % 60;

   auto& me = GlowLines::Get();

   if( !me.m_init ) return false;

   glUseProgram( me.m_program );

   glEnable( GL_BLEND);
   glLineWidth( 2.0f );

   glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA ); 

   glEnableVertexAttribArray(me.m_vert_aloc);
   
   if( !count ) GL_DEBUG_MACRO();

   bool isclosed = line.IsClosed();
   int size = isclosed ? line.size() -1 : line.size();

   GLenum linetype = isclosed ? GL_LINE_LOOP : GL_LINE_STRIP;

   Glow::LoadBuffer( me.m_vertex_buffer , line   );
   if( !count ) GL_DEBUG_MACRO();
   glVertexAttribPointer( me.m_vert_aloc , 2 , GL_FLOAT , GL_FALSE , 2*sizeof(GLfloat) , (void*)0 );

   if( !count ) GL_DEBUG_MACRO();

   glUniformMatrix4fv( me.m_mat_uloc , 1 , GL_FALSE , &mat[0][0] );
   if( !count ) GL_DEBUG_MACRO();

   glUniform1f(me.m_translate_uloc, 0.0);

   float alpha = 0.8;
   Color line_color( 0.0f , 1.0 , 0.0 , alpha );
   glUniform4f( me.m_line_color_uloc , 
         line_color.red() , line_color.green() , line_color.blue() , line_color.alpha() );
   if( !count ) GL_DEBUG_MACRO();


   glDrawArrays( linetype , 0 , size );
   //glDrawArrays( GL_POINTS , 0 , size );

   if( !count ) GL_DEBUG_MACRO();

   glDisableVertexAttribArray(me.m_vert_aloc);

   glDisable(GL_BLEND);

   glUseProgram( 0 );

   //if_EVERY_N( 240 ) }
   if( !count ) GL_DEBUG_MACRO();

   GL_DEBUG_CLEAR();

   return true;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool GlowLines::Draw(
   const glm::mat4x4& mat , 
   GeoType type,
   unsigned buffer ,
   unsigned element_buffer,
   unsigned count ,
   PrimitiveStyle style,
   bool closed, std::unique_ptr<AnimationManager> &animation_manager
)
{

   static int log_count = -1;
   ++log_count;
   log_count = log_count % 60;

   auto& me = GlowLines::Get();

   if( !me.m_init )
      return false;

   PointF geo_point(GeoPointToMercatorPoint(PointF(360.0, 0.0)));
   if(type == GeoType::LINE)
   {
   glUseProgram( me.m_program );

   glEnable( GL_BLEND);
   glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
   glUniformMatrix4fv( me.m_mat_uloc , 1 , GL_FALSE , &mat[0][0] );

   glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
   glEnableVertexAttribArray(me.m_vert_aloc);
   GLenum linetype = closed ? GL_LINE_LOOP : GL_LINE_STRIP;

   glLineWidth(8.0);
   glEnableVertexAttribArray(me.m_vert_aloc);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
   glVertexAttribPointer(me.m_vert_aloc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
         (void*)0 );

   if(style.border && animation_manager != nullptr)
   {
      glUniform4f(me.m_line_color_uloc, style.border_color.red(), style.border_color.green(),
            style.border_color.blue(), animation_manager->GetCurrentOpacity());
      glUniform1f(me.m_translate_uloc, 0.0);
      glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);

      if(Config::IsOverIDL())
      {
         glUniform1f(me.m_translate_uloc, geo_point.x());
         glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);
      }
   }
   else if(style.border)
   {
      glUniform4f(me.m_line_color_uloc, style.border_color.red(), style.border_color.green(),
            style.border_color.blue(), style.opacity);
      glUniform1f(me.m_translate_uloc, 0.0);
      glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);

      if(Config::IsOverIDL())
      {
         glUniform1f(me.m_translate_uloc, geo_point.x());
         glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);
      }
   }
   else
   {
      // Do nothing
   }

   glLineWidth(5.0);
   if(animation_manager != nullptr)
   {
      glUniform4f(me.m_line_color_uloc, style.color.red(), style.color.green(),
            style.color.blue(), animation_manager->GetCurrentOpacity());
   }
   else
   {
      glUniform4f(me.m_line_color_uloc, style.color.red(), style.color.green(),
            style.color.blue(), style.opacity);
   }

   glUniform1f(me.m_translate_uloc, 0.0);
   glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);

   if(Config::IsOverIDL())
   {
      glUniform1f(me.m_translate_uloc, geo_point.x());
      glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);
   }


   glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glDisableVertexAttribArray(me.m_vert_aloc);
   glDisable(GL_BLEND);

   glUseProgram( 0 );
   }
   else
   {
      glUseProgram( me.m_program );

      glEnable( GL_BLEND);
      glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
      glUniformMatrix4fv( me.m_mat_uloc , 1 , GL_FALSE , &mat[0][0] );

      glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
      glEnableVertexAttribArray(me.m_vert_aloc);
      GLenum linetype = closed ? GL_LINE_LOOP : GL_LINE_STRIP;

      glLineWidth(style.weight * Config::GetScreenDensity() * 2.0);
      glEnableVertexAttribArray(me.m_vert_aloc);
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
      glVertexAttribPointer(me.m_vert_aloc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
            (void*)0 );

      if(animation_manager != nullptr)
      {
         glUniform4f(me.m_line_color_uloc, style.color.red(), style.color.green(),
               style.color.blue(), animation_manager->GetCurrentOpacity());
      }
      else
      {
         glUniform4f(me.m_line_color_uloc, style.color.red(), style.color.green(),
               style.color.blue(), style.opacity);
      }

      glUniform1f(me.m_translate_uloc, 0.0);
      glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);

      if(Config::IsOverIDL())
      {
         glUniform1f(me.m_translate_uloc, geo_point.x());
         glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);
      }

      if(style.border && animation_manager != nullptr)
      {
         glUniform4f(me.m_line_color_uloc, style.border_color.red(), style.border_color.green(),
               style.border_color.blue(), animation_manager->GetCurrentOpacity());
         glUniform1f(me.m_translate_uloc, 0.0);
         glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);

         if(Config::IsOverIDL())
         {
            glUniform1f(me.m_translate_uloc, geo_point.x());
            glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);
         }
      }
      else if(style.border)
      {
         glUniform4f(me.m_line_color_uloc, style.border_color.red(), style.border_color.green(),
               style.border_color.blue(), style.opacity);
         glUniform1f(me.m_translate_uloc, 0.0);
         glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);

         if(Config::IsOverIDL())
         {
            glUniform1f(me.m_translate_uloc, geo_point.x());
            glDrawElements(linetype, count, GL_UNSIGNED_INT, (void*)0);
         }
      }
      else
      {
         // Do nothing
      }


      glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableVertexAttribArray(me.m_vert_aloc);
      glDisable(GL_BLEND);

      glUseProgram( 0 );
   }

   if( !log_count ) GL_DEBUG_MACRO();

   GL_DEBUG_CLEAR();

   return true;
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GlowLines& GlowLines::Get()
{
   static GlowLines the_instance;
   return the_instance;
}
