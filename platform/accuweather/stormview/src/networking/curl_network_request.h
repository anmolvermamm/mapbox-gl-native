
#ifndef _CURL_NETWORK_REQUEST_H_
#define _CURL_NETWORK_REQUEST_H_

#include <string>
#include <unordered_map>
#include "network_callbacks.h"
#include "network_headers.h"

typedef void* CURLMptr;

class CurlNetworkRequest
{
public:
   CurlNetworkRequest();
   virtual ~CurlNetworkRequest();

   virtual void MakeNetworkRequest( const std::string& url , const SuccessCallback& success ,
        const ErrorCallback& error );

   virtual void MakeNetworkRequest( 
        const std::string& url , 
        const NetworkHeaders& headers ,
        const SuccessCallback& success ,
        const ErrorCallback& error );

   virtual void PollNetwork();

protected:
   virtual void ProcessNetwork();
   void* SetupHandle(const std::string& url);
   void* AddHandle(void *handle);
   void* AddNetworkHeaders(void* handle , const NetworkHeaders& headers );

   void* AppendNetworkRequest(const std::string& url);
   void* AppendNetworkRequest(
         const NetworkHeaders& headers ,
         const std::string& url
         );

private:
   CURLMptr m_multi_handle;
   unsigned int m_max_connections = 10;
   int m_num_requests = 0;

   std::unordered_map<void*,NetworkCallbacks> m_requests;
   std::unordered_map<void*,void*> m_headers;
   //std::unordered_map<void*,NetworkRequest> m_requests;
   //std::unordered_map<void*,NetworkResponse> m_response;
   
   
};

#endif // end of include guard: _CURL_NETWORK_REQUEST_H_ 
