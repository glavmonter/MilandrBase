/**
 * @addtogroup applications
 * Утилиты для управления умным НЧ драйвером
 * @{
 */

/**
  ******************************************************************************
  * @file   whoiam.cpp
  * @author Vladimir Meshkov <<v.meshkov@goqrate.com>>
  * @brief  Прочитать регистр WHOIAM
  *
  * Утилитка whoiam для чтения регистра по адресу 0 (WHOIAM). Значение WHOIAM для НЧ драйвера: 0x1234
  *
  * Аргументы командной строки:
  *  - -l --list: вывести список подключенных преобразователей USB-SPI FT4222
  *  - -d --device: номер преобразователя USB-SPI, по-умолчанию 0
  *  - -t --tries: Количество попыток чтения, по-умолчанию 1
  */

/** @} */

#include <iostream>
#include <fmt/core.h>
#include <cxxopts.hpp>
#include <FtdiSpi.h>
#include <LFSmart.h>
#include <common.h>


static cxxopts::ParseResult parse(int argc, char *argv[]) {
    try {
        cxxopts::Options options(argv[0], " - DAC LF controller, Who I am??");
        options.positional_help("[optional args]").show_positional_help();
        options.add_options()
        ("h,help", "Print help")
        ("l,list", "List FTDI devices", cxxopts::value<bool>()->default_value("false"))
        ("t,tries", "Read tries, default 1", cxxopts::value<int>()->default_value("1"))
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

int main(int argc, char* argv[]) {

    auto opts = parse(argc, argv);
    if (opts.count("list")) {
        FtdiSpi::FindDevices(true);
        ::exit(RETURN_STATUS::OK);
    }

    int tries = opts["tries"].as<int>();
    int devnum = opts["device"].as<int>();
    try {
        fmt::print("Using FTDI device number {}\n", devnum);
        FtdiSpi spi(devnum);
        LFSmart lfSmart(spi, true, tries);
        uint16_t who_i_am = lfSmart.Whoiam();
        fmt::print("Who I am register: 0x{:04X}\n", who_i_am);

    } catch (const FtdiException &e) {
        fmt::print(stderr, "FTDI error: {}\n", e.what());
        return RETURN_STATUS::FTDI_ERROR;

    } catch (const LFSmartException &e) {
        fmt::print(stderr, "LFDriver error: {}\n", e.what());
        return e.CrcError() ? RETURN_STATUS::CRC_ERROR : RETURN_STATUS::LFDRV_ERROR;
    }
    return RETURN_STATUS::OK;
}
