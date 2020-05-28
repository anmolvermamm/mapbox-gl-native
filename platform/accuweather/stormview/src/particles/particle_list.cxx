#include "particle_list.h"

#include "particle_point.h"
#include "glow.h"
#include "randomf.h"
#include <iostream>
#include "particle_config.h"


/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ParticleList::ParticleList()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ParticleList::~ParticleList()
{
 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleList::SetLiveTime( float live_time )
{
   m_live_time = live_time; 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleList::SeedPoints( int num_points )
{
   m_points.clear();
   m_points.reserve( num_points );
   for( int i = 0 ; i < num_points; ++i )
   {
      m_points.emplace_back( m_live_time * RandomF::randomF( 0.0 , 1.0 ) , 0.016 );
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleList::ChangePointCount( unsigned int num_points, float time_elapsed  )
{
   if( num_points < m_points.size() )
   {
      m_points.erase( m_points.begin()+num_points , m_points.end() );
   } else if( num_points > m_points.size() )
   {
      for( unsigned int i = m_points.size() ; i <= num_points ; ++i )
      {
         m_points.emplace_back(  m_live_time , time_elapsed  );
      }
   }
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleList::UpdateList( float time_elapsed )
{
   unsigned fill_count = 0;
   for(auto& point : m_points )
   {
      point.Update( time_elapsed );
      if( point.IsDead() && fill_count < m_max_refill )
      {
         ++fill_count;
         point.Reset( m_live_time * RandomF::randomF( 0.9 , 1.0 ) , time_elapsed );
      }
      SetPointSpeed( point );
   } 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ParticleBuffer ParticleList::CreateBuffer()
{
   ParticleBuffer buffer;
   std::vector<float> verts;
   static int print_count = -1;
   int count = 0;
   ++print_count;

   for(auto& point : m_points )
   {
      const auto& pos = point.GetPos();

      if( !point.IsDead()  )
      {
         ++count;
         verts.push_back( pos.x() );
         verts.push_back( pos.y() );
      } else {
      }
   } 

   buffer.m_buffer = Glow::CreateBuffer( verts );
   buffer.m_num_points = count;

   return buffer;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ParticleBuffer ParticleList::CreateBoxBuffer()
{
   ParticleBuffer buffer;
   std::vector<float> verts;
   int count = 0;
   for(auto& point : m_points )
   {
      if( !point.IsDead()  )
      {
         const auto& pos = point.GetPos();
         verts.push_back( pos.x() );
         verts.push_back( pos.y() );
         verts.push_back( point.GetAngle() );
         verts.push_back( point.GetLength() );
         verts.push_back( point.GetWidth() );
         count++;
      }
   } 

   buffer.m_buffer = Glow::CreateBuffer( verts );
   buffer.m_num_points = count;

   return buffer;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleList::SetPointSpeed( ParticlePoint& point )
{
   //static int count = 0;
   //static ImgCoords coords{ 
   //   ParticleConfig::GetImageSize() , 
   //   ParticleConfig::GetImageRect()  ,
   //   true
   //};
   const auto& pos = point.GetPos();
   //const auto& dim = coords.GetDimFromPoint( pos );
   const auto& dim = ParticleConfig::GetDimFromPoint( pos );
   bool is_dead = !m_raw_data.DimInImage( dim );
   point.SetDead( is_dead );

   if( is_dead ) {
      return;
   }


   auto color = m_raw_data.GetColorFromPoint( dim );

   auto velocity =  PointF( 
         float(color.greenI()) - 127.0,
         float(color.blueI()) - 127.0
         );

   point.SetVelocity( velocity );
}
