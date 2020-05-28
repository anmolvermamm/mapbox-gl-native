#ifndef _NETWORK_CALLBACKS_H_
#define _NETWORK_CALLBACKS_H_

#include "byte_array.h"
#include <functional>
#include <memory>

typedef std::function<void(const ByteArray&)> SuccessCallback;
typedef std::function<void(int code, const std::string&)> ErrorCallback;

struct NetworkCallbacks
{
   SuccessCallback success_callback;
   ErrorCallback error_callback;
   ByteArray data;
};


#endif // end of include guard: _NETWORK_CALLBACKS_H_ 
