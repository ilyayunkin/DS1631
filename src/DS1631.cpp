#include "DS1631.h"
#include <Wire.h>

void DS1631::begin(uint8_t chipAddress, DS1631::Resolution resolution, ConversionMode mode, bool start)
{
    fullAddress = BASE_ADDR | chipAddress;

    Wire.begin();

    // Without configuring on every start test IC worked incorrectly.
    writeConfig(resolution | mode);

    if(!start)
        return;

    writeCommand(Commands::START_CONVERT);
}

bool DS1631::isOnLine() const
{
    Wire.beginTransmission(fullAddress);
    auto ok = !Wire.endTransmission();
    errorFlag &= !ok;
    return ok;
}

DS1631::ConfigBit DS1631::readConfig(bool &ok)
{
    uint8_t ret = 0;
    ok = readRegister(Commands::ACCESS_CONFIG, 1, &ret);

    return static_cast<ConfigBit>(ret);
}

float DS1631::readTemperature(bool &ok) const
{
    int16_t ret = 0;
    uint8_t buf[2];

    if((ok = readRegister(Commands::READ_TEMPERATURE, 2, buf)))
        ret = ((int16_t(buf[0]) << 8) | buf[1]) >> 4;

    return ret * resolutions[3];
}

bool DS1631::writeConfig(uint8_t value)
{
    if(!writeRegister(Commands::ACCESS_CONFIG, 1, &value))
        return false;

    rememberResolution(static_cast<Resolution>(value & R0_R1));
    return true;
}

bool DS1631::writeTh(int16_t value)
{
    return writeRegister(Commands::ACCESS_TH, value);
}

bool DS1631::writeTl(int16_t value)
{
    return writeRegister(Commands::ACCESS_TL, value);
}

bool DS1631::writeCommand(DS1631::Commands address)
{
    return writeRegister(address, 0, nullptr);
}

uint16_t DS1631::getConversionTimeMs() const { return conversionTime; }

uint16_t DS1631::getCurrentResolution() const { return resolution; }

bool DS1631::error() const
{
    return errorFlag;
}

void DS1631::clearError()
{
    errorFlag = false;
}

bool DS1631::readRegister(DS1631::Commands address, uint8_t size, uint8_t *buf) const
{
    Wire.beginTransmission(fullAddress);
    Wire.write(address);
    Wire.endTransmission(false);
    Wire.requestFrom(fullAddress, 2);

    const bool ok = Wire.available() >= size;
    if(ok){
        for(uint8_t i = 0; i < size; ++i)
            buf[i] = Wire.read();
    }

    const auto error = Wire.endTransmission();
    errorFlag &= !ok && !error;

    return ok && !error;
}

bool DS1631::writeRegister(DS1631::Commands address, uint8_t size, uint8_t *buf)
{
    Wire.beginTransmission(fullAddress);
    Wire.write(address);

    for(uint8_t i = 0; i < size; ++i)
        Wire.write(buf[i]);

    const bool ok = !Wire.endTransmission();
    errorFlag &= !ok;

    return ok;
}

bool DS1631::writeRegister(DS1631::Commands address, int16_t value)
{
    uint8_t buf[2] = {(value >> 8) & 0xff, value & 0xff};
    return writeRegister(address, 2, buf);
}

void DS1631::rememberResolution(DS1631::Resolution value)
{
    const auto index = value >> 2;
    resolution = resolutions[index];
    conversionTime = conversionTimesMs[index];
}
