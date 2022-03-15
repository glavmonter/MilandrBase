#include <string.h>
#include <stddef.h>
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_usb_handlers.h"
#include "MDR32F9Qx_usb_HID.h"
#include "MDR32F9Qx_usb_def.h"
#include "main_app_extern.h"

#include "log_levels.h"
#define LOG_LOCAL_LEVEL LOG_TAG_USB_LOCAL_LEVEL
#include <mdr_log.h>

const static char *TAG = " USB";


typedef struct {
    uint8_t *HID_ReceiveBuffer;
    uint32_t BufferSize;
    uint32_t Protocol;
    uint32_t IdleState;

    USB_HID_StateTypeDef HidState;
} USB_HIDContext_Typedef;


static USB_HIDContext_Typedef USB_HIDContext;


/**
 * @brief Описание стандартного дескриптора USB. USB 2.0 Table 9-8
 */
static uint8_t USB_DeviceDescriptor[USB_DEVICE_DESCRIPTOR_SIZE] = {
    USB_DEVICE_DESCRIPTOR_SIZE,         // bLength, 18 (0x12)
    USB_DEVICE,                         // bDescriptorType, 0x01 - Device
    0x10, 0x01,                         // bcdUSB, USB version
    0x00,                               // bDeviceClass, 0x00 - Класс задаётся для интерфейса отдельно
    0x00,                               // bDeviceSubClass, 0x00 так как bDeviceClass тоже 0
    0x00,                               // bDeviceProtocol, 0x00 не уточняем протокол
    MAX_PACKET_SIZE,                    // bMaxPacketSize0
    LOBYTE(USB_DEVICE_VID),             // idVendor low byte
    HIBYTE(USB_DEVICE_VID),             // idVendor high byte
    LOBYTE(USB_DEVICE_PID),             // idProduct low byte
    HIBYTE(USB_DEVICE_PID),             // idProduct high byte
    0x00, 0x02,                         // bcdDevice, Device release number
    USB_DEVICE_IDX_MFC_STR,             // iManufacturer, Index of string descriptor describing manufacturer
    USB_DEVICE_IDX_PRODUCT_STR,         // iProduct, Index of string descriptor describing product
    USB_DEVICE_IDX_SERIAL_STR,          // iSerialNumber, Index of string descriptor describing the device’s serial number
    0x01                                // bNumConfigurations, Одна конфигурация
};


/**
 * @brief Дескриптор языка, String Descriptor Zero. USB 2.0 Table 9-15
 */
static uint8_t USB_LangIdDescriptor[USB_LANGID_DESCRIPTOR_SIZE] = {
    USB_LANGID_DESCRIPTOR_SIZE,         // bLength
    USB_STRING,                         // bDescriptorType
    LOBYTE(USB_LANGID_STRING),          // wLANGID low byte
    HIBYTE(USB_LANGID_STRING)           // wLANGID high byte
};


/**
 * @brief Дескриптор строки iManufacturer, id=1, "STMicroelectronics". USB 2.0 Table 9-16
 *
 * Строка в кодировке UTF-16, https://onlineunicodetools.com/convert-unicode-to-bytes
 *
 * Размер дескриптора равен: len(str)*2 + 2. 2 байта на символ (UTF-16) и еще 2 на заголовок
 */
static uint8_t USB_ManufacturerStringDescriptor[18 * 2 + 2] = {
    18 * 2 + 2,
    USB_STRING,
    0x53, 0x00, 0x54, 0x00, 0x4d, 0x00, 0x69, 0x00,
    0x63, 0x00, 0x72, 0x00, 0x6f, 0x00, 0x65, 0x00,
    0x6c, 0x00, 0x65, 0x00, 0x63, 0x00, 0x74, 0x00,
    0x72, 0x00, 0x6f, 0x00, 0x6e, 0x00, 0x69, 0x00,
    0x63, 0x00, 0x73, 0x00
};


/**
 * @brief Дескриптор строки iProduct, id=2, "Russian Quantum Center QRate Single Photon Detector". USB 2.0 Table 9-16
 *
 * Строка в кодировке UTF-16, https://onlineunicodetools.com/convert-unicode-to-bytes
 */
static uint8_t USB_ProductStringDescriptor[51 * 2 + 2] = {
    51 * 2 + 2,
    USB_STRING,
    0x52, 0x00, 0x75, 0x00, 0x73, 0x00, 0x73, 0x00, 0x69, 0x00,
    0x61, 0x00, 0x6e, 0x00, 0x20, 0x00, 0x51, 0x00, 0x75, 0x00,
    0x61, 0x00, 0x6e, 0x00, 0x74, 0x00, 0x75, 0x00, 0x6d, 0x00,
    0x20, 0x00, 0x43, 0x00, 0x65, 0x00, 0x6e, 0x00, 0x74, 0x00,
    0x65, 0x00, 0x72, 0x00, 0x20, 0x00, 0x51, 0x00, 0x52, 0x00,
    0x61, 0x00, 0x74, 0x00, 0x65, 0x00, 0x20, 0x00, 0x53, 0x00,
    0x69, 0x00, 0x6e, 0x00, 0x67, 0x00, 0x6c, 0x00, 0x65, 0x00,
    0x20, 0x00, 0x50, 0x00, 0x68, 0x00, 0x6f, 0x00, 0x74, 0x00,
    0x6f, 0x00, 0x6e, 0x00, 0x20, 0x00, 0x44, 0x00, 0x65, 0x00,
    0x74, 0x00, 0x65, 0x00, 0x63, 0x00, 0x74, 0x00, 0x6f, 0x00,
    0x72, 0x00
};


/**
 * @brief Дескриптор строки iSerialNumber, id=3, "00000000001A". USB 2.0 Table 9-16
 *
 * Строка в кодировке UTF-16, https://onlineunicodetools.com/convert-unicode-to-bytes
 */
static uint8_t USB_SerialNumberStringDescriptor[12 * 2 + 2] = {
    12 * 2 + 2,
    USB_STRING,
    0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00,
    0x30, 0x00, 0x30, 0x00, 0x31, 0x00, 0x41, 0x00
};


/**
 * @brief Стандартный дескриптор конфигурации.
 */
static uint8_t USB_ConfigurationDescriptor[USB_HID_CONFIGURATION_DESCRIPTOR_SIZE] = {
    /* CONFIGURATION Descriptor. USB 2.0 Table 9-10 */
    0x09,                               // bLength: Размер CONFIGURATION дескриптора
    USB_CONFIGURATION,                  // bDescriptorType: Configuration, 2
    LOBYTE(USB_HID_CONFIGURATION_DESCRIPTOR_SIZE), // wTotalLength low byte: Размер всего дескриптора
    HIBYTE(USB_HID_CONFIGURATION_DESCRIPTOR_SIZE), // wTotalLength high byte
    0x01,                               // bNumInterfaces
    USB_DEVICE_CONFIGURATION,           // bConfigurationValue: Будет в запросе SetConfiguration
    0x00,                               // iConfiguration
    0xC0,                               // bmAttributes, Self-powered only
    0x19,                               // bMaxPower in 2 mA units, 0x19*2 = 50 mA

    /* INTERFACE Descriptor. USB 2.0 Table 9-12 */
    0x09,                               // bLength
    USB_INTERFACE,                      // bDescriptorType, Interface, 4
    0x00,                               // bInterfaceNumber
    0x00,                               // bAlternateSetting
    0x02,                               // bNumEndpoints
    0x03,                               // bInterfaceClass (3-HID) https://www.usb.org/defined-class-codes
    0x00,                               // bInterfaceSubClass, 0 - no subclass, 1 - boot subclass
    0x00,                               // bInterfaceProtocol, 0 - None, 1 - Keyboard, 2 - Mouse
    0x00,                               // iInterface, Index of string descriptor describing this interface

    /* HID Descriptor. USB-HID 6.2.1 */
    0x09,                               // bLength
    USB_HID_DESCRIPTOR_TYPE,            // bDescriptorType, 0x21
    0x11,                               // bcdHID LOW byte Numeric expression identifying the HID Class Specification release
    0x01,                               // bcdHID HIGH byte
    0x00,                               // bCountryCode
    0x01,                               // bNumDescriptors
    0x22,                               // bDescriptorType
    LOBYTE(USBD_CUSTOM_HID_REPORT_DESC_SIZE), // wDescriptorLength Low byte, 32b total
    HIBYTE(USBD_CUSTOM_HID_REPORT_DESC_SIZE), // wDescriptorLength High byte

    /* ENDPOINT Descriptor. USB 2.0 Table 9-13. 0x81 IN, device -> host */
    0x07,                               // bLength
    USB_ENDPOINT,                       // bDescriptorType
    0x81,                               // bEndpointAddress
    0x03,                               // bmAttributes, Interrupt transfer
    LOBYTE(MAX_PACKET_SIZE),            // wMaxPacketSize Low
    HIBYTE(MAX_PACKET_SIZE),            // wMaxPacketSize High
    0x01,                               // bInterval, 1 ms

    /* ENDPOINT Descriptor. USB 2.0 Table 9-13. 0x02 OUT, host -> device */
    0x07,                               // bLength
    USB_ENDPOINT,                       // bDescriptorType
    0x02,                               // bEndpointAddress
    0x03,                               // bmAttributes, Interrupt transfer
    LOBYTE(MAX_PACKET_SIZE),            // wMaxPacketSize Low
    HIBYTE(MAX_PACKET_SIZE),            // wMaxPacketSize High
    0x01,                               // bInterval, 1 ms
};


/**
 * @brief HID Descriptor USB-HID 6.2.1. Дублирует USB_ConfigurationDescriptor, раздел HID Descriptor
 */
static uint8_t USBD_CUSTOM_HID_Desc[USB_CUSTOM_HID_DESC_SIZ] = {
    USB_CUSTOM_HID_DESC_SIZ,            // bLength: CUSTOM_HID Descriptor size
    USB_HID_DESCRIPTOR_TYPE,            // bDescriptorType: 0x21
    0x11,                               // bcdHID LOW byte Numeric expression identifying the HID Class Specification release
    0x01,                               // bcdHID HIGH byte
    0x00,                               // bCountryCode
    0x01,                               // bNumDescriptors
    0x22,                               // bDescriptorType
    LOBYTE(USBD_CUSTOM_HID_REPORT_DESC_SIZE), // wDescriptorLength Low byte: Total length of Report descriptor
    HIBYTE(USBD_CUSTOM_HID_REPORT_DESC_SIZE), // wDescriptorLength High byte
};


/**
 * @brief HID Report Descriptor, it's a magic! DON'T TOUCH
 */
static uint8_t USB_HID_ReportDesc[USBD_CUSTOM_HID_REPORT_DESC_SIZE] = {
    0x05, 0x01,                         // USAGE_PAGE (Generic Desktop)
    0x09, 0x00,                         // USAGE (Undefined)
    0xa1, 0x01,                         // COLLECTION (Application)

    0x15, 0x00,                         //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,                   //   LOGICAL_MAXIMUM (255)

    // IN report
    0x85, 0x01,                         //   REPORT_ID (1)
    0x75, 0x08,                         //   REPORT_SIZE (8)
    0x95, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE-1, //   REPORT_COUNT (this is the byte length)
    0x09, 0x00,                         //   USAGE (Undefined)
    0x81, 0x82,                         //   INPUT (Data,Var,Abs,Vol)

    // OUT report
    0x85, 0x02,                         //   REPORT_ID (2)
    0x75, 0x08,                        //   REPORT_SIZE (8)
    0x95, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE-1,       //   REPORT_COUNT (this is the byte length)
    0x09, 0x00,                         //   USAGE (Undefined)
    0x91, 0x82,                         //   OUTPUT (Data,Var,Abs,Vol)
    0xC0    /*     END_COLLECTION	             */
};


/**
 * @brief Инициализация HID устройства
 * @param ReceiveBuffer Указатель на буфер для приходящих данных с хоста (OUT Transaction)
 * @param BufferSize Размер буфера в байтах. При заполнении буфера на BufferSize байт будет вызвана USB_HID_OnDataReceive
 * @return USB_SUCCESS всегда
 */
USB_Result USB_HID_Init(uint8_t *ReceiveBuffer, uint32_t BufferSize) {
    USB_HIDContext.HID_ReceiveBuffer = ReceiveBuffer;
    USB_HIDContext.BufferSize = BufferSize;
    return USB_SUCCESS;
}


static USB_Result USB_HID_DoDataOut(USB_EP_TypeDef EPx, uint8_t* Buffer, uint32_t Length) {
    USB_Result result = USB_EP_doDataIn(EPx, 0, 0, 0);
    MDR_LOGD(TAG, "    USB_HID_DoDataOut -> %04Xh", result);
    return result;
}


/**
 * @brief Обработка функции USB SetConfiguration()
 * @param wVALUE - Номер конфигурации
 * @retval USB_SUCCESS в случае успеха (wVALUE == USB_DEVICE_CONFIGURATION)
 * @retval USB_ERROR в остальных случаях
 */
USB_Result USB_HID_SetConfiguration(uint16_t wVALUE) {
    if (wVALUE == USB_DEVICE_CONFIGURATION) {
        return USB_SUCCESS;
    }
    return USB_ERROR;
}


/**
 * @brief Обработчик запросов для Класса HID (EP0). USB-HID 7.2
 * @retval USB_SUCCESS в случае успеха
 * @retval USB_ERROR в остальных случаях
 */
USB_Result USB_HID_ClassRequest() {
    MDR_LOGD(TAG, "USB_HID_ClassRequest: bRequest=%Xh", USB_CurrentSetupPacket.bRequest);
    USB_Result result = USB_SUCCESS;
    uint16_t wValue = USB_CurrentSetupPacket.wValue;
    uint16_t wLength = USB_CurrentSetupPacket.wLength;

    switch (USB_CurrentSetupPacket.bRequest) {
        case CUSTOM_HID_REQ_GET_REPORT:  // Обязательная реализация
            MDR_LOGE(TAG, "  CUSTOM_HID_REQ_GET_REPORT!!");
            assert_param(0);
            break;

        case CUSTOM_HID_REQ_SET_REPORT:
            MDR_LOGE(TAG, "  CUSTOM_HID_REQ_SET_REPORT");
            assert_param(0);
            break;

        case CUSTOM_HID_REQ_GET_IDLE:
            MDR_LOGD(TAG, "  CUSTOM_HID_REQ_GET_IDLE");
            result = USB_EP_doDataIn(USB_EP0, (uint8_t *)&USB_HIDContext.IdleState, 1, USB_DeviceDoStatusOutAck);
            break;

        case CUSTOM_HID_REQ_SET_IDLE:
            USB_HIDContext.IdleState = (uint8_t)(wValue >> 8);
            MDR_LOGD(TAG, "  CUSTOM_HID_REQ_SET_IDLE: %lu", USB_HIDContext.IdleState);
            break;

        case CUSTOM_HID_REQ_GET_PROTOCOL:
            MDR_LOGD(TAG, "  CUSTOM_HID_REQ_GET_PROTOCOL");
            result = USB_EP_doDataIn(USB_EP0, (uint8_t *)&USB_HIDContext.Protocol, 1, USB_DeviceDoStatusOutAck);
            break;

        case CUSTOM_HID_REQ_SET_PROTOCOL:
            USB_HIDContext.Protocol = (uint8_t)(wValue);
            MDR_LOGD(TAG, "  CUSTOM_HID_REQ_SET_PROTOCOL: %lXh", USB_HIDContext.Protocol);
            break;

        default:
            return USB_ERROR;
    }
    return result;
}


/**
 * @brief Обработчик Стандартного запроса дескриптора. USB 2.0 9.4.3
 * @param wValue Descriptor Type and Descriptor Index
 * @param wIndex Zero or Language ID
 * @param wLength Descriptor Length
 * @retval USB_SUCCESS в случае успеха
 * @retval USB_ERROR в остальных случаях
 */
USB_Result USB_HID_GetDescriptor(uint16_t wValue, uint16_t wIndex, uint16_t wLength) {
uint8_t *pDescr = NULL;
uint32_t length;
USB_Result result = USB_SUCCESS;

    MDR_LOGD(TAG, "  GetDescriptor: wValue=%04Xh, wIndex=%04Xh, wLength=%04Xh", wValue, wIndex, wLength);
    switch (wValue >> 8) {
        case USB_DEVICE:
            MDR_LOGD(TAG, "    Descriptor USB_DEVICE");
            pDescr = USB_DeviceDescriptor;
            length = sizeof(USB_DeviceDescriptor);
            break;

        case USB_CONFIGURATION:
            MDR_LOGD(TAG, "    Descriptor USB_CONFIGURATION");
            pDescr = USB_ConfigurationDescriptor;
            length = sizeof(USB_ConfigurationDescriptor);
            break;

        case USB_STRING:  // Запрос строковых констант
            MDR_LOGD(TAG, "    Descriptor USB_STRING, id=%u", ((uint8_t)wValue));
            switch ((uint8_t)wValue) {
                case USB_DEVICE_IDX_LANGID_STR:
                    pDescr = USB_LangIdDescriptor;
                    length = sizeof(USB_LangIdDescriptor);
                    break;
                case USB_DEVICE_IDX_MFC_STR:
                    pDescr = USB_ManufacturerStringDescriptor;
                    length = sizeof(USB_ManufacturerStringDescriptor);
                    break;
                case USB_DEVICE_IDX_PRODUCT_STR:
                    pDescr = USB_ProductStringDescriptor;
                    length = sizeof(USB_ProductStringDescriptor);
                    break;
                case USB_DEVICE_IDX_SERIAL_STR:
                    pDescr = USB_SerialNumberStringDescriptor;
                    length = sizeof(USB_SerialNumberStringDescriptor);
                    break;
                default:
                    length = 0;
            }
            break;

        case USB_HID_REPORT_DESCRIPTOR:
            MDR_LOGD(TAG, "    Descriptor USB_HID_REPORT_DESCRIPTOR 0x22");
            pDescr = USB_HID_ReportDesc;
            length = MIN(USBD_CUSTOM_HID_REPORT_DESC_SIZE, wLength);
            break;

        case USB_HID_DESCRIPTOR_TYPE:
            MDR_LOGD(TAG, "    Descriptor USB_HID_DESCRIPTOR_TYPE 0x21");
            pDescr = USBD_CUSTOM_HID_Desc;
            length = sizeof(USBD_CUSTOM_HID_Desc);
            break;

        default:
            MDR_LOGW(TAG, "    Descriptor UNKNOWN. wValue=%u", (wValue >> 8));
            result = USB_ERROR;
    }

    if (result == USB_SUCCESS) {
        if (length > wLength) {
            length = wLength;
        }
        result = USB_EP_doDataIn(USB_EP0, pDescr, length, USB_DeviceDoStatusOutAck);
    }
    return result;
}


/**
 * @brief Callback по окончанию передачи HID Report в хост
 * @param EPx не используется
 * @param Buffer не используется
 * @param Length не используется
 * @return USB_SUCCESS всегда
 */
static USB_Result USB_HID_InDataTransmitted(USB_EP_TypeDef EPx, uint8_t *Buffer, uint32_t Length) {
(void)EPx; (void)Buffer; (void)Length;
    USB_HIDContext.HidState = HID_STATE_IDLE;
    return USB_SUCCESS;
}


/**
 * @brief Отправляет HID Report длинной 64 байта. Копирование данных из report не производится!!
 * @param report Буфер с данными
 * @param len Размер буфера данных
 * @retval USB_SUCCESS - report помещен в стек USB и начинает передаваться
 * @retval USB_ERR_BUSY - HID в режиме передачи или USB не сконфигурировано
 * TODO Сделать буфер и копировать report для передачи в него
 */
USB_Result USB_HID_SendReport(uint8_t *report, uint16_t len) {
    if ((USB_DeviceContext.USB_DeviceState == USB_DEV_STATE_CONFIGURED) && (USB_HIDContext.HidState == HID_STATE_IDLE)) {
        USB_HIDContext.HidState = HID_STATE_BUSY;
        return USB_EP_doDataIn(USB_HID_EP_SEND, report, len, USB_HID_InDataTransmitted);
    }
    return USB_ERR_BUSY;
}


/**
 * @brief Callback по приходу данных от хоста
 * @param EPx Endpoint number
 * @param buffer указатель на принятые данные
 * @param length Размер принятых данных
 * @return USB_SUCCESS всегда
 */
USB_Result USB_HID_OnDataReceive(USB_EP_TypeDef EPx, uint8_t *buffer, uint32_t length) {
    MDR_LOGD(TAG, "OnDataReceive %lu bytes", length);
    // INFO Вызываем callback по приходу данных от хоста
    USBInProcess(buffer, length);
    // Настроим ENDPOINT на прием данных снова
    return USB_EP_doDataOut(EPx, USB_HIDContext.HID_ReceiveBuffer, USB_HIDContext.BufferSize, USB_HID_OnDataReceive);
}


/**
 * @brief Начальный сброс HID и настройка Endpoints. Функция вызывается один раз
 * @return USB_SUCCESS всегда
 */
USB_Result USB_HID_Reset() {
USB_Result result;
    USB_HIDContext.HidState = HID_STATE_IDLE;
    result = USB_DeviceReset();
    if (result == USB_SUCCESS) {
        USB_EP_Init(USB_HID_EP_SEND, USB_SEPx_CTRL_EPEN_Enable | USB_SEPx_CTRL_EPDATASEQ_Data1, 0); // Нет обработчика ошибок
        USB_EP_Init(USB_HID_EP_RECEIVE, USB_SEP_CTRL_EPEN, 0);
    }

    result = USB_EP_doDataOut(USB_HID_EP_RECEIVE, USB_HIDContext.HID_ReceiveBuffer, USB_HIDContext.BufferSize, USB_HID_OnDataReceive);
    return result;
}
