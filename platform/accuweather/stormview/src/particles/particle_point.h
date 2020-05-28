#ifndef _PARTICLE_POINT_H_
#define _PARTICLE_POINT_H_

#include "pointf.h"

class ParticlePoint
{
public:
   ParticlePoint(float live_time , float time_elapsed);

   void Reset( float live_time , float time_elapsed ); 
   void SetPosition(const PointF& pos); 
   void Update( float time_elapsed );

   bool IsDead();
   void SetDead( bool dead );//Set true if the point is dead


   void SetVelocity( const PointF& velocity );

   void CalculateLength( float frame_time );

   const PointF& GetPos()const;
   float GetAngle()  const{ return m_angle; }
   float GetLength() const{ return m_length; }
   float GetWidth()  const{ return m_width; }

   float LeftToLive() { return m_left_to_live; }

   bool print = false;

   const PointF& GetVelocity() { return m_velocity; }

private:
   void SeedPoint(float live_time , float time_elapsed );

   void MovePosition( float time_elapsed );
   void CheckInvalid();

   //Used in buffer
   PointF m_pos;//Check
   float m_angle;//Check
   float m_length;//Check
   float m_width;//Check

   //Used to calculate
   PointF m_velocity;
   float m_left_to_live = -1;
   int m_update_count = 0;
   float m_magnitude = 0.0;
    
};


#endif // end of include guard: _PARTICLE_POINT_H_ 
