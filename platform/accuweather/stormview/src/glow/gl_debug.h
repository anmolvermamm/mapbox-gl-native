#ifndef _GL_DEBUG_H_
#define _GL_DEBUG_H_


#define GL_DEBUG_MACRO() \
   if ((errCode = glGetError()) != GL_NO_ERROR) \
   { \
      printf("%s %d : A gl error occured: %s\n", __FILE__, __LINE__, \
            getErrorString(errCode)); \
   }

#define GL_DEBUG_CLEAR() \
   glGetError() 

extern unsigned int errCode;

const char* getErrorString(unsigned int errCode);

#endif // end of include guard: _GL_DEBUG_H_ 
