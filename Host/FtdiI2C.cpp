#include <vector>
#include <fmt/core.h>
#include "common.h"
#include "FtdiI2C.h"


static std::vector<FT_DEVICE_LIST_INFO_NODE> g_FT4222DevList;


FtdiI2C::FtdiI2C(int device) : m_iDeviceNumber(device) {
    if (g_FT4222DevList.empty()) {
        if (0 == FindDevices(false))
            throw FtdiException("No FTDI devices present");
    }

    FT_STATUS ftStatus;
    if (m_iDeviceNumber >= g_FT4222DevList.size())
        throw FtdiException("Device not found");

    ftStatus = FT_OpenEx((PVOID)g_FT4222DevList[m_iDeviceNumber].LocId, FT_OPEN_BY_LOCATION, &m_xHandle);
    if (FT_OK != ftStatus) {
        throw FtdiException("Cannot open device");
    }

    ftStatus = FT4222_I2CMaster_Init(m_xHandle, 100);
    if (FT_OK != ftStatus) {
        throw FtdiException("Cannot init IIC Master");
    }
}


FtdiI2C::~FtdiI2C() {
    if (m_xHandle) {
        FT4222_UnInitialize(m_xHandle);
        FT_Close(m_xHandle);
    }
}

FT4222_STATUS FtdiI2C::Read(uint16 slaveAddress, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred) {
    return FT4222_I2CMaster_Read(m_xHandle, slaveAddress, buffer, bufferSize, &sizeTransferred);
}

FT4222_STATUS FtdiI2C::ReadEx(uint16 slaveAddress, I2CFlag flag, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred) {
    return FT4222_I2CMaster_ReadEx(m_xHandle, slaveAddress, flag, buffer, bufferSize, &sizeTransferred);
}

FT4222_STATUS FtdiI2C::Write(uint16_t slaveAddress, uint8 *buffer, uint16 bufferSize, uint16_t &sizeTransferred) {
    return FT4222_I2CMaster_Write(m_xHandle, slaveAddress, buffer, bufferSize, &sizeTransferred);;
}

FT4222_STATUS FtdiI2C::WriteEx(uint16_t slaveAddress, I2CFlag flag, uint8 *buffer, uint16 bufferSize, uint16_t &sizeTransferred) {
    return FT4222_I2CMaster_WriteEx(m_xHandle, slaveAddress, flag, buffer, bufferSize, &sizeTransferred);
}

FT4222_STATUS FtdiI2C::GetStatus(uint8 &status) {
    return FT4222_I2CMaster_GetStatus(m_xHandle, &status);
}

FT4222_STATUS FtdiI2C::ResetBus() {
    return FT4222_I2CMaster_ResetBus(m_xHandle);
}


static std::string DeviceFlagToString(DWORD flags) {
    std::string msg;
    msg += (flags & 0x1)? "DEVICE_OPEN" : "DEVICE_CLOSED";
    msg += ", ";
    msg += (flags & 0x2)? "High-speed USB" : "Full-speed USB";
    return msg;
}

int FtdiI2C::FindDevices(bool show) {
    FT_STATUS ftStatus = 0;
    DWORD numOfDevices = 0;
    FT_CreateDeviceInfoList(&numOfDevices);
    g_FT4222DevList.clear();

    for (DWORD iDev = 0; iDev < numOfDevices; ++iDev) {
        FT_DEVICE_LIST_INFO_NODE devInfo{0};
        ftStatus = FT_GetDeviceInfoDetail(iDev, &devInfo.Flags, &devInfo.Type, &devInfo.ID, &devInfo.LocId,
                                          devInfo.SerialNumber, devInfo.Description, &devInfo.ftHandle);
        if (FT_OK == ftStatus) {
            const std::string desc = devInfo.Description;
            if (desc == "FT4222" || desc == "FT4222 A") {
                g_FT4222DevList.push_back(devInfo);
                if (show) {
                    fmt::print("Dev {:d}:\n", iDev);
                    fmt::print("  Flags= 0x{:X}, ({})\n", devInfo.Flags, DeviceFlagToString(devInfo.Flags));
                    fmt::print("  Type= 0x{:X}\n", devInfo.Type);
                    fmt::print("  ID= 0x{:X}\n", devInfo.ID);
                    fmt::print("  LocID= 0x{:X}\n", devInfo.LocId);
                    fmt::print("  SerialNumber= {}\n", devInfo.SerialNumber);
                    fmt::print("  Description= {}\n\n", devInfo.Description);
                }
            }
        }
    }
    return g_FT4222DevList.size();
}
