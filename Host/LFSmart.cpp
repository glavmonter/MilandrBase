#include <chrono>
#include <thread>
#include <LFSmart.h>
#include "common.h"

static uint8_t crc8(uint8_t *pcBlock, size_t len);


static lfc::Registers GetAdcCommand(lfc::Channel channel) {
    switch (channel) {
        case lfc::Channel::CHANNEL_1:
            return lfc::Registers::ADC_CH1;
        case lfc::Channel::CHANNEL_2:
            return lfc::Registers::ADC_CH2;
        case lfc::Channel::CHANNEL_3:
            return lfc::Registers::ADC_CH3;
        case lfc::Channel::CHANNEL_4:
            return lfc::Registers::ADC_CH4;
        case lfc::Channel::CHANNEL_ALL:
            return lfc::Registers::ADC_ALL;
        default:
            return lfc::Registers::INVALID;
    }
}

static lfc::Registers GetDacCommand(lfc::Channel channel) {
    switch (channel) {
        case lfc::Channel::CHANNEL_1:
            return lfc::Registers::DAC_CH1;
        case lfc::Channel::CHANNEL_2:
            return lfc::Registers::DAC_CH2;
        case lfc::Channel::CHANNEL_3:
            return lfc::Registers::DAC_CH3;
        case lfc::Channel::CHANNEL_4:
            return lfc::Registers::DAC_CH4;
        case lfc::Channel::CHANNEL_ALL:
            return lfc::Registers::DAC_ALL;
        default:
            return lfc::Registers::INVALID;
    }
}


static lfc::Registers GetDacMaximumCommand(lfc::Channel channel) {
    switch (channel) {
        case lfc::Channel::CHANNEL_1:
            return lfc::Registers::DAC_MAX_CH1;
        case lfc::Channel::CHANNEL_2:
            return lfc::Registers::DAC_MAX_CH2;
        case lfc::Channel::CHANNEL_3:
            return lfc::Registers::DAC_MAX_CH3;
        case lfc::Channel::CHANNEL_4:
            return lfc::Registers::DAC_MAX_CH4;
        default:
            return lfc::Registers::INVALID;
    }
}


static lfc::Registers GetDacDefaultCommand(lfc::Channel channel) {
    switch (channel) {
        case lfc::Channel::CHANNEL_1:
            return lfc::Registers::DAC_DEFAULT_CH1;
        case lfc::Channel::CHANNEL_2:
            return lfc::Registers::DAC_DEFAULT_CH2;
        case lfc::Channel::CHANNEL_3:
            return lfc::Registers::DAC_DEFAULT_CH3;
        case lfc::Channel::CHANNEL_4:
            return lfc::Registers::DAC_DEFAULT_CH4;
        default:
            return lfc::Registers::INVALID;
    }
}


LFSmart::LFSmart(FtdiSpi &spi, bool crc, int tries) : m_xSpi(spi), m_bUseCRC(crc), m_iTries(tries) {
}


/**
 * Чтение регистра Who I am
 * @return Число, определяющее тип устройства
 */
uint16_t LFSmart::Whoiam() {
    uint16_t read_value;
    try {
        read_value = ReadRegister16b(lfc::Registers::WHOIAM, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Возвращает Регистр Состояния (STATUS) подключенного устройства.
 * @return Состояние устройства. @ref status_flags "Описание полей"
 */
uint16_t LFSmart::Status() {
    uint16_t read_value;
    try {
        read_value = ReadRegister16b(lfc::Registers::STATUS, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Чтение регистра Последней ошибки на устройстве
 * @return Ошибки, после последней выполненной команды. @ref lfc::LastError "Описание ошибок"
 */
lfc::LastError LFSmart::LastError() {
    uint16_t read_value;
    try {
        read_value = ReadRegister16b(lfc::Registers::LAST_ERROR, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return (lfc::LastError)read_value;
}


/**
 * Запись сырых данных в канал ЦАП с возможностью вычитывания записанных данных.
 * @param channel lfc::Channel - номер канала: CHANNEL_1..CHANNEL_4
 * @param value uint16_t - сырое значение, записываемое в канал ЦАП [0..65535]
 * @param readback bool - true для вычитывания записанного значения из канала ЦАП
 * @return Считанные данные из канала ЦАП или 0 при readback - false
 */
uint16_t LFSmart::WriteDacChannel(lfc::Channel channel, uint16_t value, bool readback) {
    if (channel == lfc::Channel::CHANNEL_ALL)
        throw LFSmartException("Channel mismatch");

    uint16_t read_value = 0;
    try {
        auto cmd = GetDacCommand(channel);
        WriteRegister16b(cmd, value, m_bUseCRC);
        if (readback)
            read_value = ReadRegister16b(cmd, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Запись значения напряжения в канал ЦАП с возможностью вычитывания записанных данных.
 *
 * <b>Проверка на границы диапазона напряжения не производится</b>
 * @param channel lfc::Channel - номер канала: CHANNEL_1..CHANNEL_4
 * @param value double - значение напряжение в вольтах в диапазоне [0..LFSmart::MaximumVoltage], записываемое в канал ЦАП
 * @param readback bool - true для вычитывания записанного значения из канала ЦАП
 * @return Считанные сырые данные из канала ЦАП или 0 при readback - false
 */
uint16_t LFSmart::WriteDacChannel(lfc::Channel channel, double value, bool readback) {
    if (lfc::Channel::CHANNEL_ALL == channel)
        throw LFSmartException("Channel mismatch");

    uint16_t raw = DacRealToRaw(value);
    uint16_t read_value = 0;
    try {
        auto cmd = GetDacCommand(channel);
        WriteRegister16b(cmd, raw, m_bUseCRC);
        if (readback)
            read_value = ReadRegister16b(cmd, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Чтение сырых данных канала ЦАП
 * @param channel lfc::Channel - номер канала: CHANNEL_1..CHANNEL_4
 * @return Считанные сырые данные из канала ЦАП
 */
uint16_t LFSmart::ReadDacChannel(lfc::Channel channel) {
    if (channel == lfc::Channel::CHANNEL_ALL)
        throw LFSmartException("Channel mismatch");

    uint16_t read_value;
    try {
        read_value = ReadRegister16b(GetDacCommand(channel), m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Чтение сырых данных канала АЦП
 * @param channel lfc::Channel - номер канала: CHANNEL_1..CHANNEL_4
 * @return Считанные сырые данные из канала АЦП
 */
uint16_t LFSmart::ReadAdcChannel(lfc::Channel channel) {
    if (channel == lfc::Channel::CHANNEL_ALL)
        throw LFSmartException("Channel mismatch");

    uint16_t read_value;
    try {
        read_value = ReadRegister16b(GetAdcCommand(channel), m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}

void LFSmart::ReadAdcAll(uint16_t *channels) {
    throw LFSmartException("ReadAdcAll not implemented");
}

/**
 * Перевод сырых данных АЦП в нормальные вольты
 * @param raw uint16_t - сырые данные АЦП
 * @return Напряжение АЦП в вольтах
 */
double LFSmart::AdcRawToReal(uint16_t raw) {
    return raw / 1000.0; // TODO Вычислить реальное значение пересчета АЦП
}

/**
 * Перевод сырых данных ЦАП в нормальные вольты
 * @param raw uint16_t - сырые данные ЦАП
 * @return Напряжение ЦАП в вольтах
 */
double LFSmart::DacRawToReal(uint16_t raw) {
    return MaximumVoltage * raw / 65535.0;
}

/**
 * Перевод нормального напряжения ЦАП из вольт в сырое значение
 *
 * <b>Проверка на границы диапазона напряжения не производится</b>
 * @param real_value double - нормальное значение напряжения ЦАП [0..LFSmart::MaximumVoltage]
 * @return Сырое значение напряжения
 */
uint16_t LFSmart::DacRealToRaw(double real_value) {
    double raw_double = real_value/MaximumVoltage * 65535.0;
    return (uint16_t)round(raw_double);
}


/**
 * Записать значение по-умолчанию в канал ЦАП
 * @param channel lfc::Channel - номер канала: CHANNEL_1..CHANNEL_4
 * @param value uint16_t - Сырое значение по-умолчанию
 * @param readback bool - true для вычитывания записанного значения из канала ЦАП
 * @return Сырое значение по-умолчанию ЦАП или 0 если readback false
 */
uint16_t LFSmart::WriteDacDefault(lfc::Channel channel, uint16_t value, bool readback) {
    if (channel == lfc::Channel::CHANNEL_ALL)
        throw LFSmartException("Channel mismatch");

    uint16_t read_value = 0;
    try {
        auto cmd = GetDacDefaultCommand(channel);
        WriteRegister16b(cmd, value, m_bUseCRC);
        if (readback)
            read_value = ReadRegister16b(cmd, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Записать значение по-умолчанию в вольтах в канал ЦАП
 *
 * <b>Проверка на границы диапазона напряжения не производится</b>
 * @param channel lfc::Channel - номер канала: CHANNEL_1..CHANNEL_4
 * @param value double - Значение по-умолчанию в вольтах [0..LFSmart::MaximumVoltage]
 * @param readback bool - true для вычитывания записанного значения из канала ЦАП
 * @return Сырое значение по-умолчанию ЦАП или 0 если readback false
 */
uint16_t LFSmart::WriteDacDefault(lfc::Channel channel, double value, bool readback) {
    if (lfc::Channel::CHANNEL_ALL == channel)
        throw LFSmartException("Channel mismatch");

    uint16_t raw = DacRealToRaw(value);
    uint16_t read_value = 0;
    try {
        auto cmd = GetDacDefaultCommand(channel);
        WriteRegister16b(cmd, raw, m_bUseCRC);
        if (readback)
            read_value = ReadRegister16b(cmd, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Чтение значения по-умолчанию канала ЦАП
 * @param channel lfc::Channel - номер канала: CHANNEL_1..CHANNEL_4
 * @return Сырое значение канала ЦАП по-умолчанию
 */
uint16_t LFSmart::ReadDacDefault(lfc::Channel channel) {
    if (lfc::Channel::CHANNEL_ALL == channel)
        throw LFSmartException("Channel mismatch");

    uint16_t read_value = 0;
    try {
        auto cmd = GetDacDefaultCommand(channel);
        read_value = ReadRegister16b(cmd, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Сохранить текущее значение канала ЦАП в энергонезависимую память.
 * @param mask uint16_t - Маска записываемых каналов. @ref nv_flags "Описание полей"
 */
void LFSmart::SaveDacToEeprom(uint16_t mask) {
    try {
        WriteRegister16b(lfc::Registers::SAVE_EEP, mask, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
}


/**
 * Записать максимальное ограничение выхода ЦАП
 * @param channel lfc::Channel - номер канала: CHANNEL_1..CHANNEL_4
 * @param value uint16_t - Сырое значение ограничения
 * @param readback bool - true для вычитывания записанного значения
 * @return Считанные данные максимального ограничения канала ЦАП или 0 при readback - false
 */
uint16_t LFSmart::WriteDacChannelMaximum(lfc::Channel channel, uint16_t value, bool readback) {
    if (lfc::CHANNEL_ALL == channel)
        throw LFSmartException("Channel mismatch");

    uint16_t read_value = 0;
    try {
        auto cmd = GetDacMaximumCommand(channel);
        WriteRegister16b(cmd, value, m_bUseCRC);
        if (readback)
            read_value = ReadRegister16b(cmd, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Чтение верхнего ограничения канала ЦАП
 * @param channel lfc::Channel - номер канала: CHANNEL_1..CHANNEL_4
 * @return Сырое значение ограничения канала
 */
uint16_t LFSmart::ReadDacChannelMaximum(lfc::Channel channel) {
    if (lfc::Channel::CHANNEL_ALL == channel)
        throw LFSmartException("Channel mismatch");

    uint16_t read_value;
    try {
        auto cmd = GetDacMaximumCommand(channel);
        read_value = ReadRegister16b(cmd, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}

/**
 * Чтение температуры внешнего датчика на печатной плате НЧ драйвера
 * @return Температура датчика в Кельвинах
 */
uint16_t LFSmart::TemperaturePcb() {
    uint16_t read_value;
    try {
        read_value = ReadRegister16b(lfc::Registers::THRM_PCB, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Чтение температуры микроконтроллера НЧ драйвера
 * @return Температура микроконтроллера в Кельвинах
 */
uint16_t LFSmart::TemperatureMcu() {
    uint16_t read_value;
    try {
        read_value = ReadRegister16b(lfc::Registers::THRM_MCU, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Выполнить команду на подключенном устройстве
 * @param value uint16_t - Команда, @ref svc_flags "описание полей"
 */
void LFSmart::SVC(uint16_t value) {
    try {
        WriteRegister16b(lfc::SVC, value, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
}


/**
 * Чтение версии программного кода у подключенного устройства
 * @return Версия в виде LF_VER_MAJOR * 10000 + LF_VER_MINOR * 100 + LF_VER_PATCH
 */
uint16_t LFSmart::Version() {
    uint16_t read_value;
    try {
        read_value = ReadRegister16b(lfc::Registers::VERSION, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


std::pair<uint32_t, uint32_t> LFSmart::ReadCRC() {
    uint32_t crc_hw = 0, crc_sw = 0;
    try {
        crc_hw = ReadRegister32b(lfc::Registers::CRC_HW, m_bUseCRC);
        crc_sw = ReadRegister32b(lfc::Registers::CRC_SW, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return {crc_hw, crc_sw};
}

void LFSmart::WriteCert(uint16_t value) {
    try {
        WriteRegister16b(lfc::Registers::CERT, value, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
}


uint16_t LFSmart::ReadCert() {
    uint16_t read_value;
    try {
        read_value = ReadRegister16b(lfc::Registers::CERT, m_bUseCRC);
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Запись произвольного регистра.
 * @param command uint8_t - номер регистра 0..127
 * @param value uint16_t - записываемые данные
 * @param readback bool - true считывание записанного регистра
 * @return Значение регистра или 0 при readback false
 */
uint16_t LFSmart::WriteRaw(uint8_t command, uint16_t value, bool readback) {
    uint8 tx_buffer[3];
    tx_buffer[0] = (command << 1) | lfc::Access::WRITE;
    tx_buffer[1] = value & 0x00FF;
    tx_buffer[2] = (value & 0xFF00) >> 8;

    uint16_t read_value = 0;
    try {
        m_xSpi.Write(&tx_buffer[0], 1, FALSE);
        m_xSpi.Write(&tx_buffer[1], 2, TRUE);
        if (readback) {
            read_value = ReadRaw(command);
        }
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


/**
 * Чтение произвольного регистра
 * @param command uint8_t - номер регистра 0..127
 * @return Значение считанного регистра
 */
uint16_t LFSmart::ReadRaw(uint8_t command) {
    uint16_t read_value;
    try {
        uint8 rx_buffer[2];
        uint8 ucmd = (command << 1) | lfc::Access::READ;
        m_xSpi.Write(&ucmd, 1, FALSE);
        m_xSpi.Read(rx_buffer, 2, TRUE);
        read_value = (rx_buffer[1] << 8) | rx_buffer[0];
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


uint16_t LFSmart::ReadRegister16b(lfc::Registers cmd, bool crc) {
    if ((cmd == lfc::Registers::DAC_ALL) || (cmd == lfc::Registers::ADC_ALL))
        throw LFSmartException("Register size not 16 bit");

    int tries = m_iTries;
    uint16_t read_value;
    try {
        uint8 rx_buffer[3] = {0};
        uint8 ucmd = (cmd << 1) | lfc::Access::READ;

        while (tries > 0) {
            m_xSpi.Write(&ucmd, 1, FALSE);
            if (!crc) {
                m_xSpi.Read(rx_buffer, 2, TRUE);
            } else {
                m_xSpi.Read(rx_buffer, 3, TRUE);
            }

            read_value = (rx_buffer[1] << 8) | rx_buffer[0];
            if (crc) {
                uint8_t crc_calc = crc8(rx_buffer, 2);
                if (crc_calc == rx_buffer[2] ) {
                    break;
                }
            } else {
                break;
            }
            tries--;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if (tries == 0) {
            // Кончились попытки чтения
            throw LFSmartException("CRC error", true);
        }
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


void LFSmart::WriteRegister16b(lfc::Registers cmd, uint16_t value, bool crc) {
    if ((cmd == lfc::Registers::DAC_ALL) || (cmd == lfc::Registers::ADC_ALL))
        throw LFSmartException("Register size not 16 bit");

    uint8 tx_buffer[4];
    tx_buffer[0] = (cmd << 1) | lfc::Access::WRITE;
    tx_buffer[1] = value & 0x00FF;
    tx_buffer[2] = (value & 0xFF00) >> 8;

    if (crc) {
        tx_buffer[3] = crc8(tx_buffer, 3);
    }
    try {
        m_xSpi.Write(&tx_buffer[0], 1, FALSE);
        if (!crc) {
            m_xSpi.Write(&tx_buffer[1], 2, TRUE);
        } else {
            m_xSpi.Write(&tx_buffer[1], 3, TRUE);
        }
    } catch (const FtdiException &e) {
        throw;
    }
}


uint32_t LFSmart::ReadRegister32b(lfc::Registers cmd, bool crc) {
    if (!((cmd == lfc::Registers::CRC_SW) || (cmd == lfc::Registers::CRC_HW)))
        throw LFSmartException("Register size not 16 bit");

    int tries = m_iTries;
    uint32_t read_value;
    try {
        uint8 rx_buffer[5] = {0};
        uint8 ucmd = (cmd << 1) | lfc::Access::READ;

        while (tries > 0) {
            m_xSpi.Write(&ucmd, 1, FALSE);
            if (!crc) {
                m_xSpi.Read(rx_buffer, 4, TRUE);
            } else {
                m_xSpi.Read(rx_buffer, 5, TRUE);
            }

            read_value = (rx_buffer[3] << 24) | (rx_buffer[2] << 16) | (rx_buffer[1] << 8) | rx_buffer[0];
            if (crc) {
                uint8_t crc_calc = crc8(rx_buffer, 4);
                if (crc_calc == rx_buffer[4]) {
                    break;
                }
            } else {
                break;
            }
            tries--;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if (tries == 0) {
            // Кончились попытки чтения
            throw LFSmartException("CRC error", true);
        }
    } catch (const FtdiException &e) {
        throw;
    }
    return read_value;
}


uint16_t LFSmart::MakeVersion(uint8_t major, uint8_t minor, uint8_t patch) {
    return major*10000 + minor*100 + patch;
}


/*
  Name  : CRC-8-ITU
  Poly  : 0x07
  Init  : 0x00
  Revert: false
  XorOut: 0x55
  Check : 0xA1 ("123456789")
*/
const uint8_t Crc8Table[256] = {
        0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
        0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
        0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
        0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
        0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
        0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
        0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
        0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
        0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
        0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
        0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
        0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
        0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
        0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
        0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
        0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
        0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
        0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
        0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
        0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
        0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
        0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
        0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
        0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
        0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
        0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
        0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
        0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
        0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
        0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
        0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
        0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3,
};

static uint8_t crc8(uint8_t *pcBlock, size_t len) {
    uint8_t crc = 0x00;
    while (len--) {
        crc = Crc8Table[crc ^ *pcBlock++];
    }
    return crc ^ 0x55;
}
