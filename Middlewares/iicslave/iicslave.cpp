/**
 * @file iicslave.cpp
 * @brief Программная реализация I2C Ведомого устройства
 */

#include <bitbanding.h>
#include <MDR32F9Qx_rst_clk.h>
#include "iicslave.h"


#if defined ( __GNUC__ )
#pragma GCC optimize ("Os")
#else
#error "Enable high optimization for compilator"
#endif


static void InitPortClock(MDR_PORT_TypeDef *port);
static void InitPin(MDR_PORT_TypeDef *port, PORT_Pin_TypeDef pin, PORT_FUNC_TypeDef alt, PORT_OE_TypeDef oe);
static IRQn_Type TimerIrqNumber(MDR_TIMER_TypeDef *timer);
static void InitTimerClock(MDR_TIMER_TypeDef *timer);
static void InitTimer(MDR_TIMER_TypeDef *timer,
                      TIMER_Channel_Number_TypeDef timerSDAChannel,
                      TIMER_Channel_Number_TypeDef timerSCLChannel,
                      uint32_t IrqFlags);
static void IICs_PinLow(IICSlavePin &pin);
static void IICs_PinHigh(IICSlavePin &pin);
static uint8_t TimerIEMask(TIMER_Channel_Number_TypeDef channel);


#ifdef IICS_IN_RAM
#define ramfunc_ __attribute__ ((section(".ramfunc")))
#else
#define ramfunc_
#endif


/**
 * @brief Инициализация периферии Ведомого I2C
 *
 * Настраиваются выводы каналов таймера, как альтернативные выходы.
 *
 * Настраиваются два канала MDR_TIMERx на захват и генерирование прерываний по фронту и срезу сигналов.
 * CCR_CAP настраивается для захвата переднего фронта сигнала. CCR_CAP1 настраивается для захвата заднего фронта сигнала.
 *
 * @param slave   Структура Ведомого I2C
 * @param timer   Таймер для захвата фронтов: MDR_TIMER1..MDR_TIMER3
 * @param portSDA PORT для вывода SDA: MDR_PORTA..MDR_PORTF
 * @param pinSDA  Вывод SDA: PORT_Pin_0..PORT_Pin_15
 * @param altSDA  Режим работы вывода SDA, как входа таймера: PORT_FUNC_MAIN/PORT_FUNC_ALTER/PORT_FUNC_OVERRID
 * @param timerSDAChannel  Канал таймера для сигнала SDA: TIMER_CHANNEL1..TIMER_CHANNEL4
 * @param portSCL PORT для вывода SDA: MDR_PORTA..MDR_PORTF
 * @param pinSCL  Вывод SCL: PORT_Pin_0..PORT_Pin_15
 * @param altSCL  Режим работы вывода SCL, как входа таймера: PORT_FUNC_MAIN/PORT_FUNC_ALTER/PORT_FUNC_OVERRID
 * @param timerSCLChannel  Канал таймера для сигнала SCL: TIMER_CHANNEL1..TIMER_CHANNEL4
 */
void IICSlaveInit(IICSlave &slave, MDR_TIMER_TypeDef *timer, MDR_PORT_TypeDef *portSDA, PORT_Pin_TypeDef pinSDA,
                  PORT_FUNC_TypeDef altSDA, TIMER_Channel_Number_TypeDef timerSDAChannel, MDR_PORT_TypeDef *portSCL,
                  PORT_Pin_TypeDef pinSCL, PORT_FUNC_TypeDef altSCL, TIMER_Channel_Number_TypeDef timerSCLChannel) {

    assert_param(timerSDAChannel != timerSCLChannel);

    uint32_t bit = BYTE_TO_BITBAND(static_cast<uint32_t>(pinSDA));
    slave.hw.SDA.port = portSDA;
    slave.hw.SDA.ModeAlt = altSDA << (bit * 2);
    slave.hw.SDA.ModePort = ~(0b11 << (bit * 2));
    slave.hw.SDA.TxRx = &TO_BIT_BAND_PER(portSDA->RXTX, static_cast<uint32_t>(pinSDA));
    InitPin(portSDA, pinSDA, altSDA, PORT_OE_OUT);

    bit = BYTE_TO_BITBAND(static_cast<uint32_t>(pinSCL));
    slave.hw.SCL.port = portSCL;
    slave.hw.SCL.ModeAlt = altSCL << (bit * 2);
    slave.hw.SCL.ModePort = ~(0b11 << (bit * 2));
    slave.hw.SCL.TxRx = &TO_BIT_BAND_PER(portSCL->RXTX, static_cast<uint32_t>(pinSCL));
    InitPin(portSCL, pinSCL, altSCL, PORT_OE_OUT);

    slave.hw.timer = timer;
    slave.hw.SDARiseFlag = (TimerIEMask(timerSDAChannel) << TIMER_IE_CCR_CAP_EVENT_IE_Pos);
    slave.hw.SDAFallFlag = (TimerIEMask(timerSDAChannel) << TIMER_IE_CCR1_CAP_EVENT_IE_Pos);
    slave.hw.SCLRiseFlag = (TimerIEMask(timerSCLChannel) << TIMER_IE_CCR_CAP_EVENT_IE_Pos);
    slave.hw.SCLFallFlag = (TimerIEMask(timerSCLChannel) << TIMER_IE_CCR1_CAP_EVENT_IE_Pos);

    InitTimer(timer, timerSDAChannel, timerSCLChannel,
              slave.hw.SDARiseFlag | slave.hw.SDAFallFlag | slave.hw.SCLRiseFlag | slave.hw.SCLFallFlag);
    slave.TimerIrqNumber = TimerIrqNumber(timer);

    slave.AddressMatchCallback = nullptr;
    slave.DataReceivedCallback = nullptr;
    slave.StopCallback = nullptr;
    slave.GetTransmitByteCallback = nullptr;
}


/**
 * @brief Установка адреса Ведомого
 * @param slave
 * @param address Адрес ведомого, сдвинутый на 1 вправо (7 бит)
 */
void IICSlaveSetAddress(IICSlave &slave, uint8_t address) {
    slave.address = address;
}

/**
 * @brief Инициализация функций callback для событий ведомого I2C
 * @param slave
 * @param addressMatchCallback Callback события совпадения адреса Ведомого устройства
 * @param dataReceivedCallback Callback события получения байта данных от Ведущего
 * @param getTransmitByte Callback события запроса байта данных для передачи Ведущему
 * @param stopCallback Callback событие состояния STOP на шине I2C. Вызывается в случае совпадения адреса Ведомого
 */
void IICSlaveCallback(IICSlave &slave,
                      pAddressMatchCallback addressMatchCallback,
                      pDataReceivedCallback dataReceivedCallback,
                      pGetTransmitByte getTransmitByte,
                      pStopCallback stopCallback) {
    slave.AddressMatchCallback = addressMatchCallback;
    slave.DataReceivedCallback = dataReceivedCallback;
    slave.GetTransmitByteCallback = getTransmitByte;
    slave.StopCallback = stopCallback;
}


void IICSlaveStart(IICSlave &slave) {
    slave.State = ST_IDLE;
    NVIC_EnableIRQ(slave.TimerIrqNumber);
}

void IICSlaveStop(IICSlave &slave) {
    NVIC_EnableIRQ(slave.TimerIrqNumber);
}


/**
 * @brief Вычисление номера прерывания для таймера
 * @param timer таймер MDR_TIMER1..MDR_TIMER3
 * @return Номер прерывания или HardFault_IRQn
 */
IRQn_Type TimerIrqNumber(MDR_TIMER_TypeDef *timer) {
    if (timer == MDR_TIMER1) {
        return Timer1_IRQn;
    } else if (timer == MDR_TIMER2) {
        return Timer2_IRQn;
    } else if (timer == MDR_TIMER3) {
        return Timer3_IRQn;
    } else {
        assert_param(0);
        return HardFault_IRQn;
    }
}


/**
 * @brief Вычисление маски прерывания по настроенному фронту
 *
 * Выбор маски для разрешения прерывания по событиям CCR_CAP_EVENT и CCR_CAP1_EVENT. Раздел 21.7.13 Спецификации
 *
 * @param channel TIMER_CHANNEL1..TIMER_CHANNEL4
 * @return
 */
uint8_t TimerIEMask(TIMER_Channel_Number_TypeDef channel) {
    assert_param(IS_TIMER_CHANNEL_NUMBER(channel));
    switch (channel) {
        case TIMER_CHANNEL1:
            return 0b0001;
        case TIMER_CHANNEL2:
            return 0b0010;
        case TIMER_CHANNEL3:
            return 0b0100;
        case TIMER_CHANNEL4:
            return 0b1000;
        default:
            assert_param(0);
            return 0;
    }
}


/**
 * @brief Включение тактирования таймера
 * @param timer таймер MDR_TIMER1..MDR_TIMER3
 */
void InitTimerClock(MDR_TIMER_TypeDef *timer) {
    if (timer == MDR_TIMER1) {
        RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1, ENABLE);
    } else if (timer == MDR_TIMER2) {
        RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2, ENABLE);
    } else if (timer == MDR_TIMER3) {
        RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER3, ENABLE);
    } else {
        assert_param(0);
    }
}


/**
 * @brief Инициализация таймера на захват фронтов сигнала
 *
 * Таймер настраивается на захват переднего фронта по линии CCR и захват заднего фронта на линии CCR1.
 *
 * @param timer Периферия таймера MDR_TIMER1..MDR_TIMER3
 * @param timerSDAChannel Канал таймера для линии SDA, TIMER_CHANNEL1..TIMER_CHANNEL4
 * @param timerSCLChannel Канал таймера для линии SCL, TIMER_CHANNEL1..TIMER_CHANNEL4
 * @param IrqFlags Флаги разрешения прерывания по фронтам сигналов
 */
static void InitTimer(MDR_TIMER_TypeDef *timer,
                      TIMER_Channel_Number_TypeDef timerSDAChannel,
                      TIMER_Channel_Number_TypeDef timerSCLChannel,
                      uint32_t IrqFlags) {

    assert_param(timerSCLChannel != timerSDAChannel);
    InitTimerClock(timer);
    TIMER_DeInit(timer);
    TIMER_BRGInit(timer, TIMER_HCLKdiv1);

    timer->CNT = 0;
    timer->PSG = 0;
    timer->ARR = 0xFF;
    timer->IE = IrqFlags;

    // SDA Rise, CCR
    __IO uint32_t *channel = &timer->CH1_CNTRL + timerSDAChannel;
    *channel = (0b1 << TIMER_CH_CNTRL_CAP_NPWM_Pos) | (0b00 << TIMER_CH_CNTRL_CHSEL_Pos) | (0b0000 << TIMER_CH_CNTRL_CHFLTR_Pos);
    // SDA Fall, CCR1
    channel = &timer->CH1_CNTRL2 + timerSDAChannel;
    *channel = (0b1 << TIMER_CH_CNTRL2_CCR1_EN_Pos) | (0b01 << TIMER_CH_CNTRL2_CHSEL1_Pos);

    // SCL Rise, CCR
    channel = &timer->CH1_CNTRL + timerSCLChannel;
    *channel = (0b1 << TIMER_CH_CNTRL_CAP_NPWM_Pos) | (0b00 << TIMER_CH_CNTRL_CHSEL_Pos) | (0b0000 << TIMER_CH_CNTRL_CHFLTR_Pos);
    // SCL Fall, CCR
    channel = &timer->CH1_CNTRL2 + timerSCLChannel;
    *channel = (0b1 << TIMER_CH_CNTRL2_CCR1_EN_Pos) | (0b01 << TIMER_CH_CNTRL2_CHSEL1_Pos);

    timer->STATUS = 0; // Сбросим все флаги
    timer->CNTRL = TIMER_CNTRL_CNT_EN;  // Включаем таймер

    NVIC_SetPriority(TimerIrqNumber(timer), NVIC_EncodePriority(NVIC_GetPriorityGrouping(), IICS_TIMER_IRQ_PREEMPTIVE_PRIORITY, IICS_TIMER_IRQ_SUBPRIORITY));
}


/**
 * @brief Инициализация тактовой для PORT
 * @param port
 */
void InitPortClock(MDR_PORT_TypeDef *port) {
    if (port        == MDR_PORTA) {
        RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
    } else if (port == MDR_PORTB) {
        RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
    } else if (port == MDR_PORTC) {
        RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
    } else if (port == MDR_PORTD) {
        RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
    } else if (port == MDR_PORTE) {
        RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
    } else if (port == MDR_PORTF) {
        RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);
    } else {
        assert_param(0);
    }
}


/**
 * @brief Инициализация пина как выход максимальной скорости, открытый сток
 * @param port PORT для вывода: MDR_PORTA..MDR_PORTF
 * @param pin PORT_Pin_0..PORT_Pin_15
 * @param alt Режим работы вывода
 */
void InitPin(MDR_PORT_TypeDef *port, PORT_Pin_TypeDef pin, PORT_FUNC_TypeDef alt, PORT_OE_TypeDef oe) {
    InitPortClock(port);

PORT_InitTypeDef PORT_InitStruct;
    PORT_StructInit(&PORT_InitStruct);
    PORT_InitStruct.PORT_Pin = pin;
    PORT_InitStruct.PORT_OE = oe;
    PORT_InitStruct.PORT_FUNC = alt;
    PORT_InitStruct.PORT_SPEED = PORT_SPEED_MAXFAST;
    PORT_InitStruct.PORT_PULL_UP = PORT_PULL_UP_ON;
    PORT_InitStruct.PORT_PD = PORT_PD_OPEN;
    PORT_InitStruct.PORT_PD_SHM = PORT_PD_SHM_OFF;
    PORT_InitStruct.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_Init(port, &PORT_InitStruct);
}


#define MDR_I2C_RD_Msk    1

/**
 * @brief Опустить вывод в 0. Переключить режим на PORT и занулить
 * @param pin Вывод
 */
ramfunc_ inline void IICs_PinLow(IICSlavePin &pin) {
    pin.port->FUNC &= pin.ModePort;
    *pin.TxRx = 0;
}


/**
 * @brief Поднять вывод в 1. Записать 1 и переключить на альтернативный режим
 * @param pin Вывод
 */
ramfunc_ inline void IICs_PinHigh(IICSlavePin &pin) {
    *pin.TxRx = 1;
    pin.port->FUNC |= pin.ModeAlt;
}


/**
 * @brief Обработка состояния START на шине I2C
 * @param slave
 */
ramfunc_ static void BusSTART(IICSlave &slave) {
    if (slave.started) {
        slave.restarted = true;
    }

    slave.ClkEdgeCnt = 0;
    slave.NeedACK = false;
    slave.activeByte = 0;
    slave.started = true;
    slave.State = ST_ADDRESS;
}


/**
 * @brief Обработка состояние STOP на шине I2C
 * @param slave
 */
ramfunc_ static void BusSTOP(IICSlave &slave) {
    if (slave.started) {
        slave.State = ST_IDLE;
        slave.started = false;
        slave.restarted = false;
        slave.StopCallback();
    }
}


#define ACK_EDGE_POSITION       (16)    ///< Положение заднего фронта импульса CLK по которому выставляется сигнал ACK

ramfunc_ static void SlaveProcessClockEdge(IICSlave &slave, bool isClockRise) {
    if (slave.ClkEdgeCnt == 0) {
        if (slave.State != ST_TRANSMITTING) {
            IICs_PinHigh(slave.hw.SDA);
            slave.activeByte = 0;
        }
    }

    if (slave.ClkEdgeCnt == ACK_EDGE_POSITION) {
        // Место установки бита ACK, Fall
        if (slave.NeedACK) {
            IICs_PinLow(slave.hw.SDA);
            slave.NeedACK = false;
        } else {
            IICs_PinHigh(slave.hw.SDA);
        }
    }

    if (slave.ClkEdgeCnt < ACK_EDGE_POSITION && isClockRise) {
        // Защелкиваем бит на линии SDA по переднему фронту SCL до фронта ACK
        slave.activeByte = (slave.activeByte << 1) | (*slave.hw.SDA.TxRx);
    }

    if (slave.ClkEdgeCnt == (ACK_EDGE_POSITION - 1)) {
        // Последний бит передаваемого байта
        if (slave.State == ST_RECEIVING) { // При приёме от ведущего вызываем коллбэк на прием байта данных
            slave.NeedACK = slave.DataReceivedCallback(slave.activeByte);
        }

        // Принимаем байт адреса и выставляем ACK если адрес совпал
        if (slave.State == ST_ADDRESS) {
           bool isReadTransition = slave.activeByte & MDR_I2C_RD_Msk;
            if ((slave.activeByte >> 1) == slave.address) {
                slave.NeedACK = true;
                slave.AddressMatchCallback(isReadTransition, slave.restarted);
                if (isReadTransition) {
                    slave.State = ST_TRANSMITTING;
                } else {
                    slave.State = ST_RECEIVING;
                }
            } else {
                slave.State = ST_IDLE;
                slave.started = false;
                slave.restarted = false;
            }
        }
    }

    // Вызываем callback запроса данных для передачи Ведомому
    if (slave.State == ST_TRANSMITTING) {
        if (slave.ClkEdgeCnt == 0) {
            slave.GetTransmitByteCallback(slave.current_tx_byte);
        }
        if (slave.ClkEdgeCnt < ACK_EDGE_POSITION && !isClockRise) { // Выдаем биты на шину по заднему фронту SCL
            if (slave.current_tx_byte & 0x80) {
                IICs_PinHigh(slave.hw.SDA);
            } else {
                IICs_PinLow(slave.hw.SDA);
            }
            slave.current_tx_byte <<= 1;
        }
    }

    if (slave.ClkEdgeCnt == (ACK_EDGE_POSITION + 1)) {
        // Окончание битового потока
        slave.ClkEdgeCnt = 0;
    } else {
        slave.ClkEdgeCnt++;
    }
}

/**
 * @brief Обработчик прерывания от таймера детектирования фронтов сигналов SDA и SCL
 * @param slave
 */
ramfunc_ void TimerIIC_IRQHandler(IICSlave &slave) {
    uint32_t status = slave.hw.timer->STATUS;
    slave.hw.timer->STATUS = 0;

    if (status & slave.hw.SCLFallFlag) {
        if (slave.started) {
            SlaveProcessClockEdge(slave, false);
        }
    } else if (status & slave.hw.SCLRiseFlag) {
        if (slave.started) {
            SlaveProcessClockEdge(slave, true);
        }
    } else if (status & slave.hw.SDAFallFlag) {
        if (*slave.hw.SCL.TxRx == 1) {
            BusSTART(slave);
        }
    } else if (status & slave.hw.SDARiseFlag) {
        if (*slave.hw.SCL.TxRx == 1) {
            BusSTOP(slave);
        }
    }
}
