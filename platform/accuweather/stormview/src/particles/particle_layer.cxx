#include "particle_layer.h"

#include "camera.h"
#include "world.h"
//#include "controls.h"
#include "particle_animation.h"
#include "glm/mat4x4.hpp"
#include <iostream>

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ParticleLayer::ParticleLayer(unsigned int layer_id) : DefaultLayer(layer_id)
{
 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ParticleLayer::~ParticleLayer()
{
   if(m_animation)
   {
      m_animation.reset();
      m_animation.release();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleLayer::Init(const char* flashing_stylization, int stylization_size)
{
   std::cout << "Init particle layer" << std::endl;
   m_animation = std::make_unique<ParticleAnimation>();
   m_animation->Init(3000.0 , 1500.0 , {1.0f , 1.0f , 1.0f });
//
//   //Since we are not loading the image off the network
//   m_animation->LoadDebug();
//   m_animation->SeedPoints();
   m_animation->RequestTimes();
   this->m_initialized = true;
   this->m_is_animated = true;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleLayer::Update(World* world) 
{
   float frametime = world->GetFrameTime();
   m_animation->Update( frametime ); 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ParticleLayer::Draw(World* world) 
{
   if( m_enabled )m_animation->Draw();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool ParticleLayer::IsLayer(LayerTypes type)
{
   return type == LayerTypes::PARTICLE;
}

