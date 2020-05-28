#ifndef _RADIAL_LAYER_H_
#define _RADIAL_LAYER_H_


#include "default_layer.h"
#include "../world.h"
#include <memory>
#include <vector>

class RadialManager;
class SiteInfo;
class ByteArray;

class RadialLayer : public DefaultLayer
{
public:
   RadialLayer(unsigned int layer_id);
   virtual ~RadialLayer();

   virtual void Init(const char* flashing_stylization = "",
         int stylization_size = 0) override;
   virtual void Update(World  * ) override;
   virtual void Draw(World *) override;
   virtual bool IsLayer(LayerTypes type)override;
   virtual void SetEnabled(bool enable)override;
   virtual unsigned int GetCurrentTimeFrameEpoch()override;
   virtual const char* GetCurrentTimeFrameString()override;


   void RequestSites();
   void RequestSiteInfo(const std::string& site_list);

   void ReceiveSites(const ByteArray& data );
   void ReceiveSitesInfo(const ByteArray& data );

   void SetRadarProduct();

   //const std::vector<SiteInfo>& info 

   void ReceiveError(int code, const std::string& error );

private:
   std::unique_ptr<RadialManager> m_radial_manager;

   int m_radar_product = 0;
   int m_product_num = 0;
   int m_tilt_num = 0;
};



#endif // end of include guard: _RADIAL_LAYER_H_ 
