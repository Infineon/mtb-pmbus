/***************************************************************************//**
* \file mtb_pmbus_trgt_int.h
* \version 1.0
*
* Provides internal definition for PMBUS Target.
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

#ifndef MTB_PMBUS_TRGT_INT_H
#define MTB_PMBUS_TRGT_INT_H

#include "mtb_pmbus.h"
#include "mtb_pmbus_trgt_cmd_conf.h"
#include <string.h>

/* The Target Address is matched in the first time */
#define MTB_PMBUS_STATE_TARGET_ADDR_MATCH_1     (0x01U)
/* The Target Address is matched in the second time. The transfer direction is changed */
#define MTB_PMBUS_STATE_TARGET_ADDR_MATCH_2     (0x02U)
/* The Received byte protocol is initiated by Controller */
#define MTB_PMBUS_STATE_RECEIVED_BYTE_PROT      (0x04U)
/* The received command is matched */
#define MTB_PMBUS_STATE_CMD_MATCH               (0x08U)
#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
/* The received command is extended command */
#define MTB_PMUBS_STATE_CMD_MATCH_EXT           (0x10U)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
/* The Target Address has not matched before repeated start. Only for SMBus mode */
#define MTB_PMBUS_STATE_TARGET_ADDR_NOT_MATCH   (0x20U)
/* Write direction */
#define MTB_PMBUS_STATE_DIR_WR                  (0x40U)
/* Read direction */
#define MTB_PMBUS_STATE_DIR_RD                  (0x80U)
/* The count byte is received */
#define MTB_PMBUS_STATE_GOT_BYTE_COUNT          (0x100U)
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
/* The PEC byte is sent or received */
#define MTB_PMBUS_STATE_PEC_IS_EXECUTED         (0x200U)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
/* Implemented command is received */
#define MTB_PMBUS_STATE_IMPL_CMD                (0x400U)
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
#if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)
/* The Alert Response Address is received */
#define MTB_PMBUS_STATE_ALERT_RESP_ADDR         (0x800U)
#endif /* #if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
/* Zone Write */
#define MTB_PMBUS_STATE_ZONE_WRITE              (0x1000U)
/* Zone Read */
#define MTB_PMBUS_STATE_ZONE_READ               (0x2000U)
/* Zone Read Status */
#define MTB_PMBUS_STATE_ZONE_READ_STATUS        (0x4000U)
/* Zone Read Command */
#define MTB_PMBUS_STATE_ZONE_READ_CMD           (0x8000U)
/* Zone Response type */
#define MTB_PMBUS_STATE_ZONE_RESP_CONTINUE      (0x10000U)
/* Zone Read Retry Response after arbitration lost */
#define MTB_PMBUS_STATE_ZONE_RETRY_RESP         (0x20000U)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
#if defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)
/* The General Call address is received */
#define MTB_PMBUS_STATE_GENERAL_CALL_ADDR       (0x40000U)
#endif /* #if defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U) */
/* The command should be ignored */
#define MTB_PMBUS_CMD_IS_IGNORED                (0x80000U)
/* The PAGE_PLUS command is received */
#define MTB_PMBUS_STATE_PAGE_PLUS_CMD           (0x100000U)
/* The P2_PLUS command is received */
#define MTB_PMBUS_STATE_P2_PLUS_CMD             (0x200000U)
#if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U))
/* The Host Notify transaction is started */
#define MTB_PMBUS_STATE_HOST_NOTIFY             (0x400000U)
#endif /* #if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) */

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
#define MTB_PMBUS_CMD_CODE_EXT                  (0xFEU)
#define MTB_PMBUS_CMD_EXT_POS                   (8U)
#define MTB_PMBUS_CMD_EXT_MASK                  (0x000000FFU)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */

#define MTB_PMBUS_CMD_BLOCK_EXTRA_BYTE          (1U)
/* The value for extra bytes requested by Controller */
#define MTB_PMBUS_EXTRA_BYTE_VAL                (0xFFU)

#define MTB_PMBUS_CMD_FLAG_IS_PRESENT           (0x1U)
#define MTB_PMBUS_CMD_FLAG_IS_ENABLED           (0x2U)
#define MTB_PMBUS_CMD_FLAG_IS_TRANSMITTED       (0x4U)
/* The command is protected against write operation */
#define MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED      (0x8U)
#define MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE        (0x10U)
/* The process call response is paused */
#define MTB_PMBUS_CMD_FLAG_IS_PAUSED            (0x20U)

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
/* Value for 1.5 PMBus revision */
#define MTB_PMBUS_REV_1_5_BITS                  (0x6U)
/* Shift between revision of PMBus Spec Part 1 and Part 2 */
#define MTB_PMBUS_REV_SHIFT                     (4U)
/* The 1.4 PMBus revision returned by REVISION command */
#define MTB_PMBUS_REV_1_4                       \
    (MTB_PMBUS_REV_1_4_BITS | (MTB_PMBUS_REV_1_4_BITS << MTB_PMBUS_REV_SHIFT))
/* The 1.5 PMBus revision returned by REVISION command */
#define MTB_PMBUS_REV_1_5                       \
    (MTB_PMBUS_REV_1_5_BITS | (MTB_PMBUS_REV_1_5_BITS << MTB_PMBUS_REV_SHIFT))
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

#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
void mtb_pmbus_handle_timeout(mtb_pmbus_stc_t *inst);
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */

#if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U))
bool mtb_pmbus_handle_gen_call(mtb_pmbus_stc_t *inst);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)) */
bool mtb_pmbus_handle_addr(mtb_pmbus_stc_t *inst, uint8_t addr);
bool mtb_pmbus_handle_rx(mtb_pmbus_stc_t *inst, uint8_t byte);
void mtb_pmbus_handle_tx(mtb_pmbus_stc_t *inst, uint8_t **data_ptr, uint16_t *size);
void mtb_pmbus_handle_restart(mtb_pmbus_stc_t *inst);
void mtb_pmbus_handle_stop(mtb_pmbus_stc_t *inst);
void mtb_pmbus_handle_arb_lost(mtb_pmbus_stc_t *inst);
void mtb_pmbus_handle_bus_error(mtb_pmbus_stc_t *inst);

void mtb_pmbus_int_cmd_handle_tx(mtb_pmbus_stc_t *inst);

void mtb_pmbus_hal_init(mtb_pmbus_stc_t *inst);
#if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined (MTB_PMBUS_DOXYGEN)
void mtb_pmbus_hal_smbalert_set(mtb_pmbus_stc_t *inst);
void mtb_pmbus_hal_smbalert_clear(mtb_pmbus_stc_t *inst);
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined
       (MTB_PMBUS_DOXYGEN) */
bool mtb_pmbus_hal_is_tx_cmpl(mtb_pmbus_stc_t *inst);
#if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U)
bool mtb_pmbus_hal_check_hw_cfg(const mtb_pmbus_stc_config_t *cfg);
#endif /* #if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U) */
#if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U))
void mtb_pmbus_handle_hnp(mtb_pmbus_stc_t *inst, mtb_pmbus_host_notify_events_t event);
bool mtb_pmbus_hal_hnp_write(mtb_pmbus_stc_t *inst, uint8_t target_addr, uint32_t data);
void mtb_pmbus_hal_hnp_abort(mtb_pmbus_stc_t *inst);
#endif /* #if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) */
bool mtb_pmbus_hal_pause_tx(mtb_pmbus_stc_t *inst);
bool mtb_pmbus_hal_resume_tx(mtb_pmbus_stc_t *inst);

#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
    defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
/** Returns true for the Protect Locks group: WRITE_PROTECT, ACCESS_CONTROL, and PASSKEY.
 *  These commands must always remain writable regardless of WRITE_PROTECT level or
 *  ACCESS_CONTROL settings so that a device can always unlock itself. */
__STATIC_INLINE bool mtb_pmbus_int_is_wp_exempt(uint8_t cmd_code)
{
    return ((cmd_code == MTB_PMBUS_WRITE_PROTECT_CMD_CODE) ||
            (cmd_code == MTB_PMBUS_ACCESS_CONTROL_CMD_CODE) ||
            (cmd_code == MTB_PMBUS_PASSKEY_CMD_CODE));
}
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */
#endif /* MTB_PMBUS_TRGT_INT_H */
