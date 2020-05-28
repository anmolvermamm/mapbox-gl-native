
#ifndef _COLOR_H_
#define _COLOR_H_

class Color
{
public:
   Color(int r , int g , int b ,int a = 255 );
   Color(float r = 1.0 , float g = 0.0, float b = 0.0, float a = 1.0 );
   virtual ~Color();

   int redI() const{ return int(r*255); }
   int greenI() const{ return int(g*255); }
   int blueI() const{ return int(b*255); }

   float red() const{ return r; }
   float green() const{ return g; }
   float blue() const{ return b; }
   float alpha() const{ return a; }

   bool operator==(const Color& color) const
   {
      return (r == color.red()) && (g == color.green()) && (b == color.blue());
   }

private:

   float r;
   float g;
   float b;
   float a;
   
};


#endif // end of include guard: _COLOR_H_ 
