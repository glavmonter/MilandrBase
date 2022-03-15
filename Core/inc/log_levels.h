#ifndef MILANDRBASE_LOG_LEVELS_H
#define MILANDRBASE_LOG_LEVELS_H


/**
 * Допустимые уровни логгирования
 *   -MDR_LOG_VERBOSE
 *   -MDR_LOG_DEBUG
 *   -MDR_LOG_INFO
 *   -MDR_LOG_WARN
 *   -MDR_LOG_ERROR
 *   -MDR_LOG_NONE
 */

#ifndef LOG_TAG_USBD_LOCAL_LEVEL
#define LOG_TAG_USBD_LOCAL_LEVEL    MDR_LOG_NONE  ///< Log level for TAG "USBD" (Usb Device low level, endpoints states)
#endif


#ifndef LOG_TAG_USB_LOCAL_LEVEL
#define LOG_TAG_USB_LOCAL_LEVEL     MDR_LOG_NONE   ///< Log level for TAG "USB" (Usb HID high level)
#endif


#ifndef LOG_TAG_MAIN_LEVEL
#define LOG_TAG_MAIN_LEVEL    MDR_LOG_VERBOSE ///< Log level for TAG "MAIN"
#endif

#ifndef LOG_TAG_SSP_LOCAL_LEVEL
#define LOG_TAG_SSP_LOCAL_LEVEL     MDR_LOG_VERBOSE
#endif

#ifndef LOG_TAG_IIC_LOCAL_LEVEL
#define LOG_TAG_IIC_LOCAL_LEVEL     MDR_LOG_VERBOSE
#endif

#ifndef LOG_TAG_IIC_MASTER_LOCAL_LEVEL
#define LOG_TAG_IIC_MASTER_LOCAL_LEVEL     MDR_LOG_VERBOSE
#endif


#ifndef LOG_TAG_IICSW_LOCAL_LEVEL
#define LOG_TAG_IICSW_LOCAL_LEVEL   MDR_LOG_NONE
#endif

#endif //MILANDRBASE_LOG_LEVELS_H
