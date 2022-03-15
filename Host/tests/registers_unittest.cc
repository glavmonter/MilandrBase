#include <chrono>
#include <thread>
#include "FtdiSpi.h"
#include "LFSmart.h"
#include "gtest/gtest.h"

namespace {
    bool use_crc = true;
    int NumTries = 20;
    int ResetTime = 2000;

    TEST(Services, FTDI) {
        EXPECT_NO_THROW(FtdiSpi ftdiSpi(0));
    }

    TEST(Services, Whoiam) {
        FtdiSpi ftdiSpi(0);
        LFSmart lfSmart(ftdiSpi, use_crc, NumTries);
        uint16_t whoiam = lfSmart.Whoiam();
        EXPECT_EQ(whoiam, LFSmart::WhoiamExpected());
    }

    TEST(Services, Version_010) {
        FtdiSpi ftdiSpi(0);
        LFSmart lfSmart(ftdiSpi, use_crc, NumTries);
        uint16_t version = lfSmart.Version();
        EXPECT_NE(version, LFSmart::MakeVersion(0, 1, 0));
    }

    TEST(Services, Version_020) {
        FtdiSpi ftdiSpi(0);
        LFSmart lfSmart(ftdiSpi, use_crc, NumTries);
        uint16_t version = lfSmart.Version();
        EXPECT_EQ(version, LFSmart::MakeVersion(0, 2, 0));
    }

    TEST(DacMaximum, CHALL) {
        FtdiSpi ftdiSpi(0);
        LFSmart lfSmart(ftdiSpi, use_crc, NumTries);
        lfSmart.SVC(LF_SVC_RESET);
        std::this_thread::sleep_for(std::chrono::milliseconds(ResetTime));

        lfSmart.WriteDacChannelMaximum(lfc::Channel::CHANNEL_1, UINT16_MAX, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_NOERROR);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        lfSmart.WriteDacChannelMaximum(lfc::Channel::CHANNEL_2, UINT16_MAX, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_NOERROR);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        lfSmart.WriteDacChannelMaximum(lfc::Channel::CHANNEL_3, UINT16_MAX, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_NOERROR);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        lfSmart.WriteDacChannelMaximum(lfc::Channel::CHANNEL_4, UINT16_MAX, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_NOERROR);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        lfSmart.SaveDacToEeprom(NV_DAC_MAX_CH1 | NV_DAC_MAX_CH2 | NV_DAC_MAX_CH3 | NV_DAC_MAX_CH4);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_NOERROR);

        lfSmart.SVC(LF_SVC_RESET);
        std::this_thread::sleep_for(std::chrono::milliseconds(ResetTime));
        EXPECT_EQ(lfSmart.ReadDacChannelMaximum(lfc::Channel::CHANNEL_1), UINT16_MAX);
        EXPECT_EQ(lfSmart.ReadDacChannelMaximum(lfc::Channel::CHANNEL_2), UINT16_MAX);
        EXPECT_EQ(lfSmart.ReadDacChannelMaximum(lfc::Channel::CHANNEL_3), UINT16_MAX);
        EXPECT_EQ(lfSmart.ReadDacChannelMaximum(lfc::Channel::CHANNEL_4), UINT16_MAX);
    }

    void MaxValue_Truncate(lfc::Channel ch, uint16_t max_ch_value) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        FtdiSpi ftdiSpi(0);
        LFSmart lfSmart(ftdiSpi, use_crc, NumTries);
        lfSmart.SVC(LF_SVC_RESET);
        std::this_thread::sleep_for(std::chrono::milliseconds(ResetTime));

        uint16_t rb_val = lfSmart.WriteDacChannel(ch, (uint16_t)0, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        rb_val = lfSmart.ReadDacChannel(ch);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_EQ(0, rb_val);
        rb_val = lfSmart.WriteDacChannelMaximum(ch, max_ch_value, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_EQ(max_ch_value, rb_val);
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_NOERROR);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        lfSmart.WriteDacChannel(ch, max_ch_value, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_NOERROR);

        rb_val = lfSmart.ReadDacChannel(ch);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_EQ(rb_val, max_ch_value);

        lfSmart.WriteDacChannel(ch, (uint16_t)(max_ch_value + 1), false);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_DAC_TRUNCATE);
        rb_val = lfSmart.ReadDacChannel(ch);
        EXPECT_EQ(rb_val, max_ch_value);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    TEST(MaxValueTruncate, CH1) {
        MaxValue_Truncate(lfc::Channel::CHANNEL_1, 1234);
    }
    TEST(MaxValueTruncate, CH2) {
        MaxValue_Truncate(lfc::Channel::CHANNEL_2, 65000);
    }
    TEST(MaxValueTruncate, CH3) {
        MaxValue_Truncate(lfc::Channel::CHANNEL_3, 20000);
    }
    TEST(MaxValueTruncate, CH4) {
        MaxValue_Truncate(lfc::Channel::CHANNEL_4, 30000);
    }


    void MaxValue_Overflow(lfc::Channel ch, uint16_t max_ch_value) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        FtdiSpi ftdiSpi(0);
        LFSmart lfSmart(ftdiSpi, use_crc, NumTries);
        lfSmart.SVC(LF_SVC_RESET);
        std::this_thread::sleep_for(std::chrono::milliseconds(ResetTime));

        uint16_t rb_val;

        rb_val = lfSmart.WriteDacChannelMaximum(ch, 65535, true);
        EXPECT_EQ(65535, rb_val);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_NOERROR);

        lfSmart.WriteDacChannel(ch, max_ch_value, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        rb_val = lfSmart.ReadDacChannel(ch);
        EXPECT_EQ(max_ch_value, rb_val);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        lfSmart.WriteDacChannelMaximum(ch, max_ch_value, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_NOERROR);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        rb_val = lfSmart.ReadDacChannelMaximum(ch);
        EXPECT_EQ(rb_val, max_ch_value);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        lfSmart.WriteDacChannelMaximum(ch, (uint16_t)(max_ch_value - 1), false);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_DAC_OVERFLOW);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        rb_val = lfSmart.ReadDacChannelMaximum(ch);
        EXPECT_EQ(rb_val, max_ch_value - 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    TEST(MaxValueOverflow, CH1) {
        MaxValue_Overflow(lfc::Channel::CHANNEL_1, 1000);
    }
    TEST(MaxValueOverflow, CH2) {
        MaxValue_Overflow(lfc::Channel::CHANNEL_2, 2000);
    }
    TEST(MaxValueOverflow, CH3) {
        MaxValue_Overflow(lfc::Channel::CHANNEL_3, 3000);
    }
    TEST(MaxValueOverflow, CH4) {
        MaxValue_Overflow(lfc::Channel::CHANNEL_4, 4000);
    }


    void CheckNvDefault(lfc::Channel ch, uint16_t value, uint16_t flags) {
        FtdiSpi ftdiSpi(0);
        LFSmart lfSmart(ftdiSpi, use_crc, NumTries);
        lfSmart.SVC(LF_SVC_RESET);
        std::this_thread::sleep_for(std::chrono::milliseconds(ResetTime));

        uint16_t rb_value;
        lfSmart.WriteDacDefault(ch, value, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        lfSmart.SaveDacToEeprom(flags);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_NOERROR);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        lfSmart.SVC(LF_SVC_RESET);
        std::this_thread::sleep_for(std::chrono::milliseconds(ResetTime));
        rb_value = lfSmart.ReadDacChannel(ch);
        EXPECT_EQ(rb_value, value);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        lfSmart.WriteDacDefault(ch, uint16_t (value+1), false);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        lfSmart.SaveDacToEeprom(flags);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        EXPECT_EQ(lfSmart.LastError(), (uint16_t)lfc::LastError::LE_NOERROR);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        lfSmart.SVC(LF_SVC_RESET);
        std::this_thread::sleep_for(std::chrono::milliseconds(ResetTime));
        rb_value = lfSmart.ReadDacChannel(ch);
        EXPECT_EQ(rb_value, value+1);
    }


    TEST(NvDefault, CH1) {
        CheckNvDefault(lfc::Channel::CHANNEL_1, 1, NV_DAC_DEFAULT_CH1);
    }
    TEST(NvDefault, CH2) {
        CheckNvDefault(lfc::Channel::CHANNEL_2, 11, NV_DAC_DEFAULT_CH2);
    }
    TEST(NvDefault, CH3) {
        CheckNvDefault(lfc::Channel::CHANNEL_3, 21, NV_DAC_DEFAULT_CH3);
    }
    TEST(NvDefault, CH4) {
        CheckNvDefault(lfc::Channel::CHANNEL_4, 31, NV_DAC_DEFAULT_CH4);
    }

}
