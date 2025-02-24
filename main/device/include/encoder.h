#ifndef _ENCODER_H
#define _ENCODER_H

#include "includes.h"

typedef struct {
    rmt_encoder_t base;
    rmt_encoder_t *bytes_encoder;
    rmt_encoder_t *copy_encoder;
    int state;
    rmt_symbol_word_t reset_code;
} encoder_obj_t;

esp_err_t encoder_new(rmt_encoder_handle_t *ret_encoder);

#endif