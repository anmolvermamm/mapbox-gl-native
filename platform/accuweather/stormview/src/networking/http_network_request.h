#ifndef _HTTP_NETWORK_REQUEST_H_
#define _HTTP_NETWORK_REQUEST_H_

#include <functional>
#include <memory>
#include "byte_array.h"

#include "network_callbacks.h"
#include "network_headers.h"

class CurlNetworkRequest;

class HttpNetworkRequest
{
public:
   virtual ~HttpNetworkRequest();

   static void RequestData( const std::string& url , const SuccessCallback& success ,
        const ErrorCallback& error ); 

   static void RequestData( const std::string& url , 
        const NetworkHeaders& headers ,
        const SuccessCallback& success ,
        const ErrorCallback& error ); 

   static void PollNetwork();
   static void DeleteHttpNetworkRequest();
   static void StartHttpNetworkRequest();

   //We do not have headers at this point
   //static void RequestData( const std::string& url , const RequestHeaders, 
   //      const SuccessCallback& success , const ErrorCallback& error ); 


private:
   static HttpNetworkRequest& GetInstance();
   HttpNetworkRequest();
   HttpNetworkRequest(const HttpNetworkRequest&) = delete;
   HttpNetworkRequest(HttpNetworkRequest&&) = delete;
   HttpNetworkRequest& operator=(const HttpNetworkRequest&) = delete;
   HttpNetworkRequest& operator=(HttpNetworkRequest&&) = delete;


   std::unique_ptr<CurlNetworkRequest> m_network;

   
};


#endif // end of include guard: _HTTP_NETWORK_REQUEST_H_ 
