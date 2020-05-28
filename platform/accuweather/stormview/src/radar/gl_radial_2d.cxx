#include "gl_radial_2d.h"
#include "gl_debug.h"
#include "../glow/gl_include.h"
#include "../logging.h"
#include "../projections.h"

#include <iostream>

#include "radial_buffer.h"
#include "../glow/glow_shader_mem.h"

#include "../glow/glow.h"
#include <glm/mat4x4.hpp>
#include "../config.h"

#include "radar_palette.h"


extern bool DEBUG_GL_LOAD;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GLRadial2D::DeleteRadial2D()
{
   auto& me = Get();
   Glow::DeleteBuffer(me.m_elem_buffer);
   Glow::DeleteTexture(me.m_palette_nws_precip_tex);
   Glow::DeleteTexture(me.m_palette_accu_precip_tex);
   Glow::DeleteTexture(me.m_palette_smir_tex);
   Glow::DeleteTexture(me.m_palette_ice_tex);
   Glow::DeleteTexture(me.m_palette_mixed_tex);
   Glow::DeleteTexture(me.m_palette_snow_tex);
   glDeleteProgram(me.m_program);
   me.m_init = false;
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GLRadial2D::GLRadial2D()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GLRadial2D& GLRadial2D::Get()
{
   static GLRadial2D the_instance;
   return the_instance;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool GLRadial2D::GLInitRadial2D()
{
   GL_DEBUG_MACRO();
   auto& me = GLRadial2D::Get();

   if(me.m_init)
   {
      return true;
   }

   bool success = Glow::compileProgramFromShaderMem( 
         me.m_program,
         GlowShaderMem::m_radial_vert , 
         GlowShaderMem::m_radial_frag );

   if( !success )
   {
      printf("Program failed gl_radial_2d_load\n");
      printf("Error %s\n", Glow::GetError(me.m_program).c_str());
      me.m_init = false;
   }


//Uniform
   me.m_u_mat = Glow::GetUniformLocation( me.m_program, "u_matrix");
   me.m_u_loaded = Glow::GetUniformLocation( me.m_program, "u_loaded");
   me.m_u_data = Glow::GetUniformLocation( me.m_program, "u_data");
   me.m_u_alpha = Glow::GetUniformLocation( me.m_program, "u_alpha");
   me.m_u_center = Glow::GetUniformLocation( me.m_program, "u_center");
   me.m_u_radius = Glow::GetUniformLocation( me.m_program, "u_radius");
   me.m_u_is_smir = Glow::GetUniformLocation(me.m_program, "u_is_smir");
   me.m_u_palette = Glow::GetUniformLocation( me.m_program, "u_palette");
   me.m_u_translate = Glow::GetUniformLocation(me.m_program, "u_translate");
   me.m_u_is_8bit = Glow::GetUniformLocation(me.m_program, "u_is_8bit");
   me.m_u_palette_mixed = Glow::GetUniformLocation(me.m_program, "u_palette_mixed");
   me.m_u_palette_ice = Glow::GetUniformLocation(me.m_program, "u_palette_ice");
   me.m_u_palette_snow = Glow::GetUniformLocation(me.m_program, "u_palette_snow");
   
//Attrib
   me.m_a_texcoord= Glow::GetAttributeLocation( me.m_program,  "a_texcoord");
   me.m_a_vert= Glow::GetAttributeLocation( me.m_program, "a_vertex");

   auto rect_elems = std::vector<unsigned int>{
      0 , 1 , 2 ,
      2 , 3 , 0
   };
   
//Texture
   me.m_palette_smir_tex = Glow::Load2DImageToTextureFromRawData(
         palette_data_smir64x4.data(), palette_width, palette_height);
   me.m_palette_nws_precip_tex = Glow::Load2DImageToTextureFromRawData(
         palette_nws_precip.data(), palette_width, palette_height);
   me.m_palette_accu_precip_tex = Glow::Load2DImageToTextureFromRawData(
         palette_accu_precip.data(), palette_width, palette_height);
   me.m_u_palette_lv2_velocity = Glow::Load2DImageToTextureFromRawData(
         palette_data_lv2_velocity.data(), palette_width, palette_height);

   me.m_palette_mixed_tex = Glow::Load2DImageToTextureFromRawData(
         palette_data_mixed.data(), palette_width, palette_height);
   me.m_palette_ice_tex = Glow::Load2DImageToTextureFromRawData(
         palette_data_ice.data(), palette_width, palette_height);
   me.m_palette_snow_tex = Glow::Load2DImageToTextureFromRawData(
         palette_data_snow.data(), palette_width, palette_height);

   me.m_elem_buffer = Glow::CreateElemBuffer(rect_elems);

   if( DEBUG_GL_LOAD )
   {
      printf("GLRadial2D\n");
      printf("Program: %d, PTex: %d, UMatLoc: %d, ULoaded: %d\n",
            me.m_program, me.m_palette_nws_precip_tex, me.m_u_mat, me.m_u_loaded);
      printf("Udata: %d, UAlpha: %d, UCenterLoc: %d, URadiusLoc: %d\n",
            me.m_u_data, me.m_u_alpha, me.m_u_center, me.m_u_radius);
      printf("Upalette: %d, ATexLoc: %d, AVertLoc: %d\n",
            me.m_u_palette, me.m_a_texcoord, me.m_a_vert);
      printf("palette_smir_tex: %d, palette_ice: %d, palette_snow: %d\n",
             me.m_palette_smir_tex);
   }

   me.m_init = success;
   GL_DEBUG_MACRO();
   return success;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GLRadial2D::DrawGL(
const glm::mat4x4& mat,
const RadialBuffer& radial_buffer,
float radius,
PointF& center,
float alpha
)
{
   if(!GLRadial2D::Get().m_init)
   {
      GLRadial2D::GLInitRadial2D();
   }

   GL_DEBUG_MACRO();
   if( alpha > 1.0 ) alpha = 1.0;
   if( alpha < 0.0 ) alpha = 0.0;

   auto& me = GLRadial2D::Get();
   glUseProgram( me.m_program );

   glEnableVertexAttribArray( me.m_a_vert );
   glEnableVertexAttribArray( me.m_a_texcoord );

   auto& buffer = radial_buffer.buffer;
   auto& loaded = radial_buffer.loaded;
   auto& texture = radial_buffer.texture;
   auto& is_8bit = radial_buffer.is_8bit;

   glBindBuffer( GL_ARRAY_BUFFER,  buffer );
   glVertexAttribPointer(me.m_a_vert, 2, GL_FLOAT, false,sizeof(GLfloat)*4,0);
   glVertexAttribPointer(me.m_a_texcoord, 2, GL_FLOAT, false, sizeof(GL_FLOAT)*4, (void*)(sizeof(GL_FLOAT)*2));
   
   glUniform1i( me.m_u_is_8bit, (int) is_8bit);
   glUniform1i( me.m_u_loaded , (int) loaded );
   glUniform1i(me.m_u_is_smir, (int) Config::GetSMIRActive());
   
   GL_DEBUG_MACRO();
   if( loaded )
   {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D , texture);
      glUniform1i(me.m_u_data , 0 );
   }
   
   glActiveTexture(GL_TEXTURE1);
   if(Config::GetPaletteProduct() == Config::PaletteProduct::LV2_REFLECTIVITY &&
         Config::GetSMIRActive() && is_8bit)
      glBindTexture(GL_TEXTURE_2D, me.m_palette_smir_tex );
   else if(Config::GetPaletteProduct() == Config::PaletteProduct::LV2_REFLECTIVITY &&
         Config::GetSMIRActive() && !is_8bit)
      glBindTexture(GL_TEXTURE_2D, me.m_palette_accu_precip_tex );
   else if(Config::GetPaletteProduct() == Config::PaletteProduct::LV2_REFLECTIVITY &&
         Config::GetPaletteType() == Config::PaletteType::ACCU_PALETTE && !Config::GetSMIRActive())
      glBindTexture(GL_TEXTURE_2D, me.m_palette_accu_precip_tex );
   else if(Config::GetPaletteProduct() == Config::PaletteProduct::LV2_REFLECTIVITY &&
         Config::GetPaletteType() == Config::PaletteType::NWS_PALETTE && !Config::GetSMIRActive())
      glBindTexture(GL_TEXTURE_2D, me.m_palette_nws_precip_tex);
   else if(Config::GetPaletteProduct() == Config::PaletteProduct::LV2_VELOCITY)
      glBindTexture(GL_TEXTURE_2D, me.m_u_palette_lv2_velocity);
   else
      glBindTexture(GL_TEXTURE_2D, me.m_palette_smir_tex);
   glUniform1i(me.m_u_palette , 1);

   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_2D, me.m_palette_mixed_tex);
   glUniform1i(me.m_u_palette_mixed, 2);

   glActiveTexture(GL_TEXTURE3);
   glBindTexture(GL_TEXTURE_2D, me.m_palette_ice_tex);
   glUniform1i(me.m_u_palette_ice, 3);

   glActiveTexture(GL_TEXTURE4);
   glBindTexture(GL_TEXTURE_2D, me.m_palette_snow_tex);
   glUniform1i(me.m_u_palette_snow, 4);
   glActiveTexture(GL_TEXTURE0);
   
   glUniform1f( me.m_u_radius , radius );
   glUniform1f( me.m_u_alpha , alpha );
   glUniform2f( me.m_u_center , center.x() , center.y() );
   glUniformMatrix4fv(me.m_u_mat, 1, GL_FALSE, &mat[0][0]);
   
   GL_DEBUG_MACRO();
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER ,  me.m_elem_buffer );
   glUniform1f(me.m_u_translate, 0.0);
   glDrawElements( GL_TRIANGLES , 6 , GL_UNSIGNED_INT , 0 );

   if(Config::IsOverIDL())
   {
      PointF geo_point(GeoPointToMercatorPoint(PointF(360.0, 0.0)));
      glUniform1f(me.m_u_translate, geo_point.x());
      glDrawElements( GL_TRIANGLES , 6 , GL_UNSIGNED_INT , 0 );
   }
   
   glDisableVertexAttribArray(me.m_a_vert);
   glDisableVertexAttribArray(me.m_a_texcoord);

   GL_DEBUG_MACRO();
}
