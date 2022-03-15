/**
 * @file iicslave.h
 * @brief Программная реализация I2C Ведомого устройства
 */

#ifndef MILANDRBASE_IICSLAVE_H
#define MILANDRBASE_IICSLAVE_H

#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_timer.h>


struct IICSlave; // Forward declaration


//                                    read_flag, re_start
typedef bool (*pAddressMatchCallback)(bool,        bool); ///< Аргумент 1: признак START. Аргумент 2: признак ReSTART. @retval: не используется
typedef bool (*pDataReceivedCallback)(uint8_t);           ///< Аргумент 1: принятый байт от Ведущего. @retval: true ACK, false NACK
typedef bool (*pGetTransmitByte)(uint8_t &);              ///< Аргумент 1: ссылка на передаваемый байт. @retval: не используется
typedef bool (*pStopCallback)();                          ///< @retval не используется

typedef struct {
    __IO uint32_t *TxRx;        ///< BitBanding адрес состояния вывода
    MDR_PORT_TypeDef *port;     ///< Адрес порта MDR_PORTA..MDR_PORTF
    uint32_t ModeAlt;           ///< Битовая маска для переключения вывода в Альтернативный логическим ИЛИ
    uint32_t ModePort;          ///< Битовая маска для переключения вывода в PORT логическим И
} IICSlavePin;

typedef struct {
    IICSlavePin         SDA;            ///< Вывод SDA, на лету переключается в режим PORT для установки 0 на линию
    IICSlavePin         SCL;            ///< Вывод SCL, только вход таймера
    MDR_TIMER_TypeDef  *timer;          ///< Таймер для детектирования фронтов SCL/SDA
    uint32_t            SDARiseFlag;    ///< Маска для фронта SDA
    uint32_t            SDAFallFlag;    ///< Маска для среза SDA
    uint32_t            SCLRiseFlag;    ///< Маска для фронта SCL
    uint32_t            SCLFallFlag;    ///< Маска для среза SCL
} Hardware;


enum IICsState {
    ST_IDLE,                            ///< Ожидание START состояния
    ST_ADDRESS,                         ///< Получение Адреса и режима R/W
    ST_RECEIVING,                       ///< Режим приёма данных от мастера
    ST_TRANSMITTING                     ///< Режим передачи данных к мастеру
};


/**
 * @brief Описание Ведомого устройства I2C
 */
struct IICSlave {
    Hardware                hw;             ///< Выводы, таймеры и флаги состояния
    uint8_t                 ClkEdgeCnt;     ///< Счетчик фронтов SCL. На этом счетчике строится конечный автомат
    IICsState               State;          ///< Состояние конечного автомата
    bool                    NeedACK;        ///< Нужно ли ставить ACK на шину? Выставляется при ClkEdgeCnt==16fall, считывается при ClkEdgeCnt==17rise
    uint8_t                 address;        ///< Адрес ведомого I2C, сдвинут на 1 вправо!!
    bool                    started;        ///< Флаг получения START состояния
    bool                    restarted;      ///< Флаг получения повторного START
    uint8_t                 current_tx_byte;///< Передаваемый в настоящий момент байт данных в состояние ST_TRANSMITTING
    uint8_t                 activeByte;     ///< Принимаемый в настоящий момент байт данных/адреса в режимах ST_ADDRESS и ST_RECEIVING
    IRQn_Type               TimerIrqNumber; ///< Номер прерывания таймера

    // CallBacks
    pAddressMatchCallback   AddressMatchCallback;    ///< Callback при совпадение адреса Ведомого
    pDataReceivedCallback   DataReceivedCallback;    ///< Callback при получение байта данных от Ведущего
    pStopCallback           StopCallback;            ///< Callback при получение STOP события, если до этого было совпадение адреса
    pGetTransmitByte        GetTransmitByteCallback; ///< Callback для запроса байта данных для передачи Ведомому
};


#define IICS_TIMER_IRQ_PREEMPTIVE_PRIORITY   (4)     ///< Вытесняющий приоритет прерывания таймера
#define IICS_TIMER_IRQ_SUBPRIORITY           (0)     ///< Подприоритет прерывания таймера

void
IICSlaveInit(IICSlave &slave, MDR_TIMER_TypeDef *timer, MDR_PORT_TypeDef *portSDA, PORT_Pin_TypeDef pinSDA,
             PORT_FUNC_TypeDef altSDA, TIMER_Channel_Number_TypeDef timerSDAChannel, MDR_PORT_TypeDef *portSCL,
             PORT_Pin_TypeDef pinSCL, PORT_FUNC_TypeDef altSCL, TIMER_Channel_Number_TypeDef timerSCLChannel);

void IICSlaveSetAddress(IICSlave &slave, uint8_t address);
void IICSlaveCallback(IICSlave &slave,
                      pAddressMatchCallback addressMatchCallback,
                      pDataReceivedCallback dataReceivedCallback,
                      pGetTransmitByte getTransmitByte,
                      pStopCallback stopCallback);

void IICSlaveStart(IICSlave &slave);
void IICSlaveStop(IICSlave &slave);

void TimerIIC_IRQHandler(IICSlave &slave);

#endif //MILANDRBASE_IICSLAVE_H
