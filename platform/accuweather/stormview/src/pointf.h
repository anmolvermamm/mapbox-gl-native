#ifndef _POINTF_H_
#define _POINTF_H_

#include <iosfwd>

class PointF
{
public:
   PointF();
   PointF(float x , float y);
   
   float x() const;
   float y() const;

   float& x();
   float& y();

   PointF& setX(float x);
   PointF& setY(float y);

   float distanceSquared( const PointF& other )const;

   friend std::ostream& operator<<(std::ostream& , const PointF& point );

   bool operator==( const PointF& pointf ) const;

private:
   float m_x = 0.0;
   float m_y = 0.0;
   
};

static_assert( sizeof( PointF ) == 2 * sizeof( float ) ,
      "PointF should equal the size of two floats" );


#endif // end of include guard: _POINTF_H_ 
