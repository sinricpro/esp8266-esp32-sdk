#pragma once

#include "../EventLimiter.h"
#include "../SinricProNamespace.h"
#include "../SinricProRequest.h"
#include "../SinricProStrings.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

namespace SINRICPRO_NAMESPACE {

FSTR(CAMERA, getSnapshot);  // "getSnapshot"

using SnapshotCallback = std::function<bool(const String &)>;

template <typename T>
class CameraController {
  public:
    CameraController();
    void onSnapshot(SnapshotCallback cb);
    int uploadImage(uint8_t* buffer, size_t len);

  protected:
    bool handleCameraController(SinricProRequest &request);

  private:
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

    if (request.action == FSTR_CAMERA_getSnapshot) {
        if (getSnapshotCallback) {
            success = getSnapshotCallback(device->deviceId);
        }
    }

    return success;
}

template <typename T>
int CameraController<T>::uploadImage(uint8_t* buffer, size_t len) {
    T *device = static_cast<T *>(this);

    if (!buffer) return -1;

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    if (!http.begin(client, SINRICPRO_CAMERA_URL, 443, "/snapshot", true)) return -1;

    const String& deviceId = device->getDeviceId();
    String timestamp = String(device->getTimestamp());
    String signature = device->sign(deviceId+timestamp);

    http.addHeader("deviceid", deviceId);
    http.addHeader("timestamp", timestamp);
    http.addHeader("signature", signature);

    int resCode = http.POST(buffer, len);
    http.end();

    return resCode;
}

}  // namespace SINRICPRO_NAMESPACE