#include "particle_point.h"
#include "particle_config.h"
#include <cmath>

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
//ParticlePoint::ParticlePoint(float live_time , float time_elapsed , float varmin, float varmax ) :
//   ParticlePoint( live_time * Random::randomF(varmin, varmax ) , time_elapsed )
//{
//
//}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ParticlePoint::ParticlePoint(float live_time , float time_elapsed)
{
   SeedPoint( live_time , time_elapsed);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticlePoint::Reset( float live_time , float time_elapsed )
{
   SeedPoint( live_time , time_elapsed);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticlePoint::SeedPoint(float live_time , float time_elapsed )
{
   //m_pos.x() = Random::randomF() * 3.14 * 2 - 3.14;
   //m_pos.y = Random::randomF() * 3.14 * 2 - 3.14;
   m_left_to_live = live_time;
   m_pos = ParticleConfig::GetRandomPosInRect();
   //m_pos.setX( 0.0 );
   //m_pos.setY( 0.0 );

   m_velocity.setX( 0.0 );
   m_velocity.setY( 0.0 );

   m_width = ParticleConfig::GetWidth() / 2.0;

   CalculateLength( time_elapsed );


   m_update_count=0;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticlePoint::SetVelocity( const PointF& velocity )
{
   m_velocity = velocity;
   //m_velocity = PointF{ 20.0 , 0.0 };
   m_magnitude = std::sqrt( m_velocity.x()* m_velocity.x() + m_velocity.y() * m_velocity.y() );
   m_angle = std::atan2( m_velocity.y() , m_velocity.x() );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticlePoint::Update( float frame_time )
{

   m_left_to_live -= frame_time;
   m_update_count++;

   //if( this->IsDead() ) return;
   
   //if( manage update )
   //get_speed();

   this->MovePosition( frame_time );
   this->CalculateLength( frame_time );

   CheckInvalid();


   //Check invalid?

}
const static float SPEED_CONSTANT = 2.0;
const static float LENGTH_CONSTANT = 1.0;
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticlePoint::MovePosition( float frame_time )
{
   const auto& res = ParticleConfig::GetResolution();

   m_pos.x() += res.x() * m_velocity.x() * frame_time * 0.001 * SPEED_CONSTANT;
   m_pos.y() += res.y() * m_velocity.y() * frame_time * 0.001 * SPEED_CONSTANT;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const PointF& ParticlePoint::GetPos()const
{
   return m_pos; 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticlePoint::CalculateLength( float frame_time )
{
   m_length =  m_magnitude * frame_time * 0.001 * SPEED_CONSTANT * 
      ParticleConfig::GetResolution().x() * LENGTH_CONSTANT;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool ParticlePoint::IsDead()
{
   return m_left_to_live < 0;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticlePoint::SetDead( bool dead )
{
   m_left_to_live = dead ? -1 : m_left_to_live;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticlePoint::CheckInvalid()
{
   //if( m_pos.x() <  0.01 && 
   //    m_pos.x() > -0.01 && 
   //    m_pos.y() <  0.01 && 
   //    m_pos.y() > -0.01 ) 
   //{
   //   m_left_to_live = -1;
   //}

   if( m_magnitude < 2.0 )
   {
      m_left_to_live = -1;
   }
}
