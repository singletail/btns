#include "i2s.h"

static const char *TAG = "i2s";

// Global I2S handle
static i2s_handle_t g_i2s_handle = {0};

esp_err_t i2s_init(void) {
    INFO("i2s_init()");
    if (g_i2s_handle.state == I2S_STATE_INITIALIZED) {
        ESP_LOGW(TAG, "I2S already initialized");
        return ESP_OK;
    }

    // Channel configuration
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_PORT, I2S_ROLE_MASTER);

    // Clock configuration
    i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE);
    clk_cfg.sample_rate_hz = I2S_SAMPLE_RATE;
    clk_cfg.clk_src = I2S_CLK_SRC_DEFAULT;
    clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;

    // Slot configuration
    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_BITS_PER_SAMPLE, I2S_CHANNELS);
    slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_16BIT;
    slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_16BIT;
    slot_cfg.slot_mode = I2S_SLOT_MODE_STEREO;
    slot_cfg.slot_mask = I2S_STD_SLOT_BOTH;
    slot_cfg.ws_width = I2S_DATA_BIT_WIDTH_16BIT;
    slot_cfg.ws_pol = false;
    slot_cfg.bit_shift = false;
    slot_cfg.bit_order_lsb = false;  // MSB first

    // GPIO configuration
    i2s_std_gpio_config_t gpio_cfg = {
        .mclk = I2S_MCLK_PIN,
        .bclk = I2S_BCLK_PIN,
        .ws = I2S_LRCK_PIN,
        .dout = I2S_DOUT_PIN,
        .din = GPIO_NUM_NC,
        .invert_flags = {
            .mclk_inv = false,
            .bclk_inv = false,
            .ws_inv = false,
        },
    };

    // Standard configuration
    i2s_std_config_t std_cfg = {
        .clk_cfg = clk_cfg,
        .slot_cfg = slot_cfg,
        .gpio_cfg = gpio_cfg,
    };

    // Create I2S channel
    ESP_RETURN_ON_ERROR(i2s_new_channel(&chan_cfg, &g_i2s_handle.tx_handle, NULL), TAG, "Failed to create I2S channel");
    
    // Initialize standard mode
    ESP_RETURN_ON_ERROR(i2s_channel_init_std_mode(g_i2s_handle.tx_handle, &std_cfg), TAG, "Failed to init I2S std mode");
    
    // Enable I2S channel
    ESP_RETURN_ON_ERROR(i2s_channel_enable(g_i2s_handle.tx_handle), TAG, "Failed to enable I2S channel");

    g_i2s_handle.state = I2S_STATE_INITIALIZED;
    g_i2s_handle.enabled = true;

    ESP_LOGI(TAG, "I2S initialized successfully");
    ESP_LOGI(TAG, "Sample rate: %d Hz, Bits: %d, Channels: %d", I2S_SAMPLE_RATE, I2S_BITS_PER_SAMPLE, I2S_CHANNELS);
    ESP_LOGI(TAG, "Pins - BCLK: %d, LRCK: %d, DOUT: %d, MCLK: %d", I2S_BCLK_PIN, I2S_LRCK_PIN, I2S_DOUT_PIN, I2S_MCLK_PIN);

    return ESP_OK;
}

esp_err_t i2s_deinit(void) {
    if (g_i2s_handle.state == I2S_STATE_UNINITIALIZED) {
        ESP_LOGW(TAG, "I2S not initialized");
        return ESP_OK;
    }

    // Disable and delete I2S channel
    if (g_i2s_handle.tx_handle) {
        i2s_channel_disable(g_i2s_handle.tx_handle);
        i2s_del_channel(g_i2s_handle.tx_handle);
        g_i2s_handle.tx_handle = NULL;
    }

    g_i2s_handle.state = I2S_STATE_UNINITIALIZED;
    g_i2s_handle.enabled = false;

    ESP_LOGI(TAG, "I2S deinitialized");
    return ESP_OK;
}

esp_err_t i2s_enable(void) {
    if (g_i2s_handle.state != I2S_STATE_INITIALIZED) {
        ESP_LOGE(TAG, "I2S not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (g_i2s_handle.enabled) {
        ESP_LOGW(TAG, "I2S already enabled");
        return ESP_OK;
    }

    ESP_RETURN_ON_ERROR(i2s_channel_enable(g_i2s_handle.tx_handle), TAG, "Failed to enable I2S channel");
    
    g_i2s_handle.enabled = true;
    g_i2s_handle.state = I2S_STATE_STOPPED;

    ESP_LOGI(TAG, "I2S enabled");
    return ESP_OK;
}

esp_err_t i2s_disable(void) {
    if (g_i2s_handle.state == I2S_STATE_UNINITIALIZED) {
        ESP_LOGW(TAG, "I2S not initialized");
        return ESP_OK;
    }

    if (!g_i2s_handle.enabled) {
        ESP_LOGW(TAG, "I2S already disabled");
        return ESP_OK;
    }

    ESP_RETURN_ON_ERROR(i2s_channel_disable(g_i2s_handle.tx_handle), TAG, "Failed to disable I2S channel");
    
    g_i2s_handle.enabled = false;
    g_i2s_handle.state = I2S_STATE_STOPPED;

    ESP_LOGI(TAG, "I2S disabled");
    return ESP_OK;
}

esp_err_t i2s_write(const void *src, size_t size, size_t *bytes_written, uint32_t timeout_ms) {
    if (g_i2s_handle.state == I2S_STATE_UNINITIALIZED) {
        ESP_LOGE(TAG, "I2S not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (!g_i2s_handle.enabled) {
        ESP_LOGE(TAG, "I2S not enabled");
        return ESP_ERR_INVALID_STATE;
    }

    if (!src || size == 0) {
        ESP_LOGE(TAG, "Invalid parameters");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = i2s_channel_write(g_i2s_handle.tx_handle, src, size, bytes_written, timeout_ms);
    
    if (ret == ESP_OK) {
        g_i2s_handle.state = I2S_STATE_PLAYING;
    } else {
        ESP_LOGE(TAG, "I2S write failed: %s", esp_err_to_name(ret));
    }

    return ret;
}

i2s_state_t i2s_get_state(void) {
    return g_i2s_handle.state;
}

bool i2s_is_enabled(void) {
    return g_i2s_handle.enabled;
}
