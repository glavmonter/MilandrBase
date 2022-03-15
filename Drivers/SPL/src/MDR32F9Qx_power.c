/**
  ******************************************************************************
  * @file    MDR32F9Qx_power.c
  * @author  Milandr Application Team
  * @version V2.0.2
  * @date    21/09/2021
  * @brief   This file contains all the POWER firmware functions.
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

/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_power.h"

/** @addtogroup __MDR32Fx_StdPeriph_Driver MDR32Fx Standard Peripherial Driver
  * @{
  */

/** @defgroup POWER POWER
  * @{
  */

/** @defgroup POWER_Private_Defines POWER Private Defines
  * @{
  */

#if defined (USE_MDR1986VE9x) || defined (USE_MDR1901VC1T)
/* POWER registers bit address in the alias region */
#define PERIPH_BASE                 0x40000000
#define PERIPH_BB_BASE              0x42000000
#define POWER_OFFSET                (MDR_POWER_BASE - PERIPH_BASE)
#define BKP_OFFSET                  (MDR_BKP_BASE - PERIPH_BASE)

#define SFR_OFFSET(TP, MOD, SFR)    ((uint32_t)&((TP*)MOD)->SFR)
#define BB_ADDR(TP, MOD, SFR, BIT)  (PERIPH_BB_BASE + SFR_OFFSET(TP, MOD, SFR)*32 + BIT*4)
#define POWER_BB(SFR, BIT)          BB_ADDR(MDR_POWER_TypeDef, POWER_OFFSET, SFR, BIT)
#define BKP_BB(SFR, BIT)            BB_ADDR(MDR_BKP_TypeDef, BKP_OFFSET, SFR, BIT)

#define POWER_PVDEN_BB              POWER_BB(PVDCS, POWER_PVDCS_PVDEN_Pos)

#define BKP_FPOR_BB                 BKP_BB(REG_0E, BKP_REG_0E_FPOR_Pos)
#define BKP_STANDBY_BB              BKP_BB(REG_0F, BKP_REG_0F_STANDBY_Pos)

#endif /* #if defined (USE_MDR1986VE9x) || defined (USE_MDR1901VC1T) */

/* --------------------- POWER registers bit mask ------------------------ */

/* BKP_REG0E register bit mask */
#define DUcc_Mask         ((uint32_t)(BKP_REG_0E_LOW_Msk | BKP_REG_0E_SELECTRI_Msk))
#define DUccTrim_Mask     ((uint32_t)BKP_REG_0E_TRIM_Msk)

/** @} */ /* End of group POWER_Private_Defines */


/** @defgroup POWER_Private_Functions POWER Private Functions
  * @{
  */

/**
  * @brief  Deinitializes the POWER peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void POWER_DeInit(void)
{
    MDR_POWER->PVDCS = (uint32_t)0x00000000;
}

#if defined (MDR_LEGACY_SUPPORT) && (MDR_LEGACY_SUPPORT) == 153
/**
  * @brief  Select the internal voltage regulator mode
  * @param  DUccMode - @ref PWR_DUcc_Mode - specifies the DUcc mode.
  * @retval None
  */
void POWER_DUccMode(PWR_DUcc_Mode DUccMode)
{
    #warning "Legacy v1.5.3: POWER_DUccMode() moved to MDR32F9Qx_bkp.c as BKP_DUccMode() and will be removed from MDR32F9Qx_power.c"
    uint32_t tmpreg;

    /* Check the parameters */
    assert_param(IS_POWER_DUCC_MODE(DUccMode));

    /* Clear POWER_REG0E[5:0] bits */
    tmpreg  = MDR_BKP -> REG_0E & (uint32_t) (~DUcc_Mask);
    /* Set POWER_REG0E[5:0] bits according to DUcc mode */
    tmpreg |= DUcc_Mask & ((uint32_t)DUccMode);

    MDR_BKP -> REG_0E = tmpreg;
}

/**
  * @brief  Set the internal voltage regulator trim
  * @param  DUccTrim - @ref PWR_DUcc_Trim - specifies the DUcc trim.
  * @retval None
  */
void POWER_DUccTrim(PWR_DUcc_Trim DUccTrim)
{
    #warning "Legacy v1.5.3: POWER_DUccTrim() moved to MDR32F9Qx_bkp.c as BKP_DUccTrim() and will be removed from MDR32F9Qx_power.c"
    uint32_t tmpreg;

    /* Check the parameters */
    assert_param(IS_POWER_DUCC_TRIM(DUccTrim));

    /* Clear POWER_REG0E[5:0] bits */
    tmpreg  = MDR_BKP -> REG_0E & (uint32_t) (~DUccTrim_Mask);
    /* Set POWER_REG0E[5:0] bits according to DUcc mode */
    tmpreg |= DUccTrim_Mask & ((uint32_t)DUccTrim);

    MDR_BKP -> REG_0E = tmpreg;
}

/**
  * @brief  Enter standby mode.
  * @param  None
  * @retval None
  */
void POWER_DUccStandby(void)
{
    #warning "Legacy v1.5.3: POWER_DUccStandby() moved to MDR32F9Qx_bkp.c as BKP_EnterSTANDBYMode() and will be removed from MDR32F9Qx_power.c"

#if defined (USE_MDR1986VE9x) || defined (USE_MDR1901VC1T)
    *(__IO uint32_t *) BKP_STANDBY_BB = (uint32_t) 0x01;
#elif defined (USE_MDR1986VE3)
    MDR_BKP->REG_0F |= BKP_REG_0F_STANDBY;
#endif
}

/**
  * @brief  Enters STANDBY mode.
  * @param  None
  * @retval None
  */
void POWER_EnterSTANDBYMode(void)
{
    #warning "Legacy v1.5.3: POWER_EnterSTANDBYMode() moved to MDR32F9Qx_bkp.c as BKP_EnterSTANDBYMode() and will be removed from MDR32F9Qx_power.c"

    /* Select STANDBY mode */
#if defined (USE_MDR1986VE9x) || defined (USE_MDR1901VC1T)
    *(__IO uint32_t *) BKP_STANDBY_BB = (uint32_t) 0x01;
#elif defined (USE_MDR1986VE3) || defined (USE_MDR1986VE1T)
    MDR_BKP->REG_0F |= BKP_REG_0F_STANDBY;
#endif
}

/**
  * @brief  Set power on reset flag (FPOR).
  * @param  None
  * @retval None
  */
void POWER_SetFlagPOR(void)
{
    #warning "Legacy v1.5.3: POWER_SetFlagPOR() moved to MDR32F9Qx_bkp.c as BKP_SetFlagPOR() and will be removed from MDR32F9Qx_power.c"

#if defined (USE_MDR1986VE9x) || defined (USE_MDR1901VC1T)
    *(__IO uint32_t *) BKP_FPOR_BB = (uint32_t) 0x01;
#elif defined (USE_MDR1986VE3) || defined (USE_MDR1986VE1T)
    MDR_BKP->REG_0E |= BKP_REG_0E_FPOR;
#endif
}

/**
  * @brief  Power on reset flag (FPOR) status.
  * @param  None
  * @retval @ref ErrorStatus - SUCCESS if FPOR is zero, else ERROR
  */
ErrorStatus POWER_FlagPORstatus(void)
{
    #warning "Legacy v1.5.3: POWER_FlagPORstatus() moved to MDR32F9Qx_bkp.c as BKP_FlagPORstatus() and will be removed from MDR32F9Qx_power.c"
    ErrorStatus state = ERROR;

#if defined (USE_MDR1986VE9x) || defined (USE_MDR1901VC1T)
    if (*(__IO uint32_t *) BKP_FPOR_BB == 0)
    {
        state = SUCCESS;
    }
#elif defined (USE_MDR1986VE3) || defined (USE_MDR1986VE1T)
    if ((MDR_BKP->REG_0E & BKP_REG_0E_FPOR) == BKP_REG_0E_FPOR)
    {
        state = SUCCESS;
    }
#endif

    return state;
}

#if defined (USE_MDR1986VE9x) || defined (USE_MDR1901VC1T)
/**
  * @brief   Enters STOP mode.
  * @warning This function can be used only for microcontroller
  *          series MDR1986VE9x and MDR1901VC1T
  * @param   POWER_Regulator_state - @ref FunctionalState - specifies the regulator state in STOP mode.
  *          This parameter can be: ENABLE or DISABLE.
  *            @arg ENABLE: STOP mode with regulator ON
  *            @arg DISABLE: STOP mode with regulator in low power mode
  * @param   POWER_STOPentry - @ref PWR_Stop_Entry_Mode - specifies if STOP mode in entered with WFI or WFE instruction.
  * @retval  None
  */
void POWER_EnterSTOPMode(FunctionalState POWER_Regulator_state, PWR_Stop_Entry_Mode POWER_STOPentry)
{
    #warning "Legacy v1.5.3: POWER_EnterSTOPMode() moved to MDR32F9Qx_bkp.c as BKP_EnterSTOPMode() and will be removed from MDR32F9Qx_power.c"

    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(POWER_Regulator_state));
    assert_param(IS_POWER_STOP_ENTRY(POWER_STOPentry));

    if(POWER_Regulator_state == DISABLE)
    {
        MDR_BKP->REG_0F |= BKP_REG_0F_STANDBY;
    }
    else
    {
        /* Set SLEEPDEEP bit */
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
        /* Select STOP mode entry */
        if(POWER_STOPentry == POWER_STOPentry_WFI)
        {
            /* Request Wait For Interrupt */
            __WFI();
        }
        else
        {
            /* Request Wait For Event */
            __WFE();
        }
    }
}

#elif defined (USE_MDR1986VE1T) || defined (USE_MDR1986VE3)
/**
  * @brief   Shifting core controller into a low power consumption. In this mode,
  *          the clock frequency is applied only to the selected peripheral
  *          blocks, which interrupt the supply resumes clock on the core.
  * @warning This function can be used only for microcontroller
  *          series MDR1986VE1T and MDR1986VE3.
  * @param   None
  * @retval  None
  */
void POWER_EnterSLEEPMode(void)
{
    #warning "Legacy v1.5.3: POWER_EnterSLEEPMode() moved to MDR32F9Qx_bkp.c as BKP_EnterSLEEPMode() and will be removed from MDR32F9Qx_power.c"

    /* Enter in SLEEP mode */
    MDR_RST_CLK->ETH_CLOCK |= 1 << RST_CLK_ETH_CLOCK_SLEEP_Pos;
}

/**
  * @brief   Adjustment coefficient of the reference voltage
  *          integrated voltage regulator DUcc roughly.
  * @warning This function can be used only for microcontroller
  *          series MDR1986VE1T and MDR1986VE3.
  * @param   ducc_trim - @ref PWR_Trim - coefficient of the reference voltage.
  * @return  None.
  */
void POWER_SetTrim(PWR_Trim ducc_trim)
{
    #warning "Legacy v1.5.3: POWER_SetTrim() moved to MDR32F9Qx_bkp.c as BKP_SetTrim() and will be removed in next update from MDR32F9Qx_power.c"
    uint32_t tmpreg;

    /* Check the parameters */
    assert_param(IS_POWER_TRIM(ducc_trim));

    tmpreg = MDR_BKP->REG_0E & (~(3 << BKP_REG_0E_TRIM_34_Pos));
    tmpreg = tmpreg | ((uint32_t)ducc_trim);

    MDR_BKP->REG_0E = tmpreg;
}
#endif /* #elif defined (USE_MDR1986VE1T) || defined (USE_MDR1986VE3) */

#endif /* MDR_LEGACY_SUPPORT 153 */

/**
  * @brief  Enables or disables the Power Voltage Detectors (PVD, PVBD).
  * @param  NewState - @ref FunctionalState - new state of the PVDs.
  * @retval None
  */
void POWER_PVDenable(FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

#if defined (USE_MDR1986VE9x) || defined (USE_MDR1901VC1T)
    *(__IO uint32_t *) POWER_PVDEN_BB = (uint32_t) NewState;
#elif defined (USE_MDR1986VE3) || defined (USE_MDR1986VE1T)

    if(NewState != DISABLE)
    {
        MDR_POWER->PVDCS |= POWER_PVDCS_PVDEN;
    }
    else
    {
        MDR_POWER->PVDCS &= ~POWER_PVDCS_PVDEN;
    }
#endif
}

/**
  * @brief  Configures the voltage threshold detected
  *         by the Power Voltage Detector(PVD).
  * @param  POWER_PVDlevel - @ref PWR_Voltage_Detector_Lvl - specifies the PVD detection level.
  * @retval None
  */
void POWER_PVDlevelConfig(PWR_Voltage_Detector_Lvl POWER_PVDlevel)
{
    uint32_t tmpreg;

    /* Check the parameters */
    assert_param(IS_POWER_PVD_LEVEL(POWER_PVDlevel));

    tmpreg = MDR_POWER->PVDCS;

    /* Clear PLS[5:3] bits */
    tmpreg &= (uint32_t) ~POWER_PVDCS_PLS_Msk;
    /* Set PLS[5:3] bits according to POWER_PVDlevel value */
    tmpreg |= ((uint32_t)POWER_PVDlevel & POWER_PVDCS_PLS_Msk);

    /* Store the new value */
    MDR_POWER->PVDCS = tmpreg;
}

/**
  * @brief  Configures the voltage threshold detected by the Power Battery Voltage Detector(PVBD).
  * @param  POWER_PVBDlevel - @ref PWR_Batt_Voltage_Detector_Lvl - specifies the PVBD detection level.
  * @retval None
  */
void POWER_PVBDlevelConfig(PWR_Batt_Voltage_Detector_Lvl POWER_PVBDlevel)
{
    uint32_t tmpreg;

    /* Check the parameters */
    assert_param(IS_POWER_PVBD_LEVEL(POWER_PVBDlevel));

    tmpreg = MDR_POWER->PVDCS;

    /* Clear PBLS[2:1] bits */
    tmpreg &= (uint32_t) ~POWER_PVDCS_PBLS_Msk;
    /* Set PBLS[2:1] bits according to POWER_PVBDlevel value */
    tmpreg |= ((uint32_t)POWER_PVBDlevel & POWER_PVDCS_PBLS_Msk);

    /* Store the new value */
    MDR_POWER->PVDCS = tmpreg;
}

/**
  * @brief  Checks whether the specified POWER detection flag is set or not.
  * @param  POWER_FLAG - @ref PWR_Voltage_Detector_Flags - specifies the flag to check.
  * @retval The state of POWER_FLAG (SET or RESET).
  */
FlagStatus POWER_GetFlagStatus(PWR_Voltage_Detector_Flags POWER_FLAG)
{
    FlagStatus status;

    /* Check the parameters */
    assert_param(IS_POWER_FLAG(POWER_FLAG));

    if ((MDR_POWER->PVDCS & (uint32_t)POWER_FLAG) != (uint32_t)RESET)
    {
        status = SET;
    }
    else
    {
        status = RESET;
    }

    /* Return the flag status */
    return status;
}

/**
  * @brief  Clears the POWER's pending flags.
  * @param  POWER_FLAG: specifies the flag to clear.
  *         This parameter can be any combination of the @ref PWR_Voltage_Detector_Flags values.
  * @retval None
  */
void POWER_ClearFlag(uint32_t POWER_FLAG)
{
    /* Check the parameters */
    assert_param(IS_POWER_FLAGS(POWER_FLAG));

    MDR_POWER->PVDCS &= (uint32_t) ~POWER_FLAG;
}

/**
  * @brief  Enables or disables the specified POWER interrupts.
  * @param  POWER_IT: specifies the PVD interrupts sources to be enabled or disabled.
  *         This parameter can be any combination of the @ref PWR_Voltage_Detector_ITEnable values.
  * @param  NewState - @ref FunctionalState - new state of the specified POWER interrupts.
  * @retval None
  */
void POWER_PVD_ITconfig(uint32_t POWER_IT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_POWER_PVD_IT(POWER_IT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        MDR_POWER->PVDCS |= POWER_IT;
    }
    else
    {
        MDR_POWER->PVDCS &= (uint32_t)~POWER_IT;
    }
}

/**
  * @brief  Enables or disables inversion of the specified POWER voltage detection flags
  * @param  POWER_INV: specifies the PVD interrupts sources to be enabled or disabled.
  *         This parameter can be any combination of the @ref PWR_Voltage_Detector_IT_Inversion values.
  * @param  NewState - @ref FunctionalState - new state of the specified POWER voltage flag inversion.
  * @retval None
  */
void POWER_PVD_INVconfig(uint32_t POWER_INV, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_POWER_PVD_INV(POWER_INV));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        MDR_POWER->PVDCS |= POWER_INV;
    }
    else
    {
        MDR_POWER->PVDCS &= (uint32_t)~POWER_INV;
    }
}

/** @} */ /* End of group POWER_Private_Functions */

/** @} */ /* End of group POWER */

/** @} */ /* End of group __MDR32Fx_StdPeriph_Driver */

/*********************** (C) COPYRIGHT 2021 Milandr ****************************
*
* END OF FILE MDR32F9Qx_power.c */

