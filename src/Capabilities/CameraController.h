#pragma once

#include "../SinricProRequest.h"

#include "../EventLimiter.h"
#include "../SinricProStrings.h"
#include "../SinricProNamespace.h"

#include <FS.h>
#include <vector>

#if defined(ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
  #include <WiFiClientSecure.h>
  #include <mbedtls/base64.h>
#endif

namespace SINRICPRO_NAMESPACE {

FSTR(CAMERA, getSnapshot);            // "getSnapshot"
FSTR(CAMERA, POST);                   // "POST"
FSTR(CAMERA, getWebRTCAnswer);        // "getWebRTCAnswer"
FSTR(CAMERA, getCameraCapabilities);  // "getCameraCapabilities"
FSTR(CAMERA, offer);                  // "offer"
FSTR(CAMERA, answer);                 // "answer"
FSTR(CAMERA, iceServers);             // "iceServers"
FSTR(CAMERA, urls);                   // "urls"
FSTR(CAMERA, username);               // "username"
FSTR(CAMERA, credential);             // "credential"
FSTR(CAMERA, webrtc);                 // "webrtc"
FSTR(CAMERA, webrtcAudio);            // "webrtcAudio"
FSTR(CAMERA, webrtcVideo);            // "webrtcVideo"
FSTR(CAMERA, webrtcVideoCodecs);      // "webrtcVideoCodecs"
FSTR(CAMERA, rssi);                   // "rssi"

using SnapshotCallback = std::function<bool(const String &)>;

/**
 * @brief One STUN/TURN server URL with its (optional) credentials, as sent by the SinricPro server.
 */
struct SinricProIceServer {
  String url;         // e.g. "stun:stun.l.google.com:19302" or "turn:turn.sinric.pro:3478?transport=udp"
  String username;
  String credential;
};

/**
 * @brief Callback for WebRTC offers (ESP32 only)
 * @param[in]  deviceId    Device the offer is addressed to
 * @param[in]  offerSdp    Viewer's SDP offer (plain text, all candidates included)
 * @param[in]  iceServers  STUN/TURN servers to use for this session
 * @param[out] answerSdp   Local SDP answer; must include every gathered candidate (no trickle ICE)
 * @return true if an answer was produced
 * @note Runs synchronously inside SinricPro.handle() and may block while ICE gathers (keep it under ~4 s).
 */
using WebRTCOfferCallback = std::function<bool(const String &deviceId, const String &offerSdp, const std::vector<SinricProIceServer> &iceServers, String &answerSdp)>;

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
     * @brief Sets the callback function for WebRTC offers from the SinricPro portal and app (ESP32 only)
     * @param cb Callback that turns an SDP offer into an SDP answer, e.g. via the SinricProWebRTC library
     */
    void onWebRTCOffer(WebRTCOfferCallback cb);

    /**
     * @brief Declares that WebRTC sessions include a microphone audio track (ESP32 only)
     * @param enabled true if the WebRTC answer carries audio (e.g. XIAO ESP32S3 Sense microphone)
     * @note Reported through getCameraCapabilities so viewers request an audio track in their offer.
     */
    void enableWebRTCAudio(bool enabled = true);

    /**
     * @brief Declares that WebRTC sessions carry an H.264 video track (ESP32-S3 only)
     * @param enabled true if the session sends H.264 on a video track instead of JPEG frames
     * @note Reported through getCameraCapabilities so viewers offer a video track. Without it
     *       they fall back to JPEG over the DataChannel, which every ESP32 board can do.
     */
    void enableWebRTCVideo(bool enabled = true);

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
    bool handleWebRTCOffer(SinricProRequest &request);
    bool handleCameraCapabilities(SinricProRequest &request);

    SnapshotCallback getSnapshotCallback = nullptr;
    WebRTCOfferCallback webRTCOfferCallback = nullptr;
    bool webRTCAudio = false;
    bool webRTCVideo = false;
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

    static bool base64Decode(const char *input, String &output) {
        size_t inputLength = strlen(input);
        if (inputLength == 0) return false;

        std::vector<unsigned char> buffer(inputLength * 3 / 4 + 4);
        size_t outputLength = 0;
        if (mbedtls_base64_decode(buffer.data(), buffer.size(), &outputLength, reinterpret_cast<const unsigned char *>(input), inputLength) != 0) return false;

        output = "";
        return output.concat(reinterpret_cast<const char *>(buffer.data()), outputLength);
    }

    static bool base64Encode(const String &input, String &output) {
        // mbedtls writes a trailing NUL, hence the +1.
        std::vector<unsigned char> buffer(((input.length() + 2) / 3) * 4 + 1);
        size_t outputLength = 0;
        if (mbedtls_base64_encode(buffer.data(), buffer.size(), &outputLength, reinterpret_cast<const unsigned char *>(input.c_str()), input.length()) != 0) return false;

        output = "";
        return output.concat(reinterpret_cast<const char *>(buffer.data()), outputLength);
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
void CameraController<T>::onWebRTCOffer(WebRTCOfferCallback cb) {
    webRTCOfferCallback = cb;
}

template <typename T>
void CameraController<T>::enableWebRTCAudio(bool enabled) {
    webRTCAudio = enabled;
}

template <typename T>
void CameraController<T>::enableWebRTCVideo(bool enabled) {
    webRTCVideo = enabled;
}

template <typename T>
bool CameraController<T>::handleCameraController(SinricProRequest &request) {
    // Viewers query capabilities and reconnect within the snapshot rate limit window.
    if (request.action == FSTR_CAMERA_getCameraCapabilities) return handleCameraCapabilities(request);
    if (request.action == FSTR_CAMERA_getWebRTCAnswer) return handleWebRTCOffer(request);

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
bool CameraController<T>::handleCameraCapabilities(SinricProRequest &request) {
#if defined(ESP32)
    bool webrtc = webRTCOfferCallback != nullptr;
    // Viewers warn about a weak link before connecting: the answer still succeeds there, but the
    // DTLS handshake that follows cannot get its records out.
    request.response_value[FSTR_CAMERA_rssi] = WiFi.RSSI();
#else
    bool webrtc = false;
#endif
    bool video = webrtc && webRTCVideo;
    request.response_value[FSTR_CAMERA_webrtc]      = webrtc;
    request.response_value[FSTR_CAMERA_webrtcAudio] = webrtc && webRTCAudio;
    request.response_value[FSTR_CAMERA_webrtcVideo] = video;
    if (video) {
        JsonArray codecs = request.response_value[FSTR_CAMERA_webrtcVideoCodecs].to<JsonArray>();
        codecs.add("H264");
    }
    return true;
}

template <typename T>
bool CameraController<T>::handleWebRTCOffer(SinricProRequest &request) {
#if defined(ESP32)
    if (!webRTCOfferCallback) return false;
    T *device = static_cast<T *>(this);

    String offerSdp;
    if (!base64Decode(request.request_value[FSTR_CAMERA_offer] | "", offerSdp)) return false;

    // The server sends RTCIceServer entries ({urls: [...], username, credential}); flatten to one URL each.
    std::vector<SinricProIceServer> iceServers;
    for (JsonObject server : request.request_value[FSTR_CAMERA_iceServers].as<JsonArray>()) {
        const char *username   = server[FSTR_CAMERA_username] | "";
        const char *credential = server[FSTR_CAMERA_credential] | "";
        JsonVariant urls       = server[FSTR_CAMERA_urls];

        if (urls.is<JsonArray>()) {
            for (JsonVariant url : urls.as<JsonArray>()) {
                if (url.is<const char *>()) iceServers.push_back({url.as<const char *>(), username, credential});
            }
        } else if (urls.is<const char *>()) {
            iceServers.push_back({urls.as<const char *>(), username, credential});
        }
    }

    String answerSdp;
    if (!webRTCOfferCallback(device->deviceId, offerSdp, iceServers, answerSdp) || answerSdp.length() == 0) return false;

    String answer;
    if (!base64Encode(answerSdp, answer)) return false;

    request.response_value[FSTR_CAMERA_answer] = answer;
    return true;
#else
    (void)request;
    return false;
#endif
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

using SinricProIceServer = SINRICPRO_NAMESPACE::SinricProIceServer;
