#pragma once
#include <exception>
#include "FtdiSpi.h"
#include "commands.h"


class LFSmart {
public:
    explicit LFSmart(FtdiSpi &spi, bool crc, int tries = 1);
    uint16_t Whoiam();
    uint16_t Status();
    lfc::LastError LastError();

    uint16_t WriteDacChannel(lfc::Channel channel, uint16_t value, bool readback);
    uint16_t WriteDacChannel(lfc::Channel channel, double value, bool readback);
    uint16_t ReadDacChannel(lfc::Channel channel);
    uint16_t ReadAdcChannel(lfc::Channel channel);
    void ReadAdcAll(uint16_t *channels);

    uint16_t WriteDacDefault(lfc::Channel channel, uint16_t value, bool readback);
    uint16_t WriteDacDefault(lfc::Channel channel, double value, bool readback);
    uint16_t ReadDacDefault(lfc::Channel channel);
    void SaveDacToEeprom(uint16_t mask);

    uint16_t WriteDacChannelMaximum(lfc::Channel channel, uint16_t value, bool readback);
    uint16_t ReadDacChannelMaximum(lfc::Channel channel);

    uint16_t TemperaturePcb();
    uint16_t TemperatureMcu();

    void SVC(uint16_t value);
    void WriteCert(uint16_t value);
    uint16_t ReadCert();

    uint16_t Version();
    std::pair<uint32_t, uint32_t> ReadCRC();

    uint16_t WriteRaw(uint8_t command, uint16_t value, bool readback);
    uint16_t ReadRaw(uint8_t command);

    static double AdcRawToReal(uint16_t raw);
    static double DacRawToReal(uint16_t raw);
    static uint16_t DacRealToRaw(double real_value);

    static constexpr const double MaximumVoltage = 3.0 * (11.0e3/2.7e3 + 1); ///< Максимальное значение напряжения канала ЦАП ~15.2 В
    static uint16_t MakeVersion(uint8_t major, uint8_t minor, uint8_t patch);
    static uint16_t WhoiamExpected() { return 0x1234; }

private:
    uint16_t ReadRegister16b(lfc::Registers cmd, bool crc);
    void WriteRegister16b(lfc::Registers cmd, uint16_t value, bool crc);
    uint32_t LFSmart::ReadRegister32b(lfc::Registers cmd, bool crc);

    FtdiSpi &m_xSpi;
    bool m_bUseCRC;
    int m_iTries;
};


class LFSmartException : public std::exception {
public:
    explicit LFSmartException(const char *message, bool crc_error = false) : msg(message), crc_error(crc_error) {}
    const char *what() const noexcept override {
        return msg;
    }
    bool CrcError() const { return crc_error; }

private:
    const char *msg;
    bool crc_error;
};
