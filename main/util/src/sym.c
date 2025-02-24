#include "sym.h"


typedef struct {
    const char *name;
    const char *val;
} SymTable;

SymTable symtable[] = {{"main", "󰂄"}, {"bits", "㏡"},    {"clock", "ㆠ"}, {"config", "ぐ"},
                       {"dma", "ふ"},    {"gpio", "㐆"},    {"ha", "󰟐"},  {"info", "め"},
                       {"json", ""},  {"led", "ジ"},     {"log", "󱂅"}, {"mmwave", "ㄉ"},
                       {"mqtt", "󰑬"}, {"net", "ㄴ"},     {"seg", "󰺗"}, {"sensor", "〶"},
                       {"tick", "。"},   {"unknown", "ょ"}, {NULL, NULL}};


const char *sym(const char *key) {
        for (int i = 0; symtable[i].name != NULL; i++) {
                if (strcmp(symtable[i].name, key) == 0) {
                    return symtable[i].val;
            }
        }
    return "ょ";
}

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

const char *col_err = "\033[48;5;196m\033[38;5;255m";

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
const char *sym_net = "ㄴ";
const char *sym_seg = "󰺗";
const char *sym_sensor = "〶";
const char *sym_tick = "。";
const char *sym_unknown = "ょ";
const char *sym_abort = "㎛";

const char *sym_control = "、";
const char *sym_btns = "㇢";
const char *sym_display = "㇁";
const char *sym_sd = "󰑹";
const char *sym_gps = "ㄢ";
const char *sym_singletail = "㐪";
const char *sym_settings = "す";
const char *sym_audio = "㇧";
const char *sym_bluetooth = "㇙";
const char *sym_ble = "㇚";
const char *sym_websockets = "㈚";
const char *sym_compass = "ㄓ";
const char *sym_location = "ㄕ";
const char *sym_user_location = "ㄔ";
const char *sym_server = "㊐";
const char *sym_database = "㊘";
const char *sym_ram = "䈬";
const char *sym_cpu = "䈂";
const char *sym_bug = "〡";
const char *sym_ok = "い";
const char *sym_bomb = "だ";
const char *sym_dead = "ぢ";
const char *sym_run = "ぁ";
const char *sym_panic = "あ";
const char *sym_warn = "も";
const char *sym_event = "う";
const char *sym_emerg = "と";
const char *sym_bell = "お";
const char *sym_bel = "\033[7m";
const char *sym_alarm = "か";
const char *sym_nul = "␀";
const char *sym_zero = "㋀";
const char *sym_one = "㊱";
const char *sym_led_off = "セ";
const char *sym_led_on = "ジ";
const char *sym_check = "ん";
const char *sym_x = "ァ";
const char *sym_no = "゙";
const char *sym_ghost = "〠";
const char *sym_err = "だ";
const char *sym_home = "ㄚ";
