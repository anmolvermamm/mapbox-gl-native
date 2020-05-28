#ifndef _SKY_GUARD_LAYER_H_
#define _SKY_GUARD_LAYER_H_

//#include "layer.h"
#include <memory>
#include "default_layer.h"
class SkyGuard;

class SkyGuardLayer : public DefaultLayer
{
public:
   SkyGuardLayer(unsigned int layer_id);
   virtual ~SkyGuardLayer();

   virtual void Init(const char* flashing_stylization = "",
         int stylization_size = 0) override;
   virtual void Update(World*) override;
   virtual void Draw(World*) override;
   virtual bool IsLayer(LayerTypes type)override;

private:
   std::unique_ptr<SkyGuard> m_skyguard;
};


#endif // end of include guard: _SKY_GUARD_LAYER_H_ 
