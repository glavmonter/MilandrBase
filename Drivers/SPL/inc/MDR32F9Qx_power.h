/**
  ******************************************************************************
  * @file    MDR32F9Qx_power.h
  * @author  Milandr Application Team
  * @version V2.0.0
  * @date    06/05/2021
  * @brief   This file contains all the functions prototypes for the POWER
  *          firmware library.
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
#ifndef __MDR32F9Qx_POWER_H
#define __MDR32F9Qx_POWER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_config.h"

/** @addtogroup __MDR32Fx_StdPeriph_Driver MDR32Fx Standard Peripherial Driver
  * @{
  */

/** @addtogroup POWER
  * @{
  */

/** @defgroup POWER_Exported_Types POWER Exported Types
  * @{
  */

#if defined (MDR_LEGACY_SUPPORT) && (MDR_LEGACY_SUPPORT) == 153

#define POWER_DUCC_MODE(par) ((uint32_t)((par << 3)|(par)))
/**
 *  @brief POWER Ducc Mode
 */
typedef enum
{
    POWER_DUcc_upto_10MHz  = POWER_DUCC_MODE(0), /*!< system clock is up to 10 MHz */
    POWER_DUcc_upto_200kHz = POWER_DUCC_MODE(1), /*!< system clock is up to 200 kHz */
    POWER_DUcc_upto_500kHz = POWER_DUCC_MODE(2), /*!< system clock is up to 500 kHz */
    POWER_DUcc_upto_1MHz   = POWER_DUCC_MODE(3), /*!< system clock is up to 1 MHz */
    POWER_DUcc_clk_off     = POWER_DUCC_MODE(4), /*!< all clocks are switched off */
    POWER_DUcc_upto_40MHz  = POWER_DUCC_MODE(5), /*!< system clock is up to 40 MHz */
    POWER_DUcc_upto_80MHz  = POWER_DUCC_MODE(6), /*!< system clock is up to 80 MHz */
    POWER_DUcc_over_80MHz  = POWER_DUCC_MODE(7)  /*!< system clock is over 80 MHz */
} PWR_DUcc_Mode;

#define IS_POWER_DUCC_MODE(MODE) (((MODE) == POWER_DUcc_upto_10MHz) || \
                                  ((MODE) == POWER_DUcc_upto_200kHz)|| \
                                  ((MODE) == POWER_DUcc_upto_500kHz)|| \
                                  ((MODE) == POWER_DUcc_clk_off)    || \
                                  ((MODE) == POWER_DUcc_upto_1MHz)  || \
                                  ((MODE) == POWER_DUcc_upto_40MHz) || \
                                  ((MODE) == POWER_DUcc_upto_80MHz) || \
                                  ((MODE) == POWER_DUcc_over_80MHz))

/**
 *  @brief POWER Ducc Trim
 */
typedef enum
{
    POWER_DUcc_plus_100mV  = ((uint32_t)(0x0000 << 8)), /*!< trim DUcc with + 100mV */
    POWER_DUcc_plus_060mV  = ((uint32_t)(0x0001 << 8)), /*!< trim DUcc with + 060mV */
    POWER_DUcc_plus_040mV  = ((uint32_t)(0x0002 << 8)), /*!< trim DUcc with + 040mV */
    POWER_DUcc_plus_010mV  = ((uint32_t)(0x0003 << 8)), /*!< trim DUcc with + 010mV */
    POWER_DUcc_minus_010mV = ((uint32_t)(0x0004 << 8)), /*!< trim DUcc with - 010mV */
    POWER_DUcc_minus_040mV = ((uint32_t)(0x0005 << 8)), /*!< trim DUcc with - 040mV */
    POWER_DUcc_minus_060mV = ((uint32_t)(0x0006 << 8)), /*!< trim DUcc with - 060mV */
    POWER_DUcc_minus_100mV = ((uint32_t)(0x0007 << 8))  /*!< trim DUcc with - 100mV */
} PWR_DUcc_Trim;

#define IS_POWER_DUCC_TRIM(TRIM) (((TRIM) == POWER_DUcc_plus_100mV) || \
                                  ((TRIM) == POWER_DUcc_plus_060mV) || \
                                  ((TRIM) == POWER_DUcc_plus_040mV) || \
                                  ((TRIM) == POWER_DUcc_plus_010mV) || \
                                  ((TRIM) == POWER_DUcc_minus_010mV)|| \
                                  ((TRIM) == POWER_DUcc_minus_040mV)|| \
                                  ((TRIM) == POWER_DUcc_minus_060mV)|| \
                                  ((TRIM) == POWER_DUcc_minus_100mV))

#endif /* MDR_LEGACY_SUPPORT 153 */

/**
 *  @brief POWER Voltage Detector Level
 */
typedef enum
{
    PWR_PVDlevel_2V0 = ((uint32_t)(0x00 << 3)), /*!< PVD detection level set to 2.0V */
    PWR_PVDlevel_2V2 = ((uint32_t)(0x01 << 3)), /*!< PVD detection level set to 2.2V */
    PWR_PVDlevel_2V4 = ((uint32_t)(0x02 << 3)), /*!< PVD detection level set to 2.4V */
    PWR_PVDlevel_2V6 = ((uint32_t)(0x03 << 3)), /*!< PVD detection level set to 2.6V */
    PWR_PVDlevel_2V8 = ((uint32_t)(0x04 << 3)), /*!< PVD detection level set to 2.8V */
    PWR_PVDlevel_3V0 = ((uint32_t)(0x05 << 3)), /*!< PVD detection level set to 3.0V */
    PWR_PVDlevel_3V2 = ((uint32_t)(0x06 << 3)), /*!< PVD detection level set to 3.2V */
    PWR_PVDlevel_3V4 = ((uint32_t)(0x07 << 3))  /*!< PVD detection level set to 3.4V */
} PWR_Voltage_Detector_Lvl;

#define IS_POWER_PVD_LEVEL(LVL) (((LVL) == PWR_PVDlevel_2V0) || \
                                 ((LVL) == PWR_PVDlevel_2V2) || \
                                 ((LVL) == PWR_PVDlevel_2V4) || \
                                 ((LVL) == PWR_PVDlevel_2V6) || \
                                 ((LVL) == PWR_PVDlevel_2V8) || \
                                 ((LVL) == PWR_PVDlevel_3V0) || \
                                 ((LVL) == PWR_PVDlevel_3V2) || \
                                 ((LVL) == PWR_PVDlevel_3V4))

/**
 *  @brief POWER Battery Voltage Detector Level
 */
typedef enum
{
    PWR_PVBDlevel_1V8 = ((uint32_t)(0x00 << 1)), /*!< PVBD detection level set to 1.8V */
    PWR_PVBDlevel_2V2 = ((uint32_t)(0x01 << 1)), /*!< PVBD detection level set to 2.2V */
    PWR_PVBDlevel_2V6 = ((uint32_t)(0x02 << 1)), /*!< PVBD detection level set to 2.6V */
    PWR_PVBDlevel_3V0 = ((uint32_t)(0x03 << 1))  /*!< PVBD detection level set to 3.0V */
} PWR_Batt_Voltage_Detector_Lvl;

#define IS_POWER_PVBD_LEVEL(LVL) (((LVL) == PWR_PVBDlevel_1V8) || \
                                  ((LVL) == PWR_PVBDlevel_2V2) || \
                                  ((LVL) == PWR_PVBDlevel_2V6) || \
                                  ((LVL) == PWR_PVBDlevel_3V0))

/**
 *  @brief POWER Voltage Detector Output Flags
 */
typedef enum
{
    POWER_FLAG_PVD  = ((uint32_t)POWER_PVDCS_PVD), /*!< Power voltage detection flag */
    POWER_FLAG_PVBD = ((uint32_t)POWER_PVDCS_PVBD) /*!< Battery voltage detection flag */
} PWR_Voltage_Detector_Flags;

#define IS_POWER_FLAG(F) (((F) == POWER_FLAG_PVD) || ((F) == POWER_FLAG_PVBD))

#define IS_POWER_FLAGS(F) (((F) == POWER_FLAG_PVD)  || \
                           ((F) == POWER_FLAG_PVBD) || \
                           ((F) == (POWER_FLAG_PVD & POWER_FLAG_PVBD)))

/**
 *  @brief POWER Voltage Detector Interrupt Enable
 */
typedef enum
{
    POWER_PVD_IT  = ((uint32_t)POWER_PVDCS_IEPVD), /*!< PVD interrupt enable  */
    POWER_PVBD_IT = ((uint32_t)POWER_PVDCS_IEPVBD) /*!< PVBD interrupt enable */
} PWR_Voltage_Detector_ITEnable;

#define IS_POWER_PVD_IT(F) (((F) == POWER_PVD_IT) || ((F) == POWER_PVBD_IT))

/**
 *  @brief POWER Voltage Detector Interrupt Inversion
 */
typedef enum
{
    POWER_PVD_INV  = ((uint32_t)POWER_PVDCS_INV), /*!< PVD inversion enable  */
    POWER_PVBD_INV = ((uint32_t)POWER_PVDCS_INVB) /*!< PVBD inversion enable */
} PWR_Voltage_Detector_IT_Inversion;

#define IS_POWER_PVD_INV(F) (((F) == POWER_PVD_INV) || ((F) == POWER_PVBD_INV))


#if defined (MDR_LEGACY_SUPPORT) && (MDR_LEGACY_SUPPORT) == 153

#if defined (USE_MDR1986VE1T) || defined (USE_MDR1986VE3)
/**
 *  @brief POWER Trim
 */
typedef enum
{
    POWER_TRIM_1_8_V = (0 << BKP_REG_0E_TRIM_34_Pos),
    POWER_TRIM_1_6_V = (1 << BKP_REG_0E_TRIM_34_Pos),
    POWER_TRIM_1_4_V = (2 << BKP_REG_0E_TRIM_34_Pos)
} PWR_Trim;

#define IS_POWER_TRIM(TRIM)     (((TRIM) == POWER_TRIM_1_4_V) || \
                                 ((TRIM) == POWER_TRIM_1_6_V) || \
                                 ((TRIM) == POWER_TRIM_1_8_V))

#endif /* #if defined (USE_MDR1986VE1T) || defined (USE_MDR1986VE3) */

#if defined (USE_MDR1986VE9x) || defined (USE_MDR1901VC1T)
/**
 *  @brief POWER Stop Entry Mode
 */
typedef enum
{
    POWER_STOPentry_WFE = ((uint32_t)0x00), /*!< enter STOP mode with WFE instruction */
    POWER_STOPentry_WFI = ((uint32_t)0x01)  /*!< enter STOP mode with WFI instruction */
} PWR_Stop_Entry_Mode;

#define IS_POWER_STOP_ENTRY(F)      (((F) == POWER_STOPentry_WFI) || \
                                     ((F) == POWER_STOPentry_WFE))

#endif /* #if defined (USE_MDR1986VE9x) || defined (USE_MDR1901VC1T) */

#endif /* MDR_LEGACY_SUPPORT 153 */

/** @} */ /* End of group POWER_Exported_Types */


/** @defgroup POWER_Exported_Functions POWER Exported Functions
  * @{
  */

void POWER_DeInit(void);

#if defined (MDR_LEGACY_SUPPORT) && (MDR_LEGACY_SUPPORT) == 153
    void POWER_DUccMode(PWR_DUcc_Mode DUccMode);
    void POWER_DUccTrim(PWR_DUcc_Trim DUccTrim);
    void POWER_DUccStandby(void);
    void POWER_EnterSTANDBYMode(void);
    void POWER_SetFlagPOR(void);
    ErrorStatus POWER_FlagPORstatus(void);
#if defined (USE_MDR1986VE9x) || defined (USE_MDR1901VC1T)
    void POWER_EnterSTOPMode(FunctionalState POWER_Regulator_state, PWR_Stop_Entry_Mode POWER_STOPEntry);
#elif defined (USE_MDR1986VE1T) || defined (USE_MDR1986VE3)
    void POWER_EnterSLEEPMode(void);
    void POWER_SetTrim(PWR_Trim ducc_trim);
#endif
#endif /* MDR_LEGACY_SUPPORT 153 */
void POWER_PVDenable(FunctionalState NewState);
void POWER_PVDlevelConfig(PWR_Voltage_Detector_Lvl POWER_PVDlevel);
void POWER_PVBDlevelConfig(PWR_Batt_Voltage_Detector_Lvl POWER_PVBDlevel);
FlagStatus POWER_GetFlagStatus(PWR_Voltage_Detector_Flags POWER_FLAG);
void POWER_ClearFlag(uint32_t POWER_FLAG);
void POWER_PVD_ITconfig(uint32_t POWER_IT, FunctionalState NewState);
void POWER_PVD_INVconfig(uint32_t POWER_INV, FunctionalState NewState);

/** @} */ /* End of group POWER_Exported_Functions */

/** @} */ /* End of group POWER */

/** @} */ /* End of group __MDR32Fx_StdPeriph_Driver */

#ifdef __cplusplus
} // extern "C" block end
#endif

#endif /* __MDR32F9Qx_POWER_H */

/*********************** (C) COPYRIGHT 2021 Milandr ****************************
*
* END OF FILE MDR32F9Qx_power.h */

