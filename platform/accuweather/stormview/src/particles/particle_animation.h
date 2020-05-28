#ifndef _PARTICLE_ANIMATION_H_
#define _PARTICLE_ANIMATION_H_

#include "color.h"

#include "glm/fwd.hpp"
#include "time_stack.h"
#include "particle_list.h"



class World;
class ParticleBuffer;
class ByteArray;

class ParticleAnimation
{
public:
   ParticleAnimation();
   virtual ~ParticleAnimation();

   void Init(int num_points , float animation_length , const Color& color );

   void SetPoints( int num_points );
   void SetAnimationLength( float animation_length );
   void SetColor( const Color& color  );

   void Update( float time_elapsed );
   void Draw();

   void SeedPoints();
   void CalcMaxRefill();
   //void LoadDebug();

   //Access into DTS?
   void RequestTimes();
   void ReceiveTimes( const ByteArray& data );

   void RequestImage(const std::string& time );
   void ReceiveImage( const ByteArray& data );
   void ReceiveBackgroundImage( const ByteArray& data );

   void ReceiveError(int code, const std::string& error_msg );
   bool HasAllRequests();

   bool m_draw_data_ref = true;
   bool m_show_tails = true;

private:
   unsigned m_data_texture = 0;
   float m_default_alpha = 0.8;
   int m_num_points;
   std::string m_host_url;
   std::string m_request_time;

   ParticleList m_particle_list;

   TimeStack<std::unique_ptr<ParticleBuffer> > m_time_stack;

   
};

#endif // end of include guard: _PARTICLE_ANIMATION_H_ 
