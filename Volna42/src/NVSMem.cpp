
#include <NVSMem.h>
#if defined(ESP32)

/* 
    вариант замены методов ESP8266 для работы с энергозависимой памятью (сохранение переменных при переходе в сон)
    в ESP32 память постоянная
    опционально при необходимости можно расширить функционал добавлением хранилищ с другими названиями
*/

bool readRTCUserMemory(uint32_t index, uint32_t* data, size_t len) {
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

bool writeRTCUserMemory(uint32_t index, uint32_t* data, size_t len) {
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