#ifndef I2S_H
#define I2S_H

#include "includes.h"


// I2S Configuration
#define I2S_SAMPLE_RATE     44100
#define I2S_BITS_PER_SAMPLE 16
#define I2S_CHANNELS        2
#define I2S_DMA_BUF_COUNT   4
#define I2S_DMA_BUF_LEN     1024

// I2S Pin Configuration (for PCM5102)
#define I2S_BCLK_PIN        9   // Bit Clock
#define I2S_LRCK_PIN        10  // Left/Right Clock (Word Select)
#define I2S_DOUT_PIN        11  // Data Out
#define I2S_MCLK_PIN        0   // Master Clock (optional, set to 0 to disable)

// I2S Port
#define I2S_PORT            I2S_NUM_0

// I2S State
typedef enum {
    I2S_STATE_UNINITIALIZED,
    I2S_STATE_INITIALIZED,
    I2S_STATE_PLAYING,
    I2S_STATE_STOPPED
} i2s_state_t;

// I2S Handle
typedef struct {
    i2s_chan_handle_t tx_handle;
    i2s_state_t state;
    bool enabled;
} i2s_handle_t;

// Initialize I2S
esp_err_t i2s_init(void);

// Deinitialize I2S
esp_err_t i2s_deinit(void);

// Enable I2S
esp_err_t i2s_enable(void);

// Disable I2S
esp_err_t i2s_disable(void);

// Write audio data to I2S
esp_err_t i2s_write(const void *src, size_t size, size_t *bytes_written, uint32_t timeout_ms);

// Get I2S state
i2s_state_t i2s_get_state(void);

// Check if I2S is enabled
bool i2s_is_enabled(void);

#endif // I2S_H
