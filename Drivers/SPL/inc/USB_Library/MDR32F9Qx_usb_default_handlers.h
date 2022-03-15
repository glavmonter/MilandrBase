/**
  ******************************************************************************
  * @file    MDR32F9Qx_usb_default_handlers.h
  * @author  Milandr Application Team
  * @version V2.0.0
  * @date    06/05/2021
  * @brief   This file contains all the functions prototypes for the USB
  *          default handlers.
  ******************************************************************************
  * <br><br>
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MILANDR SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
  * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2021 Milandr</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MDR32F9Qx_USB_DEFAULT_HANDLERS_H
#define __MDR32F9Qx_USB_DEFAULT_HANDLERS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_usb_HID.h"

/* Configuration file MDR32F9Qx_config.h should be included before */
#ifndef __MDR32F9Qx_CONFIG_H
    #error "Configuration file MDR32F9Qx_config.h should be included before"
#endif

/** @addtogroup __MDR32Fx_StdPeriph_Driver MDR32Fx Standard Peripheral Driver
  * @{
  */

/** @addtogroup USB_DEVICE_AND_HID USB DEVICE AND HID
  * @{
  */

/** @defgroup USB_Configuring USB Configuring
  * @{
  */

/** @defgroup USB_Device_Configuring USB Device Configuring
  * @{
  */

/**
  * @brief Bus reset handler
  */
#define USB_DEVICE_HANDLE_RESET  USB_DeviceReset()

/**
  * @brief Device enter suspended mode handler
  */
#define USB_DEVICE_HANDLE_SUSPEND  USB_DeviceSuspend()

/**
  * @brief Device exit from suspended mode handler
  */
#define USB_DEVICE_HANDLE_RESUME  USB_DeviceResume()

/**
  * @brief Control Setup Packet general processing handle
  */
#define USB_DEVICE_HANDLE_SETUP   USB_DeviceSetupPacket

/**
  * @brief GET_STATUS standard request handler
  */
#define USB_DEVICE_HANDLE_GET_STATUS(Recipient, wINDEX)    USB_SUCCESS
/* To redefine handler for your own function choose the variant below using
 * USB_DeviceDummyGetStatus as an example and replacing it with your own function
#define USB_DEVICE_HANDLE_GET_STATUS(Recipient, wINDEX)    USB_DeviceDummyGetStatus(Recipient, wINDEX)
 */

/**
  * @brief CLEAR_FEATURE standard request handler
  */
#define USB_DEVICE_HANDLE_CLEAR_FEATURE(Recipient, wVALUE, wINDEX) \
        USB_DeviceClearFeature(Recipient, wVALUE, wINDEX)

/**
  * @brief SET_FEATURE standard request handler
  */
#define USB_DEVICE_HANDLE_SET_FEATURE(Recipient, wVALUE, wINDEX)  \
        USB_DeviceSetFeature(Recipient, wVALUE, wINDEX)

/**
  * @brief SET_ADDRESS standard request handler (preparation phase)
  */
#define USB_DEVICE_HANDLE_SET_ADDRESS(wVALUE)  USB_SUCCESS
/* To redefine handler for your own function choose the variant below using
 * USB_DeviceDummySetAddress as an example and replacing it with your own function
#define USB_DEVICE_HANDLE_SET_ADDRESS(wVALUE)  USB_DeviceDummySetAddress(wVALUE)
 */

/**
  * @brief GET_DESCRIPTOR standard request handler
  */
#define USB_DEVICE_HANDLE_GET_DESCRIPTOR(wVALUE, wINDEX, wLENGTH)  USB_ERROR
/* To redefine handler for your own function choose the variant below using
 * USB_DeviceDummyGetDescriptor as an example and replacing it with your own function
#define USB_DEVICE_HANDLE_GET_DESCRIPTOR(wVALUE, wINDEX, wLENGTH)  USB_DeviceDummyGetDescriptor(wVALUE, wINDEX, wLENGTH)
 */

/**
  * @brief SET_DESCRIPTOR standard request handler
  */
#define USB_DEVICE_HANDLE_SET_DESCRIPTOR(wVALUE, wINDEX, wLENGTH)  USB_ERROR
/* To redefine handler for your own function choose the variant below using
 * USB_DeviceDummySetDescriptor as an example and replacing it with your own function
#define USB_DEVICE_HANDLE_SET_DESCRIPTOR(wVALUE, wINDEX, wLENGTH)  USB_DeviceDummySetDescriptor(wVALUE, wINDEX, wLENGTH)
 */

/**
  * @brief GET_CONFIGURATION standard request handler
  */
#define USB_DEVICE_HANDLE_GET_CONFIGURATION  1
/* To redefine handler for your own function choose the variant below using
 * USB_DeviceDummyGetConfiguration as an example and replacing it with your own function
#define USB_DEVICE_HANDLE_GET_CONFIGURATION  USB_DeviceDummyGetConfiguration()
 */

/**
  * @brief SET_CONFIGURATION standard request handler
  */
#define USB_DEVICE_HANDLE_SET_CONFIGURATION(wVALUE)  \
        ((wVALUE) == 1 ? USB_SUCCESS : USB_ERROR)
/* To redefine handler for your own function choose the variant below using
 * USB_DeviceDummySetConfiguration as an example and replacing it with your own function
#define USB_DEVICE_HANDLE_SET_CONFIGURATION(wVALUE)  USB_DeviceDummySetConfiguration(wVALUE)
 */

/**
  * @brief GET_INTERFACE standard request handler
  */
#define USB_DEVICE_HANDLE_GET_INTERFACE(wINDEX)  0
/* To redefine handler for your own function choose the variant below using
 * USB_DeviceDummyGetInterface as an example and replacing it with your own function
#define USB_DEVICE_HANDLE_GET_INTERFACE(wINDEX)  USB_DeviceDummyGetInterface(wINDEX)
 */

/**
  * @brief SET_INTERFACE standard request handler
  */
#define USB_DEVICE_HANDLE_SET_INTERFACE(wVALUE, wINDEX) \
        ((wINDEX) == 0 && (wVALUE) == 0 ? USB_SUCCESS : USB_ERROR)
/* To redefine handler for your own function choose the variant below using
 * USB_DeviceDummySetInterface as an example and replacing it with your own function
#define USB_DEVICE_HANDLE_SET_INTERFACE(wVALUE, wINDEX)  USB_DeviceDummySetInterface(wVALUE, wINDEX)
 */

/**
  * @brief SYNC_FRAME standard request handler
  */
#define USB_DEVICE_HANDLE_SYNC_FRAME(wINDEX, DATA)  USB_ERROR
/* To redefine handler for your own function choose the variant below using
 * USB_DeviceDummySyncFrame as an example and replacing it with your own function
#define USB_DEVICE_HANDLE_SYNC_FRAME(wINDEX, DATA)  USB_DeviceDummySyncFrame(wINDEX, DATA)
 */

/**
  * @brief Class-type request handler
  */
#define USB_DEVICE_HANDLE_CLASS_REQUEST  USB_ERROR
/* To redefine handler for your own function choose the variant below using
 * USB_DeviceDummyClassRequest as an example and replacing it with your own function
#define USB_DEVICE_HANDLE_CLASS_REQUEST  USB_DeviceDummyClassRequest()
 */

/**
  * @brief Vendor-type request handler
  */
#define USB_DEVICE_HANDLE_VENDOR_REQUEST  USB_ERROR
/* To redefine handler for your own function choose the variant below using
 * USB_DeviceDummyVendorRequest as an example and replacing it with your own function
#define USB_DEVICE_HANDLE_VENDOR_REQUEST  USB_DeviceDummyVendorRequest()
 */

/** @} */ /* End of group USB_Device_Configuring */

/** @defgroup USB_HID_Configuring USB HID Configuring
  * @{
  */

#if (USB_DEVICE_CLASS == USB_DEVICE_CLASS_HID)

/** @defgroup USB_HID_Used_Device_Handlers USB HID Used Device Handlers
  * @{
  */

#undef  USB_DEVICE_HANDLE_RESET
#define USB_DEVICE_HANDLE_RESET                                     USB_HID_Reset()

#undef  USB_DEVICE_HANDLE_CLASS_REQUEST
#define USB_DEVICE_HANDLE_CLASS_REQUEST                             USB_HID_ClassRequest()

#undef  USB_DEVICE_HANDLE_GET_DESCRIPTOR
#define USB_DEVICE_HANDLE_GET_DESCRIPTOR(a, b, c)   USB_HID_GetDescriptor(a, b, c)

#undef USB_DEVICE_HANDLE_SET_CONFIGURATION
#define USB_DEVICE_HANDLE_SET_CONFIGURATION(a)                 USB_HID_SetConfiguration(a)

/** @} */ /* End of group USB_HID_Used_Device_Handlers */

#endif /* USB_DEVICE_CLASS == USB_DEVICE_CLASS_HID */

/** @} */ /* End of group USB_HID_Configuring */

/** @} */ /* End of group USB_Configuring */

/** @} */ /* End of group USB_DEVICE_AND_HID */

/** @} */ /* End of group __MDR32Fx_StdPeriph_Driver */

#ifdef __cplusplus
} // extern "C" block end
#endif

#endif /* __MDR32F9Qx_USB_DEFAULT_HANDLERS_H */

/*********************** (C) COPYRIGHT 2021 Milandr ****************************
*
* END OF FILE MDR32F9Qx_usb_default_handlers.h */

