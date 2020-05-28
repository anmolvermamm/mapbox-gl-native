#include <GLES2/gl2.h>
#include <android/log.h>
#include <jni.h>
#include <mbgl/gl/custom_layer.hpp>
#include <sstream>
#include <mbgl/util/timer.hpp>
#include "viewer_api.h"
#include "math.h"
#include <chrono>
#include <ctime>
#include <math.h>

const char *LOG_TAG = "LocalRadar Layer";

class LocalRadarLayer : mbgl::style::CustomLayerHost {
public:
    ~LocalRadarLayer() {
    }

    void initialize() {
        __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "Initialize");
        if (!viewer_exists()) {
            viewer_init();
            viewer_set_screen_density(density);
            viewer_change_radar_product(SMIR_REFLECTIVITY_REF, TILT_1);
            viewer_add_radar_layer(14);
            viewer_enable_layer(14, true);
        }
    }

    static void setExtents(double top, double right, double bottom, double left) {
        if (mLeft != left || mRight != right || mBottom != bottom || mTop != top) {
            mLeft = static_cast<float>(left);
            mRight = static_cast<float>(right);
            mTop = static_cast<float>(top);
            mBottom = static_cast<float>(bottom);
            viewer_set_extents(mLeft, mRight, mTop, mBottom);
            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                                "latlngbounds top, right, bottom,left %lf  %lf  %lf  %lf", top,
                                right, bottom, left);

        }
    }

    void render(const mbgl::style::CustomLayerRenderParameters &parameters) {
        __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "inside Render");

        viewer_set_frame_time_ms(60);
        if (viewer_exists()) {
            viewer_update();
            viewer_draw();
            get_viewer_error_msg();
            if (parameters.zoom < 6) {
                __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "not Rendering parameters.zoom < 6");
                return;
            } else {
                __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "Rendering because zoom");
            }
            viewer_set_viewport(parameters.width, parameters.height);
            canChangeRadarLocation(parameters);
            canUpdateViewerTimes();
        }
    }

    void canUpdateViewerTimes() {
        time_t result = time(nullptr);
        long currentEpochTime = result;
        int loopTime = currentEpochTime - ((1.0f - currentPercent()) * pastTime);
        viewer_update_times(pastTime, currentEpochTime,
                            loopTime);
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%li", currentEpochTime);

        int radarTime = get_current_radar_time_frame_epoch(14);
        selectedTime = radarTime * 1000;
    }

    static void setIndex(int index) {
        float incrementPercent = 0.05f;
        percent = index * (0.05f);
        float next = currentPercent() + incrementPercent;
        if (next >= 1.0f + incrementPercent) {
            percent = 0.0f;
        }
    }

    static int currentIndex() {
        float some = fmod(percent, 1.0f);
        if (some == 0.0f) {
            return static_cast<int>(round(percent));
        } else {
            return static_cast<int>(round(percent)) + 1;
        }
    }

    void canChangeRadarLocation(const mbgl::style::CustomLayerRenderParameters &parameters) {
        if (lastLat == 0) {
            updateChangeRadarLocation(parameters);
        } else {
            // distance greater than 10 kms
            if (distance(lastLat, lastLng, parameters.latitude, parameters.longitude) >
                MIN_DISTANCE) {
                updateChangeRadarLocation(parameters);
            }
        }
    }

    void updateChangeRadarLocation(const mbgl::style::CustomLayerRenderParameters &parameters) {
        updateLocationParams(parameters);
        viewer_change_radar_location(latToMer(parameters.latitude),
                                     longitudeToMercator(parameters.longitude));
    }

    void updateLocationParams(const mbgl::style::CustomLayerRenderParameters &parameters) {
        lastLat = parameters.latitude;
        lastLng = parameters.longitude;
    }

    static float currentPercent() {
        return std::max(0.0F, std::min(1.0F, percent));
    }

    // Utility function for
    // converting degrees to radians
    long double toRadians(const long double degree) {
        // cmath library in C++
        // defines the constant
        // M_PI as the value of
        // pi accurate to 1e-30
        long double one_deg = (M_PI) / 180;
        return (one_deg * degree);
    }

    long double distance(long double lat1, long double long1,
                         long double lat2, long double long2) {
        // Convert the latitudes
        // and longitudes
        // from degree to radians.
        lat1 = toRadians(lat1);
        long1 = toRadians(long1);
        lat2 = toRadians(lat2);
        long2 = toRadians(long2);

        // Haversine Formula
        long double dlong = long2 - long1;
        long double dlat = lat2 - lat1;

        long double ans = pow(sin(dlat / 2), 2) +
                          cos(lat1) * cos(lat2) *
                          pow(sin(dlong / 2), 2);

        ans = 2 * asin(sqrt(ans));

        // Radius of Earth in
        // Kilometers, R = 6371
        // Use R = 3956 for miles
        long double R = 6371;

        // Calculate the result
        ans = ans * R;

        return ans;
    }

    float latToMer(double lat) {
        float radLat = static_cast<float>(lat * (M_PI / 180.0));
        return static_cast<float>(0.5 * log((1.0 + sin(radLat)) / (1.0 - sin(radLat))));
    }

    float longitudeToMercator(double lon) {
        return static_cast<float>((M_PI / 180.0) * (lon));
    }


    void contextLost() {
        __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "ContextLost");
    }

    void deinitialize() {
        __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "DeInitialize");
        viewer_enable_layer(14, false);
        viewer_destroy();
    }

    int pastTime = 3600;
    static float percent;
    static float density;

    static float mRight;
    static float mLeft;
    static float mBottom;
    static float mTop;

    long double MIN_DISTANCE = 10;
    double lastLat = 0;
    double lastLng = 0;
    static long selectedTime;
};

float  LocalRadarLayer::mLeft = 0.0f;
float  LocalRadarLayer::mRight = 0.0f;
float  LocalRadarLayer::mBottom = 0.0f;
float  LocalRadarLayer::mTop = 0.0f;
float  LocalRadarLayer::density = 0.0f;
float  LocalRadarLayer::percent = 0.0f;
long   LocalRadarLayer::selectedTime = 0;

extern "C" JNIEXPORT jlong JNICALL
Java_com_mapbox_mapboxsdk_testapp_model_customlayer_MapBoxLocalRadarLayer_createContext(JNIEnv *,
                                                                                        jobject,
                                                                                        jfloat density) {
    __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "nativeCreateContext");
    auto localRadarLayer = std::make_unique<LocalRadarLayer>();
    LocalRadarLayer::density = density;
    return reinterpret_cast<jlong>(localRadarLayer.release());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_mapbox_mapboxsdk_testapp_model_customlayer_MapBoxLocalRadarLayer_setExtents(JNIEnv *,
                                                                                     jobject,
                                                                                     jdouble top,
                                                                                     jdouble right,
                                                                                     jdouble bottom,
                                                                                     jdouble left) {
    if (viewer_exists()) {
        LocalRadarLayer::setExtents(top, right, bottom, left);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_mapbox_mapboxsdk_testapp_model_customlayer_MapBoxLocalRadarLayer_setIndex(JNIEnv *,
                                                                                   jobject,
                                                                                   jint index) {
    if (viewer_exists()) {
        LocalRadarLayer::setIndex(index);
    }
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_com_mapbox_mapboxsdk_testapp_model_customlayer_MapBoxLocalRadarLayer_currentPercent(
        JNIEnv *, jobject) {
    return LocalRadarLayer::currentPercent();
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_mapbox_mapboxsdk_testapp_model_customlayer_MapBoxLocalRadarLayer_currentIndex(JNIEnv *,
                                                                                       jobject) {
    return LocalRadarLayer::currentIndex();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_mapbox_mapboxsdk_testapp_model_customlayer_MapBoxLocalRadarLayer_setPercentage(JNIEnv *,
                                                                                        jobject,
                                                                                        jfloat percent) {
    LocalRadarLayer::percent = percent;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_mapbox_mapboxsdk_testapp_model_customlayer_MapBoxLocalRadarLayer_getCurrentDate(
        JNIEnv *, jobject) {
    return LocalRadarLayer::selectedTime;
}