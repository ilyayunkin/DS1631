#include <Arduino.h>
#include <stdint.h>

#include <DS1631.h>

const uint8_t CHIP_ADDR = 0x00;

DS1631 thermostat;

void setup()
{
    Serial.begin(9600);
    thermostat.begin(CHIP_ADDR, DS1631::RESOLUTION_9BIT);
    delay(thermostat.getConversionTimeMs());
    auto online = thermostat.isOnLine();
    Serial.print( !online ? "NOT Found\r\n" : "Found\r\n");
}

void loop()
{
    bool ok = false;
    auto value = thermostat.readConfig(ok);
    if(ok)
    {
        Serial.print("Config byte: ");
        Serial.print(value, HEX);
        Serial.print("\r\n");
    }
    {
        bool ok = false;
        auto value = thermostat.readTemperature(ok);
        if(ok)
        {
            Serial.print(" Temperature: ");
            Serial.print(value);
            Serial.print("\r\n");
        }
    }
    delay(1000);
}
