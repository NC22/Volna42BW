
#include <esp32/NVSMem.h>
#if defined(ESP32)

RTC_DATA_ATTR uint8_t RTCLastStateRaw[sizeof(rtcData)];
RTC_DATA_ATTR bool RTCLastStateRawWrited = false;

bool readRTCUserMemoryActualRTC(rtcData &lastState) {

    if (!RTCLastStateRawWrited) return false;

    memcpy(&lastState, RTCLastStateRaw, sizeof(rtcData));
    return true;
}

bool writeRTCUserMemoryActualRTC(rtcData &lastState) {

    memcpy(RTCLastStateRaw, &lastState, sizeof(rtcData));
    RTCLastStateRawWrited = true;
    return true;
}

bool readRTCUserMemoryNVS(uint32_t* data, size_t len) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvsHandle);
    if (err != ESP_OK) {
        return false;
    }

    size_t required_size = len;
    err = nvs_get_blob(nvsHandle, "lastState", data, &required_size);
    nvs_close(nvsHandle);

    return (err == ESP_OK);
}

bool writeRTCUserMemoryNVS(uint32_t* data, size_t len) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        return false;
    }

    err = nvs_set_blob(nvsHandle, "lastState", data, len);
    if (err == ESP_OK) {
        err = nvs_commit(nvsHandle);
    }
    nvs_close(nvsHandle);

    return (err == ESP_OK);
}
#endif