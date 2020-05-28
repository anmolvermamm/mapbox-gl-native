#ifndef _ERROR_CODE_H_
#define _ERROR_CODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "network_error_codes.h"

#define LAYEROFFSET 16

enum LAYERERROR {
  RADARERROR     =  1 << (LAYEROFFSET + 0), 
  WINDERROR      =  1 << (LAYEROFFSET + 1), 
  SKYGUARDERROR  =  1 << (LAYEROFFSET + 2), 
};

#define ERRORTYPEOFFSET 0

enum ERRORTYPE {
  NETWORKERROR     =  1<< (ERRORTYPEOFFSET + 0), 
  INITERROR        =  1<< (ERRORTYPEOFFSET + 1), 
  NODATAERROR      =  1<< (ERRORTYPEOFFSET + 2), 
  TIMEOUTERROR     =  1<< (ERRORTYPEOFFSET + 3), 
  BADHOSTERROR     =  1<< (ERRORTYPEOFFSET + 4), 
  OTHERERROR       =  1<< (ERRORTYPEOFFSET + 5), 
};



#define RADARNETWORKERROR RADARERROR + NETWORKERROR
#define WINDNETWORKERROR WINDERROR + NETWORKERROR
#define SKYGUARDNETWORKERROR SKYGUARDERROR + NETWORKERROR


ERRORTYPE inline http_error_type( int http_error_code )
{
   switch( http_error_code )
   {
   case HTTP_TIMEOUT:
       return TIMEOUTERROR; 
   case HTTP_COULDNT_RESOLVE_HOST:
       return BADHOSTERROR; 
   default:
       return NETWORKERROR;
   }
}


#ifdef __cplusplus
}
#endif

#endif // end of include guard: _ERROR_CODE_H_ 
