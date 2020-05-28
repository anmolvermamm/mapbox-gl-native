#ifndef _NETWORK_ERROR_CODES_H_
#define _NETWORK_ERROR_CODES_H_


#ifdef __cplusplus
extern "C" {
#endif


//We can flesh these things out as they happen
enum HTTP_ERROR
{
   HTTP_COULDNT_RESOLVE_HOST = 6,
   HTTP_TIMEOUT = 28
};


#ifdef __cplusplus
}
#endif


#endif // end of include guard: _NETWORK_ERROR_CODES_H_ 
