#include "exception_handlers.h"

void Default_Handler (void) __attribute__((weak));

/* 1986VE92 Specific Interrupts */
void CAN1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void USB_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void SSP1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void POWER_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void WWDG_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void Timer1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void Timer2_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void Timer3_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void COMPARATOR_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void SSP2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void BACKUP_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void EXT_INT1_IRQHandler    (void) __attribute__ ((weak, alias("Default_Handler")));
void EXT_INT2_IRQHandler    (void) __attribute__ ((weak, alias("Default_Handler")));
void EXT_INT3_IRQHandler    (void) __attribute__ ((weak, alias("Default_Handler")));
void EXT_INT4_IRQHandler    (void) __attribute__ ((weak, alias("Default_Handler")));

extern unsigned int __stack;

typedef void (*const pHandler)(void);

// The vector table.
// This relies on the linker script to place at correct location in memory.

pHandler __isr_vectors[] __attribute__ ((section(".isr_vector"),used)) =  {
        (pHandler) &__stack,                      // The initial stack pointer
        Reset_Handler,                            // The reset handler

        NMI_Handler,                              // The NMI handler
        HardFault_Handler,                        // The hard fault handler

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
        MemManage_Handler,                        // The MPU fault handler
        BusFault_Handler,// The bus fault handler
        UsageFault_Handler,// The usage fault handler
#else
        0, 0, 0,				  // Reserved
#endif
        0,                                        // Reserved
        0,                                        // Reserved
        0,                                        // Reserved
        0,                                        // Reserved
        SVC_Handler,                              // SVCall handler
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
        DebugMon_Handler,                         // Debug monitor handler
#else
        0,					  // Reserved
#endif
        0,                                        // Reserved
        PendSV_Handler,                           // The PendSV handler
        SysTick_Handler,                          // The SysTick handler

		/* External interrupts */
        CAN1_IRQHandler,                           /*  IRQ0:  CAN1                      */
        CAN2_IRQHandler,                           /*  IRQ1:  CAN2                      */
        USB_IRQHandler,                            /*  IRQ2:  USB                       */
		0,                                         /*  IRQ3:  Reserved                  */
		0,                                         /*  IRQ4:  Reserved                  */
        DMA_IRQHandler,                            /*  IRQ5:  DMA                       */
        UART1_IRQHandler,                          /*  IRQ6:  UART1                     */
        UART2_IRQHandler,                          /*  IRQ7:  UART2                     */
        SSP1_IRQHandler,                           /*  IRQ8:  SSP1                      */
		0,                                         /*  IRQ9:  Reserved                  */
        I2C_IRQHandler,                            /* IRQ10:  I2C                       */
        POWER_IRQHandler,                          /* IRQ11:  POWER detector            */
        WWDG_IRQHandler,                           /* IRQ12:  WWDG                      */
		0,                                         /* IRQ13:  Reserved                  */
        Timer1_IRQHandler,                         /* IRQ14:  TIMER1                    */
        Timer2_IRQHandler,                         /* IRQ15:  TIMER2                    */
        Timer3_IRQHandler,                         /* IRQ16:  TIMER3                    */
        ADC_IRQHandler,                            /* IRQ17:  ADC                       */
		0,                                         /* IRQ18:  Reserved                  */
        COMPARATOR_IRQHandler,                     /* IRQ19:  Comparator                */
        SSP2_IRQHandler,                           /* IRQ20:  SSP1 (SPI)                */
		0,                                         /* IRQ21:  Reserved                  */
		0,                                         /* IRQ22:  Reserved                  */
		0,                                         /* IRQ23:  Reserved                  */
		0,                                         /* IRQ24:  Reserved                  */
		0,                                         /* IRQ25:  Reserved                  */
		0,                                         /* IRQ26:  Reserved                  */
        BACKUP_IRQHandler,                         /* IRQ27:  BKP and RTC               */
        EXT_INT1_IRQHandler,                       /* IRQ28:  External Interrupt 1      */
        EXT_INT2_IRQHandler,                       /* IRQ29:  External Interrupt 2      */
        EXT_INT3_IRQHandler,                       /* IRQ30:  External Interrupt 3      */
        EXT_INT4_IRQHandler                        /* IRQ31:  External Interrupt 4      */
};

// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
__attribute__ ((section(".after_vectors")))
void Default_Handler (void)
{
	while (1) ;
}
