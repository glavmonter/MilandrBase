#include "common.h"


bool no_channel_selected(const cxxopts::ParseResult &opts) {
    bool one_of = opts.count("ch1") | opts.count("ch2") | opts.count("ch3") | opts.count("ch4");
    return !one_of;
}


lfc::Channel GetChannelFromString(const std::string &ch) {
    if (ch == "ch1")
        return lfc::Channel::CHANNEL_1;
    if (ch == "ch2")
        return lfc::Channel::CHANNEL_2;
    if (ch == "ch3")
        return lfc::Channel::CHANNEL_3;
    if (ch == "ch4")
        return lfc::Channel::CHANNEL_4;
    return lfc::Channel::CHANNEL_INVALID;
}


uint16_t GetEepromDefaultMask(lfc::Channel channel) {
    switch (channel) {
        case lfc::Channel::CHANNEL_1:
            return NV_DAC_DEFAULT_CH1;
        case lfc::Channel::CHANNEL_2:
            return NV_DAC_DEFAULT_CH2;
        case lfc::Channel::CHANNEL_3:
            return NV_DAC_DEFAULT_CH3;
        case lfc::Channel::CHANNEL_4:
            return NV_DAC_DEFAULT_CH4;
        case lfc::Channel::CHANNEL_ALL:
            return NV_DAC_DEFAULT_CH1 | NV_DAC_DEFAULT_CH2 | NV_DAC_DEFAULT_CH3 | NV_DAC_DEFAULT_CH4;
        default:
            return 0;
    }
}


uint16_t GetEepromMaximumMask(lfc::Channel channel) {
    switch (channel) {
        case lfc::Channel::CHANNEL_1:
            return NV_DAC_MAX_CH1;
        case lfc::Channel::CHANNEL_2:
            return NV_DAC_MAX_CH2;
        case lfc::Channel::CHANNEL_3:
            return NV_DAC_MAX_CH3;
        case lfc::Channel::CHANNEL_4:
            return NV_DAC_MAX_CH4;
        case lfc::Channel::CHANNEL_ALL:
            return NV_DAC_MAX_CH1 | NV_DAC_MAX_CH2 | NV_DAC_MAX_CH3 | NV_DAC_MAX_CH4;
        default:
            return 0;
    }
}
