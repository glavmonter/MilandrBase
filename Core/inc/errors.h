/**
  * \file    errors.h
  * \version V1.0.0
  * \date    21 августа 2020
  * \brief   Функия assert и ошибки
  */


#ifndef ERRORS_H_
#define ERRORS_H_

#include <stdint.h>
#include <MDR32F9Qx_config.h>

#ifdef __cplusplus
extern "C" {
#endif

void Error_Handler(void);

#if USE_ASSERT_INFO == 1
void assert_failed(uint8_t *file, uint32_t line);
#elif USE_ASSERT_INFO == 2
void assert_failed(uint8_t* file, uint32_t line, const uint8_t* expr);
#endif

#ifdef __cplusplus
}
#endif


#endif /* ERRORS_H_ */
