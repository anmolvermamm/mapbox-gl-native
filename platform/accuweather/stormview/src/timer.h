#ifndef _TIMER_H_
#define _TIMER_H_

#include <unordered_set>
#include <memory>
#include <functional>

class DateTime;



class Timer
{
public:
   friend class World;

   Timer();
   virtual ~Timer();

   void setCallback( const std::function<void(void)>& callback );

   template <typename T, typename Callback>
   void setCallback( T , Callback );

   Timer(Timer&&) =delete;
   Timer& operator=(const Timer&) = delete;
   Timer& operator=(Timer&&) = delete;

protected:
   virtual void Update() = 0;
   void Emit();

   virtual bool TimerFinished() = 0;
   virtual bool Repeatable() { return false; }
   virtual void Reset() {}

   std::function<void(void)> m_callback;

private:
   static void UpdateAllTimers( );
   static void UpdateOne( );
   static void UpdateTwo( );
   static void AddTimerToList( Timer* timer );
   static void RemoveTimerFromList( Timer* timer );
   static std::unordered_set<Timer*> timers;

   static bool print;

   
};


class CountDownTimer : public Timer
{
public:
   void SetCountDownTimeMs( float time_ms , bool repeat = false );

protected:
   virtual void Update()override;
   virtual void Reset()override;
   virtual bool Repeatable() override { return m_repeat; }
   virtual bool TimerFinished()override;

   bool m_repeat;
   float target_time = 0.0;
   float elapsed_time = 0.0;

};


class TargetTimer : public Timer
{
public:
   void SetTargetTime( const DateTime& time  );

protected:
   virtual void Update()override;
   virtual bool TimerFinished()override;

   int target_time = 0;

};



#endif // end of include guard: _TIMER_H_ 
