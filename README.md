# kbutton.c

A small, portable button state machine for embedded systems.

The core is plain C. It does not know about GPIO, Arduino, RTOS tasks, interrupts,
or a particular clock source. Feed it the logical pressed state and a millisecond
timestamp; it turns that stream into button events.

```text
GPIO / input driver
       |
       v
 pressed + time
       |
       v
   kbutton.c
       |
       +-- press / release
       +-- click x N
       +-- long press
       +-- long repeat
       +-- long release
```

An optional header-only C++ wrapper adds the familiar Arduino GPIO + `millis()`
experience without changing the core.

## Why

Buttons look simple until every project reimplements the same details:
debouncing, click windows, double/triple/multi-click, long press, release, and
repeat behavior.

`kbutton.c` keeps that policy in one deterministic state machine:

- plain C11 core
- no dynamic allocation
- no hidden global state
- no hardware dependency
- arbitrary multi-click count (1-255)
- wrap-safe 32-bit millisecond timestamps
- optional zero-allocation Arduino C++ wrapper
- host tests with no GPIO mocks required for the core

## C API

```c
#include "kbutton.h"

static kbutton_t button;

static void on_button(
    kbutton_t *button,
    kbutton_event_t event,
    uint8_t clicks,
    void *user)
{
    (void)button;
    (void)user;

    if (event == KBUTTON_EVENT_CLICK) {
        // clicks == 1, 2, 3, 4, ...
    }
}

void app_init(void)
{
    kbutton_config_t config = kbutton_default_config();

    kbutton_init(&button, &config);
    kbutton_set_callback(&button, on_button, NULL);
}

void app_loop(void)
{
    bool pressed = board_button_pressed();
    uint32_t now_ms = board_uptime_ms();

    kbutton_update(&button, pressed, now_ms);
}
```

The `pressed` argument is logical, not electrical. Active-low GPIO, pull-ups,
interrupts, polling, and pin configuration belong to the platform layer.

This makes the same core usable from Arduino, ESP-IDF, STM32 HAL, NuttX, Zephyr,
FreeRTOS, bare-metal firmware, or any other environment that can provide a
boolean input and a monotonic millisecond counter.

## Events

| Event | Meaning |
| --- | --- |
| `KBUTTON_EVENT_PRESS` | Debounced transition to pressed |
| `KBUTTON_EVENT_RELEASE` | Debounced transition to released |
| `KBUTTON_EVENT_CLICK` | A completed short-click sequence; `clicks` contains the count |
| `KBUTTON_EVENT_LONG_PRESS` | The hold threshold was reached |
| `KBUTTON_EVENT_LONG_REPEAT` | Optional periodic event while held |
| `KBUTTON_EVENT_LONG_RELEASE` | Release after a long press |

A single, double, triple, or four-click gesture is the same event with a
different count. The API does not grow a separate callback for every count.

A short click followed by a held press is treated as two gestures: the completed
click sequence is emitted before `KBUTTON_EVENT_LONG_PRESS`.

## Timing

Defaults:

| Setting | Default | Notes |
| --- | ---: | --- |
| debounce | 30 ms | Stable time required before a level transition |
| click interval | 300 ms | Wait after release for another click |
| long press | 800 ms | Hold time before long press |
| repeat | disabled | Set a non-zero interval to enable |

```c
kbutton_config_t config = {
    .debounce_ms = 20,
    .click_ms = 250,
    .long_press_ms = 700,
    .repeat_ms = 200,
};
```

Set `long_press_ms` to zero to disable long-press detection. Set
`repeat_ms` to zero to disable long-repeat events. Set `click_ms` to zero to
emit every short click immediately.

The first call to `kbutton_update()` establishes the initial state without
emitting a synthetic press or release event. If the button boots held, long
press timing starts from that first sample.

## Arduino / C++

The wrapper is intentionally thin:

```cpp
#include <kbutton.hpp>

KButton button(0); // active LOW, INPUT_PULLUP

void setup()
{
    Serial.begin(115200);

    button.onClicks([](uint8_t count) {
        Serial.printf("clicks: %u\n", count);
    });

    button.onLongPress([]() {
        Serial.println("long press");
    });

    button.begin();
}

void loop()
{
    button.tick();
}
```

For other electrical arrangements:

```cpp
KButton button(pin, HIGH, INPUT);
```

The wrapper uses function pointers rather than `std::function`, so it does not
need heap allocation. Captureless lambdas work directly.

## Installation

PlatformIO:

```ini
lib_deps =
    https://github.com/lsongdev/kbutton.c.git
```

Arduino IDE users can install the repository as a ZIP library.

Pure C projects can compile `src/kbutton.c` and include `src/kbutton.h`
directly.

## Development

```sh
make test
```

The test suite builds the C core with `-std=c11 -Wall -Wextra -Werror -pedantic`
and separately compiles the Arduino C++ wrapper against a tiny host stub.

## Version 3

Version 3 is a deliberate breaking rewrite. The old Arduino-only `KButton.cpp`
API is not retained as a compatibility layer.

The original KButton project was inspired by the OneButton approach. Version 3
rebuilds the implementation around a platform-neutral C state machine and keeps
Arduino support as an optional layer above it.

## License

MIT
