#include "MDR32Fx.h"

extern void __attribute__((noreturn)) NVIC_SystemReset (void);

// This is the default hardware reset routine; it can be
// redefined in the application for more complex applications.
//
// Called from _exit().

void __attribute__((weak,noreturn)) __reset_hardware ()
{
	NVIC_SystemReset();
}
