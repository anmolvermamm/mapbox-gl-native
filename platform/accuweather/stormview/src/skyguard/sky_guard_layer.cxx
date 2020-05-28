#include "sky_guard_layer.h"
#include "sky_guard.h"

extern bool SKYGUARD_LOG;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
SkyGuardLayer::SkyGuardLayer(unsigned int layer_id) : DefaultLayer(layer_id)
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
SkyGuardLayer::~SkyGuardLayer()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuardLayer::Init(const char* flashing_stylization, int stylization_size)
{
   m_skyguard = std::make_unique<SkyGuard>();
   if( SKYGUARD_LOG )
      printf("Init Skyguard Layer\n");

   m_skyguard->Init();
   this->m_initialized = true;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuardLayer::Update(World*) 
{
   if( m_enabled )
   {
      m_skyguard->Update();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuardLayer::Draw(World*) 
{
   if( m_enabled )
   {
      m_skyguard->Draw();
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool SkyGuardLayer::IsLayer(LayerTypes type)
{
   return type == LayerTypes::SKYGUARD;
}
