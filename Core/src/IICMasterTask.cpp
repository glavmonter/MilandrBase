#include <cstring>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_i2c.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "IICMasterTask.hpp"
#include "ring_buffer.h"


#include "log_levels.h"
#define LOG_LOCAL_LEVEL LOG_TAG_IIC_MASTER_LOCAL_LEVEL
#include <mdr_log.h>
const static char *TAG = "IICM";

#define ADDRESS (0xA0)

// TODO Необходимо поставить таймауты на ожидание флагов I2C

static void WriteTestPoll();
static void ReadTestPoll();
static void WriteTestInterrupt(uint32_t address, const void *buffer, size_t buffer_len);
static void ReadTestInterrupt(uint32_t address, void *buffer, size_t buffer_len);

__IO static uint32_t iReceiveLen = 0;
RingBuffer<64, uint8_t> ringBuffer;

SemaphoreHandle_t IrqSemaphore;

static void InitHW();

static void Execute(void *pvParameters) {
static uint8_t rx_buffer[32] {};

    MDR_LOGI(TAG, "Start!");
    InitHW();
    NVIC_SetPriority(I2C_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 6, 0));
    I2C_Cmd(ENABLE);

    uint32_t address = 0;
    int index = 0;
    uint8_t tx_buffer[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xA0, 0xA5, 0x01, 0x02,
                           0x03, 0x04, 0x05, 0x54, 0x49, 0x23, 0x69, 0x11};
    for (;;) {
        WriteTestInterrupt(address, tx_buffer, sizeof(tx_buffer));
        index += 1;
        tx_buffer[0] += index;
        vTaskDelay(12);
        ::memset(rx_buffer, 0, sizeof(rx_buffer));

        ReadTestInterrupt(address, rx_buffer, sizeof(tx_buffer));
        MDR_LOGI(TAG, "Receive Successfully");
        MDR_LOG_BUFFER_HEXDUMP(TAG, rx_buffer, sizeof(rx_buffer), MDR_LOG_INFO);
        vTaskDelay(20);
        address += 32;
    }
}


void ReadTestInterrupt(uint32_t address, void *buffer, size_t buffer_len) {

    // Ожидаем шину
    while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET) {}

    uint8_t slave_address = ADDRESS + (((address & 0x30000) >> 16) << 1);
    I2C_Send7bitAddress(slave_address, I2C_Direction_Transmitter);
    while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}
    if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) {
        // Отправляем адрес ячейки
        I2C_SendByte((address & 0xFF00) >> 8); while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}
        I2C_SendByte(address & 0xFF); while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}

        I2C_Send7bitAddress(slave_address, I2C_Direction_Receiver);
        while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}
        if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) {
            I2C_GetReceivedData(); // Пустое чтение иначе та
            I2C_ClearITPendingBit();

            iReceiveLen = buffer_len;
            if (iReceiveLen == 1) {
                I2C_StartReceiveData(I2C_Send_to_Slave_NACK);
            } else {
                I2C_StartReceiveData(I2C_Send_to_Slave_ACK);
            }
            I2C_ITConfig(ENABLE);
            NVIC_EnableIRQ(I2C_IRQn);
            if (xSemaphoreTake(IrqSemaphore, 5000) != pdTRUE) {
                MDR_LOGE(TAG, "I2C Read Error!!");
            }
        }
    }
    I2C_SendSTOP();
    NVIC_DisableIRQ(I2C_IRQn);
    I2C_ITConfig(DISABLE);
    I2C_ClearITPendingBit();

    MDR_LOGI(TAG, "Rcv Ring buffer Size: %d", ringBuffer.Count());
    auto v_buf = static_cast<uint8_t *>(buffer);
    uint8_t v; int v_index = 0;
    while (ringBuffer.Read(v)) {
        v_buf[v_index] = v;
        v_index++;
    }
}


void ReadTestPoll() {
uint8_t rx_data[16] {};

    // Ждем освобождения шины
    while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET) {}

    // Шлем адрес ведомого на запись адреса ячейки
    I2C_Send7bitAddress(ADDRESS, I2C_Direction_Transmitter);
    while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}
    if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) {
        // Два байта адреса адреса ячейки
        I2C_SendByte(5); while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}
        I2C_SendByte(6); while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}

        // Шлем новый START на чтение
        I2C_Send7bitAddress(ADDRESS, I2C_Direction_Receiver);
        while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}
        if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) {
            for (int i = 0; i < sizeof(rx_data)/sizeof(rx_data[0]); i++) {
                if (i == sizeof(rx_data)/sizeof(rx_data[0]) - 1)
                    I2C_StartReceiveData(I2C_Send_to_Slave_NACK);
                else
                    I2C_StartReceiveData(I2C_Send_to_Slave_ACK);
                while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}
                rx_data[i] = I2C_GetReceivedData();
            }
        }
    }
    I2C_SendSTOP();
    MDR_LOGD(TAG, "Received data:");
    MDR_LOG_BUFFER_HEXDUMP(TAG, rx_data, sizeof(rx_data), MDR_LOG_INFO);
}

void WriteTestInterrupt(uint32_t address, const void *buffer, size_t buffer_len) {
    // Ожидаем шину
    while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET) {}

    uint8_t slave_address = ADDRESS + (((address & 0x30000) >> 16) << 1);
    I2C_Send7bitAddress(slave_address, I2C_Direction_Transmitter);
    while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}
    if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) {
        // Отправляем адрес ячейки
        I2C_SendByte((address & 0xFF00) >> 8); while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}
        I2C_SendByte(address & 0xFF); while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}

        auto const_buf = static_cast<const uint8_t *>(buffer);
        for (int i = 1; i < buffer_len; i++) {
            ringBuffer.Write(const_buf[i]);
        }

        // Включаем прерывание и ждем
        I2C_ClearITPendingBit();
        I2C_ITConfig(ENABLE);
        NVIC_EnableIRQ(I2C_IRQn);
        I2C_SendByte(const_buf[0]);

        if (xSemaphoreTake(IrqSemaphore, 5000) != pdTRUE) {
            MDR_LOGE(TAG, "I2C Transmit Error!");
        } else {
            MDR_LOGI(TAG, "I2C Write OK");
        }
    }
    I2C_SendSTOP();
    NVIC_DisableIRQ(I2C_IRQn);
    I2C_ITConfig(DISABLE);
    I2C_ClearITPendingBit();
}


void WriteTestPoll() {
    // Ждем освобождения шины
    while (I2C_GetFlagStatus(I2C_FLAG_BUS_FREE) != SET) {}

    // Шлем адрес, ожидаем окончания передачи и читаем ACK
    I2C_Send7bitAddress(ADDRESS, I2C_Direction_Transmitter);
    while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}

    if (I2C_GetFlagStatus(I2C_FLAG_SLAVE_ACK) == SET) {
        // Шлем данные
        for (int i = 0; i < 10; i++) {
            I2C_SendByte(i + 5);
            while (I2C_GetFlagStatus(I2C_FLAG_nTRANS) != SET) {}
        }
    }
    I2C_SendSTOP();
}


void InitHW() {
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC | RST_CLK_PCLK_I2C, ENABLE);

PORT_InitTypeDef PORT_InitStructure;
    PORT_StructInit(&PORT_InitStructure);
    // PC0 - SCL, PC1 - SDA. Все как Альтернативная
    PORT_InitStructure.PORT_Pin = PORT_Pin_0 | PORT_Pin_1;
    PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
    PORT_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PORT_InitStructure.PORT_PD = PORT_PD_OPEN;
    PORT_InitStructure.PORT_PD_SHM = PORT_PD_SHM_OFF;
    PORT_InitStructure.PORT_GFEN = PORT_GFEN_OFF;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
    PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
    PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);

I2C_InitTypeDef I2C_InitStructure;
    I2C_InitStructure.I2C_ClkDiv = 155; // 155 - примерно 100 кГц. 36 - примерно 400 кГц,
    I2C_InitStructure.I2C_Speed = I2C_SPEED_UP_TO_400KHz;

    I2C_DeInit();
    I2C_Init(&I2C_InitStructure);
}

extern "C" void I2C_IRQHandler() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (I2C_GetITStatus() == SET) {
        I2C_ClearITPendingBit();

        if (I2C_GetFlagStatus(I2C_FLAG_BUS_BUSY) == SET) {
            if (iReceiveLen) {
                ringBuffer.Write(I2C_GetReceivedData());
                iReceiveLen--;
                if (iReceiveLen == 0) {
                    xSemaphoreGiveFromISR(IrqSemaphore, &xHigherPriorityTaskWoken);
                } else if (iReceiveLen == 1) {
                    I2C_StartReceiveData(I2C_Send_to_Slave_NACK);
                } else {
                    I2C_StartReceiveData(I2C_Send_to_Slave_ACK);
                }
                return;
            }

            if (ringBuffer.Count() > 0) {
                uint8_t v;
                ringBuffer.Read(v);
                I2C_SendByte(v);
            } else {
                xSemaphoreGiveFromISR(IrqSemaphore, &xHigherPriorityTaskWoken);
            }
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void IICMasterTaskStart() {
    IrqSemaphore = xSemaphoreCreateBinary();
    xSemaphoreTake(IrqSemaphore, 0);
    xTaskCreate(Execute, "IICMaster", configMINIMAL_STACK_SIZE * 2, nullptr, tskIDLE_PRIORITY, nullptr);
}
