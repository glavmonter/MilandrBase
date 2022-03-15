#include <vector>
#include <fmt/core.h>
#include "common.h"
#include "FtdiSpi.h"


static std::vector<FT_DEVICE_LIST_INFO_NODE> g_FT4222DevList;


FtdiSpi::FtdiSpi(int device) : m_iDeviceNumber(device) {
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

    ftStatus = FT4222_SetClock(m_xHandle, SYS_CLK_24);
    if (FT_OK != ftStatus) {
        throw FtdiException("Error on setting Clock");
    }

    ftStatus = FT4222_SPIMaster_Init(m_xHandle, SPI_IO_SINGLE, CLK_DIV_8, CLK_IDLE_LOW, CLK_TRAILING, 1);
    if (FT_OK != ftStatus) {
        throw FtdiException("Cannot init SPI Master");
    }
}


FtdiSpi::~FtdiSpi() {
    if (m_xHandle) {
        FT4222_UnInitialize(m_xHandle);
        FT_Close(m_xHandle);
    }
}

bool FtdiSpi::Read(uint8 *buffer, uint16 bufferSize, BOOL isEndTransaction) {
    uint16 transferred = 0;
    FT_STATUS ftStatus;
    ftStatus = FT4222_SPIMaster_SingleRead(m_xHandle, buffer, bufferSize, &transferred, isEndTransaction);
    if (FT_OK != ftStatus)
        throw FtdiException("FT4222_SPIMaster_SingleRead error in FtdiSpi::Read");
    return transferred == bufferSize;
}

bool FtdiSpi::Write(uint8 *buffer, uint16 bufferSize, BOOL isEndTransaction) {
    uint16 transferred = 0;
    FT_STATUS ftStatus;
    ftStatus = FT4222_SPIMaster_SingleWrite(m_xHandle, buffer, bufferSize, &transferred, isEndTransaction);
    if (FT_OK != ftStatus)
        throw FtdiException("FT4222_SPIMaster_SingleWrite error in FtdiSpi::Write");
    return transferred == bufferSize;
}


static std::string DeviceFlagToString(DWORD flags) {
    std::string msg;
    msg += (flags & 0x1)? "DEVICE_OPEN" : "DEVICE_CLOSED";
    msg += ", ";
    msg += (flags & 0x2)? "High-speed USB" : "Full-speed USB";
    return msg;
}

int FtdiSpi::FindDevices(bool show) {
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
