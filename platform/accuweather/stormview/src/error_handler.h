#ifndef _ERROR_HANDLER_H_
#define _ERROR_HANDLER_H_

#include <string>
#include <vector>

class Error
{
public:
   //Error() = default;
   //Error(int code , const std::string& str ) : error_code( code ), msg( str) {};
   int error_code;
   std::string msg;
};


class ErrorHandler
{
public:
   virtual ~ErrorHandler();

   static const char* GetTopErrorMessage();
   static int GetTopErrorCode();
   static int GetTopErrorSize();

   static void ClearTopError();

   static int GetErrorCount();

   static void PushError( int code , const std::string& error );
   static void PushHttpError( int product, int http_code , const std::string& error );

private:
   ErrorHandler();
   static ErrorHandler& GetInstance();
   ErrorHandler(const ErrorHandler& ) = delete;
   ErrorHandler& operator=(ErrorHandler&) = delete;

   std::vector<Error> m_errors;
   
};

#endif // end of include guard: _ERROR_HANDLER_H_ 
