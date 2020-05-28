#include "glow_shader_mem.h"


std::string GlowShaderMem::m_image_2d_vert =
R"(
   uniform mat4 matrix;
   attribute vec3 vertex_in;
   attribute vec2 texcoord_in;
   varying vec2 texcoord;
   uniform float u_translate;

   void main(void)
   {
      gl_Position = matrix * vec4(vertex_in.x + u_translate, vertex_in.yz,1.0);
      //gl_Position =  vec4(vertex_in,1.0);
      texcoord = texcoord_in;
   }
)";
std::string GlowShaderMem::m_image_2d_frag =
R"(
   precision highp float;
   uniform sampler2D data;
   varying vec2 texcoord;
   uniform int has_data;
   void main()
   {
      if( has_data == 0 ){
         //gl_FragColor = vec4( texcoord.rg , 0.0 , 1.0 );
         discard;
         return;
      }
      vec2 tex_coord = vec2( texcoord.x , texcoord.y );
      vec4 img = texture2D( data , tex_coord );
      gl_FragColor = vec4(img.r , img.g , img.b , 0.5 );
      //gl_FragColor = vec4(1.0 , 0.0  , 0.0 , 1.0 );
   }
)";


std::string GlowShaderMem::m_particle_2d_vert =
R"(
uniform mat4 u_matrix;
attribute vec2 a_vertex;
uniform float u_point_size;

void main() {
   gl_PointSize =  u_point_size;

   vec4 pos = vec4( a_vertex , 0.2 , 1.0 );
   //vec4 pos = vec4( a_vertex , 1.0 );

   gl_Position = u_matrix * pos;
}
)";

std::string GlowShaderMem::m_particle_2d_frag =
R"(
precision highp float;
uniform vec4 u_color;

void main() {
   float radius = 0.5;
   float border = 0.25;
   vec2 m = gl_PointCoord.xy - vec2(0.5, 0.5);
   float dist = radius - sqrt(m.x * m.x + m.y * m.y);

   float alpha = u_color.a;
   if( dist < 0.0  )
   {

      discard;
   }
   if( dist < border )
   {
      alpha = ( dist / border ) * u_color.a;
      //discard;
   }

   gl_FragColor = vec4(u_color.rgb , alpha );
   //gl_FragColor = vec4(1.0 , 1.0 , 1.0 , 1.0  );

}
)";


std::string GlowShaderMem::m_point_2d_vert =
R"(
uniform mat4 u_matrix;
uniform float u_point_size;
uniform float u_translate;

attribute vec3 a_position;

void main() {
    vec4 position = u_matrix * vec4(a_position.x + u_translate, a_position.yz,  1.0 );
    gl_PointSize = u_point_size;
    gl_Position =  position;
}
)";

std::string GlowShaderMem::m_point_2d_frag =
R"(
precision highp float;

uniform vec4 u_line_color;
uniform vec4 u_fill_color;
uniform float u_border_width;
//const float BORDER_SIZE = 0.1;

void main()
{
   bool is_line =
      ( gl_PointCoord.x < (u_border_width / 10.0) ) ||
      ( gl_PointCoord.x > ( 1.0 - (u_border_width / 10.0) ) ) ||
      ( gl_PointCoord.y < (u_border_width / 10.0) ) ||
      ( gl_PointCoord.y > ( 1.0 - (u_border_width / 10.0) ) );

   vec4 color = is_line ?
      u_line_color :
      u_fill_color;

   gl_FragColor = color;
}
)";

std::string GlowShaderMem::m_point_2d_vert_tex =
R"(
uniform mat4 u_matrix;
uniform float u_point_size;
uniform float u_translate;

attribute vec3 a_position;

void main() {
    vec4 position = u_matrix * vec4(a_position.x + u_translate, a_position.yz,  1.0 );
    gl_PointSize = u_point_size;
    gl_Position =  position;
}
)";

std::string GlowShaderMem::m_point_2d_frag_tex =
R"(
precision highp float;

uniform sampler2D u_tex;

void main()
{
   vec2 pos = gl_PointCoord.xy;
   pos.y = 1.0 - pos.y;


   gl_FragColor = texture2D(u_tex, pos);
}
)";


std::string GlowShaderMem::m_basic_sprite_vert =
R"(
uniform mat4 u_matrix;
uniform float u_translate;

attribute vec3 a_rect;

void main() {
    vec4 position = u_matrix * vec4(a_rect.x + u_translate, a_rect.yz, 1.0 );
    gl_Position =  position;
}
)";

std::string GlowShaderMem::m_basic_sprite_frag =
R"(
precision highp float;

uniform vec4 u_fill_color;

void main()
{
   gl_FragColor = u_fill_color;
}
)";


std::string GlowShaderMem::m_line_2d_vert =
R"(
uniform mat4 u_matrix;
uniform float u_translate;
attribute vec3 a_position;

void main() {
    vec4 position = u_matrix * vec4(a_position.x + u_translate, a_position.yz, 1.0 );
    gl_Position =  position;
}
)";

std::string GlowShaderMem::m_line_2d_frag =
R"(
precision highp float;

uniform vec4 u_line_color;

void main()
{
   gl_FragColor = u_line_color;
}
)";



std::string GlowShaderMem::m_pbox_2d_vert =
R"(
uniform mat4 u_matrix;
uniform float u_res;
attribute vec2 a_vertex;
attribute vec2 a_position;
attribute float a_angle;
attribute float a_length;
attribute float a_width;
varying vec2 v_pos_merc;

vec2 rotate( vec2 vec  , float angle_rad );

void main() {


//Scale
//X offset
   float x_offset = -1.0 * a_vertex.x * a_length;
   float y_offset = a_vertex.y * a_width * u_res;

   vec2 vertex = vec2( x_offset , y_offset );

//Rotate
   vertex = rotate( vertex , a_angle );

//Translate
   vertex.x += a_position.x;
   vertex.y += a_position.y;

//Position
   gl_Position = u_matrix * vec4( vertex , 0.7 , 1.0);


   //v_texcoord = a_vertex;
   v_pos_merc = a_vertex.xy;
}

vec2 rotate( vec2 vec, float angle )
{
   vec2 ret;
   ret.x = vec.x * cos( angle ) - vec.y * sin( angle );
   ret.y = vec.y * cos( angle ) + vec.x * sin( angle );
   return ret;
}
)";
std::string GlowShaderMem::m_pbox_2d_frag =
R"(
precision highp float;
uniform vec4 u_color;

void main() {
   gl_FragColor = u_color;
   gl_FragColor.a = u_color.a * u_color.a;
}
)";

std::string GlowShaderMem::m_default_vert =
R"(
   layout(location = 0) in vec3 vertexPosition_modelspace;

   void main() {
     gl_Position.xyz = vertexPosition_modelspace;
     gl_Position.w = 1.0;
   }
)";

std::string GlowShaderMem::m_default_frag =
R"(
   out vec3 color;
   void main() {
     color = vec3(1,0,0);
   }
)";


std::string GlowShaderMem::m_radial_vert =
R"(
uniform mat4 u_matrix;
attribute vec2 a_vertex;
attribute vec2 a_texcoord;
varying vec2 v_texcoord;
varying vec2 v_pos_merc;
uniform float u_translate;

void main() {

   // multiply each vertex by a matrix.
   gl_Position = u_matrix * vec4( a_vertex.x + u_translate, a_vertex.y , 0.0 , 1.0);

   v_texcoord = a_texcoord.xy;
   v_pos_merc = a_vertex.xy;
}
)";


std::string GlowShaderMem::m_radial_frag =
R"(
precision highp float;
varying vec2 v_texcoord;
varying vec2 v_pos_merc;

uniform vec2 u_center;
uniform float u_radius;
uniform float u_alpha;
uniform int u_is_smir;
uniform int u_is_8bit;

uniform sampler2D u_data;
uniform sampler2D u_palette;
uniform sampler2D u_palette_mixed;
uniform sampler2D u_palette_ice;
uniform sampler2D u_palette_snow;
uniform int u_loaded;

const float _R = 6371.0;
const float _180_by_PI =  57.2957795131;
const float _PI_by_2 =  1.57079632679;
const float _PI =  3.14159265359;
const float _2PI =  6.28318530718;

vec2 merc_to_geo( in vec2 merc );
float get_distance( vec2 latlon1 , vec2 latlon2 );
float get_angle( vec2 latlon1 , vec2 latlon2 );

void main() {


   vec2 geo_coord_rad = merc_to_geo( v_pos_merc );
   vec2 center_rad = radians( u_center );

   gl_FragColor = vec4( 0.0 , 0.0 , 0.0 , u_alpha );

   float distance = get_distance( geo_coord_rad , center_rad );
   float angle = get_angle( center_rad , geo_coord_rad );

   float angle_value = angle / 360.0;
   float distance_value = distance / u_radius;

   if( distance_value > 1.0 )
   {
      discard;
   }

   //If Loaded
   if( u_loaded != 0 )
   {
      vec4 data = texture2D( u_data , vec2( angle_value , distance_value ) );
      gl_FragColor = data;

      if(u_is_8bit == 0)
      {
         if(u_is_smir == 0)
         {
            vec4 data = texture2D( u_data , vec2( angle_value , distance_value ) );
            gl_FragColor = data;
            if( data.b < (  2.0 / 255.0 ) )
            {
               discard;
            }
            gl_FragColor.rgb = texture2D( u_palette , vec2( data.b , 1.0 ) ).rgb;
            gl_FragColor.a = u_alpha;
         }
         else
         {
            vec4 data = texture2D( u_data , vec2( angle_value , distance_value ) );
            if( data.b < (  2.0 / 255.0 ) )
            {
               discard;
            }
            if(data.g < (2.0/255.0))
            {
               gl_FragColor.rgb = texture2D( u_palette , vec2( data.b , 1.0 ) ).rgb;
               gl_FragColor.a = u_alpha;
            }
            else if(data.g < (4.0/255.0) && data.g >= (2.0/255.0))
            {
               gl_FragColor.rgb = texture2D( u_palette_ice , vec2( data.b , 1.0 ) ).rgb;
               gl_FragColor.a = u_alpha;
            }
            else if(data.g < (8.0/255.0) && data.g >= (4.0/255.0))
            {
               gl_FragColor.rgb = texture2D( u_palette_mixed , vec2( data.b , 1.0 ) ).rgb;
               gl_FragColor.a = u_alpha;
            }
            else// if(data.b < (8.0/255.0) && data.b >= (4.0/255.0))
            {
               gl_FragColor.rgb = texture2D( u_palette_snow , vec2( data.b , 1.0 ) ).rgb;
               gl_FragColor.a = u_alpha;
            }
         }
      }
      else
      {
         if(u_is_smir != 0)
         {
            if( data.r == 0.0 || data.r == (64.0/255.0) || data.r == (128.0/255.0) || data.r == (192.0/255.0))
            {
               discard;
            }
         }
         else
         {
            if( data.r == 0.0 )
            {
               discard;
            }
         }

         gl_FragColor.rgb = texture2D( u_palette , vec2( data.r , 1.0 ) ).rgb;
         gl_FragColor.a = u_alpha;
      }
   //Else not loaded
   } else
   {
      vec4 temp_color = texture2D( u_palette , v_texcoord );
      gl_FragColor = vec4( angle_value , v_texcoord.x , v_texcoord.y , 0.0 );
   }
}

vec2 merc_to_geo( in vec2 merc ) {
   vec2 geo;
   geo.y = 2.0*atan( exp( merc.y ) ) - _PI_by_2;
   geo.x =  merc.x;
   return geo;
}

float get_distance( vec2 latlon1 , vec2 latlon2 ){
   float lat1 = latlon1.y;
   float lat2 = latlon2.y;

   float dLat = (latlon2.y - latlon1.y);
   float dLon = (latlon2.x - latlon1.x);
   float a =
     sin(dLat/2.0) * sin(dLat/2.0) +
     cos((lat1)) * cos((lat2)) *
     sin(dLon/2.0) * sin(dLon/2.0) ;

   float c = 2.0 * atan( sqrt(a), sqrt(1.0-a) );
   float d = _R * c; // Distance in km
   return d;
}

float get_angle( vec2 latlon1 , vec2 latlon2 ) {
    float long1 = (latlon1.x);
    float long2 = (latlon2.x);
    float lat1 =  (latlon1.y);
    float lat2 =  (latlon2.y);

    float dLon = (long2 - long1);

    float y = sin(dLon) * cos(lat2);
    float x = cos(lat1) * sin(lat2) - sin(lat1)
            * cos(lat2) * cos(dLon);

    float brng = atan(y, x);
    brng = degrees(brng);
    brng = mod( (brng + 360.0) , 360.0 );

    return brng;
}
)";
