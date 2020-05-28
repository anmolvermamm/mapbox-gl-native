#ifndef _RANDOMF_H_
#define _RANDOMF_H_

class RandomF
{
public:
   static float randomF();
   static float randomF(float min , float max );

private:
   static RandomF& GetInstance();
   RandomF();
   RandomF(const RandomF& ) = delete;
   RandomF& operator=(RandomF) = delete;
   
};



#endif // end of include guard: _RANDOMF_H_ 
