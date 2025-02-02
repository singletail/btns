#ifndef SYM_H
#define SYM_H

#include "include.h"

extern const char *sym(const char *key);
extern const char *sym_lvl(const char *key);
extern const char *sym_col(const char *key);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

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

/*
extern const char *sym_control = "、";
extern const char *sym_btns = "㇢";
extern const char *sym_display = "㇁";
extern const char *sym_sd = "󰑹";
extern const char *sym_gps = "ㄢ";
extern const char *sym_singletail = "㐪";
extern const char *sym_settings = "す";
extern const char *sym_audio = "㇧";
extern const char *sym_bluetooth = "㇙";
extern const char *sym_ble = "㇚";
extern const char *sym_websockets = "㈚";
extern const char *sym_compass = "ㄓ";
extern const char *sym_location = "ㄕ";
extern const char *sym_user_location = "ㄔ";
extern const char *sym_server = "㊐";
extern const char *sym_database = "㊘";
extern const char *sym_ram = "䈬";
extern const char *sym_cpu = "䈂";
extern const char *sym_bug = "〡";
extern const char *sym_ok = "い";
extern const char *sym_bomb = "だ";
extern const char *sym_dead = "ぢ";
extern const char *sym_run = "ぁ";
extern const char *sym_panic = "あ";
extern const char *sym_warn = "も";
extern const char *sym_event = "う";
extern const char *sym_emerg = "と";
extern const char *sym_bell = "お";
extern const char *sym_bel = "\033[7m";
extern const char *sym_alarm = "か";
extern const char *sym_nul = "␀";
extern const char *sym_zero = "㋀";
extern const char *sym_one = "㊱";
extern const char *sym_led_off = "セ";
extern const char *sym_led_on = "ジ";
extern const char *sym_check = "ん";
extern const char *sym_x = "ァ";
extern const char *sym_no = "゙";
extern const char *sym_ghost = "〠";
*/

// ANSI color codes

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

/*
extern const char *col_bgr = "\033[48;5;196m";
extern const char *col_bgo = "\033[48;5;202m";
extern const char *col_bgy = "\033[48;5;220m";
extern const char *col_bgg = "\033[48;5;46m";
extern const char *col_bgb = "\033[48;5;21m";
extern const char *col_bgv = "\033[48;5;93m";
extern const char *col_bgp = "\033[48;5;200m";
extern const char *col_bgc = "\033[48;5;51m";
extern const char *col_bgw = "\033[48;5;255m";
extern const char *col_bold = "\033[1m";
extern const char *col_italic = "\033[3m";
extern const char *col_underline = "\033[4m";
extern const char *col_blink = "\033[5m";
extern const char *col_reverse = "\033[7m";
*/

#pragma GCC diagnostic pop
#endif
