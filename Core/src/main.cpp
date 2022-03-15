#include <SEGGER_RTT.h>
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_usb_default_handlers.h"
#include <mdr_log.h>
#include "main_app.hpp"
#include <FreeRTOS.h>
#include <task.h>

static void CPU_Init();
static void Setup_USB();


static uint8_t HidBuffer[64];

int main(int argc, char* argv[]) {
(void)argc;
(void)argv;
    DWT->CYCCNT = 0;
    DWT->CTRL |= 1;
    CPU_Init();
    if (CONFIG_LOG_MAXIMUM_LEVEL > MDR_LOG_NONE) {
        SEGGER_RTT_ConfigUpBuffer(0, nullptr, nullptr, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
        mdr_log_set_vprintf([](const char *sFormat, va_list va) { return SEGGER_RTT_vprintf(0, sFormat, &va); });
    }
    MDR_LOGI("MAIN", "Init!!");
    USB_HID_Init(HidBuffer, sizeof(HidBuffer));
    Setup_USB();

    NVIC_SetPriorityGrouping(0);
    InitApp();
    vTaskStartScheduler();

    return 0;
}


/**
 * @brief Конфигурирование тактовой частоты ядра на 80 МГц от внешнего кварца через CPU_PLL
 */
void CPU_Init() {
    // Установить латентность FLASH в 3 такта, 100 МГц максимальная частота
    EEPROM_SetLatency(EEPROM_Latency_3);
    // Включаем внешний кварц HSE и ждем его стабилизации
    RST_CLK_HSEconfig(RST_CLK_HSE_ON);
    if (RST_CLK_HSEstatus() == SUCCESS) {

        // Включаем PLL от внешнего кварца HSE с умножением на 10 и ждем стабилизации PLL
        RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul10);
        RST_CLK_CPU_PLLcmd(ENABLE);
        if (RST_CLK_CPU_PLLstatus() == SUCCESS) {
            // Последовательно переключаем тактирование ядра на CPU_C3, PLL
            RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);  // Выбираем делитель для CPU_C3
            RST_CLK_CPU_PLLuse(ENABLE);                         // Выбираем тактовую для CPU_C2
            RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);     // Выбираем тактовую для ядра, равную CPU_C3 (PLL)
        }
    }
    SystemCoreClockUpdate();
}

/**
 * @brief Конфигурирование тактовой частоты USB, настройка USB, включение прерываний
 */
void Setup_USB() {
USB_Clock_TypeDef USB_Clock_InitStruct;
USB_DeviceBUSParam_TypeDef USB_DeviceBUSParam;

    RST_CLK_PCLKcmd(RST_CLK_PCLK_USB, ENABLE);

    USB_Clock_InitStruct.USB_USBC1_Source = USB_C1HSEdiv2;
    USB_Clock_InitStruct.USB_PLLUSBMUL = USB_PLLUSBMUL12;

    USB_DeviceBUSParam.MODE = USB_SC_SCFSP_Full;
    USB_DeviceBUSParam.SPEED = USB_SC_SCFSR_12Mb;
    USB_DeviceBUSParam.PULL = USB_HSCR_DP_PULLUP_Set;

    USB_DeviceInit(&USB_Clock_InitStruct, &USB_DeviceBUSParam);

    USB_SetSIM(USB_SIS_Msk);
    USB_DevicePowerOn();

#ifdef USB_INT_HANDLE_REQUIRED
    NVIC_SetPriority(USB_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(USB_IRQn);
#endif

    USB_DEVICE_HANDLE_RESET;
}

