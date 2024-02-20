#include <Arduino.h>
#include <stdint.h>
#include <iarduino_OLED_txt.h>
#include <DS1631.h>

iarduino_OLED_txt myOLED(0x3C);
extern uint8_t MediumFont[];

const uint8_t CHIP_ADDR = 0x00;
DS1631 thermostat;

void setCursor()
{
    myOLED.setCursor(16,4);
}

void setup()
{
    myOLED.begin();
    myOLED.setFont(MediumFont);
    thermostat.begin(CHIP_ADDR, DS1631::RESOLUTION_9BIT);
    delay(thermostat.getConversionTimeMs());
    auto online = thermostat.isOnLine();
    if(!online)
    {
        setCursor();
        myOLED.print("T not Found\r\n");
    }
}

void loop()
{
    bool ok = false;
    auto value = thermostat.readConfig(ok);
    {
        bool ok = false;
        auto value = thermostat.readTemperature(ok);
        if(ok)
        {
            myOLED.setCursor(16,4);
            if(value > 0)
                myOLED.print("+");
            myOLED.print(value);
            myOLED.print(" C");
        }
    }
    delay(1000);
}
