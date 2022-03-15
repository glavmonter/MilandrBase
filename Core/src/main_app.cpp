#include <cstring>
#include <MDR32Fx.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_timer.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "main_app.hpp"
#include "main_app_extern.h"
#include "SSPIrqTask.hpp"
#include "SSPPollTask.hpp"
#include "SSPDmaTask.hpp"
#include "SSPSlaveTask.hpp"
#include "IICSlaveTask.hpp"
#include "IICMasterTask.hpp"
#include <bitbanding.h>


#include "log_levels.h"
#include <mdr_log.h>
#include "MDR32F9Qx_usb_default_handlers.h"

static const char *TAG_MAIN = "MAIN";
static const char *TAG_BLINK = "BLINK";
static const char *TAG_PORT = "PORT";


static void InitTimerAndPort();


static QueueHandle_t usbin;
static uint8_t txBuffer[64];

void vMainApp(void *pvParameters) {
    mdr_log_level_set(TAG_MAIN, LOG_TAG_MAIN_LEVEL);
    MDR_LOGI(TAG_MAIN, "Init!!");
    MDR_LOGI(TAG_MAIN, "Delay 4 seconds while USB is enumerated");
    vTaskDelay(4000);

    float hs = -273;
    USBMessage _m;

    for (;;) {
        memset(txBuffer, 0, sizeof(txBuffer));
        if (xQueueReceive(usbin, &_m, 40) == pdTRUE) {
            MDR_LOGI(TAG_MAIN, "USB data received");
            MDR_LOG_BUFFER_HEXDUMP(TAG_MAIN, _m, sizeof(_m), MDR_LOG_VERBOSE);
            txBuffer[0] = 1;
            txBuffer[1] = _m[1];
        } else {
            txBuffer[0] = 1;
            txBuffer[1] = 0;
            memcpy(&txBuffer[2], &hs, sizeof(hs));
            hs += 0.1f;
        }
        USB_HID_SendReport(txBuffer, sizeof(txBuffer));
    }
}

void USBInProcess(uint8_t *data, uint16_t len) {
BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    assert_param(len == sizeof(USBMessage));
    xQueueSendFromISR(usbin, data, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vBlinker(void *pvParameters) {
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
PORT_InitTypeDef PORT_InitStructure;
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = PORT_Pin_1;
    PORT_InitStructure.PORT_OE = PORT_OE_OUT;
    PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
    PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);

    for (;;) {
        PORT_SetBits(MDR_PORTC, PORT_Pin_1);
        vTaskDelay(200);
        PORT_ResetBits(MDR_PORTC, PORT_Pin_1);
        vTaskDelay(200);
    }
}

QueueHandle_t irq_queue = nullptr;
void PortReceiver(void *pvParameters) {
    irq_queue = xQueueCreate(1, sizeof(uint32_t));
    InitTimerAndPort();

    for (;;) {
        uint32_t irq_status;

        if (xQueueReceive(irq_queue, &irq_status, portMAX_DELAY) == pdTRUE) {
            PORT_WriteBit(MDR_PORTE, PORT_Pin_2, RESET);
            if (irq_status & TIMER_STATUS_CCR_CAP_CH1) {
                PORT_WriteBit(MDR_PORTE, PORT_Pin_2, SET);
                PORT_WriteBit(MDR_PORTE, PORT_Pin_2, RESET);
                MDR_LOGI(TAG_PORT, "BUTTON SELECT");
            } else if (irq_status & TIMER_STATUS_CCR_CAP_CH3) {
                PORT_WriteBit(MDR_PORTE, PORT_Pin_2, SET);
                PORT_WriteBit(MDR_PORTE, PORT_Pin_2, SET);
                PORT_WriteBit(MDR_PORTE, PORT_Pin_2, SET);
                PORT_WriteBit(MDR_PORTE, PORT_Pin_2, RESET);
                MDR_LOGI(TAG_PORT, "BUTTON UP");
            }

            MDR_LOGI(TAG_PORT, "IRQ, Status: 0x%08lX", irq_status);
        }
    }
}


void InitApp() {
    usbin = xQueueCreate(1, sizeof(USBMessage));
    xTaskCreate(vMainApp, "Main", configMINIMAL_STACK_SIZE * 2, nullptr, tskIDLE_PRIORITY, nullptr);
//    xTaskCreate(vBlinker, "Blink", configMINIMAL_STACK_SIZE * 2, nullptr, tskIDLE_PRIORITY + 1, nullptr);
    xTaskCreate(PortReceiver, "IRQ", configMINIMAL_STACK_SIZE * 2, nullptr, configMAX_PRIORITIES - 1, nullptr);

//    SSPPoolTaskStart();
//    SSPIrqTaskStart();
//    SSPDmaTaskStart();
//    SSPSlaveTaskStart();
    IICSlaveTaskStart();
    IICMasterTaskStart();
}


/**
 * @brief Инициализация таймера 3 для захвата фронтов от кнопок
 *
 * PC2 - кнопка SELECT, TMR3_CH1, PullUp 10k.
 *
 * PB5 - кнопка UP, TMR3_CH3, PullUp 10k.
 */
void InitTimerAndPort() {
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER3, ENABLE);

PORT_InitTypeDef PORT_InitStructure;
    PORT_StructInit(&PORT_InitStructure);

    // PC2, Альтернативная 10.
    PORT_InitStructure.PORT_Pin = PORT_Pin_2;
    PORT_InitStructure.PORT_OE = PORT_OE_IN;
    PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER; // Поля MODE[1:0], выбор альтернативной функции
    PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);

    // PB5, Альтернатива 11
    PORT_InitStructure.PORT_Pin = PORT_Pin_5;
    PORT_InitStructure.PORT_FUNC = PORT_FUNC_OVERRID; // MODE[1:0] = 11, переопределенная
    PORT_Init(MDR_PORTB, &PORT_InitStructure);

    // TIMER3, CH1 и CH3
    TIMER_DeInit(MDR_TIMER3);
    TIMER_BRGInit(MDR_TIMER3, TIMER_HCLKdiv1); // Включаем тактовую на таймер

    MDR_TIMER3->CNT = 0;
    MDR_TIMER3->PSG = 0;
    MDR_TIMER3->ARR = 0xFF;
    MDR_TIMER3->IE = (0b0101 << TIMER_IE_CCR_CAP_EVENT_IE_Pos); // Прерывание CCR CAP EVENT для 1 и 3 канала

    // 15b канал в режиме захват
    // 0011 CHFLTR[3:0], Сигнал зафиксирован в 8 триггерах на частоте TIM_CLK
    // 01 CHSEL[5:4] Отрицательный фронт на CH
    MDR_TIMER3->CH1_CNTRL = (0b1 << TIMER_CH_CNTRL_CAP_NPWM_Pos) | (0b01 << TIMER_CH_CNTRL_CHSEL_Pos) | (0b0000 << TIMER_CH_CNTRL_CHFLTR_Pos);
    MDR_TIMER3->CH3_CNTRL = (0b1 << TIMER_CH_CNTRL_CAP_NPWM_Pos) | (0b01 << TIMER_CH_CNTRL_CHSEL_Pos) | (0b0000 << TIMER_CH_CNTRL_CHFLTR_Pos);

    MDR_TIMER3->CNTRL = 0x1;
    NVIC_SetPriority(Timer3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 6, 0));
    NVIC_EnableIRQ(Timer3_IRQn);
}


extern "C" void Timer3_IRQHandler() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t ls = MDR_TIMER3->STATUS;
    xQueueSendFromISR(irq_queue, &ls, &xHigherPriorityTaskWoken);
    MDR_TIMER3->STATUS = 0;
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
