#include "pcm5102.h"
//#include "log.h"
//#include "i2s.h"
//#include <math.h>

//static const char *TAG = "pcm5102";

// Global PCM5102 handle
static pcm5102_handle_t g_pcm5102_handle = {0};

esp_err_t pcm5102_init(void) {
    INFO("pcm5102_init()");
    
    if (g_pcm5102_handle.initialized) {
        WARN("PCM5102 already initialized");
        return ESP_OK;
    }

    // Initialize I2S first
    esp_err_t ret = i2s_init();
    if (ret != ESP_OK) {
        ERR("Failed to initialize I2S: %s", esp_err_to_name(ret));
        return ret;
    }

    // Enable I2S
    ret = i2s_enable();
    if (ret != ESP_OK) {
        ERR("Failed to enable I2S: %s", esp_err_to_name(ret));
        return ret;
    }

    g_pcm5102_handle.initialized = true;
    g_pcm5102_handle.playing = false;

    INFO("PCM5102 initialized successfully");
    INFO("Sample rate: %d Hz, Bits: %d, Channels: %d", 
         PCM5102_SAMPLE_RATE, PCM5102_BITS_PER_SAMPLE, PCM5102_CHANNELS);

    return ESP_OK;
}

esp_err_t pcm5102_deinit(void) {
    INFO("pcm5102_deinit()");
    
    if (!g_pcm5102_handle.initialized) {
        WARN("PCM5102 not initialized");
        return ESP_OK;
    }

    // Stop any ongoing playback
    pcm5102_stop();

    // Disable and deinitialize I2S
    i2s_disable();
    i2s_deinit();

    g_pcm5102_handle.initialized = false;
    g_pcm5102_handle.playing = false;

    INFO("PCM5102 deinitialized");
    return ESP_OK;
}

esp_err_t pcm5102_play_test_tone(void) {
    if (!g_pcm5102_handle.initialized) {
        ERR("PCM5102 not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (g_pcm5102_handle.playing) {
        WARN("PCM5102 already playing");
        return ESP_ERR_INVALID_STATE;
    }

    INFO("Playing %d Hz test tone for %d ms", PCM5102_TEST_FREQUENCY, PCM5102_TEST_DURATION);

    // Calculate samples needed for 2 seconds
    const int samples_per_second = PCM5102_SAMPLE_RATE * PCM5102_CHANNELS;
    const int total_samples = (PCM5102_SAMPLE_RATE * PCM5102_TEST_DURATION) / 1000 * PCM5102_CHANNELS;
    
    // Generate test tone (1kHz sine wave)
    int16_t *audio_buffer = malloc(total_samples * sizeof(int16_t));
    if (!audio_buffer) {
        ERR("Failed to allocate audio buffer");
        return ESP_ERR_NO_MEM;
    }

    // Generate sine wave
    for (int i = 0; i < total_samples; i += PCM5102_CHANNELS) {
        // Calculate sample value (sine wave)
        double time = (double)(i / PCM5102_CHANNELS) / PCM5102_SAMPLE_RATE;
        double sample = sin(2.0 * M_PI * PCM5102_TEST_FREQUENCY * time);
        
        // Convert to 16-bit signed integer
        int16_t sample_value = (int16_t)(sample * 16383);  // Use 75% of max amplitude
        
        // Set both left and right channels
        audio_buffer[i] = sample_value;     // Left channel
        audio_buffer[i + 1] = sample_value; // Right channel
    }

    g_pcm5102_handle.playing = true;

    // Send audio data to I2S
    size_t bytes_written;
    esp_err_t ret = i2s_write(audio_buffer, total_samples * sizeof(int16_t), &bytes_written, 5000);
    
    if (ret != ESP_OK) {
        ERR("Failed to write audio data: %s", esp_err_to_name(ret));
        g_pcm5102_handle.playing = false;
    } else {
        INFO("Test tone played successfully (%zu bytes written)", bytes_written);
    }

    // Clean up
    free(audio_buffer);
    g_pcm5102_handle.playing = false;

    return ret;
}

esp_err_t pcm5102_stop(void) {
    if (!g_pcm5102_handle.initialized) {
        WARN("PCM5102 not initialized");
        return ESP_OK;
    }

    if (!g_pcm5102_handle.playing) {
        WARN("PCM5102 not playing");
        return ESP_OK;
    }

    INFO("Stopping PCM5102 playback");
    g_pcm5102_handle.playing = false;

    return ESP_OK;
}

bool pcm5102_is_initialized(void) {
    return g_pcm5102_handle.initialized;
}

bool pcm5102_is_playing(void) {
    return g_pcm5102_handle.playing;
}

void pcm5102_test_task(void *arg) {
    INFO("pcm5102_test_task() - Starting PCM5102 boot test");
    
    // Initialize PCM5102
    esp_err_t ret = pcm5102_init();
    if (ret != ESP_OK) {
        ERR("Failed to initialize PCM5102: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    // Wait a moment for initialization to settle
    vTaskDelay(pdMS_TO_TICKS(100));

    // Play test tone
    ret = pcm5102_play_test_tone();
    if (ret != ESP_OK) {
        ERR("Failed to play test tone: %s", esp_err_to_name(ret));
    } else {
        INFO("PCM5102 test tone completed successfully");
    }

    // Task is complete - delete itself
    INFO("PCM5102 test task completed, deleting task");
    vTaskDelete(NULL);
}
