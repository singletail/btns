#ifndef SYM_H
#define SYM_H

#include "includes.h"

extern const char *sym(const char *key);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

extern const char *col_reset;
extern const char *col_r;
extern const char *col_o;
extern const char *col_y;
extern const char *col_g;
extern const char *col_b;
extern const char *col_v;
extern const char *col_p;
extern const char *col_c;
extern const char *col_w;

extern const char *col_err;

extern const char *sym_main;
extern const char *sym_bits;
extern const char *sym_clock;
extern const char *sym_config;
extern const char *sym_dma;
extern const char *sym_gpio;
extern const char *sym_ha;
extern const char *sym_info;
extern const char *sym_json;
extern const char *sym_led;
extern const char *sym_log;
extern const char *sym_mmwave;
extern const char *sym_mqtt;
extern const char *sym_net;
extern const char *sym_seg;
extern const char *sym_sensor;
extern const char *sym_tick;
extern const char *sym_unknown;
extern const char *sym_abort;

extern const char *sym_control;
extern const char *sym_btns;
extern const char *sym_display;
extern const char *sym_sd;
extern const char *sym_gps;
extern const char *sym_singletail;
extern const char *sym_settings;
extern const char *sym_audio;
extern const char *sym_bluetooth;
extern const char *sym_ble;
extern const char *sym_websockets;
extern const char *sym_compass;
extern const char *sym_location;
extern const char *sym_user_location;
extern const char *sym_server;
extern const char *sym_database;
extern const char *sym_ram;
extern const char *sym_cpu;
extern const char *sym_bug;
extern const char *sym_ok;
extern const char *sym_bomb;
extern const char *sym_dead;
extern const char *sym_run;
extern const char *sym_panic;
extern const char *sym_warn;
extern const char *sym_event;
extern const char *sym_emerg;
extern const char *sym_bell;
extern const char *sym_bel;
extern const char *sym_alarm;
extern const char *sym_nul;
extern const char *sym_zero;
extern const char *sym_one;
extern const char *sym_led_off;
extern const char *sym_led_on;
extern const char *sym_check;
extern const char *sym_x;
extern const char *sym_no;
extern const char *sym_ghost;
extern const char *sym_err;
extern const char *sym_home;

#pragma GCC diagnostic pop
#endif