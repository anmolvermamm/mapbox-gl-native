#include "particle_animation.h"
#include "glm/mat4x4.hpp"
#include "config.h"

#include "glow.h"

#include "particle_config.h"
#include "../glow/glow_image_2d.h"
#include "gl_particles_2d.h"
#include "gl_particle_box_2d.h"
#include "point.h"
#include "particle_point.h"
#include "http_network_request.h"
#include "byte_array.h"
#include "json.hpp"

#include <string>
#include <algorithm>
#include <iostream>

#include "error_code.h"
#include "error_handler.h"


bool PARTICLE_LOG = false;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ParticleAnimation::ParticleAnimation()
{
   m_host_url = "http://tilergrp.accuaws.com/gfs";
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ParticleAnimation::~ParticleAnimation()
{
   GLParticles2D::DeleteParticles2D();
   GLParticleBox2D::DeleteParticleBox2D();
   GlowImage2D::DeleteGlowImage2D();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::Init(int num_points , float animation_length , const Color& color )
{
   GLParticles2D::InitGLParticles2D();
   GLParticleBox2D::Init();
   GlowImage2D::InitGlowImage2D();

   SetPoints( num_points );
   SetAnimationLength( animation_length );
   m_time_stack.set_time_to_keep( animation_length );
   SetColor( color );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::SetPoints( int num_points )
{
   m_num_points = num_points;
   CalcMaxRefill();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::CalcMaxRefill()
{
   const float expected_frame_time = 0.016;
   m_particle_list.m_max_refill = std::max( 
         unsigned( m_num_points / float( expected_frame_time ) * 3 ),
         unsigned(5) );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::SetAnimationLength( float animation_length )
{
   m_particle_list.SetLiveTime( animation_length );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::SeedPoints()
{
   //m_particle_list.SetLiveTime( m_animation_length );
   m_particle_list.SeedPoints( m_num_points );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::SetColor( const Color& color  )
{
   //UNUSED(color); 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::Update( float time_elapsed )
{

   ParticleConfig::CalculateParameters();

   m_particle_list.UpdateList( time_elapsed ); 

   //auto next_buff = m_particle_list.CreateBoxBuffer();
   auto next_buff = m_particle_list.CreateBuffer();
   auto ptr = std::make_unique<ParticleBuffer>();
   *ptr = next_buff;
   ptr->m_owns = true;

   for( auto& buff : m_time_stack )
   {
      buff->m_time += time_elapsed;
      buff->m_alpha = 
         ( m_particle_list.m_live_time - buff->m_time ) 
         / m_particle_list.m_live_time  * m_default_alpha;
   }

   int removed = m_time_stack.remove_old_items();
   (void)(removed);

   m_time_stack.push_back( std::move( ptr ) );
   
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::Draw()
{

   const auto& mat = Config::GetMatrix();
   static auto tex_box = ParticleConfig::GetImageRect();
   
   if( !HasAllRequests() )
      return;

   if( m_draw_data_ref )
   {
      GlowImage2D::Draw( mat , m_data_texture , tex_box );
   }

   //if( !m_time_stack.empty() )
   //{
   //   GLParticles2D::DrawGL( *(m_time_stack.back()) );
   //}

   //GLParticleBox2D::Enable( mat );
   if( m_show_tails )
   {
      for( auto& ptr : m_time_stack )
      {
         GLParticles2D::DrawGL(mat,  *ptr );
         //GLParticleBox2D::DrawGL(mat ,*ptr );
      }
   } else 
   {
      if( !m_time_stack.empty() )
      {
         //GLParticleBox2D::DrawGL(mat , *m_time_stack.back() );
         GLParticles2D::DrawGL(mat , *m_time_stack.back() );
      }
   }
   //GLParticleBox2D::Disable();

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::RequestTimes()
{
   std::string product_frames_path=m_host_url+"/preferred_product_frames?products=26-1020";

   auto success = [this]( const ByteArray& data ) {
      this->ReceiveTimes( data );
   };

   auto failure = [this](int code , const std::string& error ) {
      this->ReceiveError( code , error );
   };

   if( PARTICLE_LOG )
      printf("Request data: %s \n", product_frames_path.c_str());

   HttpNetworkRequest::RequestData( product_frames_path , success , failure );


}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::ReceiveTimes( const ByteArray& data )
{
   using nlohmann::json;
   auto j =  json::parse( data.to_string() );
   auto frames = j["frames"];

   if( !frames.empty() )
   {
      auto& front = frames.front();
      m_request_time = front;
      RequestImage( front );
   } else
   {
      ErrorHandler::PushError( WINDERROR + NODATAERROR , "No Wind Data to Display" );
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::RequestImage(const std::string& time )
{
   std::string custom_image_request=m_host_url;
   custom_image_request+=
      "/custom?topmerc=3.1415&bottommerc=-3.1415&rightmerc=3.1415&leftmerc=-3.1415"
      "&imgheight=1440&imgwidth=1440"
      "&display_mode=10&blend=1&display_products=26-1020";
   custom_image_request += "&frametime=" + time;


   std::string background_image_request=m_host_url;
   background_image_request+=
      "/custom?topmerc=3.1415&bottommerc=-3.1415&rightmerc=3.1415&leftmerc=-3.1415"
      "&imgheight=1440&imgwidth=1440"
      "&display_mode=11&blend=1&display_products=26-1020" 
      "&frametime=" + time;

   if( PARTICLE_LOG )
      printf("Request image url: %s \n", custom_image_request.c_str());
   if( PARTICLE_LOG )
      printf("Request image url: %s \n", background_image_request.c_str());


   auto success = [this]( const ByteArray& data ) {
      this->ReceiveImage( data );
   };

   auto bg_success = [this]( const ByteArray& data ) {
      this->ReceiveBackgroundImage( data );
   };

   auto failure = [this](int code , const std::string& error ) {
      this->ReceiveError(  code, error );
   };

   HttpNetworkRequest::RequestData( background_image_request , bg_success , failure );
   HttpNetworkRequest::RequestData( custom_image_request , success , failure );

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::ReceiveImage( const ByteArray& data )
{
   unsigned int format;
   m_particle_list.m_raw_data.LoadFromMem(data, format);

   if( PARTICLE_LOG )
      printf("Received network image: %d, width: %d, height: %d, size: %lu Seed points!\n",
        m_data_texture, m_particle_list.m_raw_data.m_size.x(), m_particle_list.m_raw_data.m_size.y(),
        m_particle_list.m_raw_data.m_data.size());

   this->SeedPoints();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::ReceiveBackgroundImage( const ByteArray& data )
{
   //Otherwise we leak memory from not destroying the texture..
   if( m_data_texture )
      return;

   RawImageData image;
   unsigned int format;
   image.LoadFromMem( data, format );

   if (PARTICLE_LOG )
      printf("Received network image: %d, width: %d, height: %d, size: %lu Seed points!\n",
        m_data_texture, image.m_size.x(), image.m_size.y(), image.m_data.size());

   m_data_texture = Glow::Load2DImageToTextureFromRawData( 
         image.m_data.data(),
         image.m_size.x() , 
         image.m_size.y(),
         Glow::LINEAR );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleAnimation::ReceiveError(int code, const std::string& error_msg )
{
   static int count = 0;
   if( m_request_time.empty() )
   {
      auto msg = std::string( "Error receving Wind Time Data. ");
      msg + error_msg;
      ErrorHandler::PushHttpError( WINDERROR ,  code  ,msg );
   } else 
   {
      ++count;
      if( count % 2 )
      {
         auto msg = std::string( "Error receving Wind Image Data. ");
         msg + error_msg;

         ErrorHandler::PushHttpError( WINDERROR ,  code  , msg );

         RequestImage( m_request_time );
      }
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool ParticleAnimation::HasAllRequests()
{
   if( m_data_texture != 0 && m_particle_list.m_raw_data.m_data.size() != 0 )
      return true;
   return false;
}

