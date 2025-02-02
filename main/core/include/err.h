#ifndef _ERR_H
#define _ERR_H

#include "include.h"

#define END(x)                                                                 \
    do {                                                                       \
        esp_err_t __err = (x);                                                 \
        if (__err != ESP_OK) {                                                 \
            printf("%s[%s] HALT: %s failed (%s) at %s:%d%s\n", #x, col_r,      \
                sym_abort, esp_err_to_name(__err), __FILE__, __LINE__,         \
                col_reset);                                                    \
            abort();                                                           \
        }                                                                      \
    } while (0)

#define FAIL(x)                                                                \
    do {                                                                       \
        esp_err_t _err = (x);                                                  \
        if (_err != ESP_OK) {                                                  \
            ERR("HALT: %s failed at %s:%d, error: %s", #x, __FILE__, __LINE__, \
                esp_err_to_name(_err));                                        \
            abort();                                                           \
        }                                                                      \
    } while (0)

#define NONULL(x)                                                              \
    do {                                                                       \
        if (x == NULL) {                                                       \
            ERR("NONULL fail:", #x, __FILE__, __LINE__);                       \
            abort();                                                           \
        }                                                                      \
    } while (0)

#define NOERR(x)                                                               \
    do {                                                                       \
        esp_err_t err_rc_ = (x);                                               \
        if (unlikely(err_rc_ != ESP_OK)) {                                     \
            ERR("NOERR fail:", __FILE__, __LINE__, err_rc_,                    \
                esp_err_to_name(err_rc_), __ASSERT_FUNC, #x);                  \
            _esp_error_check_failed(                                           \
                err_rc_, __FILE__, __LINE__, __ASSERT_FUNC, #x);               \
        }                                                                      \
    } while (0)

#define OK(x)                                                                  \
    do {                                                                       \
        esp_err_t _err = (x);                                                  \
        if (_err != ESP_OK) {                                                  \
            WARN("%s returned error at %s:%d, error: %s", #x, __FILE__,        \
                __LINE__, esp_err_to_name(_err));                              \
        }                                                                      \
    } while (0)

#endif

/*
// FAIL - Aborts on error
#define FAIL(x) \
  do { \
    esp_err_t __err = (x); \
    if (__err != ESP_OK) { \
      ERR("HALT: %s failed (%s) at %s:%d", #x, esp_err_to_name(__err), __FILE__,
__LINE__);                            \
      abort(); \
    } \ } while (0)

// NONULL - Aborts if pointer is NULL
#define NONULL(x) \
  do { \
    if ((x) == NULL) { \
      ERR("NULL pointer (%s) at %s:%d", #x, __FILE__, __LINE__); \
      abort(); \
    } \ } while (0)

// NOERR - Uses esp_error_check_failed for better error reporting
#define NOERR(x) \
  do { \
    esp_err_t __err = (x); \
    if (unlikely(__err != ESP_OK)) { \
      ERR("%s failed (%s) at %s:%d", #x, esp_err_to_name(__err), __FILE__,
__LINE__);                                  \
      esp_err_check_failed(__err, __FILE__, __LINE__, __ASSERT_FUNC, #x); \
    } \ } while (0)

// OK - Warns on error but continues
#define OK(x) \
  do { \
    esp_err_t __err = (x); \
    if (__err != ESP_OK) { \
      WARN("%s returned %s at %s:%d", #x, esp_err_to_name(__err), __FILE__,
__LINE__);                                 \
    } \ } while (0)
*/