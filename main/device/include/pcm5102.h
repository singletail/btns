#ifndef PCM5102_H
#define PCM5102_H

#include "includes.h"

// PCM5102 Configuration
#define PCM5102_SAMPLE_RATE     44100
#define PCM5102_BITS_PER_SAMPLE 16
#define PCM5102_CHANNELS        2

// Test tone configuration
#define PCM5102_TEST_FREQUENCY  1000  // 1kHz test tone
#define PCM5102_TEST_DURATION   2000  // 2 seconds in milliseconds

// PCM5102 Handle
typedef struct {
    bool initialized;
    bool playing;
} pcm5102_handle_t;

// Initialize PCM5102
esp_err_t pcm5102_init(void);

// Deinitialize PCM5102
esp_err_t pcm5102_deinit(void);

// Play test tone
esp_err_t pcm5102_play_test_tone(void);

// Stop playback
esp_err_t pcm5102_stop(void);

// Check if PCM5102 is initialized
bool pcm5102_is_initialized(void);

// Check if PCM5102 is playing
bool pcm5102_is_playing(void);

// PCM5102 test task (for one-time boot test)
void pcm5102_test_task(void *arg);

#endif // PCM5102_H
