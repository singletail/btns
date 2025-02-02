#include "nvs.h"

void nvs_init(void) {
    LOG("nvs_init()");
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        WARN("NVS needs recovery - erasing flash");
        err = nvs_flash_erase();
        if (err != ESP_OK) {
            ERR("nvs_flash_erase failed: %s", esp_err_to_name(err));
            return;
        }
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        ERR("NVS init failed: %s", esp_err_to_name(err));
        abort();
    }
}
