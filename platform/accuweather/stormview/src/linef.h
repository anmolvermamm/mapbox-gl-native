#ifndef _LINEF_H_
#define _LINEF_H_

#include <vector>
#include "pointf.h"

//PolyF????
enum LineType { CLOSED , OPEN };

class LineF : private std::vector<PointF>
{
public:
   typedef std::vector<PointF> Vector;
   using Vector::Vector;
   using Vector::at;
   using Vector::clear;
   using Vector::iterator;
   using Vector::const_iterator;
   using Vector::begin;
   using Vector::end;
   using Vector::cbegin;
   using Vector::cend;
   using Vector::crbegin;
   using Vector::crend;
   using Vector::data;
   using Vector::empty;
   using Vector::size;
   using Vector::reserve;
   using Vector::operator[];
   using Vector::assign;
   using Vector::insert;
   using Vector::erase;
   using Vector::front;
   using Vector::back;
   using Vector::push_back;
   using Vector::pop_back;
   using Vector::resize;


   //Intersects...
   //BoudingRect...
   //Closed...
   bool IsClosed() const {
      if( this->size() < 2 )
         return false;

      return this->front() == this->back();
      
   };

};



#endif // end of include guard: _LINEF_H_ 
