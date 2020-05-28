#include "color.h"

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
Color::Color(int r , int g , int b ,int a )
   : r(float(r)/255.0) , g(float(g)/255.0) , b(float(b)/255.0) , a(float(a)/255.0 )
{
 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
Color::Color(float r , float g , float b , float a )
   : r(r) , g(g) , b(b) , a(a )
{
 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
Color::~Color()
{
 
}







