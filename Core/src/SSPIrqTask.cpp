#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_ssp.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "SSPIrqTask.hpp"

#include "log_levels.h"
#define LOG_LOCAL_LEVEL LOG_TAG_SSP_LOCAL_LEVEL
#include <mdr_log.h>
const static char *TAG = " SSP";


struct SSPIrqTransmitter {
    uint32_t current_index;
    uint32_t total_worlds;
    uint16_t *buffer;
};

static void InitHWIrq();

static SSPIrqTransmitter Transmitter;
SemaphoreHandle_t SSPSemaphore;

// INFO Время передачи IRQ 31,68 мкс
static uint16_t TxData[] = {
        0x0000, 0x1234, 0x5974, 0xfA5B,
        0x24CD, 0x4444, 0xAA55, 0xAAAA,
        0xFFFF, 0x5555, 0xDEAD, 0xBEEF};



static void Execute(void *pvParameters) {
    MDR_LOGI(TAG, "Start!");
    InitHWIrq();
    NVIC_SetPriority(SSP2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 7, 0));
    NVIC_EnableIRQ(SSP2_IRQn);

    uint16_t index = 0;
    for (;;) {
        vTaskDelay(20);
        TxData[0] = index++;

        Transmitter.current_index = 0;
        Transmitter.total_worlds = sizeof(TxData) / sizeof(TxData[0]);
        Transmitter.buffer = TxData;
        SSP_Cmd(MDR_SSP2, ENABLE);  // TODO один раз включить или в прерывание отключать
        SSP_ITConfig(MDR_SSP2, SSP_IMSC_TXIM, ENABLE);
        if (xSemaphoreTake(SSPSemaphore, portMAX_DELAY) == pdTRUE) {
            MDR_LOGI(TAG, "Semaphore Taken: %04X", index - 1);
        }
    }
}

void InitHWIrq() {
PORT_InitTypeDef PORT_InitStructure;
    RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_PORTD | RST_CLK_PCLK_PORTC | RST_CLK_PCLK_SSP2, ENABLE);
    PORT_StructInit(&PORT_InitStructure);

    /*
     * Вывод FSS PD3 определить как ALTER, то SSP им управляет
     * Вывод FSS PD3 определить как PORT, то SSP им уже не управляет
     * Вывод FSS PC0 определяем как OVERRID (Переопределенная), то SSP им управляет
     */
    // PD3, PD5, PD6 - FSS, CLK, TXD выходы
    PORT_InitStructure.PORT_Pin = PORT_Pin_3 | PORT_Pin_5 | PORT_Pin_6;
    PORT_InitStructure.PORT_OE = PORT_OE_OUT;
    PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
    PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTD, &PORT_InitStructure);

    // PD2 - RXD вход
    PORT_InitStructure.PORT_Pin = PORT_Pin_2;
    PORT_InitStructure.PORT_OE = PORT_OE_IN;
    PORT_Init(MDR_PORTD, &PORT_InitStructure);

SSP_InitTypeDef SSP_InitStructure;
    SSP_StructInit(&SSP_InitStructure);
    SSP_DeInit(MDR_SSP2);
    SSP_BRGInit(MDR_SSP2, SSP_HCLKdiv1);
    /*
     * Тактовая частота SSP_CLKOUT = PCLK/(CPSDVSR * (SCR + 1))
     */
    SSP_InitStructure.SSP_SCR = 3;
    SSP_InitStructure.SSP_CPSDVSR = 2; // Только четные делители
    SSP_InitStructure.SSP_Mode = SSP_ModeMaster;
    SSP_InitStructure.SSP_WordLength = SSP_WordLength16b;
    SSP_InitStructure.SSP_SPH = SSP_SPH_1Edge;
    SSP_InitStructure.SSP_SPO = SSP_SPO_Low;
    SSP_InitStructure.SSP_FRF = SSP_FRF_SPI_Motorola;
    SSP_InitStructure.SSP_HardwareFlowControl = SSP_HardwareFlowControl_None; // Отключен SSP
    SSP_Init(MDR_SSP2, &SSP_InitStructure);
}


extern "C" void SSP2_IRQHandler() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (MDR_SSP2->MIS & SSP_MIS_TXMIS) {
        while ((SSP_GetITStatus(MDR_SSP2, SSP_IT_TX) == SET) && (Transmitter.current_index < Transmitter.total_worlds)) {
            SSP_SendData(MDR_SSP2, Transmitter.buffer[Transmitter.current_index]);
            Transmitter.current_index++;
        }
        if (Transmitter.current_index == Transmitter.total_worlds) {
            SSP_Cmd(MDR_SSP2, DISABLE);
            SSP_ITConfig(MDR_SSP2, SSP_IMSC_TXIM, DISABLE);
            xSemaphoreGiveFromISR(SSPSemaphore, &xHigherPriorityTaskWoken);
        }
        MDR_SSP2->ICR = 0b11; // Очистить все прерывания
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void SSPIrqTaskStart() {
    SSPSemaphore = xSemaphoreCreateBinary();
    xSemaphoreTake(SSPSemaphore, 0);
    xTaskCreate(Execute, "SSPIrq", configMINIMAL_STACK_SIZE * 2, nullptr, configMAX_PRIORITIES - 1, nullptr);
}
