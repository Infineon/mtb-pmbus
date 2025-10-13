/***************************************************************************//**
* \file mtb_pmbus_cmd_conf.h
* \version 1.0
*
* Pre-implemented commands info
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

#ifndef MTB_PMBUS_CMD_CONF_H
#define MTB_PMBUS_CMD_CONF_H

#include "mtb_pmbus.h"

#if (defined (MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U))
    #define MTB_PMBUS_IMPL_CMD_REVISION_CODE    (0x98U)
    #define MTB_PMBUS_IMPL_CMD_REVISION_CAP     (MTB_PMBUS_CMD_CAP_DIR_RD)
    #define MTB_PMBUS_IMPL_CMD_REVISION_SIZE    (1U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U))
    #define MTB_PMBUS_IMPL_CMD_CAPABILITY_CODE  (0x19U)
    #define MTB_PMBUS_IMPL_CMD_CAPABILITY_CAP   (MTB_PMBUS_CMD_CAP_DIR_RD)
    #define MTB_PMBUS_IMPL_CMD_CAPABILITY_SIZE  (1U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U))
    #define MTB_PMBUS_IMPL_CMD_QUERY_CODE       (0x1AU)
    #define MTB_PMBUS_IMPL_CMD_QUERY_CAP        (MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL | MTB_PMBUS_CMD_CAP_BLOCK)
    #define MTB_PMBUS_IMPL_CMD_QUERY_SIZE       (2U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U))
    #define MTB_PMBUS_IMPL_CMD_PAGE_CODE        (0x00U)
    #define MTB_PMBUS_IMPL_CMD_PAGE_CAP         (MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_DIR_WR)
    #define MTB_PMBUS_IMPL_CMD_PAGE_SIZE        (1U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U))
    #define MTB_PMBUS_IMPL_CMD_PHASE_CODE       (0x04U)
    #define MTB_PMBUS_IMPL_CMD_PHASE_CAP        (MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_DIR_WR)
    #define MTB_PMBUS_IMPL_CMD_PHASE_SIZE       (1U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U))
    #define MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_CODE (0x07U)
    #define MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_CAP  (MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD)
    #define MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_SIZE (2U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U))
    #define MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_CODE (0x08U)
    #define MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_CAP  (MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD)
    #define MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_SIZE (2U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U)) */

#endif /* MTB_PMBUS_CMD_CONF_H */
