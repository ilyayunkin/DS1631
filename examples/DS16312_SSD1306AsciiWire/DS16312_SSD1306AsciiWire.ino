#include <Arduino.h>
#include <stdint.h>
#include <SSD1306AsciiWire.h>
#include <DS1631.h>

namespace{
SSD1306AsciiWire oled;
const uint8_t OLED_ADDR = 0x3C;

const uint8_t THERMOSTAT_ADDR = 0x00;
DS1631 thermostat;

float prevValue = -274;
}

void setup()
{
    Wire.begin();
    Wire.setClock(400000L);
    oled.begin(&Adafruit128x64, OLED_ADDR);
    oled.setFont(Adafruit5x7);
    oled.set2X();
    oled.clear();
    oled.setCursor(0, 3);

    thermostat.begin(THERMOSTAT_ADDR, DS1631::RESOLUTION_9BIT);
    delay(thermostat.getConversionTimeMs());
    auto online = thermostat.isOnLine();
    if(!online)
    {
        oled.print("\r\nT not Found\r\n");
    }
}

void loop()
{
    bool ok = false;
    auto value = thermostat.readConfig(ok);
    {
        bool ok = false;
        auto value = thermostat.readTemperature(ok);
        if(ok && value != prevValue)
        {
            oled.setCursor(0, 3);
            if(value > 0)
                oled.print('+');
            oled.print(value);
            oled.print(" C");
            oled.clearToEOL();
        }
        prevValue = value;
    }
    delay(1000);
}
