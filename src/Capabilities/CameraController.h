#pragma once

// Required header includes
#include "../EventLimiter.h"
#include "../SinricProNamespace.h"
#include "../SinricProRequest.h"
#include "../SinricProStrings.h"
#include <HTTPClient.h>

#if defined(ESP32)
  #include <WiFiClientSecure.h>
#endif

namespace SINRICPRO_NAMESPACE {

FSTR(CAMERA, getSnapshot);  // "getSnapshot"

using SnapshotCallback = std::function<bool(const String &)>;

/**
 * @brief Camera
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
     * @return HTTP response code
     */
    int sendSnapshot(uint8_t* buffer, size_t len);

  protected:
    /**
     * @brief Handles incoming camera control requests
     * @param request The incoming request object
     * @return true if request was handled successfully, false otherwise
     */
    bool handleCameraController(SinricProRequest &request);

  private:
    // Callback handler for snapshot requests
    SnapshotCallback getSnapshotCallback = nullptr;
};

template <typename T>
CameraController<T>::CameraController() {
    T *device = static_cast<T *>(this);
    device->registerRequestHandler(std::bind(&CameraController<T>::handleCameraController, this, std::placeholders::_1));
}

template <typename T>
void CameraController<T>::onSnapshot(SnapshotCallback cb) {
    getSnapshotCallback = cb;
}

template <typename T>
bool CameraController<T>::handleCameraController(SinricProRequest &request) {
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
    int resCode = -1;

#if defined(ESP32)
    T *device = static_cast<T *>(this);
    
    // Validate input buffer
    if (!buffer) return resCode;

    HTTPClient http;
    bool beginSuccess = false;

    #ifdef SINRICPRO_NOSSL
        WiFiClient *client = new WiFiClient();
        if (!client) return resCode;
        
        beginSuccess = http.begin(*client, SINRICPRO_CAMERA_URL, 80, SINRICPRO_CAMERA_PATH, false);
    #else
        WiFiClientSecure *secureClient = new WiFiClientSecure();
        if (!secureClient) return resCode;
        
        secureClient->setInsecure(); // Skip certificate validation
        beginSuccess = http.begin(*secureClient, SINRICPRO_CAMERA_URL, 443, SINRICPRO_CAMERA_PATH, true);
    #endif

    if (!beginSuccess) {
        http.end();
        return resCode;
    }

    const String& deviceId = device->getDeviceId();
    String createdAt = String(device->getTimestamp());
    String signature = device->sign(deviceId+createdAt);

    http.addHeader(FSTR_SINRICPRO_deviceId, deviceId);
    http.addHeader(FSTR_SINRICPRO_createdAt, createdAt);
    http.addHeader(FSTR_SINRICPRO_signature, signature);

    resCode = http.POST(buffer, len);
    http.end();
#endif    
    
    return resCode;
}

}  // namespace SINRICPRO_NAMESPACE