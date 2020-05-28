#ifndef _PARTICLE_LIST_H_
#define _PARTICLE_LIST_H_

//#include "particle_point.h"
#include <vector>
#include "particle_buffer.h"
#include "raw_image_data.h"


class ParticlePoint;

class ParticleList
{
public:
   ParticleList();
   ~ParticleList();
   void SetLiveTime( float live_time );
   void SeedPoints( int num_points );

   void ChangePointCount( unsigned int num_points , float time_elapsed );

   void UpdateList( float time_elapsed );

   ParticleBuffer CreateBuffer();
   ParticleBuffer CreateBoxBuffer();

   void SetPointSpeed( ParticlePoint& point );

   float m_live_time = 1000;//ms
   //NumPoints should be part of the buffer
   //Color
   //Time
   unsigned int m_max_refill = 20;

   std::vector<ParticlePoint> m_points;
   RawImageData m_raw_data;

   
};



#endif // end of include guard: _PARTICLE_LIST_H_ 
