#include "sym.h"

const char *col_reset = "\033[0m";
const char *col_r = "\033[38;5;196m";
const char *col_o = "\033[38;5;202m";
const char *col_y = "\033[38;5;220m";
const char *col_g = "\033[38;5;46m";
const char *col_b = "\033[38;5;21m";
const char *col_v = "\033[38;5;93m";
const char *col_p = "\033[38;5;200m";
const char *col_c = "\033[38;5;51m";
const char *col_w = "\033[38;5;255m";

typedef struct {
    const char *name;
    const char *val;
} SymTable;

SymTable symtable[] = {{"main", "󰂄"}, {"bits", "㏡"}, {"clock", "ㆠ"},
    {"config", "ぐ"}, {"dma", "ふ"}, {"gpio", "㐆"}, {"ha", "󰟐"},
    {"info", "め"}, {"json", ""}, {"led", "ジ"}, {"log", "󱂅"},
    {"mmwave", "ㄉ"}, {"mqtt", "󰑬"}, {"net", "ㄱ"}, {"seg", "󰺗"},
    {"sensor", "〶"}, {"tick", "。"}, {"unknown", "ょ"}, {NULL, NULL}};

SymTable lvltable[] = {{"error", "だ"}, {"emerg", "で"}, {"warn", "と"},
    {"log", "ひ"}, {"info", "め"}, {"debug", "〡"}, {"yes", "ゔ"}, {"no", "イ"},
    {NULL, NULL}};

SymTable coltable[] = {{"error", "\033[38;5;196m"}, {"emerg", "\033[38;5;202m"},
    {"warn", "\033[38;5;220m"}, {"log", "\033[38;5;46m"},
    {"info", "\033[38;5;21m"}, {"debug", "\033[38;5;93m"},
    {"yes", "\033[38;5;51m"}, {"no", "\033[38;5;200m"}, {NULL, NULL}};

const char *sym(const char *key) {
    for (int i = 0; symtable[i].name != NULL; i++) {
        if (strcmp(symtable[i].name, key) == 0) {
            return symtable[i].val;
        }
    }
    return "ょ";
}

const char *sym_lvl(const char *key) {
    for (int i = 0; lvltable[i].name != NULL; i++) {
        if (strcmp(lvltable[i].name, key) == 0) {
            return lvltable[i].val;
        }
    }
    return "ひ";
}

const char *sym_col(const char *key) {
    for (int i = 0; coltable[i].name != NULL; i++) {
        if (strcmp(coltable[i].name, key) == 0) {
            return coltable[i].val;
        }
    }
    return "\033[0m";
}

const char *sym_main = "󰂄";
const char *sym_bits = "㏡";
const char *sym_clock = "ㆠ";
const char *sym_config = "ぐ";
const char *sym_dma = "ふ";
const char *sym_gpio = "㐆";
const char *sym_ha = "󰟐";
const char *sym_info = "め";
const char *sym_json = "";
const char *sym_led = "ジ";
const char *sym_log = "󱂅";
const char *sym_mmwave = "ㄉ";
const char *sym_mqtt = "󰑬";
const char *sym_net = "ㄱ";
const char *sym_seg = "󰺗";
const char *sym_sensor = "〶";
const char *sym_tick = "。";
const char *sym_unknown = "ょ";
const char *sym_abort = "㎛";
