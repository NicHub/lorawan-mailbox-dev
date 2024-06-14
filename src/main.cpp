#include <Arduino.h>
#include <LittleFS.h>

/**
 * READ PHOTORESISTOR
 * 4_294_336_512
 */

#define SENSOR_PIN GPIO_NUM_36
#define DATA_FILE_NAME "/data.csv"

// If you modify `DATA_FORMAT`,
// adapt `MEASUREMENT_CHAR_COUNT` accordingly.
#define MEASUREMENT_CHAR_COUNT 16
#define DATA_FORMAT "%10lu %4lu\n"

enum class Mode
{
    DATA_ACQUISITION,
    DATA_FILE_DELETE,
    DATA_FILE_READ,
    IDLE
};

Mode CURRENT_MODE = Mode::DATA_ACQUISITION;

uint32_t FREE_BYTES;
uint32_t TOTAL_POSSIBLE_MEASUREMENT_COUNT;
uint32_t MEASUREMENT_COUNT = 0;

bool differenceWithinLimit(uint16_t a, uint16_t b, uint16_t limit)
{
    int16_t diff = static_cast<int16_t>(a) - static_cast<int16_t>(b);
    return abs(diff) <= limit;
}

void setup()
{
    Serial.begin(BAUD_RATE);
    Serial.println("\n\n\n\n\n\n\n");

    pinMode(LED_BUILTIN, OUTPUT);

    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS Mount Failed");
        while (true)
            yield();
    }

    // Get free flash with some safety margin.
#define safety_margin 100
    uint32_t totalBytes = LittleFS.totalBytes();
    uint32_t usedBytes = LittleFS.usedBytes();
    FREE_BYTES = totalBytes - usedBytes - safety_margin;
    TOTAL_POSSIBLE_MEASUREMENT_COUNT = FREE_BYTES / MEASUREMENT_CHAR_COUNT;
    if (LittleFS.exists(DATA_FILE_NAME))
    {
        File dataFile = LittleFS.open(DATA_FILE_NAME, "a");
        MEASUREMENT_COUNT = dataFile.size() / MEASUREMENT_CHAR_COUNT;
    }
    Serial.printf("FREE FLASH (B):                   %10lu\n", FREE_BYTES);
    Serial.printf("TOTAL POSSIBLE MEASUREMENT COUNT: %10lu\n", TOTAL_POSSIBLE_MEASUREMENT_COUNT);
    Serial.printf("MEASUREMENT COUNT IN FILE:        %10lu\n", MEASUREMENT_COUNT);
    Serial.printf("MEASUREMENT COUNT REMAINING:      %10lu\n", TOTAL_POSSIBLE_MEASUREMENT_COUNT - MEASUREMENT_COUNT);

    Serial.println("LittleFS Mount Succeed");
}

void loop()
{
    switch (CURRENT_MODE)
    {
    case Mode::DATA_ACQUISITION:
    {
        static uint16_t prevLightVal = 0;
        uint16_t lightVal = analogRead(SENSOR_PIN);
        if (differenceWithinLimit(lightVal, prevLightVal, 100))
        {
            delay(5);
            break;
        }
        unsigned long _t = millis();

        Serial.printf(DATA_FORMAT, _t, lightVal);

        File dataFile = LittleFS.open(DATA_FILE_NAME, "a");
        if (dataFile)
        {
            if (MEASUREMENT_COUNT >= TOTAL_POSSIBLE_MEASUREMENT_COUNT)
            {
                dataFile.print("# FLASH IS FULL");
                Serial.println("# FLASH IS FULL");
                CURRENT_MODE = Mode::IDLE;
            }
            else
            {
                // Each measurement is 28 char long.
                dataFile.printf(DATA_FORMAT, _t, lightVal);
            }
            dataFile.close();

            ++MEASUREMENT_COUNT;
        }
        prevLightVal = lightVal;
        break;
    }
    case Mode::DATA_FILE_DELETE:
    {
        LittleFS.remove(DATA_FILE_NAME);
        CURRENT_MODE = Mode::IDLE;
        break;
    }
    case Mode::DATA_FILE_READ:
    {
        if (!LittleFS.exists(DATA_FILE_NAME))
        {
            Serial.println("FILE DOES NOT EXISTS");
            break;
        }
        File dataFile = LittleFS.open(DATA_FILE_NAME, "r");
        if (dataFile)
        {
            while (dataFile.available())
            {
                Serial.println(dataFile.readStringUntil('\n'));
            }
            dataFile.close();
        }
        CURRENT_MODE = Mode::IDLE;
        break;
    }
    case Mode::IDLE:
    {
        yield();
    }
    }

    if (Serial.available())
    {
        int c = Serial.read();
        if (c == (int)'a')
        {
            CURRENT_MODE = Mode::DATA_ACQUISITION;
            Serial.println("CURRENT_MODE = DATA_ACQUISITION");
        }
        else if (c == (int)'d')
        {
            CURRENT_MODE = Mode::DATA_FILE_DELETE;
            Serial.println("CURRENT_MODE = DATA_FILE_DELETE");
        }
        else if (c == (int)'r')
        {
            CURRENT_MODE = Mode::DATA_FILE_READ;
            Serial.println("CURRENT_MODE = DATA_FILE_READ");
        }
    }
}