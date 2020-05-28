#include "gl_debug.h"
#include "gl_include.h"

unsigned int errCode;

const char* getErrorString(unsigned int errCode)
{
   return 
      errCode == GL_NO_ERROR ?  "GL_NO_ERROR" :
      errCode == GL_INVALID_ENUM ? "GL_INVALID_ENUM" :
      errCode == GL_INVALID_VALUE ? "GL_INVALID_VALUE" :
      errCode == GL_INVALID_OPERATION ? "GL_INVALID_OPERATION" :
      errCode == GL_INVALID_FRAMEBUFFER_OPERATION ? "GL_INVALID_FRAMEBUFFER_OPERATION" :
      errCode == GL_OUT_OF_MEMORY ? "GL_OUT_OF_MEMORY" 
                                  : "UKNOWN";
}
