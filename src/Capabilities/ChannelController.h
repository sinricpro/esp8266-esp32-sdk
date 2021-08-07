#pragma once

#include "../EventLimiter.h"
#include "../SinricProNamespace.h"
#include "../SinricProRequest.h"
#include "../SinricProStrings.h"
namespace SINRICPRO_NAMESPACE {

FSTR(CHANNEL, changeChannel);  // "changeChannel"
FSTR(CHANNEL, channel);        // "channel"
FSTR(CHANNEL, name);           // "name"
FSTR(CHANNEL, number);         // "number"
FSTR(CHANNEL, skipChannels);   // "skipChannels"
FSTR(CHANNEL, channelCount);   // "channelCount"

/**
 * @brief Callback definition for onChangeChannel function
 * 
 * Gets called when device receive a `changeChannel` request by using channel name
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   channel      String with channel name device is requested to switch to
 * @param[out]  channel      String with channel name device has switchted to
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section ChangeChannel Example-Code
 * @snippet callbacks.cpp onChangeChannel
 **/
using ChangeChannelCallback = std::function<bool(const String &, String &)>;

/**
 * @brief Callback definition for onChangeChannelNumber function
 * 
 * Gets called when device receive a `changeChannel` request by using channel number 
 * @param[in]   deviceId      String which contains the ID of device
 * @param[in]   channelNumber Integer with channel number device is requested to switch to
 * @param[out]  channelName   String with channel name device has switchted to
 * @return      the success of the request
 * @retval      true          request handled properly
 * @retval      false         request was not handled properly because of some error
 * 
 * @section ChangeChannelNumber Example-Code
 * @snippet callbacks.cpp onChangeChannelNumber
 **/
using ChangeChannelNumberCallback = std::function<bool(const String &, int, String &)>;

/**
 * @brief Callback definition for onSkipChannels function
 * 
 * Gets called when device receive a `changeChannel` request by using channel number 
 * @param[in]   deviceId      String which contains the ID of device
 * @param[in]   channelCount  Integer with channels device is requested to skip `-n`..`+n`
 * @param[out]  channelName   String with channel name device has switchted to
 * @return      the success of the request
 * @retval      true          request handled properly
 * @retval      false         request was not handled properly because of some error
 * 
 * @section SkipChannels Example-Code
 * @snippet callbacks.cpp onSkipChannels
 **/
using SkipChannelsCallback = std::function<bool(const String &, int, String &)>;

/**
 * @brief ChannelController
 * @ingroup Capabilities
 **/
template <typename T>
class ChannelController {
  public:
    ChannelController();

    void onChangeChannel(ChangeChannelCallback cb);
    void onChangeChannelNumber(ChangeChannelNumberCallback cb);
    void onSkipChannels(SkipChannelsCallback cb);

    bool sendChangeChannelEvent(String channelName, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    virtual bool onChangeChannel(String &channelName);
    virtual bool onChangeChannelNumber(int channelNumber, String &channelName);
    virtual bool onSkipChannels(int channelCount, String &channelName);

    bool handleChannelController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    ChangeChannelCallback changeChannelCallback;
    ChangeChannelNumberCallback changeChannelNumberCallback;
    SkipChannelsCallback skipChannelsCallback;
};

template <typename T>
ChannelController<T>::ChannelController()
    : event_limiter(EVENT_LIMIT_STATE) {
    T *device = static_cast<T *>(this);
    device->registerRequestHandler(std::bind(&ChannelController<T>::handleChannelController, this, std::placeholders::_1));
}

/**
 * @brief Set callback function for `changeChannel` request
 * 
 * @param cb Function pointer to a `ChangeChannelCallback` function
 * @return void
 * @see ChangeChannelCallback
 **/
template <typename T>
void ChannelController<T>::onChangeChannel(ChangeChannelCallback cb) {
    changeChannelCallback = cb;
}

/**
 * @brief Set callback function for `changeChannel` request
 * 
 * @param cb Function pointer to a `ChangeChannelNumberCallback` function
 * @return void
 * @see ChangeChannelNumberCallback
 **/
template <typename T>
void ChannelController<T>::onChangeChannelNumber(ChangeChannelNumberCallback cb) {
    changeChannelNumberCallback = cb;
}

/**
 * @brief Set callback function for `skipChannels` request
 * 
 * @param cb Function pointer to a `SkipChannelsCallback` function
 * @return void
 * @see SkipChannelsCallback
 **/
template <typename T>
void ChannelController<T>::onSkipChannels(SkipChannelsCallback cb) {
    skipChannelsCallback = cb;
}

template <typename T>
bool ChannelController<T>::onChangeChannel(String &channelName) {
    T *device = static_cast<T *>(this);
    if (changeChannelCallback) return changeChannelCallback(device->deviceId, channelName);
    return false;
}

template <typename T>
bool ChannelController<T>::onChangeChannelNumber(int channelNumber, String &channelName) {
    T *device = static_cast<T *>(this);
    if (changeChannelNumberCallback) return changeChannelNumberCallback(device->deviceId, channelNumber, channelName);
    return false;
}

template <typename T>
bool ChannelController<T>::onSkipChannels(int channelCount, String &channelName) {
    T *device = static_cast<T *>(this);
    if (skipChannelsCallback) return skipChannelsCallback(device->deviceId, channelCount, channelName);
    return false;
}

/**
 * @brief Send `changeChannel` event to SinricPro Server to report selected channel
 * 
 * @param channelName     String with actual channel
 * @param cause           (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool ChannelController<T>::sendChangeChannelEvent(String channelName, String cause) {
    if (event_limiter) return false;
    T *device = static_cast<T *>(this);

    DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_CHANNEL_changeChannel, cause.c_str());
    JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
    event_value[FSTR_CHANNEL_channel][FSTR_CHANNEL_name] = channelName;
    return device->sendEvent(eventMessage);
}

template <typename T>
bool ChannelController<T>::handleChannelController(SinricProRequest &request) {
    bool success = false;

    if (request.action == FSTR_CHANNEL_changeChannel) {
        if (request.request_value[FSTR_CHANNEL_channel].containsKey(FSTR_CHANNEL_name)) {
            String channelName = request.request_value[FSTR_CHANNEL_channel][FSTR_CHANNEL_name] | "";
            success = onChangeChannel(channelName);
            request.response_value[FSTR_CHANNEL_channel][FSTR_CHANNEL_name] = channelName;
        }

        if (request.request_value[FSTR_CHANNEL_channel].containsKey(FSTR_CHANNEL_number)) {
            String channelName("");
            int channelNumber = request.request_value[FSTR_CHANNEL_channel][FSTR_CHANNEL_number];
            success = onChangeChannelNumber(channelNumber, channelName);
            request.response_value[FSTR_CHANNEL_channel][FSTR_CHANNEL_name] = channelName;
        }
        return success;
    }

    if (skipChannelsCallback && request.action == FSTR_CHANNEL_skipChannels) {
        String channelName;
        int channelCount = request.request_value[FSTR_CHANNEL_channelCount] | 0;
        success = onSkipChannels(channelCount, channelName);
        request.response_value[FSTR_CHANNEL_channel][FSTR_CHANNEL_name] = channelName;
        return success;
    }

    return success;
}

}  // namespace SINRICPRO_NAMESPACE