/***************************************************************************//**
* \file mtb_pmbus_hal.h
* \version 1.0
*
* Provides the common part of the HAL layer definition
* for the Target and the Controller.
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

#ifndef MTB_PMBUS_HAL_H
#define MTB_PMBUS_HAL_H

#include "cy_pdl.h"
#include "mtb_pmbus_conf_def.h"
#include "cy_result.h"

#define MTB_PMBUS_HOST_NOTIFY_DATA_SIZE        (3U)

/**
 * \addtogroup group_pmbus_common_hal_apis
 * \{
 */

#if (defined (CY_IP_MXSCB_VERSION) && (CY_IP_MXSCB_VERSION >= 4U) && \
    (CY_IP_MXSCB_VERSION_MINOR >= 4U)) || defined (MTB_PMBUS_DOXYGEN)
#if !defined (MTB_PMBUS_HAL_USE_TGS) || defined (MTB_PMBUS_DOXYGEN)
/** Use TGS HW features of SCB block for timeout detection
 *
 * This macro is enabled by default when the SCB supports a TGS counter.
 * The PMBus Solution Personality generates this macro automatically. For manual
 * configuration, to use a TCPWM counter instead, redefine this macro to (0U)
 * and define \ref MTB_PMBUS_HAL_USE_TCPWM as (1U) in the mtb_pmbus_conf.h file.
 */
    #define MTB_PMBUS_HAL_USE_TGS               (1U)
#endif /* #ifndef (MTB_PMBUS_HAL_USE_TGS) */
/**
 * The number of TGS timer used for SMBus timeout handling by PMBus Middleware.
 */
    #define MTB_PMBUS_TGS_TIMER_NUM             (0U)
#endif /* #if (defined (CY_IP_MXSCB_VERSION) && (CY_IP_MXSCB_VERSION >= 4U) &&
              (CY_IP_MXSCB_VERSION_MINOR >= 4U)) || defined (MTB_PMBUS_DOXYGEN) */

#if !(defined (CY_IP_MXSCB_VERSION) && (CY_IP_MXSCB_VERSION >= 4U) && \
    (CY_IP_MXSCB_VERSION_MINOR >= 4U)) || defined (MTB_PMBUS_DOXYGEN)
#if !defined (MTB_PMBUS_HAL_USE_TCPWM) || defined (MTB_PMBUS_DOXYGEN)
/** Use one of the TCPWM counters for the timeout detection
 *
 * This macro is enabled by default when the SCB does not support a TGS counter.
 * The PMBus Solution Personality generates this macro automatically. For manual
 * configuration, when enabling this macro on an SCB that supports a TGS counter,
 * redefine \ref MTB_PMBUS_HAL_USE_TGS as (0U) in the mtb_pmbus_conf.h file.
 */
    #define MTB_PMBUS_HAL_USE_TCPWM             (1U)
#endif /* #ifndef (MTB_PMBUS_HAL_USE_TCPWM) */
#endif /* #if !(defined (CY_IP_MXSCB_VERSION) && (CY_IP_MXSCB_VERSION >= 4U) &&
               (CY_IP_MXSCB_VERSION_MINOR >= 4U)) || defined (MTB_PMBUS_DOXYGEN) */

/** \} group_pmbus_common_hal_apis */
#endif /* MTB_PMBUS_HAL_H */
