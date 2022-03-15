#ifndef MILANDRBASE_MAIN_APP_EXTERN_H
#define MILANDRBASE_MAIN_APP_EXTERN_H

#include <stdint.h>

typedef uint8_t USBMessage[64];


#ifdef __cplusplus
extern "C" {
#endif

void USBInProcess(uint8_t *data, uint16_t len);

#ifdef __cplusplus
};
#endif

#endif //MILANDRBASE_MAIN_APP_EXTERN_H
