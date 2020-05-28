#ifndef _GL_INCLUDE_H_
#define _GL_INCLUDE_H_

#include "logging.h"

//Is this right??
#ifdef __APPLE__
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

#elif __ANDROID__
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#else
// Desktop
#include <GL/glew.h>
#endif

#endif // end of include guard: _GL_INCLUDE_H_ 

