#ifndef MDR32F9QX_USB_DEF_H
#define MDR32F9QX_USB_DEF_H

#ifdef __cplusplus
extern "C" {
#endif


#define LOBYTE(x)  ((uint8_t)(x & 0x00FF))
#define HIBYTE(x)  ((uint8_t)((x & 0xFF00) >>8))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))


#ifdef __cplusplus
}
#endif

#endif //MDR32F9QX_USB_DEF_H
