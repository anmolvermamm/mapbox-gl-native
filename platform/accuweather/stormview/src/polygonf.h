#ifndef _POLYGONF_H_
#define _POLYGONF_H_

#include <vector>
#include "pointf.h"
#include <iostream>
#include <algorithm>

class PolygonF;

class PolygonF : private std::vector<PointF>
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
   using Vector::emplace;
   using Vector::emplace_back;
   using Vector::pop_back;
   using Vector::resize;
   using Vector::value_type;

   void AddHoleToPolygon(PolygonF hole)
   {
      holes.emplace_back(hole);
   }

   std::vector<PolygonF> holes;
   bool right_handed;

   //Intersects...
   //BoudingRect...
   //Closed...
   bool IsClosed() const {
      if( this->size() < 2 )
         return false;

      return this->front() == this->back();
      
   };

   friend std::ostream& operator<<( std::ostream& os , const PolygonF& polygon );

   void Append( const PolygonF& other ) 
   {
      std::copy( other.begin() , other.end() , std::back_inserter( *this ) );
   }

   private:

};

inline std::ostream& operator<<( std::ostream& os , const PolygonF& polygon )
{
   bool first = true;
   os<< "{";
   for( const auto& point : polygon )
   {
      if( !first ) os << ",";
      os << point;

      first = false;
   }
   os<< "}";
   return os;
}



#endif // end of include guard: _POLYGONF_H_ 
