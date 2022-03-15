#pragma once
#include <ftd2xx.h>
#include <LibFT4222.h>

enum I2CFlag {
    FLAG_NONE = 0x80,
    FLAG_START = 0x02,
    FLAG_REPEATED_START = 0x03,
    FLAG_STOP = 0x04,
    FLAG_START_AND_STOP = 0x06
};

class FtdiI2C {
public:
    explicit FtdiI2C(int device = 0);
    ~FtdiI2C();

    static int FindDevices(bool show = false);
    FT4222_STATUS Write(uint16_t slaveAddress, uint8 *buffer, uint16 bufferSize, uint16_t &sizeTransferred);
    FT4222_STATUS WriteEx(uint16_t slaveAddress, I2CFlag flag, uint8 *buffer, uint16 bufferSize, uint16_t &sizeTransferred);
    FT4222_STATUS Read(uint16 slaveAddress, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred);
    FT4222_STATUS ReadEx(uint16 slaveAddress, I2CFlag flag, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred);
    FT4222_STATUS GetStatus(uint8 &status);
    FT4222_STATUS ResetBus();


private:
    int m_iDeviceNumber = 0;
    FT_HANDLE m_xHandle {};
};



