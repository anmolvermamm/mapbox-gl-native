#ifndef _ROLLING_DEQUE_H_
#define _ROLLING_DEQUE_H_

#include <deque>
#include <functional>

#ifndef __APPLE__
//#include <glog/logging.h>
#endif

#include <iostream>


#ifndef DEBUG_ROLLING_DEQUE
#define DEBUG_ROLLING_DEQUE 0
#endif

// struct deref_comp_tt
// This structure uses template template parameters
// and partial specialization to determine 
// how to compare values.  
// References, pointers, 
// and shared pointers should all be properly dereferenced
// before comparing.
// This prevents the problem of comparing memory values
// instead of the values they point to
template <typename T , template<class> class C>
struct deref_comp_tt {
   bool operator()( const T& t1 , const T& t2 ) const{
      return comp( t1 , t2 );
   }
   C<T> comp;
};


template <typename T, template<class> class C>
struct deref_comp_tt <T*, C> {
   bool operator()( const T* t1 ,
         const T* t2 ) const{
      return comp( *t1 , *t2 );
   }
   C<T> comp;
};


#define ROLLING_DEQUE_USE_CPP11
#ifdef ROLLING_DEQUE_USE_CPP11
#include <memory>
template <typename T, template<class> class C>
struct deref_comp_tt <std::shared_ptr<T>, C> {
   bool operator()( const std::shared_ptr<T>& t1 , 
         const std::shared_ptr<T>& t2 ) const{
      return comp( *t1 , *t2 );
   }
   C<T> comp;
};
#endif


#define SKIP_QSHARED_POINTER
#ifndef SKIP_QSHARED_POINTER
#include <QSharedPointer>
template <typename T, template<class> class C>
struct deref_comp_tt <QSharedPointer<T>, C> {
   bool operator()( const QSharedPointer<T>& t1 , 
         const QSharedPointer<T>& t2 ) const{
      return comp( *t1 , *t2 );
   }
   C<T> comp;
};
#endif


// class RollingDeque
// This class is designed to have items appended to the back
// and then drop off the front after a certain limit is reached
// This allows us to continually add new products as they are generated
// and not have to worry about when to remove the old products from the list
// This class also sorts the items so the newest item should be on the top
// of the stack

//Compare is a structure that compares items
//Using default combo of deref_comp_tt and std::less to 
//do a < comparision between two items, see above for details
template <typename T ,
          typename Comp = deref_comp_tt<T, std::less> > 
          //typename Comp = std::less<T> >
          //typename Equals = std::equal_to<T> >
class RollingDeque
{
public:
   RollingDeque( unsigned int max_count ) :
      m_max_count( max_count ) 
   {
   
   }

  // RollingDeque() :
  //    m_max_count( 50 ) 
  // {
  // 
  // }
   virtual ~RollingDeque(){

   }

   //The rolling deque maintains an order
   //If you are pushing back, the "largest" item
   //is put on the back, otherwise it is inserted into the
   //list, that mid list insertions shouldn't happen
   //THAT often
   void push_back(const T& t ){
     if( m_deque.empty() )
       m_deque.push_back( t );
     else {
         if( comp ( m_deque.back() ,   t )   ) {
            m_deque.push_back(t);
         } else {
            insert_in_order( t );
         }
     }
      resize();
   }

   //Pushes, but not in any specific order
   void push_stack(const T& t ){
      m_deque.push_back( t );
      resize();
   }

   void pop_front() {
      return m_deque.pop_front();
   }

   T& front() {
      return m_deque.front();
   }

   const T& front() const {
      return m_deque.front();
   }

   T& back() {
      return m_deque.back();
   }

   const T& back() const {
      return m_deque.back();
   }

   typename std::deque<T>::size_type size() const {
      return m_deque.size();
   }

   T& operator[](typename std::deque<T>::size_type n ) {
     return m_deque[n];
   }

   const T& operator[](typename std::deque<T>::size_type n ) const {
     return m_deque[n];
   }

   void changeDequeSize( unsigned int max_count){
      m_max_count = max_count;
      resize();
   }

   void clear() {
      return m_deque.clear();
   }

   bool empty() const {
      return m_deque.empty();
   }




   //Typedefs would have made the functions below easier to write...
   typedef typename std::deque<T>::iterator iterator;
   typedef typename std::deque<T>::const_iterator const_iterator;
   typedef typename std::deque<T>::reverse_iterator reverse_iterator;
   typedef typename std::deque<T>::const_reverse_iterator const_reverse_iterator;

   //Forward the STL functions
   typename std::deque<T>::iterator begin() {
      return m_deque.begin();
   }
   typename std::deque<T>::const_iterator begin() const {
      return m_deque.begin();
   }
   typename std::deque<T>::reverse_iterator rbegin() {
      return m_deque.rbegin();
   }
   typename std::deque<T>::const_reverse_iterator rbegin() const {
      return m_deque.rbegin();
   }

   typename std::deque<T>::iterator end() {
      return m_deque.end();
   }
   typename std::deque<T>::const_iterator end() const {
      return m_deque.end();
   }
   typename std::deque<T>::reverse_iterator rend() {
      return m_deque.rend();
   }
   typename std::deque<T>::const_reverse_iterator rend() const {
      return m_deque.rend();
   }

   iterator erase( iterator pos ){
      return m_deque.erase( pos );   
   }


   const_iterator find_matching( const T& t )
   {
      typename std::deque<T>::iterator i;
      for ( i = m_deque.begin(); i != m_deque.end(); ++i) 
      {
         if( !comp( *i , t ) && !comp( t, *i )  ) 
         {
            return i;
         }
      }
      return i;
   }


   //Temporary, will need to delete
   std::deque<T>& get_deque() { return m_deque; }

protected:
   void resize(){
      //if( m_deque.size() > m_max_count )
      while( m_deque.size() > m_max_count )
         m_deque.pop_front();
   }

   //Insert linearly, if equals do not insert twice
   //Lists should usually be built in order so i'm hoping this doesn't
   //get called too often, lists should also be relatively small
   //If the item goes on the back it should be O(1) time to push it
   //A middle insert is O(N) but these shouldn't be that common
   void insert_in_order( const T& t ) {

     //This is for items that should be put on the back of the deque
     //There is no need to do this if the deque max size has been reached
     if( comp( t , m_deque.front() ) ) {
         //The deque still has space remaining on the back, push_front
         if(m_deque.size() < m_max_count ) {
           m_deque.push_front(  t );
           if(DEBUG_ROLLING_DEQUE)
              printf("Item belongs at front, inserting\n");
         //The max deque size has been reached, do nothing
         } else { 
            if(DEBUG_ROLLING_DEQUE)
               printf("Item belongs at front but MAX SIZE exceeded, ignoring\n");
         }
         return;
     }
     
     //Item does not belong at the front or the back
     //Loop through and determine where it belongs
     for (typename std::deque<T>::reverse_iterator i = m_deque.rbegin(); i != m_deque.rend(); ++i) {
        //I have changed this to replace items that are equal.  
        //It should be rare that items come in from the network that are equal
        //but it is likely that an item will need to be replaced manually
        if( !comp( *i , t ) && !comp( t, *i )  ) {
           if(DEBUG_ROLLING_DEQUE)
              printf("Item has equal reassign\n");
           *i = t;
           return;
        }
        //This means the item goes in the middle of the deque
        else if( comp( *i,   t )   ) {
           if(DEBUG_ROLLING_DEQUE)
              printf("Mid Position Found, inserting\n");
            m_deque.insert( i.base()  , t );
            return;
        } 
     }

   }


private:
   Comp comp;
   //Equals equals;
   unsigned int m_max_count;  
   std::deque<T> m_deque;
};

#endif // end of include guard: _ROLLING_DEQUE_H_ 


