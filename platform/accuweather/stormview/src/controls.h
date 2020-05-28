#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include "pointf.h"

class Controls
{
public:
   Controls();
   virtual ~Controls();

   bool LMouseDown();
   bool RMouseDown();

   void SetMousePos( const PointF& pos );
   void SetMouseState(bool l_mouse , bool r_mouse );


private:
   PointF m_mouse_pos;
   bool m_l_mouse_down;
   bool m_r_mouse_down;
   
};

#endif // end of include guard: _CONTROLS_H_ 
