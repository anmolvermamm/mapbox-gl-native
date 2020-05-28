#ifndef _DEFAULT_LAYER_H_
#define _DEFAULT_LAYER_H_

#include <vector>
#include <memory>

#include "default_renderer.h"
#include "layer_types.h"
#include "pointf.h"

class World;

class DefaultLayer
{
   public:
      DefaultLayer(int layer_id);
      virtual ~DefaultLayer();

      virtual void Init(const char* animation_stylization = "",
            int stylization_size = 0);
      virtual void Update(World* world);
      virtual void Draw(World* world);
      virtual bool IsLayer(LayerTypes type);

      virtual void SetEnabled(bool enable);
      virtual bool GetLayerEnabled();
      virtual int GetLayerId() const;
      virtual void SetLayerId(int);
      virtual bool IsAnimatedLayer();
      virtual bool IsInitialized();
      virtual unsigned int GetCurrentTimeFrameEpoch();
      virtual const char* GetCurrentTimeFrameString();

      //virtual bool AddRenderItem(int item_id, float z_order,
      //      std::vector<float> vectors, std::vector<float> color);
      virtual void RemoveRenderItem(int item_id);
      void AddToCustomLayer(const char* geo_json_data, int geo_size,
            const char* stylization_json_data, int stylization_size);
      void RegisterIconToCustomLayer(int icon_id, const char *icon_png, unsigned int size);
      void AddIconToCustomLayer(int icon_id, const char *geo_json_data, int geo_size,
            const char *stylization_json_data, int stylization_size);
      void RemoveIconFromCustomLayer(int item_id);
      void ClearCustomLayer();

      bool operator< (const DefaultLayer& right) const
      {
         return m_layer_id < right.m_layer_id;
      }

   protected:
      bool m_enabled = false;
      bool m_initialized = false;
      bool m_is_animated = false;
      int m_layer_id;

      std::unique_ptr<DefaultRenderer> m_default_renderer;
};

#endif // end of include guard: _DEFAULT_LAYER_H_
