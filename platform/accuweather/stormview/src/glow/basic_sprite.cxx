#include "basic_sprite.h"

#include "glow.h"
#include "glow_shader_mem.h"
#include "gl_debug.h"
#include "gl_include.h"

#include "glm/mat4x4.hpp"
#include "pointf.h"
#include "color.h"
#include "../config.h"

#include <iostream>
#include <string>
#include <cmath>

extern bool DEBUG_GL_LOAD;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void BasicSprite::DeleteBasicSprite()
{
   auto& prog = Get();
   Glow::DeleteBuffer(prog.m_rect_buffer);
   glDeleteProgram(prog.m_program);
   prog.m_init = false;
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool BasicSprite::InitBasicSprite()
{
   auto& prog = Get();
   if(prog.m_init)
      return prog.m_init;

   GL_DEBUG_MACRO();

   bool success = Glow::compileProgramFromShaderMem(prog.m_program,
         GlowShaderMem::m_basic_sprite_vert,
         GlowShaderMem::m_basic_sprite_frag);

   if(!success)
   {
      std::cout << "Failed to basic_sprite_load" << "\n";
      std::cout << "Error " << Glow::GetError(prog.m_program) << "\n";
      prog.m_init = false;
   }

   GL_DEBUG_MACRO();

   prog.m_rect_buffer = Glow::CreateBuffer();
   GLfloat max_box[35] = {
      (-1.0 * M_PI), (-1.0 * M_PI / 2.0), 0.1f, 1.0f, 0.0f, 0.0f, 0.8f,
      (-1.0 * M_PI), (M_PI / 2.0), 0.1f, 1.0f, 0.0f, 0.0f, 0.8f,
      M_PI, (M_PI / 2.0), 0.1f, 1.0f, 0.0f, 0.0f, 0.8f,
      M_PI, (-1.0 * M_PI / 2.0), 0.1f, 1.0f, 0.0f, 0.0f, 0.8f,
      (-1.0 * M_PI), (-1.0 * M_PI / 2.0), 0.1f, 1.0f, 0.0f, 0.0f, 0.8f
   };
   glBindBuffer(GL_ARRAY_BUFFER, prog.m_rect_buffer);
   glBufferData(GL_ARRAY_BUFFER, 35 * sizeof(float), max_box, GL_STATIC_DRAW);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   prog.m_rect_aloc = Glow::GetAttributeLocation(prog.m_program, "a_rect");

   prog.m_vert_aloc = Glow::GetAttributeLocation(prog.m_program, "a_position");

   prog.m_color_uloc = Glow::GetUniformLocation(prog.m_program, "u_fill_color");
   prog.m_mat_uloc = Glow::GetUniformLocation(prog.m_program, "u_matrix");
   prog.m_translate_uloc = Glow::GetUniformLocation(prog.m_program, "u_translate");

   GL_DEBUG_MACRO();

   if(DEBUG_GL_LOAD)
   {
      printf("BasicSprite\n");
      printf("program: %d, vert_aloc: %d, color_uloc: %d\n",
            prog.m_program, prog.m_vert_aloc, prog.m_color_uloc);
   }

   GL_DEBUG_MACRO();

   prog.m_init = success;

   return prog.m_init;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool BasicSprite::Draw(const glm::mat4x4 &mat, unsigned buffer, unsigned element_buffer,
      unsigned count, PrimitiveStyle style,
      std::unique_ptr<AnimationManager> &animation_manager)
{
   auto& prog = BasicSprite::Get();
   if(!prog.m_init)
      return false;

   if(style.fill_opacity > 0.0)
   {
      glUseProgram(prog.m_program);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      //glEnable(GL_DEPTH_TEST);
      //glDepthFunc(GL_GREATER);
      //glClear( GL_DEPTH_BUFFER_BIT);
      //glClearDepthf( 0.0f );

      glEnable(GL_STENCIL_TEST);
      glClear(GL_STENCIL_BUFFER_BIT);

      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glStencilFunc(GL_ALWAYS, 0, 1);
      glStencilOp(GL_KEEP, GL_INVERT, GL_INVERT);
      glStencilMask(1);

      glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

      glUniformMatrix4fv(prog.m_mat_uloc, 1, GL_FALSE, &mat[0][0]);

      glEnableVertexAttribArray(prog.m_rect_aloc);
      glVertexAttribPointer(prog.m_rect_aloc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

      if(animation_manager != nullptr)
      {
         glUniform4f(prog.m_color_uloc,
               style.color.red(), style.color.green(), style.color.blue(),
               animation_manager->GetCurrentFillOpacity());
      }
      else
      {
         glUniform4f(prog.m_color_uloc,
               style.color.red(), style.color.green(), style.color.blue(),
               style.fill_opacity);
      }

      glUniform1f(prog.m_translate_uloc, 0.0);
      glDrawElements(GL_TRIANGLE_FAN, count, GL_UNSIGNED_INT, (void*)0);

      PointF geo_point(GeoPointToMercatorPoint(PointF(360.0, 0.0)));
      if(Config::IsOverIDL())
      {
         glUniform1f(prog.m_translate_uloc, geo_point.x());
         glDrawElements(GL_TRIANGLE_FAN, count, GL_UNSIGNED_INT, (void*)0);
      }

      glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
      glDisableVertexAttribArray(prog.m_rect_aloc);

      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glStencilFunc(GL_EQUAL, 1, 1);
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

      glBindBuffer(GL_ARRAY_BUFFER, prog.m_rect_buffer);
      glEnableVertexAttribArray(prog.m_rect_aloc);
      glVertexAttribPointer(prog.m_rect_aloc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);

      glDrawArrays(GL_TRIANGLE_FAN, 0, 5);

      if(Config::IsOverIDL())
      {
         glUniform1f(prog.m_translate_uloc, geo_point.x());
         glDrawArrays(GL_TRIANGLE_FAN, 0, 5);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisable(GL_STENCIL_TEST);
      //glDisable(GL_DEPTH_TEST);
      glDisableVertexAttribArray(prog.m_rect_aloc);
      glDisable(GL_BLEND);
      glUseProgram(0);
   }

   return true;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
BasicSprite& BasicSprite::Get()
{
   static BasicSprite the_instance;
   return the_instance;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
BasicSprite::BasicSprite()
{

}
