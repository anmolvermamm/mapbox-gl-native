#ifndef _LAYER_H_
#define _LAYER_H_

#include "layer_types.h"
#include <string>

class World;
class PointF;

class Layer
{
public:
   Layer();
   virtual ~Layer();

   virtual void Init() = 0;
   virtual void Update(World*) = 0;
   virtual void Draw(World*) = 0;

   virtual bool IsLayer(LayerTypes type) = 0;
   virtual void Enable(bool enable) = 0;
   virtual std::string Query( const PointF& pos ) = 0;

private:
   
};


#endif // end of include guard: _LAYER_H_ 
