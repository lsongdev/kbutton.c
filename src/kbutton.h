#ifndef KBUTTON_H
#define KBUTTON_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KBUTTON_DEFAULT_DEBOUNCE_MS 30u
#define KBUTTON_DEFAULT_CLICK_MS 300u
#define KBUTTON_DEFAULT_LONG_PRESS_MS 800u
#define KBUTTON_DEFAULT_REPEAT_MS 0u

typedef enum {
    KBUTTON_EVENT_PRESS = 0,
    KBUTTON_EVENT_RELEASE,
    KBUTTON_EVENT_CLICK,
    KBUTTON_EVENT_LONG_PRESS,
    KBUTTON_EVENT_LONG_REPEAT,
    KBUTTON_EVENT_LONG_RELEASE,
} kbutton_event_t;

typedef struct {
    uint32_t debounce_ms;
    uint32_t click_ms;
    uint32_t long_press_ms;
    uint32_t repeat_ms;
} kbutton_config_t;

struct kbutton;
typedef struct kbutton kbutton_t;

typedef void (*kbutton_callback_t)(
    kbutton_t *button,
    kbutton_event_t event,
    uint8_t clicks,
    void *user);

struct kbutton {
    kbutton_config_t config;
    kbutton_callback_t callback;
    void *user;

    uint32_t raw_changed_at;
    uint32_t pressed_at;
    uint32_t released_at;
    uint32_t repeated_at;

    uint8_t clicks;
    bool initialized;
    bool raw_pressed;
    bool pressed;
    bool long_pressed;
};

kbutton_config_t kbutton_default_config(void);

void kbutton_init(kbutton_t *button, const kbutton_config_t *config);
void kbutton_reset(kbutton_t *button);
void kbutton_configure(kbutton_t *button, const kbutton_config_t *config);
void kbutton_set_callback(
    kbutton_t *button,
    kbutton_callback_t callback,
    void *user);

void kbutton_update(kbutton_t *button, bool pressed, uint32_t now_ms);

bool kbutton_is_pressed(const kbutton_t *button);
bool kbutton_is_long_pressed(const kbutton_t *button);

#ifdef __cplusplus
}
#endif

#endif
