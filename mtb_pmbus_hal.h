/***************************************************************************//**
* \file mtb_pmbus_hal.h
* \version 1.0
*
* Provides the HAL layer definition.
*
********************************************************************************
* \copyright
* (c) (2025), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation. All rights reserved.
********************************************************************************
* This software, including source code, documentation and related materials
* ("Software") is owned by Cypress Semiconductor Corporation or one of its
* affiliates ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/

#ifndef MTB_PMBUS_HAL_H
#define MTB_PMBUS_HAL_H

#include "cy_pdl.h"

#include "mtb_pmbus_conf_def.h"
#include "cy_result.h"


#define MTB_PMBUS_HAL_WRITE_BUF_SIZE           (2U)


/**
* \addtogroup group_pmbus_hal_apis
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
/** Wrapper over status requested action is aborted for the command as this command takes part in communication */
#define MTB_PMBUS_HAL_STATUS_CMD_IS_ACTIVE      CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 6U)
/** Wrapper over status requested action is aborted as the PMBus instance takes part in communication */
#define MTB_PMBUS_HAL_STATUS_IS_BUSY            CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 7U)

#if defined (COMPONENT_PSC3) || defined (MTB_PMBUS_DOXYGEN)
#if !defined (MTB_PMBUS_HAL_USE_TCPWM) || defined (MTB_PMBUS_DOXYGEN)
    /** Use one of the TCPWM counters for the timeout detection */
    #define MTB_PMBUS_HAL_USE_TCPWM             (1U)
#endif /* #ifndef (MTB_PMBUS_HAL_USE_TCPWM) */
#endif /* #if defined (COMPONENT_PSC3))*/

/** HAL Configuration structure */
typedef struct 
{
    /** The pointer to SCB */
    CySCB_Type * hw_ptr;
    /** The pointer to I2C driver context structure */
    cy_stc_scb_i2c_context_t * pdl_i2c_context;
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    /** The pointer to TCPWM object
     * \note This field is only available when \ref MTB_PMBUS_ENABLE_TIMEOUT and \ref MTB_PMBUS_HAL_USE_TCPWM are enabled at compile time options.
     */
    TCPWM_Type * timeout_tcpwm_base;
    /** The number of TCPWM counter used for timeout detection
     * \note This field is only available when \ref MTB_PMBUS_ENABLE_TIMEOUT and \ref MTB_PMBUS_HAL_USE_TCPWM are enabled at compile time options.
     */
    uint32_t timeout_tcpwm_cntnum;
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
#if (defined (MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U))
    /** The pointer to GPIO port for SMBALERT signal 
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile time options.
     */
    GPIO_PRT_Type * smbalert_port_addr;
    /** The pin number of GPIO port for SMBALERT signal
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile time options.
     */
    uint32_t smbalert_pin_num;
#endif /* #if (defined (MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) */
    /** The read buffer for Controller */
    uint8_t * hal_read_buf_ptr;
    /** The size of read buffer */
    uint16_t hal_read_buf_size;
    /** The write buffer for Controller */
    uint8_t hal_write_buf[MTB_PMBUS_HAL_WRITE_BUF_SIZE];
} mtb_pmbus_stc_config_hal_t;

/** \} group_pmbus_hal_apis */

#endif /* MTB_PMBUS_HAL_H */
