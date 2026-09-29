#include <kbutton.hpp>

KButton button(0);

void setup()
{
    Serial.begin(115200);

    button.setDebounceMs(30);
    button.setClickMs(300);
    button.setLongPressMs(800);
    button.setRepeatMs(250);

    button.onPress([]() {
        Serial.println("press");
    });

    button.onRelease([]() {
        Serial.println("release");
    });

    button.onClicks([](uint8_t count) {
        Serial.print("clicks: ");
        Serial.println(count);
    });

    button.onLongPress([]() {
        Serial.println("long press");
    });

    button.onLongRepeat([]() {
        Serial.println("long repeat");
    });

    button.onLongRelease([]() {
        Serial.println("long release");
    });

    button.begin();
}

void loop()
{
    button.tick();
}
