#ifndef _BASIC_SPRITE_H_
#define _BASIC_SPRITE_H_

#include "glow.h"
#include "glow_shader_mem.h"
#include "gl_debug.h"
#include "gl_include.h"
#include "../animation_manager.h"

#include "glm/fwd.hpp"
#include "color.h"
#include "default_vbo.h"

class PointF;

class BasicSprite
{
   public:
      static bool InitBasicSprite();
      static void DeleteBasicSprite();

      static bool Draw(const glm::mat4x4 &mat, unsigned buffer, unsigned element_buffer,
            unsigned count, PrimitiveStyle style,
            std::unique_ptr<AnimationManager> &animation_manager);

   private:
      static BasicSprite& Get();
      BasicSprite();
      BasicSprite(const BasicSprite&) = delete;
      BasicSprite& operator=(BasicSprite&) = delete;

      bool m_init = false;
      unsigned int m_program = 0;

      unsigned int m_mat_uloc;
      unsigned int m_color_uloc;
      unsigned int m_translate_uloc;

      unsigned int m_vert_aloc;
      unsigned int m_rect_buffer = 0;
      unsigned int m_rect_aloc;
};

#endif // end of include guard: _BASIC_SPRITE_H_
