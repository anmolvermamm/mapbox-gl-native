#ifndef _DEFAULT_RENDERER_H_
#define _DEFAULT_RENDERER_H_

#include <vector>
#include <deque>
#include "color.h"
#include "pointf.h"
#include <memory>
#include "png_image.h"
#include "animation_manager.h"
//#include "ext/json.hpp"

class DefaultVBO;

class DefaultRenderer
{
   public:
      DefaultRenderer();
      virtual ~DefaultRenderer();

      virtual void Init(const char* animation_stylization, int stylization_size);
      virtual void Update(float milliseconds);
      virtual void Draw();

      //virtual void AddRenderItem(int item_id, float z_order,
      //      std::vector<float> vectors, std::vector<float> color);
      virtual void RemoveRenderItem(int item_id);
      void AddToCustomLayer(const char* geo_json_data, int geo_size,
            const char* stylization_json_data, int stylization_size);
      void RegisterIconToCustomLayer(int icon_id, const char *icon_png, unsigned int size);
      void AddIconToCustomLayer(int icon_id, const char *geo_json_data, int geo_size,
            const char *stylization_json_data, int stylization_size);
      void RemoveIconFromCustomLayer(int item_id);
      void ClearCustomLayer();

      std::unique_ptr<DefaultVBO> m_vbo_manager;
      std::unique_ptr<AnimationManager> m_animation_manager;

      std::deque<nlohmann::json> m_process_json_queue;
      std::deque<nlohmann::json> m_process_json_icon_queue;

      std::map<int, size_t> m_style_point_map;
      std::map<int, size_t> m_style_line_map;
      std::map<int, size_t> m_style_poly_map;
      std::map<int, size_t> m_style_icon_map;
};

#endif // end of include guard: _DEFAULT_RENDERER_H_
