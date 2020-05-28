#include "http_network_request.h"
#include "curl_network_request.h"



/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
HttpNetworkRequest::~HttpNetworkRequest()
{
   auto& me = GetInstance();
   me.m_network.reset();
   me.m_network.release();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void HttpNetworkRequest::RequestData( const std::string& url , const SuccessCallback& success ,
const ErrorCallback& error )
{
   auto& me = GetInstance();
   me.m_network->MakeNetworkRequest( url , success , error );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void HttpNetworkRequest::RequestData( const std::string& url , 
const NetworkHeaders& headers ,
const SuccessCallback& success ,
const ErrorCallback& error )
{
   auto& me = GetInstance();
   me.m_network->MakeNetworkRequest( url , headers, success , error );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void HttpNetworkRequest::PollNetwork()
{
   auto& me = GetInstance();
   me.m_network->PollNetwork();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
HttpNetworkRequest& HttpNetworkRequest::GetInstance()
{
   static HttpNetworkRequest the_instance;
   return the_instance;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
HttpNetworkRequest::HttpNetworkRequest()
{
   m_network = std::make_unique<CurlNetworkRequest>(); 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void HttpNetworkRequest::DeleteHttpNetworkRequest()
{
   auto& me = GetInstance();
   me.m_network.reset();
   me.m_network.release();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void HttpNetworkRequest::StartHttpNetworkRequest()
{
   auto& me = GetInstance();
   if(!me.m_network)
      me.m_network = std::make_unique<CurlNetworkRequest>();
}
