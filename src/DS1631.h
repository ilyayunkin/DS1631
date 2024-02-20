#pragma once

#include <stdint.h>

// I2C thermostat driver
class DS1631
{
public:
    // Enumerations according to the Datasheet. Don't change them.
    enum Commands
    {
        START_CONVERT    = 0x51,
        STOP_CONVERT     = 0x22,
        READ_TEMPERATURE = 0xAA,
        ACCESS_TH        = 0xA1,
        ACCESS_TL        = 0xA2,
        ACCESS_CONFIG    = 0xAC,
        POWER_ON_TEST    = 0x54,
    };

    enum ConfigBit
    {
        DONE     = 0x80,
        THF      = 0x40,
        TLF      = 0x20,
        NVB      = 0x10,
        R1       = 0x08,
        R0       = 0x04,
        POL      = 0x02,
        ONE_SHOT = 0x01,

        R0_R1    = 0x0C,
    };

    enum Resolution
    {
        RESOLUTION_9BIT  = 0x0,
        RESOLUTION_10BIT = 0x1 << 2,
        RESOLUTION_11BIT = 0x2 << 2,
        RESOLUTION_12BIT = 0x3 << 2,
    };

    enum ConversionMode
    {
        CONTINUOUS_MODE = 0,
        ONE_SHOT_MODE   = 1,
    };

    // Writes config too EEPROM. Conversion mode shouldn't change because chip's EEPROM guarantees only few writing cycles.
    void begin(uint8_t chipAddress, Resolution resolution = Resolution::RESOLUTION_12BIT, ConversionMode mode = ConversionMode::CONTINUOUS_MODE, bool start = true);
    // Checks if IC with the given address is responding.
    bool isOnLine() const;

    ConfigBit readConfig(bool &ok);
    // Returns temperature in Celsius degrees.
    float readTemperature(bool &ok) const;
    // NOTE: non-volatile configurations cn't be written while conversion is running - send STOP_CONVERT before.
    bool writeConfig(uint8_t value);
    bool writeTh(int16_t value);
    bool writeTl(int16_t value);
    // Writes commands that don't demand data set.
    bool writeCommand(Commands address);

    // Depend purely on what user passed to begin() or writeConfig(). Don't execute any bus operations.
    uint16_t getConversionTimeMs() const;
    uint16_t getCurrentResolution() const;

    // Error flag can be set if a bus operation failed. Clean it after reading manually.
    bool error() const;
    void clearError();

private:
    bool readRegister(Commands address, uint8_t size, uint8_t *buf) const;
    bool writeRegister(Commands address, uint8_t size, uint8_t *buf);
    bool writeRegister(Commands address, int16_t value);

    void rememberResolution(Resolution value);

private:
    const uint8_t BASE_ADDR = 0x48;
    uint8_t fullAddress = 0;
    float resolution = resolutions[RESOLUTION_12BIT >> 2];
    float conversionTime = conversionTimesMs[RESOLUTION_12BIT >> 2];
    const float resolutions[4] = { 0.5, 0.25, 0.125, 0.0625 };
    const uint16_t conversionTimesMs[4] = { 94, 188, 376, 750 };
    mutable bool errorFlag;
};
