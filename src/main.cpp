#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setupSSD1306()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        while (true)
            yield();
    }
    display.clearDisplay();
    display.display();
    display.dim(true);
    display.setRotation(0);

    display.setFont(&FreeMonoBold9pt7b);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.println(F("LoRaWAN"));
    display.setCursor(0, 45);
    display.println(F("    MAILBOX"));
    display.display();
}

void setupSerial()
{
    Serial.begin(BAUD_RATE);
}

void setup()
{
    setupSerial();
    setupSSD1306();
}

void loop()
{
    yield();
}
