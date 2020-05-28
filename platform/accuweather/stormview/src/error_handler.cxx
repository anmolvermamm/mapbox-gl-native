#include "error_handler.h"
#include "error_code.h"

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ErrorHandler::~ErrorHandler()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
const char* ErrorHandler::GetTopErrorMessage()
{
   auto& inst = GetInstance();
   if( inst.m_errors.empty() )
      return 0;

   return inst.m_errors.back().msg.c_str();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int ErrorHandler::GetTopErrorCode()
{
   auto& inst = GetInstance();
   if( inst.m_errors.empty() )
      return 0;

   return inst.m_errors.back().error_code;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int ErrorHandler::GetTopErrorSize()
{
   auto& inst = GetInstance();
   if( inst.m_errors.empty() )
      return 0;

   return inst.m_errors.back().msg.size();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ErrorHandler::ClearTopError()
{
   auto& inst = GetInstance();
   inst.m_errors.pop_back();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ErrorHandler::PushError( int code , const std::string& msg )
{
   auto& inst = GetInstance();
   //inst.m_errors.emplace_back(  code , msg  );
   inst.m_errors.push_back( { code , msg } );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ErrorHandler::PushHttpError( int product, int http_code  , const std::string& msg )
{
   auto& inst = GetInstance();
   inst.m_errors.push_back( {product + http_error_type( http_code ) , msg } );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
int ErrorHandler::GetErrorCount()
{
   auto& inst = GetInstance();
   return inst.m_errors.size();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ErrorHandler::ErrorHandler()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
ErrorHandler& ErrorHandler::GetInstance()
{
   static ErrorHandler the_instance;
   return the_instance;
}

