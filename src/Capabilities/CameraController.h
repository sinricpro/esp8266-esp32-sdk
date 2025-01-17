#pragma once

#include "../SinricProRequest.h"

#include "../EventLimiter.h"
#include "../SinricProStrings.h"
#include "../SinricProNamespace.h"

#include <FS.h>

#if defined(ESP32)  
  #include <HTTPClient.h>  
  #include <WiFiClientSecure.h>
#endif

namespace SINRICPRO_NAMESPACE {

FSTR(CAMERA, getSnapshot);  // "getSnapshot"
FSTR(CAMERA, POST);         // "POST"

using SnapshotCallback = std::function<bool(const String &)>;

/**
 * @brief CameraController class for managing camera operations in SinricPro
 * @ingroup Capabilities
 **/
template <typename T>
class CameraController {
  public:
    CameraController();

    /**
     * @brief Sets the callback function for snapshot requests
     * @param cb Callback function to handle snapshot requests
     */
    void onSnapshot(SnapshotCallback cb);

    /**
     * @brief Sends a camera snapshot to the SinricPro server
     * @param buffer Pointer to the image data buffer
     * @param len Length of the image data in bytes
     * @return error code
     */
    int sendSnapshot(uint8_t* buffer, size_t len);

    /**
     * @brief Sends motion detection data from file
     * @param fs Filesystem containing the motion data
     * @param path Path to motion data file
     * @return error code
     */
    int sendMotion(fs::FS &fs, const char * path);
    

  protected:
    /**
     * @brief Handles incoming camera control requests
     * @param request The incoming request object
     * @return true if request was handled successfully, false otherwise
     */
    bool handleCameraController(SinricProRequest &request);

  private:
    SnapshotCallback getSnapshotCallback = nullptr;
    EventLimiter event_limiter;

#if defined(ESP32)
    std::unique_ptr<WiFiClient> createClient() {
#ifdef SINRICPRO_NOSSL
        auto client = std::make_unique<WiFiClient>();
#else
        auto client = std::make_unique<WiFiClientSecure>();
        client->setInsecure();
#endif
        if (client) client->setTimeout(TCP_CONNECTION_TIMEOUT_VALUE);
        return client;
    }

    bool setupHttpConnection(HTTPClient& http, WiFiClient* client, const char* path) {
#ifdef SINRICPRO_NOSSL
        return http.begin(*client, SINRICPRO_CAMERA_URL, 80, path, false);
#else
        return http.begin(*client, SINRICPRO_CAMERA_URL, 443, path, true);
#endif
    }

    void setupHttpHeaders(HTTPClient& http, T* device) {
        const String& deviceId = device->getDeviceId();
        String createdAt = String(device->getTimestamp());
        String signature = device->sign(deviceId + createdAt);

        http.addHeader(FSTR_SINRICPRO_deviceId, deviceId);
        http.addHeader(FSTR_SINRICPRO_createdAt, createdAt);
        http.addHeader(FSTR_SINRICPRO_signature, signature);
        http.setTimeout(HTTP_TIMEOUT_VALUE);
    } 
#endif
};

template <typename T>
CameraController<T>::CameraController()
: event_limiter (EVENT_LIMIT_STATE) {
    T *device = static_cast<T *>(this);
    device->registerRequestHandler(std::bind(&CameraController<T>::handleCameraController, this, std::placeholders::_1));
}

template <typename T>
void CameraController<T>::onSnapshot(SnapshotCallback cb) {
    getSnapshotCallback = cb;
}

template <typename T>
bool CameraController<T>::handleCameraController(SinricProRequest &request) {
    if (event_limiter) return false;
    T *device = static_cast<T *>(this);
    bool success = false;

    // Handle getSnapshot action
    if (request.action == FSTR_CAMERA_getSnapshot) {
        if (getSnapshotCallback) {
            success = getSnapshotCallback(device->deviceId);
        }
    }
    return success;
}

template <typename T>
int CameraController<T>::sendSnapshot(uint8_t* buffer, size_t len) {
    
#if defined(ESP32)
    if (!buffer || len == 0) return -1;

    T* device = static_cast<T*>(this);
    HTTPClient http;
    auto client = createClient();
        
    if (!setupHttpConnection(http, client.get(), SINRICPRO_CAMERA_API_SNAPSHOT_PATH)) {
        http.end();
        return -1;
    }

    setupHttpHeaders(http, device);

    int resCode = http.POST(buffer, len);
    http.end();
    
    return resCode;
#else
    return -1;
#endif  
}
 
template <typename T>
int CameraController<T>::sendMotion(fs::FS &fs, const char * path) {
    
#if defined(ESP32)
    File file = fs.open(path);
    if (!file) return -1;

    T* device = static_cast<T*>(this);
    HTTPClient http;
    auto client = createClient();
        
    if (!setupHttpConnection(http, client.get(), SINRICPRO_CAMERA_API_MOTION_PATH)) {
        http.end();
        return -1;
    }

    setupHttpHeaders(http, device);

    int resCode = http.sendRequest(FSTR_CAMERA_POST, &file, file.size());
    
    file.close();
    http.end();

    return resCode;
#else
    return -1;
#endif
}

}  // namespace SINRICPRO_NAMESPACE 

template <typename T>
using CameraController = SINRICPRO_NAMESPACE::CameraController<T>;