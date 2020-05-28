#include "world_layer.h"

#include "glow_image_2d.h"
#include "world.h"
#include "rectf.h"
#include "camera.h"

#include <iostream>
#include "glm/mat4x4.hpp"

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
WorldLayer::WorldLayer(unsigned int layer_id) : DefaultLayer(layer_id)
{
 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
WorldLayer::~WorldLayer()
{
 
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void WorldLayer::Init(const char* flashing_stylization, int stylization_size)
{
   GlowImage2D::InitGlowImage2D();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void WorldLayer::Update(World*) 
{
   //Nothing to do here 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void WorldLayer::Draw(World* world) 
{
   static RectF verts{ -3.14 , 3.14 , -3.14 , 3.14 };
   auto& camera = world->GetCamera(); 
   const auto& mat = camera.GetWorldMatrix();
   GlowImage2D::Draw( mat , 0 , verts );
}







