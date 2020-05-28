#ifndef _VIEWER_API_H_
#define _VIEWER_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"

struct options_struct {
      bool ordering;
};

typedef struct options_struct default_options;

// Commented out products not currently in relay service production yet
enum SS_RADAR_PRODUCT {
   // Nexrad Level 2 Products
   SMIR_REFLECTIVITY_REF = -1, // May have tilt 1 - 14 (Only tilts 1 & 2 are in production)
   LV2_REFLECTIVITY_REF = 1000, // May have tilt 1 - 14 (Only tilt 1 & 2 are in production)
   LV2_VELOCITY_VEL = 2000 // May have tilt 1 - 14 (Only tilt 1 is in production)
   //LV2_SPECTRUM_WIDTH_SW = 3000,         // May have tilt 1 - 14
   //LV2_DEALIASED_VELOCITY_D_VEL = 4000,  // May have tilt 1 - 14
   //LV2_DIFFERENTIAL_REFLECT_ZDR = 5000,  // May have tilt 1 - 14
   //LV2_CORRELATION_COEF_CC = 7000,       // May have tilt 1 - 14

   // NOAA Port Products
   //NOAAPORT_REFLECTIVITY_HR_ALT_REF = 6000,           // May have tilt 1 - 6
   //NOAAPORT_VELOCITY_HR_ALT_VEL = 6006,               // May have tilt 1 - 6
   //NOAAPORT_DIGITAL_ACCUM_ARRAY_DAA = 6013,           // No Tilt Product
   //NOAAPORT_DIGITAL_STORM_TOTAL_ACCUM_DSTA = 6014,    // No Tilt Product
   //NOAAPORT_DIGITAL_ONE_HOUR_DIFF_ACCUM_DOHDA = 6015, // No Tilt Product
   //NOAAPORT_STORM_TOTAL_DIFF_ACCUM_DSTDA = 6016,      // No Tilt Product
   //NOAAPORT_SPECIFIC_DIFFERENTIAL_PHASE_KDP = 6016,   // May have tilt 1 - 6
   //NOAAPORT_DIGITAL_CORRELATION_COEF_DCC = 6022,      // May have tilt 1 - 6
   //NOAAPORT_INSTANT_RAIN_RATE_DPR = 8000,             // May have tilt 1

   // Nexrad Level 3 Products
   //LV3_BASE_REFLECTIVITY_124_NM_BR = 19,       // May have tilts 1 - 6
   //LV3_BASE_REFLECTIVITY_248_NM_BR248 = 20,    // May have tilt 1
   //LV3_BASE_VELOCITY_BV = 27,                  // May have tilts 1 - 6
   //LV3_COMPOSITE_REFLECTIVITY_248_NM_CR = 38,  // No Tilt Product
   //LV3_VOLUMETRIC_PRODUCTS_ET = 41,            // No Tilt Product
   //LV3_STORM_RELATIVE_VELOCITY_SRV = 56,       // May have tilts 1 - 3
   //LV3_VOLUMETRIC_PRODUCTS_VILS = 57,          // No Tilt Product
   //LV3_PRECIP_TOTALS_PR_1H = 78,               // No Tilt Product
   //LV3_PRECIP_TOTALS_PR_3H = 79,               // No Tilt Product
   //LV3_PRECIP_TOTALS_PR_ST = 80,               // No Tilt Product
   //LV3_HCA = 308

   // Other Countries Products

};

enum SS_RADAR_PRODUCT_TILT {
   NO_TILT = 0,
   TILT_1 = 1,
   TILT_2 = 2
   //TILT_3 = 3, // Tilt not in production
   //TILT_4 = 4, // Tilt not in production
   //TILT_5 = 5  // Tilt not in production
};

enum PALETTE_TYPE {
   ACCU_PALETTE = 0,
   NWS_PALETTE = 1
};

void viewer_init();
void viewer_init_params(default_options options);
void viewer_destroy();
bool viewer_exists();
void viewer_set_viewport( float width , float height );
void viewer_set_extents( float left , float right , float bottom, float top );
void viewer_set_screen_density(float density);
void viewer_set_mos_pos( float x , float y  , bool left_down , bool right_down );
void viewer_set_frame_time_ms( float frame_time );
void viewer_update();
void viewer_draw();
void viewer_draw_within_mapbox_layer();
void viewer_update_apikey(const char* apikey);
void viewer_need_to_draw();
void viewer_debug();

// API calls only if radar layer is added
//void viewer_change_radar_product(int moment, int tilt);
void viewer_change_radar_location(float lat, float lon);
void viewer_change_radar_product(SS_RADAR_PRODUCT product_num,
      SS_RADAR_PRODUCT_TILT tilt_num);
void viewer_change_radar_palette(PALETTE_TYPE palette_type);
unsigned int get_current_radar_time_frame_epoch(int layer_id);
const char* get_current_radar_time_frame_string(int layer_id);
void viewer_set_ss_radar_max_frame_count(int max_frame_count);
unsigned int viewer_get_ss_past_span_time();

// API looping calls
void viewer_update_times(unsigned int past_time, unsigned int now_time,
      unsigned int current_loop_time);

void viewer_enable_layer(int layer_id, bool enable);
//bool viewer_add_skyguard_layer(int layer_id);
bool viewer_add_wind_layer(int layer_id);
bool viewer_add_radar_layer(int layer_id);
bool viewer_add_custom_layer(int layer_id);
bool viewer_add_animation_layer(int layer_id, const char* animation_stylization,
      int styliztion_size);
void viewer_clear_custom_layer(int layer_id);
void viewer_remove_layer(int layer_id);

void viewer_add_to_custom_layer(int layer_id, const char* geo_json_data, int geo_size,
      const char* stylization_json_data, int stylization_size);
void viewer_remove_render_item_from_layer(int layer_id, int item_id);

void viewer_register_icon_to_custom_layer(int layer_id, int icon_id,
      const char* icon_png, unsigned int size);
void viewer_add_icon_to_custom_layer(int layer_id, int icon_id,
      const char* geo_json_data, int geo_size, const char* stylization_json_data,
      int stylization_size);
void viewer_remove_icon_from_custom_layer(int layer_id, int item_id);
//void viewer_remove_object_from_geojson(int layer_id, int item_id);

//void viewer_add_render_item_to_layer(int layer_id, int item_id,
//      float z_order, float* vertices, int vertices_len, float* color,
//      int color_len);

void viewer_enable_mutual_exclusion( bool enable );

int get_viewer_error_code();
int get_viewer_error_size();
const char * get_viewer_error_msg();
int get_viewer_error_count();
void clear_viewer_error();
int get_unique_id();

#ifdef __cplusplus
}
#endif


#endif // end of include guard: _LIB_API_H_ 
