#include "curl_network_request.h"

#include <sstream>
#include <iostream>

#include "logging.h"

#ifdef __APPLE__
#include "../../curl/curl.h"
#else
#include "curl/curl.h"
#endif

#include "network_headers.h"


size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
   //((std::string*)stream)->append((char*)ptr, size * nmemb);
   auto buffer = (std::vector<char>*)stream;
   //buffer->reserve( size * nmemb );
   buffer->insert( buffer->end() , (char*)ptr , (char*)ptr + size * nmemb );
   return size * nmemb;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
CurlNetworkRequest::CurlNetworkRequest()
{
   m_multi_handle = curl_multi_init(); 
   if( !m_multi_handle )
   {
      printf("Error initializing curl\n");
   }
   curl_multi_setopt(m_multi_handle, CURLMOPT_MAXCONNECTS, (long)m_max_connections);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
CurlNetworkRequest::~CurlNetworkRequest()
{
   curl_multi_cleanup( m_multi_handle ); 
   curl_global_cleanup();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void CurlNetworkRequest::MakeNetworkRequest( const std::string& url , const SuccessCallback& success ,
const ErrorCallback& error )
{
   ++m_num_requests;

   auto key = AppendNetworkRequest( url );

   auto& request = m_requests[key];
   request.success_callback = success;
   request.error_callback = error;
   //m_requests.insert( std::make_pair( key , std::move(callbacks) ) );
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void CurlNetworkRequest::MakeNetworkRequest( 
const std::string& url , 
const NetworkHeaders& headers ,
const SuccessCallback& success ,
const ErrorCallback& error )
{
   ++m_num_requests;

   auto key = AppendNetworkRequest( 
         headers, url );

   auto& request = m_requests[key];
   request.success_callback = success;
   request.error_callback = error;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void CurlNetworkRequest::PollNetwork()
{
   static int prev_read = 0; 
   int this_read;
   curl_multi_perform(m_multi_handle, &this_read);

   long L;
   int M = -1;
   fd_set R, W, E;

   FD_ZERO(&R);
   FD_ZERO(&W);
   FD_ZERO(&E);

   if(curl_multi_fdset(m_multi_handle, &R, &W, &E, &M)) 
   {
      printf("Curl Error?\n");
   } 

   if(curl_multi_timeout(m_multi_handle, &L)) 
   {
      printf("Curl Error?\n");
   }

   if( prev_read != this_read )
   {
      ProcessNetwork();
   } 
   prev_read = this_read;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void CurlNetworkRequest::ProcessNetwork()
{
   CURLMsg *msg;
   int running;

   while(( msg = curl_multi_info_read( m_multi_handle , &running )) ) {

      if( msg->msg != CURLMSG_DONE )
      {
         printf("Message is not done\n");
         continue;
      }

      char *last_url;
      CURL *e = msg->easy_handle;
      
      curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &last_url);
      std::string eff_url = last_url;

      long response_code;
      curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &response_code);

      int error = msg->data.result;
      std::string err_str = curl_easy_strerror(msg->data.result);

      auto request_iter = m_requests.find(e);

      if(  request_iter == m_requests.end() )
      {
         printf("Error, could not look up Curl Request");
         continue;
      }

      auto& response = request_iter->second;
      auto& data = response.data;

      if( !error )
      {
         response.success_callback( data );
      } else {
         response.error_callback( error, err_str );
      }
      m_requests.erase( e );


      auto i_h = m_headers.find(e);
      if( i_h != m_headers.end() )
      {
         struct curl_slist *list = 
            (curl_slist*)i_h->second;     
         curl_slist_free_all(list);
         //std::cout << "Free memory: " << list << "\n";
         i_h = m_headers.erase( i_h );
      }

      curl_multi_remove_handle(m_multi_handle, e);
      curl_easy_cleanup(e);
      
      --m_num_requests;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void* CurlNetworkRequest::SetupHandle(const std::string& url)
{
   auto easy_handle = curl_easy_init(); 

   curl_easy_setopt(easy_handle, CURLOPT_URL, url.c_str());
   curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
   curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);
   curl_easy_setopt(easy_handle, CURLOPT_NOSIGNAL, 1);
   curl_easy_setopt(easy_handle, CURLOPT_ACCEPT_ENCODING, "deflate");
   curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, 30L);

   return easy_handle;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void* CurlNetworkRequest::AddHandle(void *handle)
{
   //decltype( curl_easy_init ) easy_handle = handle;
   CURL* easy_handle = handle;

   curl_multi_add_handle( m_multi_handle , easy_handle );

   auto& response = m_requests[easy_handle];
   auto& out = response.data;
   curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, (void *)&out); 

   return easy_handle;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void* CurlNetworkRequest::AddNetworkHeaders(void* handle , const NetworkHeaders& headers )
{
   CURL* easy_handle = handle;
   struct curl_slist *list = NULL;

   for( const auto& item : headers )
   {
      auto str = std::string{ item.first + ": " + item.second };
      list = curl_slist_append(list, str.c_str() );
   }

   if( !list )
   {
      printf("Created curl header list???\n");
   } 
   //else
   //{
   //   std::cout << "Alloc memory: " << list << "\n";
   //}
 
   curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, list);
 
   m_headers[easy_handle] = list;

   return handle;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void* CurlNetworkRequest::AppendNetworkRequest(const std::string& url)
{
   auto handle = SetupHandle( url );
   AddHandle( handle );
   return handle;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void* CurlNetworkRequest::AppendNetworkRequest(
const NetworkHeaders& headers ,
const std::string& url
)
{
   auto handle = SetupHandle( url );
   AddNetworkHeaders(  handle , headers );
   AddHandle( handle );
   return handle;
}

