#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_timer.h>
#include <FreeRTOS.h>
#include <task.h>
#include <iicslave.h>
#include "IICSlaveTask.hpp"

#include "log_levels.h"
#define LOG_LOCAL_LEVEL LOG_TAG_IIC_LOCAL_LEVEL
#include <mdr_log.h>
const static char *TAG = "IICS";

// NOTE Программная реализация I2C https://startmilandr.ru/doku.php/prog:i2c:timersorfi2c
static IICSlave xIICSlave;



// Тестовые данные
uint8_t reg_address;
uint8_t rx_buff_index;
uint8_t rx_buff[8];

uint8_t tx_buffer_index;
uint8_t tx_buffer[8] = {0xA0, 0xA1, 0xBC, 0xCC,
                        0xDE, 0x12, 0x68, 0x57};
bool AddressMatch(bool read_transition, bool restarted) {
    rx_buff_index = 0;
    tx_buffer_index = 0;
    return true;
}

bool DataReceived(uint8_t data) {
    if (rx_buff_index == 0) {
        reg_address = data;
    }
    rx_buff[rx_buff_index] = data;
    rx_buff_index++;
    return true;
}

bool GetTransmittedByte(uint8_t &data) {
    data = tx_buffer[tx_buffer_index];
    tx_buffer_index++;
    return true;
}

bool StopCallback() {
    MDR_LOGD(TAG, "STOP Register: %d", reg_address);
    MDR_LOG_BUFFER_HEXDUMP(TAG, rx_buff, rx_buff_index, MDR_LOG_DEBUG);
    return true;
}

void Execute(void *pvParameters) {
    MDR_LOGI(TAG, "Start!");
    IICSlaveInit(xIICSlave,
                 MDR_TIMER1,
                 MDR_PORTA, PORT_Pin_1, PORT_FUNC_ALTER, TIMER_CHANNEL1,  // SDA, PA1 ALTER
                 MDR_PORTA, PORT_Pin_3, PORT_FUNC_ALTER, TIMER_CHANNEL2   // SCL, PA3 ALTER
                 );

    IICSlaveSetAddress(xIICSlave, 0x37);
    IICSlaveCallback(xIICSlave, AddressMatch, DataReceived, GetTransmittedByte, StopCallback);
    IICSlaveStart(xIICSlave);
    for (;;) {
        vTaskDelay(1);
    }
}

extern "C" __attribute__ ((section(".ramfunc"))) void Timer1_IRQHandler()  {
    TimerIIC_IRQHandler(xIICSlave);
}


void IICSlaveTaskStart() {
    xTaskCreate(Execute, "IICSlave", configMINIMAL_STACK_SIZE * 2, nullptr, tskIDLE_PRIORITY, nullptr);
}
