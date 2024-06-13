#include <Arduino.h>

/**
 * READ PHOTORESISTOR
 * ~2700
 * ~800
 */

#define SENSOR_PIN GPIO_NUM_36

int lightInit;
int lightVal;

void setup()
{
    Serial.begin(BAUD_RATE);
    lightInit = analogRead(SENSOR_PIN);
    pinMode(LED_BUILTIN, OUTPUT);
    for (size_t cnt = 0; cnt < 10; cnt++)
    {
        digitalWrite(LED_BUILTIN, LOW);
        delay(10);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(90);
    }
}

void loop()
{
    lightVal = analogRead(SENSOR_PIN);
    Serial.println(lightVal);
    delay(500);
}
