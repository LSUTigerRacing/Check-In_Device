#include <Adafruit_NeoPixel.h>

//Defining ESP32 LEDs
#define RGB_PIN 47
#define NUM_LEDS 1
Adafruit_NeoPixel rgb_led(NUM_LEDS, RGB_PIN, NEO_GRB + NEO_KHZ800);

//Prepares the LEd to work
void initLED() {
  rgb_led.begin();
  rgb_led.show();
}

//Set the colors up
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  rgb_led.setPixelColor(0, r, g, b);
  rgb_led.show();
}

//This function allows the LED to blink
//Default flash speed is 100 ms on and 100 ms off
void flashColor(uint8_t r, uint8_t g, uint8_t b, int times, int onTime, int offTime) {
  for (int i = 0; i < times; i++) {
    setColor(r, g, b);
    delay(onTime);
    setColor(0, 0, 0);
    delay(offTime);
  }
}