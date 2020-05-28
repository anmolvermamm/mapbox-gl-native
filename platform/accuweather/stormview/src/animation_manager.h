#ifndef _ANIMATION_MANAGER_H_
#define _ANIMATION_MANAGER_H_

#include <memory>
#include <set>

#include "geo_json_parser.h"

struct AnimationStyle {
      AnimationStyle(int id, float milliseconds, float opacity, float fill_opacity)
            : id(id), milliseconds(milliseconds), opacity(opacity), fill_opacity(fill_opacity)
      {

      }

      int id;
      float milliseconds;
      float opacity;
      float fill_opacity;
};

class AnimationManager
{
   public:
      AnimationManager(nlohmann::json animation_stylization);
      void UpdateTime(float milliseconds);
      virtual ~AnimationManager();

      float GetCurrentOpacity();
      float GetCurrentFillOpacity();

   private:
      std::set<std::unique_ptr<AnimationStyle>, bool (*)(const std::unique_ptr<AnimationStyle>&,
            const std::unique_ptr<AnimationStyle>&)> m_animation_style;

      float m_current_milliseconds = 0;
};

#endif // end of include guard: _ANIMATION_MANAGER_H_
