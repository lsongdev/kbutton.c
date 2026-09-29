#include "kbutton.hpp"

#include <cassert>
#include <cstdint>
#include <cstdio>

static uint8_t fake_pin;
static uint8_t fake_mode;
static int fake_level = HIGH;
static uint32_t fake_now;

void pinMode(uint8_t pin, uint8_t mode)
{
    fake_pin = pin;
    fake_mode = mode;
}

int digitalRead(uint8_t pin)
{
    assert(pin == fake_pin);
    return fake_level;
}

uint32_t millis(void)
{
    return fake_now;
}

static unsigned presses;
static unsigned releases;
static unsigned long_presses;
static uint8_t click_count;

static void on_press()
{
    presses++;
}

static void on_release()
{
    releases++;
}

static void on_clicks(uint8_t count)
{
    click_count = count;
}

static void on_long_press()
{
    long_presses++;
}

int main()
{
    KButton button(7);
    button.setDebounceMs(0);
    button.setClickMs(50);
    button.setLongPressMs(100);

    button.onPress(on_press);
    button.onRelease(on_release);
    button.onClicks(on_clicks);
    button.onLongPress(on_long_press);

    fake_now = 0;
    button.begin();

    assert(fake_pin == 7);
    assert(fake_mode == INPUT_PULLUP);

    fake_level = LOW;
    fake_now = 10;
    button.tick();
    assert(presses == 1);
    assert(button.pressed());

    fake_level = HIGH;
    fake_now = 20;
    button.tick();
    assert(releases == 1);

    fake_now = 70;
    button.tick();
    assert(click_count == 1);

    fake_level = LOW;
    fake_now = 100;
    button.tick();
    fake_now = 200;
    button.tick();
    assert(long_presses == 1);
    assert(button.longPressed());

    std::puts("arduino wrapper tests passed");
    return 0;
}
