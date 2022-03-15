#ifndef MDR32F9QX_USB_HID_H
#define MDR32F9QX_USB_HID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "USB_Library/MDR32F9Qx_usb_device.h"


#define USB_DEVICE_VID                                          0x0483
#define USB_DEVICE_PID                                          0x5750
#define USB_LANGID_STRING                                       0x0409          ///< English USA
#define USB_DEVICE_CONFIGURATION                                (0x01)    ///< bConfigurationValue для запроса SetConfiguration

#define USB_DEVICE_DESCRIPTOR_SIZE              (0x12)
#define USB_HID_CONFIGURATION_DESCRIPTOR_SIZE   (41)
#define USB_LANGID_DESCRIPTOR_SIZE              (4)



#define  USB_DEVICE_IDX_LANGID_STR                              (0x00)    ///< Индекс String Descriptor Zero
#define  USB_DEVICE_IDX_MFC_STR                                 (0x01)    ///< Индекс String iManufacturer
#define  USB_DEVICE_IDX_PRODUCT_STR                             (0x02)    ///< Индекс String iProduct
#define  USB_DEVICE_IDX_SERIAL_STR                              (0x03)    ///< Индекс String iSerialNumber



#define USB_HID_EP_SEND             USB_EP1  ///< Адрес IN ENDPOINT жестко привязан к её адресу в процессоре
#define USB_HID_EP_RECEIVE          USB_EP2  ///< Адрес OUT ENDPOINT жестко привязан к её адресу в процессоре


#define USB_HID_DESCRIPTOR_TYPE                 (0x21)
#define USB_HID_REPORT_DESCRIPTOR               (0x22)
#define USBD_CUSTOMHID_OUTREPORT_BUF_SIZE     64
#define USBD_CUSTOM_HID_REPORT_DESC_SIZE     32
#define USB_CUSTOM_HID_DESC_SIZ              9


#define CUSTOM_HID_REQ_SET_PROTOCOL          0x0B
#define CUSTOM_HID_REQ_GET_PROTOCOL          0x03
#define CUSTOM_HID_REQ_SET_IDLE              0x0A
#define CUSTOM_HID_REQ_GET_IDLE              0x02
#define CUSTOM_HID_REQ_SET_REPORT            0x09
#define CUSTOM_HID_REQ_GET_REPORT            0x01


typedef enum {
    HID_STATE_IDLE = 0,
    HID_STATE_BUSY
} USB_HID_StateTypeDef;


USB_Result USB_HID_Init(uint8_t *ReceiveBuffer, uint32_t BufferSize);
USB_Result USB_HID_GetDescriptor(uint16_t wValue, uint16_t wIndex, uint16_t wLength);
USB_Result USB_HID_Reset();
USB_Result USB_HID_ClassRequest();
USB_Result USB_HID_SetConfiguration(uint16_t wVALUE);

USB_Result USB_HID_SendReport(uint8_t *report, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif //MDR32F9QX_USB_HID_H
