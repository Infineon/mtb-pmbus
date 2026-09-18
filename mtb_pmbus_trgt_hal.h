/***************************************************************************//**
* \file mtb_pmbus_trgt_hal.h
* \version 1.0
*
* Provides the HAL layer definition for the PMBus Target.
*
********************************************************************************
* \copyright
* (c) (2026), Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG.  All rights reserved.
********************************************************************************
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#ifndef MTB_PMBUS_TRGT_HAL_H
#define MTB_PMBUS_TRGT_HAL_H

#include "cy_pdl.h"

#include "mtb_pmbus_conf_def.h"
#include "mtb_pmbus_hal.h"
#include "cy_result.h"


#define MTB_PMBUS_HAL_WRITE_BUF_SIZE           (2U)


/**
 * \addtogroup group_pmbus_trgt_hal_apis
 * \{
 */

/** Wrapper over Correct status, No error */
#define MTB_PMBUS_HAL_STATUS_SUCCESS            CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 0U)
/** Wrapper over status provided command code is incorrect */
#define MTB_PMBUS_HAL_STATUS_INVALID_CMD_CODE   CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 1U)
/** Wrapper over status provided page is incorrect */
#define MTB_PMBUS_HAL_STATUS_INVALID_PAGE       CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 2U)
/** Wrapper over status provided phase is incorrect */
#define MTB_PMBUS_HAL_STATUS_INVALID_PHASE      CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 3U)
/** Wrapper over status one or more of input parameters are invalid */
#define MTB_PMBUS_HAL_STATUS_BAD_PARAM          CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 4U)
/** Wrapper over status requested size is bigger than command size */
#define MTB_PMBUS_HAL_STATUS_TOO_MANY_BYTES     CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 5U)
/** Wrapper over status requested action is aborted for the command as this command takes part in
   communication */
#define MTB_PMBUS_HAL_STATUS_CMD_IS_ACTIVE      CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 6U)
/** Wrapper over status requested action is aborted as the PMBus instance takes part in
   communication */
#define MTB_PMBUS_HAL_STATUS_IS_BUSY            CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 7U)
#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
    defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U)) || defined(MTB_PMBUS_DOXYGEN)
/** Wrapper over status one or more mandatory Security Level 0 commands are missing from the
   command table */
#define MTB_PMBUS_HAL_STATUS_L0_CMDS_MISSING    CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 8U)
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */

/** HAL Configuration structure */
typedef struct
{
    /** The pointer to an SCB */
    CySCB_Type *hw_ptr;
    /** The pointer to an I2C driver context structure */
    cy_stc_scb_i2c_context_t *pdl_i2c_context;
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    /** The pointer to TCPWM object
     * \note This field is only available when \ref MTB_PMBUS_ENABLE_TIMEOUT and \ref
     * MTB_PMBUS_HAL_USE_TCPWM are enabled at compile time options.
     */
    TCPWM_Type *timeout_tcpwm_base;
    /** The number of TCPWM counter used for timeout detection
     * \note This field is only available when \ref MTB_PMBUS_ENABLE_TIMEOUT and \ref
     * MTB_PMBUS_HAL_USE_TCPWM are enabled at compile time options.
     */
    uint32_t timeout_tcpwm_cntnum;
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
#if (defined (MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U))
    /** The pointer to GPIO port for SMBALERT signal
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile
     * time options.
     */
    GPIO_PRT_Type *smbalert_port_addr;
    /** The pin number of GPIO port for SMBALERT signal
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile
     * time options.
     */
    uint32_t smbalert_pin_num;
#endif /* #if (defined (MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) */
    /** The read buffer for Controller */
    uint8_t *hal_read_buf_ptr;
    /** The size of read buffer */
    uint16_t hal_read_buf_size;
    /** The write buffer for Controller */
    uint8_t hal_write_buf[MTB_PMBUS_HAL_WRITE_BUF_SIZE];
#if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U))
    uint8_t notify_data[MTB_PMBUS_HOST_NOTIFY_DATA_SIZE];
#endif /* #if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) */
} mtb_pmbus_stc_config_hal_t;

/** \} group_pmbus_trgt_hal_apis */
#endif /* MTB_PMBUS_TRGT_HAL_H */
