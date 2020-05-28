#ifndef _RECTF_H_
#define _RECTF_H_

#include <iosfwd>

class PointF;

class RectF
{
public: RectF(); RectF(float l , float r , float b , float t );
   //RectF(const PointF& top_left , const PointF& bottom_right );
   //RectF(const PointF& top_left );
   virtual ~RectF();

   void SetSize( const PointF& size );
   void CalcSize();

   friend std::ostream& operator<<(std::ostream& , const RectF& rect );

   float left()const;
   float right()const;
   float bottom()const;
   float top()const;

   float width()const;
   float height()const;

   static RectF FromString( const std::string& str );

   void GetCenter( PointF& center )const;

   bool isNull() const;
   RectF& operator+=(const RectF& other );

   bool ContainsPoint( const PointF& pos );

   void ExpandSides( float size );

private:
   float m_l = 0;
   float m_r = 0;
   float m_b = 0;
   float m_t = 0;
   float m_w = 0;
   float m_h = 0;
   //bool m_invert_y;//Not used yet, should implement
};


#endif // end of include guard: _RECTF_H_ 
