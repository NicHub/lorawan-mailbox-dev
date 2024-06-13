#include <Arduino.h>

/**
 * READ PHOTORESISTOR
 * ~2700
 * ~800
 */

#define SENSOR_PIN GPIO_NUM_36

void blink(size_t cntMax)
{
    for (size_t cnt = 0; cnt < cntMax; cnt++)
    {
        digitalWrite(LED_BUILTIN, LOW);
        delay(10);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(90);
    }
}

void setup()
{
    Serial.begin(BAUD_RATE);
    pinMode(LED_BUILTIN, OUTPUT);
    blink(10);
}

bool differenceWithinLimit(uint16_t a, uint16_t b, uint16_t limit)
{
    int16_t diff = static_cast<int16_t>(a) - static_cast<int16_t>(b);
    return abs(diff) <= limit;
}

void loop()
{
    static uint16_t prevLightVal = 0;
    uint16_t lightVal;
    lightVal = analogRead(SENSOR_PIN);
    if (differenceWithinLimit(lightVal, prevLightVal, 100))
    {
        delay(5);
        return;
    }
    Serial.printf("%20lu", millis());
    Serial.printf(" %5lu\n", lightVal);
    prevLightVal = lightVal;
}
