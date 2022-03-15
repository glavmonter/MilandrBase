#pragma once
#include <ftd2xx.h>
#include <LibFT4222.h>


class FtdiSpi {
public:
    explicit FtdiSpi(int device = 0);
    ~FtdiSpi();

    static int FindDevices(bool show = false);
    bool Write(uint8 *buffer, uint16 bufferSize, BOOL isEndTransaction);
    bool Read(uint8 *buffer, uint16 bufferSize, BOOL isEndTransaction);

private:
    int m_iDeviceNumber = 0;
    FT_HANDLE m_xHandle {};
};
