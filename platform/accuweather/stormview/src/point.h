#ifndef _POINT_H_
#define _POINT_H_

#include <iosfwd>

class Point
{
public:
   Point();
   Point(int x , int y);
   virtual ~Point();
   
   int x() const;
   int y() const;

   int& x();
   int& y();


   Point& setX(int x);
   Point& setY(int y);

   friend std::ostream& operator<<(std::ostream& , const Point& point );

private:
   int m_x = 0.0;
   int m_y = 0.0;
   
};



#endif // end of include guard: _POINT_H_ 
