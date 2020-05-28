#ifndef _TIME_STACK_H_
#define _TIME_STACK_H_

#include <deque>
#include <memory>



template<typename T>
struct get_time_default {

float operator()(const T& t ) {
   return t.getTime();
}   
};

template<typename T> 
struct get_time_default<T*> {
float operator()(const T* t ) {
   return t->getTime();
}
};


template<typename T> 
struct get_time_default<std::shared_ptr<T>> {
float operator()(const std::shared_ptr<T>& ptr ) {
   return ptr->getTime();
}
};

template<typename T> 
struct get_time_default<std::unique_ptr<T>> {
float operator()(const std::unique_ptr<T>& ptr ) {
   return ptr->getTime();
}
};

template <typename T , typename GetTime = get_time_default<T> >
class TimeStack
{
public:
   typedef typename std::deque<T>::iterator iterator;
   typedef typename std::deque<T>::const_iterator const_iterator;


public:
   //TimeStack(float time_to_keep) : m_time_to_keep(time_to_keep) {}
   TimeStack() {}

   void set_time_to_keep( float time_to_keep )
   {
      m_time_to_keep = time_to_keep;
   }

   size_t size() const
   {
      return m_list.size();
   }

   bool empty()const
   {
      return m_list.empty();
   }

   T& back()
   {
      return m_list.back();
   }


   void push_back(const T& t)
   {
      m_list.push_back(t);
   }

   void push_back(T&& t)
   {
      m_list.push_back(std::move(t));
   }


   void clear()
   {
      return m_list.clear();
   }

   iterator begin()
   {
      return m_list.begin();
   }

   iterator end()
   {
      return m_list.end();
   }
   
   int remove_old_items()
   {
      int removed = 0;
      for( auto i = m_list.begin() ; i != m_list.end() ; )
      {
         float oldest_time = m_get_time(*i);
         if( oldest_time > m_time_to_keep )
         {
            i = m_list.erase( i );
            ++removed;
         } else {
            break;
         }
      }
      return removed;
   }



private:
   std::deque<T> m_list;
   GetTime m_get_time;
   float m_time_to_keep;
   
   
};

#endif // end of include guard: _TIME_STACK_H_ 
