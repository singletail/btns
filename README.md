# btns

A modular esp-idf project for home automation and other useless things.

It's in a mostly working state. Some of the code is quite good, and worth copying. Some of it, not so much.

## Notable features

### WS2812B LED control

- Written from scratch.
- RMT peripheral with DMA buffers.
- Runs at 80 ticks per microsecond.
- New encoder with timings from the datasheet.
- 74HCT245 pinout is in doc/circuits/74HCT24.md.

### 7-segment display

- TM1637 4-digit 7-seg display.
- Based on the reference code from 2014.
- Uses low-level HAL GPIO calls for speed.
- No button control, because mine don't have buttons.

### MMWAVE presence detection

- Written for LD2410C, but should work with the rest of the line.
- Interrupt-based task.
- Engineering mode not yet implemented.

### Home Assistant integration

- MQTT auto-discovery.
- Incoming alerts via MQTT.

### Sound playback

- Plays WAV files from SPIFFS.
- Uses I2S peripheral.
- SD card support coming soon.

## Config

Copy main/config/config.h.sample to config.h and edit as needed. Note that it is (and should be) in .gitignore.

## Setup

If this doesn't run, you'll need to spend some time setting up ESP-IDF:

source $IDF_PATH/export.sh

## Contact

t@wse.nyc

## License

BSD-4-Clause. Deal with it. 🕶️
