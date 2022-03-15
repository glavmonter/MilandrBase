#ifndef LFDRIVER_COMMON_H
#define LFDRIVER_COMMON_H

#include <string>
#include <exception>
#include "cxxopts.hpp"
#include "commands.h"


bool no_channel_selected(const cxxopts::ParseResult &opts);
lfc::Channel GetChannelFromString(const std::string &ch);
uint16_t GetEepromDefaultMask(lfc::Channel channel);
uint16_t GetEepromMaximumMask(lfc::Channel channel);


enum RETURN_STATUS {
    OK = 0,
    FTDI_ERROR = 1,
    LFDRV_ERROR = 2,
    CRC_ERROR = 3,
    COMMANDLINE_ERROR = 4
};


class FtdiException : public std::exception {
public:
    explicit FtdiException(const char *message) : msg(message) {}
    const char *what() const noexcept override {
        return msg;
    }

private:
    const char *msg;
};


#endif //LFDRIVER_COMMON_H
