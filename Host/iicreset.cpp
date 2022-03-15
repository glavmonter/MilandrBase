/**
 * @addtogroup applications
 * Утилиты для тестирования периферии 1986ВЕ92QI
 * @{
 */

/**
  ******************************************************************************
  * @file   iicwrite.cpp
  * @author Vladimir Meshkov <<v.meshkov@goqrate.com>>
  * @brief  Сброс шины I2C
  *
  *
  * Аргументы командной строки:
  *  - -l --list: вывести список подключенных преобразователей USB-SPI/I2C FT4222
  *  - -d --device: номер преобразователя USB-SPI/I2C, по-умолчанию 0
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
                ("d,device", "FTDI device number, default 0", cxxopts::value<int>()->default_value("0"));

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

int main(int argc, char *argv[]) {
    auto opts = parse(argc, argv);
    if (opts.count("list")) {
        FtdiI2C::FindDevices(true);
        ::exit(RETURN_STATUS::OK);
    }

    int devnum = opts["device"].as<int>();
    try {
        fmt::print("Using FTDI device number {}\n", devnum);
        FtdiI2C iic(devnum);
        iic.ResetBus();
    } catch (const FtdiException &e) {
        fmt::print(stderr, "FTDI error: {}\n", e.what());
        return RETURN_STATUS::FTDI_ERROR;
    }
    return 0;
}
