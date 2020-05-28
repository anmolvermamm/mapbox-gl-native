#ifndef _CXX_CAMERA_H_
#define _CXX_CAMERA_H_

//We should make a proxy class
//and calculate the matrix once per frame
//but we will be calling it so few times it won't matter
//#include <glm/mat4x4.hpp>
#include "glm/fwd.hpp"

#include "rectf.h"
#include "pointf.h"

class Camera
{
public:
   Camera();
   virtual ~Camera();

   //const glm::mat4x4& GetWorldMatrix();
   glm::mat4x4 GetWorldMatrix()const;

   void SetExtents( const RectF& rect );
   //void SetCenter( const RectF& rect );
   
   void SetViewPort ( const PointF& size );

   const RectF& GetExtents()const;

   PointF GetCenter()const;

   PointF GetViewport() const { return m_viewport; }

   void CalculateWorldToViewport();
   const PointF& GetWorldToViewport()const;


private:
   RectF m_extents;

   //They are ints but since we use these
   //for matrices we are just going to leave them as 
   //floats
   PointF m_viewport;                      

   PointF m_world_to_viewport;                      
   
};


#endif // end of include guard: _CAMERA_H_ 

