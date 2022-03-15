#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_ssp.h>
#include <MDR32F9Qx_dma.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "SSPDmaTask.hpp"

#include "log_levels.h"
#define LOG_LOCAL_LEVEL LOG_TAG_SSP_LOCAL_LEVEL
#include <mdr_log.h>
const static char *TAG = " SSP";

#define SSP_MASTER_HW      MDR_SSP2

// INFO Время передачи DMA 20.95 мкс
static uint16_t TxData[] = {
        0x0000, 0x1234, 0x5974, 0xfA5B,
        0x24CD, 0x4444, 0xAA55, 0xAAAA,
        0xFFFF, 0x5555, 0xDEAD, 0xBEEF};



DMA_ChannelInitTypeDef DMA_ChannelInitStructure;
DMA_CtrlDataInitTypeDef DMA_CtrlDataInitStructure;

SemaphoreHandle_t DmaSemaphore;

static void InitHW();

static void Execute(void *pvParameters) {
    MDR_LOGI(TAG, "Start!");
    vTaskDelay(100);

    InitHW();
    NVIC_SetPriority(DMA_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 7, 0));
    NVIC_EnableIRQ(DMA_IRQn);
    SSP_Cmd(SSP_MASTER_HW, ENABLE);

    uint16_t index = 0;
    for (;;) {
        TxData[0] = index++;
        DMA_Init(DMA_Channel_SSP2_TX, &DMA_ChannelInitStructure);
        SSP_DMACmd(SSP_MASTER_HW, SSP_DMA_TXE, ENABLE);

        // Ожидаем окончания передачи
        // while (SSP_GetFlagStatus(SSP_MASTER_HW, SSP_FLAG_BSY) == SET){}
        // Или ждем прерывание
        if (xSemaphoreTake(DmaSemaphore, portMAX_DELAY) == pdTRUE) {
            SSP_DMACmd(SSP_MASTER_HW, SSP_DMA_TXE, DISABLE);
            MDR_LOGD(TAG, "DMA Transfer complete: %d", index - 1);
        }
        vTaskDelay(100);
    }
}

void InitHW() {
    PORT_InitTypeDef PORT_InitStructure;
    RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | RST_CLK_PCLK_DMA | RST_CLK_PCLK_SSP2, ENABLE);
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

    // Если включить периферию на несколько выводов, то сигнал будет на обоих. В этом варианте CLK на выводах PD5 и PC1
    // PC1 - CLK OVERRID
    PORT_InitStructure.PORT_Pin = PORT_Pin_1;
    PORT_InitStructure.PORT_FUNC = PORT_FUNC_OVERRID;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);

    // PD2 - RXD вход
    PORT_InitStructure.PORT_Pin = PORT_Pin_2;
    PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
    PORT_InitStructure.PORT_OE = PORT_OE_IN;
    PORT_Init(MDR_PORTD, &PORT_InitStructure);

    SSP_InitTypeDef SSP_InitStructure;
    SSP_StructInit(&SSP_InitStructure);
    SSP_DeInit(SSP_MASTER_HW);
    SSP_BRGInit(SSP_MASTER_HW, SSP_HCLKdiv1);
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
    SSP_Init(SSP_MASTER_HW, &SSP_InitStructure);

    // INFO Если DMA_Channels_Number < 9 и DMA_AlternateData == (0/1), то SSP2_TX, канал 6, не работает.

    // Инициализация DMA
    DMA_DeInit();
    DMA_StructInit(&DMA_ChannelInitStructure);
    // Primary Control
    DMA_CtrlDataInitStructure.DMA_SourceBaseAddr = reinterpret_cast<uint32_t>(TxData);
    DMA_CtrlDataInitStructure.DMA_DestBaseAddr = reinterpret_cast<uint32_t>(&SSP_MASTER_HW->DR);
    DMA_CtrlDataInitStructure.DMA_SourceIncSize = DMA_SourceIncHalfword;
    DMA_CtrlDataInitStructure.DMA_DestIncSize = DMA_DestIncNo;
    DMA_CtrlDataInitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_CtrlDataInitStructure.DMA_Mode = DMA_Mode_Basic;
    DMA_CtrlDataInitStructure.DMA_CycleSize = sizeof(TxData)/ sizeof(TxData[0]);
    DMA_CtrlDataInitStructure.DMA_NumContinuous = DMA_Transfers_1;
    DMA_CtrlDataInitStructure.DMA_SourceProtCtrl = DMA_SourcePrivileged;
    DMA_CtrlDataInitStructure.DMA_DestProtCtrl = DMA_DestPrivileged;

    // Set Channel structure
    DMA_ChannelInitStructure.DMA_PriCtrlData = &DMA_CtrlDataInitStructure;
    DMA_ChannelInitStructure.DMA_Priority = DMA_Priority_High;
    DMA_ChannelInitStructure.DMA_UseBurst = DMA_BurstClear;
    DMA_ChannelInitStructure.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;
}

extern "C" void DMA_IRQHandler() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (DMA_GetCurrTransferCounter(DMA_Channel_SSP2_TX, DMA_CTRL_DATA_PRIMARY) == 1) {
        // Если счетчик передач 0, то закончили цикл. Функция DMA_GetCurrTransferCounter +1 возвращает.
        // Если не отключить источник прерывания, то SSP будет сыпать запросами и, соответственно, будут прерывания.
        SSP_DMACmd(SSP_MASTER_HW, SSP_DMA_TXE, DISABLE);
        xSemaphoreGiveFromISR(DmaSemaphore, &xHigherPriorityTaskWoken);
    }
    NVIC_ClearPendingIRQ(DMA_IRQn);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void SSPDmaTaskStart() {
    DmaSemaphore = xSemaphoreCreateBinary();
    xSemaphoreTake(DmaSemaphore, 0);
    xTaskCreate(Execute, "SSPDma", configMINIMAL_STACK_SIZE * 2, nullptr, configMAX_PRIORITIES - 1, nullptr);
}
