/**
 * @addtogroup applications
 * Утилиты для тестирования периферии 1986ВЕ92QI
 * @{
 */

/**
  ******************************************************************************
  * @file   iicwrite.cpp
  * @author Vladimir Meshkov <<v.meshkov@goqrate.com>>
  * @brief  Записать в I2C
  *
  *
  * Аргументы командной строки:
  *  - -l --list: вывести список подключенных преобразователей USB-SPI/I2C FT4222
  *  - -d --device: номер преобразователя USB-SPI/I2C, по-умолчанию 0
  *  - -a --addr: адрес I2C ведомого устройства
  *  - -r --reg: адрес регистра ведомого устройства
  */

/** @} */

#include <iostream>
#include <fmt/core.h>
#include <cxxopts.hpp>
#include <FtdiI2C.h>
#include <common.h>


static cxxopts::ParseResult parse(int argc, char *argv[]) {
    try {
        cxxopts::Options options(argv[0], " - 1986VE92 I2C test utility");
        options.positional_help("[optional args]").show_positional_help();
        options.add_options()
                ("h,help", "Print help")
                ("l,list", "List FTDI devices", cxxopts::value<bool>()->default_value("false"))
                ("d,device", "FTDI device number, default 0", cxxopts::value<int>()->default_value("0"))
                ("a,addr", "I2C slave device address", cxxopts::value<uint8_t>()->default_value("12"))
                ("r,reg", "Slave Register address", cxxopts::value<uint8_t>()->default_value("0"));

        auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::cout << options.help({}) << std::endl;
            ::exit(RETURN_STATUS::OK);
        }
        return result;

    } catch (const cxxopts::OptionException &e) {
        fmt::print(stderr, "error parsing options: {}\n", e.what());
        ::exit(RETURN_STATUS::COMMANDLINE_ERROR);
    }
}

void PrintStatus(uint8 status) {
    fmt::print("Status:\n");
    if (status & (1 << 0)) {
        fmt::print("    Controller busy\n");
    }
    if (status & (1 << 1)) {
        fmt::print("    Error condition\n");
    }
    if (status & (1 << 2)) {
        fmt::print("    Slave not ACK during last operation\n");
    }
    if (status & (1 << 3)) {
        fmt::print("    Data not ACK during last operation\n");
    }
    if (status & (1 << 4)) {
        fmt::print("    Arbitration LOST\n");
    }
    if (status & (1 << 5)) {
        fmt::print("    Controller IDLE\n");
    }
    if (status & (1 << 6)) {
        fmt::print("    BUS busy\n");
    }
}

int main(int argc, char *argv[]) {
    auto opts = parse(argc, argv);
    if (opts.count("list")) {
        FtdiI2C::FindDevices(true);
        ::exit(RETURN_STATUS::OK);
    }

    int devnum = opts["device"].as<int>();
    uint8_t slaveAddress = opts["addr"].as<uint8_t>();
    uint8_t regAddress = opts["reg"].as<uint8_t>();
    try {
        fmt::print("Using FTDI device number {}\n", devnum);
        FtdiI2C iic(devnum);
        uint8_t tx_buffer[] = {regAddress, 0xDE, 0xAD, 0xBE, 0xEF};
        uint16 written;
        auto ret = iic.WriteEx(slaveAddress, FLAG_START_AND_STOP, tx_buffer, sizeof(tx_buffer), written);
        fmt::print("Write START_AND_STOP {}\n", ret == FT4222_OK ? "Success" : "Not Success");
        uint8 status = 0;
        iic.GetStatus(status);
        PrintStatus(status);
    } catch (const FtdiException &e) {
        fmt::print(stderr, "FTDI error: {}\n", e.what());
        return RETURN_STATUS::FTDI_ERROR;
    }
    return 0;
}
