
#ifndef _PARTICLE_LAYER_H_
#define _PARTICLE_LAYER_H_

#include "default_layer.h"
#include <memory>


class ParticleAnimation;

class ParticleLayer : public DefaultLayer
{
public:
   ParticleLayer(unsigned int layer_id);
   virtual ~ParticleLayer();

   virtual void Init(const char* flashing_stylization = "",
         int stylization_size = 0) override;
   virtual void Update(World*) override;
   virtual void Draw(World*) override;
   virtual bool IsLayer(LayerTypes type)override;

private:
   std::unique_ptr<ParticleAnimation> m_animation;
};


#endif // end of include guard: _PARTICLE_LAYER_H_ 
