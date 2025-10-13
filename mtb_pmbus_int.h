/***************************************************************************//**
* \file mtb_pmbus_int.h
* \version 1.0
*
* Provides internal definition for PMBUS.
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

#ifndef MTB_PMBUS_INT_H
#define MTB_PMBUS_INT_H

#include "mtb_pmbus.h"
#include "mtb_pmbus_cmd_conf.h"
#include <string.h>


#define MTB_PMBUS_CMD_BLOCK_EXTRA_BYTE          (1U)
/* The value for extra bytes requested by Controller */
#define MTB_PMBUS_EXTRA_BYTE_VAL                (0xFFU)

#define MTB_PMBUS_CMD_FLAG_IS_PRESENT           (0x1U)
#define MTB_PMBUS_CMD_FLAG_IS_ENABLED           (0x2U)
#define MTB_PMBUS_CMD_FLAG_IS_TRANSMITTED       (0x4U)
/* The command is protected against write operation */
#define MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED      (0x8U)
#define MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE        (0x10U)

/* The maximum number of pages */
#define MTB_PMBUS_PAGE_MAX_NUM                  (0x20U)
#define MTB_PMBUS_PAGE_ALL                      (0xFFU)
/* The maximum number of phases */
#define MTB_PMBUS_PHASE_MAX_NUM                 (0x80U)
#define MTB_PMBUS_PHASE_ALL                     (0xFFU)

/* All Zones */
#define MTB_PMBUS_ZONE_ALL                      (0xFFU)
/* No Zones */
#define MTB_PMBUS_ZONE_NONE                     (0xFEU)

/* Zone Status Bits */
#define MTB_PMBUS_ZONE_AR_MASK                  (0x80U)
#define MTB_PMBUS_ZONE_AR_POS                   (7U)
#define MTB_PMBUS_ZONE_ST_MASK                  (0x40U)
#define MTB_PMBUS_ZONE_ST_POS                   (6U)
#define MTB_PMBUS_ZONE_DI_MASK                  (0x20U)
#define MTB_PMBUS_ZONE_DI_POS                   (5U)
#define MTB_PMBUS_ZONE_DS_MASK                  (0x10U)
#define MTB_PMBUS_ZONE_DS_POS                   (4U)

/* Mask for received address - clear R/W bit */
#define MTB_PMBUS_ADDR_MASK                     (0xFEU)
#if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)
/* Alert Response Address (0x0C) with one shifted bit of W/R */
#define MTB_PMBUS_ALERT_RESPONSE_ADDR           (0x18U)
#endif /* #if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
/* Zone Read Address (0x28) with one shifted bit of W/R */
#define MTB_PMBUS_ZONE_READ_ADDR                (0x50U)
/* Zone Write Address (0x37) with one shifted bit of W/R */
#define MTB_PMBUS_ZONE_WRITE_ADDR               (0x6EU)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

#if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)
/* Value for 1.4 PMBus revision */
#define MTB_PMBUS_REV_1_4_BITS                  (0x5U)
/* Shift between revision of PMBus Spec Part 1 and Part 2 */
#define MTB_PMBUS_REV_SHIFT                     (4U)
/* The 1.4 PMBus revision returned by REVISION command */
#define MTB_PMBUS_REV_1_4                       (MTB_PMBUS_REV_1_4_BITS | (MTB_PMBUS_REV_1_4_BITS << MTB_PMBUS_REV_SHIFT))
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)
#define MTB_PMBUS_CAPABILITY_PEC_POS            (7U)
#define MTB_PMBUS_CAPABILITY_SPEED_POS          (5U) /* 2 bits wide */
#define MTB_PMBUS_CAPABILITY_SMBALERT_POS       (4U)
#define MTB_PMBUS_CAPABILITY_FORMAT_POS         (3U)
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U)
#define MTB_PMBUS_QUERY_CMD_SUP_POS             (7U)
#define MTB_PMBUS_QUERY_CMD_SUP_WR_POS          (6U)
#define MTB_PMBUS_QUERY_CMD_SUP_RD_POS          (5U)
#define MTB_PMBUS_QUERY_CMD_FORMAT_POS          (2U) /* 3 bits wide */
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U) */

#if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U))
bool mtb_pmbus_handle_gen_call(mtb_pmbus_stc_t * inst);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)) */
bool mtb_pmbus_handle_addr(mtb_pmbus_stc_t * inst, uint8_t addr);
bool mtb_pmbus_handle_rx(mtb_pmbus_stc_t * inst, uint8_t byte);
void mtb_pmbus_handle_tx(mtb_pmbus_stc_t * inst, uint8_t ** data_ptr, uint16_t * size);
void mtb_pmbus_handle_restart(mtb_pmbus_stc_t * inst);
void mtb_pmbus_handle_stop(mtb_pmbus_stc_t * inst);
void mtb_pmbus_handle_arb_lost(mtb_pmbus_stc_t * inst);
void mtb_pmbus_handle_bus_error(mtb_pmbus_stc_t * inst);

void mtb_pmbus_hal_init(mtb_pmbus_stc_t * inst);
#if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined (MTB_PMBUS_DOXYGEN)
void mtb_pmbus_hal_smbalert_set(mtb_pmbus_stc_t * inst);
void mtb_pmbus_hal_smbalert_clear(mtb_pmbus_stc_t * inst);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
bool mtb_pmbus_hal_is_tx_cmpl(mtb_pmbus_stc_t * inst);
#if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U)
bool mtb_pmbus_hal_check_hw_cfg(const mtb_pmbus_stc_config_t * cfg);
#endif /* #if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U) */

#endif /* MTB_PMBUS_INT_H */
