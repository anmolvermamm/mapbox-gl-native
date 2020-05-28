#ifndef _WORLD_LAYER_H_
#define _WORLD_LAYER_H_

//#include "layer.h"
#include "default_layer.h"

class WorldLayer : public DefaultLayer
{
public:
   WorldLayer(unsigned int layer_id);
   virtual ~WorldLayer();

   virtual void Init(const char* flashing_stylization = "",
         int stylization_size = 0) override;
   virtual void Update(World* world) override;
   virtual void Draw(World* world) override;
};

#endif // end of include guard: _WORLD_LAYER_H_ 
