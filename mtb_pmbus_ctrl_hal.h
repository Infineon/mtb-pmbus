/***************************************************************************//**
* \file mtb_pmbus_ctrl_hal.h
* \version 1.0
*
* Provides the HAL layer definition for PMBus Controller.
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

#ifndef MTB_PMBUS_CTRL_HAL_H
#define MTB_PMBUS_CTRL_HAL_H

#include "cy_pdl.h"
#include "mtb_pmbus_conf_def.h"
#include "mtb_pmbus_hal.h"
#include "cy_result.h"

/**
 * \addtogroup group_pmbus_ctrl_hal_apis
 * \{
 */

/** HAL Configuration structure for Controller */
typedef struct
{
    /** The pointer to an SCB object */
    CySCB_Type *hw_ptr;
    /** The pointer to an I2C driver context structure */
    cy_stc_scb_i2c_context_t *pdl_i2c_context;
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    /** The pointer to a TCPWM object
     * \note This field is only available when \ref MTB_PMBUS_ENABLE_TIMEOUT and \ref
     * MTB_PMBUS_HAL_USE_TCPWM are enabled at the compile time options.
     */
    TCPWM_Type *timeout_tcpwm_base;
    /** The number of TCPWM counter used for the timeout detection.
     * \note This field is only available when \ref MTB_PMBUS_ENABLE_TIMEOUT and \ref
     * MTB_PMBUS_HAL_USE_TCPWM are enabled at the compile time options.
     */
    uint32_t timeout_tcpwm_cntnum;
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
    /** \cond INTERNAL */
    cy_stc_scb_i2c_master_xfer_config_t transfer_cfg;
#if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U))
    uint8_t notify_data[MTB_PMBUS_HOST_NOTIFY_DATA_SIZE];
#endif /* #if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U)) */
    /** \endcond */
} mtb_pmbus_ctrl_stc_config_hal_t;

/** \} group_pmbus_ctrl_hal_apis */
#endif /* MTB_PMBUS_CTRL_HAL_H */
