#ifndef _SKYGUARD_VBO_CXX_
#define _SKYGUARD_VBO_CXX_

#include "sky_guard_vbo.h"
#include "glow.h"
#include "polygonf.h"
#include <iostream>


/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
SkyGuardVBO::~SkyGuardVBO()
{

}


/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
SkyGuardVBOGeometry::SkyGuardVBOGeometry( 
      GeoType type , 
      unsigned id , 
      const PolygonF& poly 
      ) : 
    id( id ),
    type( type ),
    color( 0 , 0 , 0 )
{
   buffer = Glow::CreateBuffer( poly );
   count = poly.size();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
SkyGuardVBOGeometry::SkyGuardVBOGeometry(SkyGuardVBOGeometry&& rhs) :
   color( rhs.color )
{
   this->buffer = rhs.buffer;
   this->id = rhs.id;
   this->count = rhs.count;
   this->type = rhs.type;

   rhs.buffer = 0;
   rhs.prev_buffer = this->buffer;
   rhs.id = 0;
   rhs.count = 0;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
SkyGuardVBOGeometry::~SkyGuardVBOGeometry()
{
   if( buffer )  
   {
      Glow::DeleteBuffer( buffer );
   } else
   {
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SkyGuardVBO::PushPolygon( GeoType type , int id , const PolygonF& poly )
{
   geometry.emplace_back( type , id , poly ); 
}

#endif // end of include guard: _SKYGUARD_VBO_CXX_ 
