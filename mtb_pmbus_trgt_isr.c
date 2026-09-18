/***************************************************************************//**
* \file mtb_pmbus_trgt_isr.c
* \version 1.0
*
* Provides ISR handler for the PMBus Target Middleware.
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

#include "mtb_pmbus_trgt_int.h"


/* The size of received byte protocol (Size without PEC) */
#define MTB_PMBUS_RECEIVED_BYTE_SIZE            (1U)
/* The size of response on Alert Address (Size without PEC) */
#define MTB_PMBUS_ALERT_ADDR_RESPONSE_SIZE      (1U)

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
/* Position for the status field in status response for the PMBus zone read protocol */
#define MTB_PMBUS_ZONE_READ_STS_POS             (0U)
/* Position for the address field in status response for the PMBus zone read protocol */
#define MTB_PMBUS_ZONE_READ_ADDR_POS            (1U)
/* Position for the page field in status response for the PMBus zone read protocol */
#define MTB_PMBUS_ZONE_READ_PAGE_POS            (2U)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

/* Size (in bytes) of the status response for the PMBus zone read command */
#define MTB_PMBUS_ZONE_READ_STATUS_RESP_SIZE    (3U)

#if defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)
/* The General Call address */
#define MTB_PMBUS_GENERAL_CALL_ADDR             (0U)
#endif /* #if defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U) */

/* The minimum size of the PAGE_PLUS command block count */
#define MTB_PMBUS_PAGE_PLUS_MIN_BLOCK_SIZE      (2U)
/* The size of PAGE data in PAGE_PLUS command */
#define MTB_PMBUS_PAGE_PLUS_SUP_DATA_SIZE       (1U)
/* The minimum size of the P2_PLUS command block count */
#define MTB_PMBUS_P2_PLUS_MIN_BLOCK_SIZE        (3U)
/* The size of PAGE and PHASE data in P2_PLUS command */
#define MTB_PMBUS_P2_PLUS_SUP_DATA_SIZE         (2U)

#define MTB_PMBUS_ACK_BIT                       (true)
#define MTB_PMBUS_NACK_BIT                      (false)

#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
__STATIC_INLINE void mtb_pmbus_int_crc_calc(mtb_pmbus_stc_t *inst, uint8_t byte);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */

static void mtb_pmbus_int_update_data(mtb_pmbus_stc_t *inst);

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
static void mtb_pmbus_int_reverse_byte_order(uint8_t *array, uint32_t length);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
static bool mtb_pmbus_int_impl_cmd_handler(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)
static bool mtb_pmbus_int_impl_cmd_rev(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)
static bool mtb_pmbus_int_impl_cmd_capability(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U)
static bool mtb_pmbus_int_impl_cmd_query(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U)
static bool mtb_pmbus_int_impl_cmd_page(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U)
static bool mtb_pmbus_int_impl_cmd_phase(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U)
static bool mtb_pmbus_int_impl_cmd_zone_cfg(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U)
static bool mtb_pmbus_int_impl_cmd_zone_act(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE != 0U)
static bool mtb_pmbus_int_impl_cmd_page_plus_write(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ != 0U)
static bool mtb_pmbus_int_impl_cmd_page_plus_read(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE != 0U)
static bool mtb_pmbus_int_impl_cmd_p2_plus_write(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ != 0U)
static bool mtb_pmbus_int_impl_cmd_p2_plus_read(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ != 0U) */
#if (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U)
static bool mtb_pmbus_int_handle_p_plus_cmd(mtb_pmbus_stc_t *inst);
#endif /* #if (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U) */

void mtb_pmbus_int_reset_state(mtb_pmbus_stc_t *inst);

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
static uint8_t mtb_pmbus_int_get_num_available_zone(mtb_pmbus_stc_t *inst);
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
static uint8_t mtb_pmbus_int_get_next_page2actzone(mtb_pmbus_stc_t *inst);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */


#if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U))
bool mtb_pmbus_handle_gen_call(mtb_pmbus_stc_t *inst)
{
    bool status = MTB_PMBUS_NACK_BIT;

    if (inst->cfg->enable_gen_call_addr)
    {
        MTB_PMBUS_LOG_DBG("Gen Call Addr");
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
        mtb_pmbus_int_crc_calc(inst, MTB_PMBUS_GENERAL_CALL_ADDR);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
        inst->state |= MTB_PMBUS_STATE_GENERAL_CALL_ADDR;
        inst->state |= MTB_PMBUS_STATE_DIR_WR;

        /* Set default cmd and lookup tables */
        inst->active_cmd_tbl = inst->cfg->cmd_table;
        inst->active_lookup_tbl = inst->cmd_lookup_tbl;
        status = MTB_PMBUS_ACK_BIT;
    }
    return status;
}
#endif /* #if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)) */

bool mtb_pmbus_handle_addr(mtb_pmbus_stc_t *inst, uint8_t addr)
{
    MTB_PMBUS_LOG_DBG("Addr [%x]", addr);
    bool status = MTB_PMBUS_ACK_BIT;
    bool dir = (bool)(addr & (~MTB_PMBUS_ADDR_MASK));

    /* Check the address */
    if (((addr & (uint8_t)MTB_PMBUS_ADDR_MASK) == (inst->cfg->address << 1U)) &&
        ((inst->state & MTB_PMBUS_STATE_TARGET_ADDR_NOT_MATCH) == 0U))
    {
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
        mtb_pmbus_int_crc_calc(inst, addr);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
        if (inst->errors == 0U)
        {
            if ((inst->state & MTB_PMBUS_STATE_TARGET_ADDR_MATCH_1) != 0U)
            {
                MTB_PMBUS_LOG_DBG("Target Addr in Second time");
                inst->state |= MTB_PMBUS_STATE_TARGET_ADDR_MATCH_2;

                /* Check transfer direction */
                if (dir)
                {
                    /* Update the direction in state machine */
                    inst->state |= MTB_PMBUS_STATE_DIR_RD;
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
                    /* The handler of the case when PEC was executed in the Process Call Write Word
                     * part
                     */
                    if (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) != 0U) &&
                        ((inst->state & MTB_PMBUS_STATE_PEC_IS_EXECUTED) != 0U))
                    {
                        inst->errors &= ~MTB_PMBUS_ERR_CORRUPTED_DATA;
                        inst->errors |= MTB_PMBUS_ERR_WR_TOO_MANY_BYTES;
                        MTB_PMBUS_LOG_WRN(
                            "Controller attempts to write too many bytes(Process Call with PEC in Write Word part) [%d]",
                            inst->byte_received + 1U);
                    }
                    else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
                    {
                        /* Store the received data in case of process call */
                        if ((((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) !=
                              0U) && (0U != inst->byte_received)))
                        {
                            inst->is_pause_allowed = true;
                            /* Check if all requested data is received */
                            if (inst->byte_received == inst->byte_requested)
                            {
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
                                if ((inst->state & MTB_PMBUS_STATE_IMPL_CMD) != 0U)
                                {
                                    (void)mtb_pmbus_int_impl_cmd_handler(inst, MTB_PMBUS_CMD_WRITE_DONE);
                                }
                                else
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
                                {
                                    mtb_pmbus_int_update_data(inst);
                                    /* Call the command callback */
                                    if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                                    {
                                        (void)inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_WRITE_DONE,
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                                                                           (int32_t)inst->act_page,
#else
                                                                                           MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                                                                                           (int32_t)inst->act_phase,
#else
                                                                                           MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U))
                                                                                           0U, 0U);
#else
                                                                                           0U);
#endif /* #if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U)) */
                                    }
#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
                                    defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
                                    /* ACL Write Once [2]: auto-lock after first successful write
                                     * (Process Call write phase) */
                                    if ((inst->errors == 0U) &&
                                        ((inst->acl_table[inst->cmd_code] & MTB_PMBUS_ACL_BIT_WRITE_ONCE) != 0U))
                                    {
                                        inst->acl_table[inst->cmd_code] |= MTB_PMBUS_ACL_BIT_WRITE_ACCESS;
                                        inst->active_lookup_tbl[inst->cmd_code].flags |=
                                            MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED;
                                        MTB_PMBUS_LOG_DBG("Write Once: cmd [%x] write-locked", inst->cmd_code);
                                    }
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */
                                }
                            }
                            else
                            {
                                inst->errors |= MTB_PMBUS_ERR_WR_LESS_BYTES;
                                MTB_PMBUS_LOG_WRN("Less bytes are received");
                            }
                            inst->is_pause_allowed = false;
                        }

                        if (!(
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) != 0U) &&
                                 (inst->act_page == MTB_PMBUS_PAGE_ALL))
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                                || (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) != 0U) &&
                                    (inst->act_phase == MTB_PMBUS_PHASE_ALL))
#endif
#elif (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                                (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) != 0U) &&
                                 (inst->act_phase == MTB_PMBUS_PHASE_ALL))
#else
                                0U != 0U
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                                ))
                        {
                            if (
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
                                (0U != (inst->state & MTB_PMBUS_STATE_IMPL_CMD)) ||
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
                                (0U == (inst->active_lookup_tbl[inst->cmd_code].flags & MTB_PMBUS_CMD_FLAG_IS_PAUSED)))
                            {
#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
                                defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
                                /* ACL read restriction (ACCESS_CONTROL bit [6]):
                                 * applied to all commands including pre-implemented ones */
                                if ((inst->acl_table[inst->cmd_code] & MTB_PMBUS_ACL_BIT_READ_ACCESS) != 0U)
                                {
                                    inst->errors |= MTB_PMBUS_ERR_RD_FROM_PROT_CMD;
                                    MTB_PMBUS_LOG_WRN("Cmd [%x] is ACL read-protected", inst->cmd_code);
                                    /* int_buff left unset — controller receives FFh */
                                }
                                else
                                {
                                    mtb_pmbus_int_cmd_handle_tx(inst);
                                }
#else
                                mtb_pmbus_int_cmd_handle_tx(inst);
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */
                            }
                        }
                        else
                        {
                            inst->errors |= MTB_PMBUS_ERR_READ_REQ_FOR_FF;
                            MTB_PMBUS_LOG_WRN("Read request for paged/phased command when page or phase is set to 0xFF");
                        }
                    }
                }
                else
                {
                    /* Error: After second address match only read direction is expected */
                    inst->errors |= MTB_PMBUS_ERR_WR_AFTER_SECOND_ADDR;
                    MTB_PMBUS_LOG_WRN("Only Read direction is expected when the Target Addr is received in second time");
                }
            }
            else /* The first occurrence of target address */
            {
                MTB_PMBUS_LOG_DBG("Target Addr in First time");

                inst->state |= MTB_PMBUS_STATE_TARGET_ADDR_MATCH_1;

                /* After first address match, the read direction is
                 * applicable only for received byte protocol or Quick command
                 */
                if (dir)
                {
#if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U))
                    if (inst->cfg->enable_pmbus)
                    {
                        inst->errors |= MTB_PMBUS_ERR_ADDR_WITH_RD_BIT;
                        MTB_PMBUS_LOG_WRN(
                            "The protocol can not be started with Target Address and RD direction. (Only for PMBus)");
                    }
                    else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */
                    {
                        MTB_PMBUS_LOG_DBG("Received byte protocol");
                        inst->state |= MTB_PMBUS_STATE_DIR_RD | MTB_PMBUS_STATE_RECEIVED_BYTE_PROT;

                        /* Call the User callback to update data before sending */
                        if (inst->cfg->gen_callback != NULL)
                        {
                            inst->cfg->gen_callback(MTB_PMBUS_RECEIVE_BYTE_EVENT);
                        }
                        inst->byte_to_send = MTB_PMBUS_RECEIVED_BYTE_SIZE;
                        inst->int_buff[0U] = inst->received_byte;
                    }
                }
                else
                {
                    inst->state |= MTB_PMBUS_STATE_DIR_WR;
                }

                /* Set default cmd and lookup tables */
                inst->active_cmd_tbl = inst->cfg->cmd_table;
                inst->active_lookup_tbl = inst->cmd_lookup_tbl;
            }
        }
    }
#if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)
    else if (((addr & MTB_PMBUS_ADDR_MASK) == MTB_PMBUS_ALERT_RESPONSE_ADDR) && inst->smbalert_is_trig &&
             inst->cfg->enable_smbalert)
    {
        MTB_PMBUS_LOG_DBG("Alert Response Addr");
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
        mtb_pmbus_int_crc_calc(inst, addr);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */

        inst->state |= MTB_PMBUS_STATE_ALERT_RESP_ADDR | MTB_PMBUS_STATE_DIR_RD;
        inst->byte_to_send = MTB_PMBUS_ALERT_ADDR_RESPONSE_SIZE;
        inst->int_buff[0U] = inst->cfg->address << 1U;
    }
#endif /* #if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
    else if ((addr == MTB_PMBUS_ZONE_WRITE_ADDR) && inst->cfg->enable_zone && inst->cfg->enable_pmbus)
    {
        /* The Zone write needs to be always processed despite the zone configuration
         * because the ZONE_ACTIVE command shall be processed for all Zone capable devices
         */
        MTB_PMBUS_LOG_DBG("Zone Write Addr");
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
        mtb_pmbus_int_crc_calc(inst, addr);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
        inst->state |= MTB_PMBUS_STATE_ZONE_WRITE | MTB_PMBUS_STATE_DIR_WR;

        /* Set default cmd and lookup tables */
        inst->active_cmd_tbl = inst->cfg->cmd_table;
        inst->active_lookup_tbl = inst->cmd_lookup_tbl;
    }
    else if (((addr & MTB_PMBUS_ADDR_MASK) == MTB_PMBUS_ZONE_READ_ADDR) && inst->cfg->enable_zone &&
             inst->cfg->enable_pmbus)
    {
        MTB_PMBUS_LOG_DBG("Zone Read Addr");

        if ((inst->state & MTB_PMBUS_STATE_ZONE_READ) != 0U)
        {
            if (!dir)
            {
                /* Error incorrect direction */
                MTB_PMBUS_LOG_WRN("Incorrect direction for Zone Read Addr");
            }
            else
            {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                if ((inst->state & MTB_PMBUS_STATE_ZONE_RETRY_RESP) == 0U)
                {
                    /* Set the next page for the active zone */
                    inst->act_page = mtb_pmbus_int_get_next_page2actzone(inst);
                    MTB_PMBUS_LOG_DBG("Set active page [%x] for zone [%x]", inst->act_page, inst->act_read_zone);
                }
                else
                {
                    /* Skip page update if arb lost occurred during previous response */
                    inst->state &= ~MTB_PMBUS_STATE_ZONE_RETRY_RESP;
                }
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                /* Change direction in state machine */
                inst->state |= MTB_PMBUS_STATE_DIR_RD;
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
                /* Reset PEC, as it is calculated only for second part of transfer */
                inst->crc = 0U;
                mtb_pmbus_int_crc_calc(inst, addr);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */

                /* The status need to be sent for all pages
                 * If command is not pages, it also should be sent for all pages
                 * If command is paged, it should be send only for supported pages
                 */
                if (inst->zone_pages < inst->num_available_zone)
                {
                    if ((inst->state & MTB_PMBUS_STATE_ZONE_READ_STATUS) != 0U)
                    {
                        MTB_PMBUS_LOG_DBG("Control byte %x", inst->cmd_ctrl_byte);

                        {
                            uint8_t zone_status = 0x0U;

                            if (inst->cfg->zone_callback != NULL)
                            {
                                if ((inst->cmd_ctrl_byte & MTB_PMBUS_ZONE_DS_MASK) != 0U)
                                {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                    zone_status = inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_BYTE_REQ,
                                                                           inst->zone_sts_mask, inst->act_page);
#else
                                    zone_status = inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_BYTE_REQ,
                                                                           inst->zone_sts_mask,
                                                                           MTB_PMBUS_NO_PAGE_PHASE);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                                }
                                else
                                {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                    zone_status = inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_WORD_REQ,
                                                                           inst->zone_sts_mask, inst->act_page);
#else
                                    zone_status = inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_WORD_REQ,
                                                                           inst->zone_sts_mask,
                                                                           MTB_PMBUS_NO_PAGE_PHASE);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                                }
                            }
                            else
                            {
                                MTB_PMBUS_LOG_ERR("The incorrect status is send as Zone Callback is not registered");
                            }

                            /* Put the status into internal buffer */
                            if ((inst->cmd_ctrl_byte & MTB_PMBUS_ZONE_DI_MASK) == 0U)
                            {
                                inst->int_buff[MTB_PMBUS_ZONE_READ_STS_POS] = zone_status;
                            }
                            else
                            {
                                inst->int_buff[MTB_PMBUS_ZONE_READ_STS_POS] = ~zone_status;
                            }
                            /* Put the address and page number into internal buffer */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                            if (inst->cfg->num_pages > 0U)
                            {
                                inst->int_buff[MTB_PMBUS_ZONE_READ_ADDR_POS] = ((inst->cfg->address << 1U) | 0x01U);
                                inst->int_buff[MTB_PMBUS_ZONE_READ_PAGE_POS] = inst->act_page;
                            }
                            else
                            {
                                inst->int_buff[MTB_PMBUS_ZONE_READ_ADDR_POS] = (inst->cfg->address << 1U);
                                inst->int_buff[MTB_PMBUS_ZONE_READ_PAGE_POS] = 0xFFU;
                            }
#else
                            inst->int_buff[MTB_PMBUS_ZONE_READ_ADDR_POS] = (inst->cfg->address << 1U);
                            inst->int_buff[MTB_PMBUS_ZONE_READ_PAGE_POS] = 0xFFU;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                            inst->byte_to_send = MTB_PMBUS_ZONE_READ_STATUS_RESP_SIZE;
                            inst->byte_sent = 0U;
                        }
                    }
                    else /* Command handling */
                    {
                        if ((inst->state & MTB_PMBUS_STATE_CMD_MATCH) != 0U)
                        {
                            mtb_pmbus_int_cmd_handle_tx(inst);

                            if ((inst->cmd_ctrl_byte & MTB_PMBUS_ZONE_DS_MASK) != 0U)
                            {
                                mtb_pmbus_int_reverse_byte_order(inst->int_buff + (uint8_t)(inst->is_cmd_block),
                                                                 (uint32_t)(inst->byte_to_send) -
                                                                 (uint32_t)(inst->is_cmd_block));
                            }

                            if ((inst->cmd_ctrl_byte & MTB_PMBUS_ZONE_DI_MASK) != 0U)
                            {
                                for (uint16_t i = 0U; i < inst->byte_to_send; i++)
                                {
                                    inst->int_buff[i] = ~inst->int_buff[i];
                                }
                            }

                            inst->byte_to_send += 2U;
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                            if ((inst->cfg->num_pages > 0U) &&
                                ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) != 0U))
                            {
                                inst->int_buff[inst->byte_to_send - 2U] = ((inst->cfg->address << 1U) | 0x01U);
                                inst->int_buff[inst->byte_to_send - 1U] = inst->act_page;
                            }
                            else
                            {
                                inst->int_buff[inst->byte_to_send - 2U] = (inst->cfg->address << 1U);
                                inst->int_buff[inst->byte_to_send - 1U] = 0xFFU;
                                /* Set number of responses to 1 if command does not support pages */
                                inst->num_available_zone = 1U;
                            }
#else
                            inst->int_buff[inst->byte_to_send - 2U] = (inst->cfg->address << 1U);
                            inst->int_buff[inst->byte_to_send - 1U] = 0xFFU;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                        }
                        else
                        {
                            /* Command does not match */
                        }
                    }
                }
                else
                {
                    /* No more pages to read */
                    MTB_PMBUS_LOG_DBG("Zone doesn't have more data to read");
                    inst->zone_is_read = true;
                    status = MTB_PMBUS_NACK_BIT;
                }
            }
        }
        else
        {
            /* Set default cmd and lookup tables */
            inst->active_cmd_tbl = inst->cfg->cmd_table;
            inst->active_lookup_tbl = inst->cmd_lookup_tbl;

            inst->num_available_zone = mtb_pmbus_int_get_num_available_zone(inst);

#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
            if (inst->num_available_zone > 0U)
            {
                /* Store the current page */
                inst->stored_page = inst->act_page;
            }
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */

            inst->state |= MTB_PMBUS_STATE_ZONE_READ | MTB_PMBUS_STATE_DIR_WR;
            MTB_PMBUS_LOG_DBG("Zone Read first time");
        }
    }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
    else /* Unsupported address */
    {
        /* Just ignore the address */
        status = MTB_PMBUS_NACK_BIT;
    }

    return status;
}


/* Return true for ACK and false for NACK */
bool mtb_pmbus_handle_rx(mtb_pmbus_stc_t *inst, uint8_t byte)
{
    MTB_PMBUS_LOG_DBG("RX");
    bool response = false;
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
    bool skip_data_handling = false;
    bool pages_is_act = false;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

    if (((inst->state & MTB_PMBUS_STATE_DIR_WR) != 0U) && (inst->errors == 0U))
    {
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
        if ((inst->state & MTB_PMBUS_STATE_ZONE_READ) != 0U)
        {
            if (inst->byte_received > 2U)
            {
                MTB_PMBUS_LOG_WRN("(Zone Read) Controller sent too many bytes");
                skip_data_handling = true;
                inst->errors |= MTB_PMBUS_ERR_WR_TOO_MANY_BYTES;
            }
            else if (inst->byte_received == 0U)
            {
                if (inst->num_available_zone > 0U)
                {
                    MTB_PMBUS_LOG_DBG("The control byte [%x]", byte);
                    skip_data_handling = true;
                    response = true;
                    inst->cmd_ctrl_byte = byte;
                    inst->byte_received++;
                }
                else
                {
                    MTB_PMBUS_LOG_WRN("Ignored zone read operation");
                    skip_data_handling = true;
                }
            }
            else
            {
                if ((inst->cmd_ctrl_byte & MTB_PMBUS_ZONE_AR_MASK) != 0U)
                {
                    inst->state |= MTB_PMBUS_STATE_ZONE_RESP_CONTINUE;
                }

                if ((inst->cmd_ctrl_byte & MTB_PMBUS_ZONE_ST_MASK) != 0U)
                {
                    /* Handle status */
                    MTB_PMBUS_LOG_DBG("Zone Read: Handle Status");
                    inst->state |= MTB_PMBUS_STATE_ZONE_READ_STATUS;
                    inst->zone_sts_mask = byte;
                    skip_data_handling = true;
                }
                else
                {
                    /* Handle command */
                    MTB_PMBUS_LOG_DBG("Zone Read: Handle Command");
                    inst->state |= MTB_PMBUS_STATE_ZONE_READ_CMD;
                    inst->cmd_code = byte;
                }
                response = true;
                inst->byte_received++;
            }
        }
        if (!skip_data_handling)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
        {
            if ((inst->state & MTB_PMBUS_STATE_CMD_MATCH) != 0U)
            {
                /* Check if command supports write direction */
                if ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap &
                     (MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL)) != 0U)
                {
                    /* Determine the correct lookup index */
                    uint8_t wr_prot_idx = inst->cmd_code;
                    #if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
                    if ((inst->state & MTB_PMBUS_STATE_IMPL_CMD) != 0U)
                    {
                        wr_prot_idx = inst->cmd_pos;
                    }
                    #endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */

#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
                    defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
                    /* Check if command is protected — exempt the Protect Locks group
                     * (ACCESS_CONTROL 0x0F and PASSKEY 0x0E):
                     * "A device shall allow write transactions on these commands regardless of
                     * WRITE_PROTECT or ACCESS_CONTROL itself."
                     * Exception: when PASSKEY is Locked or LockedOut, ACCESS_CONTROL writes
                     * are NOT exempt — they must be NACKed to enforce the lock. */

                    /* cmd_code_full encodes FE-extended commands with MTB_PMBUS_CMD_CODE_EXT
                     * in the upper byte, so a plain compare against the 8-bit standard codes
                     * naturally excludes any FE-prefixed command without an explicit ext guard. */
                    bool is_passkey_cmd = (inst->cmd_code_full == MTB_PMBUS_PASSKEY_CMD_CODE);
                    bool is_acl_cmd     = (inst->cmd_code_full == MTB_PMBUS_ACCESS_CONTROL_CMD_CODE);

                    bool passkey_locked =
                        ((inst->passkey_state == MTB_PMBUS_PASSKEY_ST_LOCKED) ||
                         (inst->passkey_state == MTB_PMBUS_PASSKEY_ST_LOCKED_OUT));

                    /* Explicit block: PASSKEY Locked/LockedOut overrides all other flags for
                     * ACCESS_CONTROL — even if IS_WR_PROTECTED is not set. */
                    bool passkey_blocks_acl = is_acl_cmd && passkey_locked;

                    bool is_protect_lock_bypass =
                        is_passkey_cmd || (is_acl_cmd && !passkey_locked);

                    if (!passkey_blocks_acl &&
                        (((inst->active_lookup_tbl[wr_prot_idx].flags & MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED) == 0U) ||
                         is_protect_lock_bypass))
#else
                    if ((inst->active_lookup_tbl[wr_prot_idx].flags & MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED) == 0U)
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */
                    {
                        /* Check if it is a Count byte*/
                        if (inst->is_cmd_block && ((inst->state & MTB_PMBUS_STATE_GOT_BYTE_COUNT) == 0U))
                        {
#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
                            defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
                            bool passkey_stealth =
                                (is_passkey_cmd &&
                                 ((inst->passkey_state == MTB_PMBUS_PASSKEY_ST_LOCKED) ||
                                  (inst->passkey_state == MTB_PMBUS_PASSKEY_ST_LOCKED_OUT)));
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */

#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
                            defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
                            if (passkey_stealth)
                            {
                                /* Special length rules when Locked */
                                if (byte < MTB_PMBUS_PASSKEY_STEALTH_MIN_LEN)
                                {
                                    /* Block count below minimum — NACK, no STATUS bits */
                                    response = false;
                                    MTB_PMBUS_LOG_WRN("PASSKEY Locked: block count [%x] < %u (NACK)",
                                                      byte, MTB_PMBUS_PASSKEY_STEALTH_MIN_LEN);
                                }
                                else if ((byte > MTB_PMBUS_PASSKEY_STEALTH_MAX_LEN) ||
                                         (byte > inst->active_cmd_tbl[inst->cmd_pos].data_size))
                                {
                                    /* Block count exceeds the stealth range (> 8) OR the device
                                     * buffer size — NACK + increment fail counter.
                                     * Both limits are checked independently (OR) to prevent an
                                     * OOB write into int_buff when data_size < STEALTH_MAX_LEN:
                                     * any count > data_size must be rejected even if count <= 8. */
                                    if (inst->passkey_state == MTB_PMBUS_PASSKEY_ST_LOCKED)
                                    {
                                        inst->passkey_fail_cnt++;
                                        if (inst->passkey_fail_cnt >= MTB_PMBUS_PASSKEY_MAX_FAIL_CNT)
                                        {
                                            inst->passkey_state = MTB_PMBUS_PASSKEY_ST_LOCKED_OUT;
                                            inst->errors |= MTB_PMBUS_ERR_PASSKEY_LOCKED_OUT;
                                        }
                                    }
                                    response = false;
                                    MTB_PMBUS_LOG_WRN(
                                        "PASSKEY Locked: block count [%x] exceeds stealth or device max (NACK)", byte);
                                }
                                else
                                {
                                    /* Stealth ACK — accept all bytes without setting error bits */
                                    response = true;
                                    inst->byte_requested = (uint16_t)(byte) + 1U;
                                    inst->int_buff[inst->byte_received] = byte;
                                    inst->byte_received++;
                                    inst->state |= MTB_PMBUS_STATE_GOT_BYTE_COUNT;
                                }
                            }
                            else
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */
                            {
                                /* Normal block-count check for all other commands */
                                if (byte > inst->active_cmd_tbl[inst->cmd_pos].data_size)
                                {
                                    inst->errors |= MTB_PMBUS_ERR_BYTE_COUNT_TOO_BIG;
                                    MTB_PMBUS_LOG_WRN("Count byte [%x] exceed length", byte);
                                }
                                else
                                {
                                    /* Send ACK */
                                    response = true;

                                    /* Store count value */
                                    inst->byte_requested = (uint16_t)(byte) + 1U;
                                    inst->int_buff[inst->byte_received] = byte;
                                    inst->byte_received++;
                                    inst->state |= MTB_PMBUS_STATE_GOT_BYTE_COUNT;
                                }
                            }
                        }
                        else
                        {
                            /* Check if all data is already received */
                            if (inst->byte_received == inst->byte_requested)
                            {
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
                                /* Check if PEC is supported */
                                if (inst->cfg->enable_pec)
                                {
                                    if (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap &
                                          MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) != 0U) &&
                                        ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_WR) ==
                                         0U))
                                    {
                                        inst->errors |= MTB_PMBUS_ERR_WR_TOO_MANY_BYTES;
                                        MTB_PMBUS_LOG_WRN(
                                            "Controller attempts to write too many bytes(Process Call with PEC in Write Word part) [%d]",
                                            inst->byte_received + 1U);
                                    }
                                    else if ((inst->state & MTB_PMBUS_STATE_PEC_IS_EXECUTED) == 0U)
                                    {
                                        /* Check if PEC is correct */
                                        if (byte == inst->crc)
                                        {
                                            /* Send ACK */
                                            response = true;
                                        }
                                        else
                                        {
                                            inst->errors |= MTB_PMBUS_ERR_CORRUPTED_DATA;
                                            MTB_PMBUS_LOG_WRN("PEC [%x] is not correct. Exp [%x]", byte, inst->crc);
                                        }
                                        inst->state |= MTB_PMBUS_STATE_PEC_IS_EXECUTED;
                                    }
                                    else
                                    {
                                        inst->errors |= MTB_PMBUS_ERR_WR_TOO_MANY_BYTES;
                                        MTB_PMBUS_LOG_WRN("Controller attempts to write too many bytes (PEC) [%d]",
                                                          inst->byte_received + 1U);
                                    }
                                }
                                else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
                                {
                                    inst->errors |= MTB_PMBUS_ERR_WR_TOO_MANY_BYTES;
                                    MTB_PMBUS_LOG_WRN("Controller attempts to write too many bytes [%d]",
                                                      inst->byte_received + 1U);
                                }
                            }
                            else
                            {
                                /* Store byte into internal buffer to use it by pre-implemented
                                   commands */
                                inst->int_buff[inst->byte_received] = byte;
                                if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                                {
                                    response = inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_WRITE_BYTE,
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                                                                            (int32_t)inst->act_page,
#else
                                                                                            MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                                                                                            (int32_t)inst->act_phase,
#else
                                                                                            MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U))
                                                                                            byte,
                                                                                            inst->byte_received - 1U);
#else
                                                                                            byte);
#endif /* #if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U)) */
                                }
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
                                else if ((inst->state & MTB_PMBUS_STATE_IMPL_CMD) != 0U)
                                {
                                    response = mtb_pmbus_int_impl_cmd_handler(inst, MTB_PMBUS_CMD_WRITE_BYTE);
                                }
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
                                else
                                {
                                    response = true;
                                }

                                if (response)
                                {
                                    inst->byte_received++;
                                    MTB_PMBUS_LOG_DBG("WR Byte [%x]", byte);
                                }
                                else
                                {
                                    inst->errors |= MTB_PMBUS_ERR_APP_NACK_BYTE;
                                    MTB_PMBUS_LOG_WRN("Application sends Nack on byte [%x]", byte);
                                }
                            }
                        }
                    }
                    else
                    {
                        inst->errors |= MTB_PMBUS_ERR_WR_TO_PROT_CMD;
                        MTB_PMBUS_LOG_WRN("Cmd [%x] is protected against write", inst->cmd_code);
                    }
                }
                else
                {
                    inst->errors |= MTB_PMBUS_ERR_REQ_WR_FOR_RD_ONLY;
                    MTB_PMBUS_LOG_WRN("Cmd [%x] doesn't support write dir", inst->cmd_code);
                }
            }
            else if ((inst->errors & MTB_PMBUS_ERR_UNSUPPORTED_CMD) == 0U)
            {
#if ((defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)))
                if ((inst->state & MTB_PMBUS_STATE_ZONE_WRITE) != 0U)
                {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                    if (inst->cfg->num_pages != 0U)
                    {
                        /* Find if at least one page has an active Zone */
                        for (uint8_t i = 0U; i < inst->cfg->num_pages; i++)
                        {
                            if ((inst->act_write_zone == inst->write_zones[i]) ||
                                ((inst->act_write_zone == MTB_PMBUS_ZONE_ALL) &&
                                 (inst->write_zones[i] != MTB_PMBUS_ZONE_NONE)))
                            {
                                pages_is_act = true;
                                MTB_PMBUS_LOG_DBG("Page [%x] has active zone [%x]", i, inst->act_write_zone);
                                break;
                            }
                        }
                    }
                    else
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                    {
                        /* If page is not supported, check the global zone */
                        if ((inst->act_write_zone == inst->write_zones[0U]) ||
                            ((inst->act_write_zone == MTB_PMBUS_ZONE_ALL) &&
                             (inst->write_zones[0U] != MTB_PMBUS_ZONE_NONE)))
                        {
                            pages_is_act = true;
                            MTB_PMBUS_LOG_DBG("Target has active zone [%x]", inst->act_write_zone);
                        }
                    }
                }
#endif \
                /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) &&
                   (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                /* Check if implemented commands are present */
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
                if ((inst->cfg->impl_cmd_mask != 0U) && inst->cfg->enable_pmbus)
                {
                    /* Find the implemented command */
                    for (uint8_t i = 0U; i < MTB_PMBUS_IMPL_CMD_NUM; i++)
                    {
                        if ((inst->pre_impl_cmd_table[i].cmd_code == byte) &&
                            ((inst->pre_impl_cmd_lookup_tbl[i].flags &
                              (MTB_PMBUS_CMD_FLAG_IS_PRESENT | MTB_PMBUS_CMD_FLAG_IS_ENABLED)) ==
                             (MTB_PMBUS_CMD_FLAG_IS_PRESENT | MTB_PMBUS_CMD_FLAG_IS_ENABLED)))
                        {
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
                            if (((inst->state & MTB_PMBUS_STATE_ZONE_WRITE) != 0U) &&
#if (defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U))
                                (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_CODE != byte) &&
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U)) */
                                (!pages_is_act))
                            {
                                /* Ignore the command, Send NACK */

                                /* Set state to impl command to skip user's command processing */
                                inst->state = MTB_PMBUS_STATE_IMPL_CMD;
                                MTB_PMBUS_LOG_DBG("Zone Write: Cmd [%x] is ignored", byte);
                            }
                            else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
                            {
                                /* Store the command position */
                                inst->cmd_pos = i;

                                /* Switch command table */
                                inst->active_cmd_tbl = inst->pre_impl_cmd_table;
                                inst->active_lookup_tbl = inst->pre_impl_cmd_lookup_tbl;

                                inst->state |= MTB_PMBUS_STATE_CMD_MATCH | MTB_PMBUS_STATE_IMPL_CMD;

                                inst->cmd_code = byte;
                                inst->cmd_code_full = inst->cmd_code;

                                inst->byte_requested = inst->active_cmd_tbl[inst->cmd_pos].data_size;

                                if ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_BLOCK) != 0U)
                                {
                                    inst->is_cmd_block = true;
                                }

                                /* In case of PAGE_PLUS or P2_PLUS cmds, receive response from
                                   mtb_pmbus_int_handle_p_plus_cmd */
                                response = mtb_pmbus_int_impl_cmd_handler(inst, MTB_PMBUS_CMD_MATCH);

                                MTB_PMBUS_LOG_DBG("Cmd [%x] is pre-impl", byte);
                            }

                            break;
                        }
                    }
                }
                if ((inst->state & MTB_PMBUS_STATE_IMPL_CMD) == 0U)
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
                {
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
                    if (((inst->state & MTB_PMBUS_STATE_ZONE_WRITE) == 0U) || (pages_is_act))
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
                    {
#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
                        /* Check if extended commands are supported */
                        if ((inst->ext_cmd_en) &&
                            ((byte == MTB_PMBUS_CMD_CODE_EXT) && ((inst->state & MTB_PMUBS_STATE_CMD_MATCH_EXT) == 0U)))
                        {
                            /* Switch command table */
                            inst->active_cmd_tbl = inst->cfg->ext_cmd_table;
                            inst->active_lookup_tbl = inst->ext_cmd_lookup_tbl;

                            /* Update State Machine: Extended Code */
                            inst->state |= MTB_PMUBS_STATE_CMD_MATCH_EXT;
                            MTB_PMBUS_LOG_DBG("Cmd [%x] is extended", byte);

                            /* Send ACK */
                            response = true;
                        }
                        /* Check if command is present in command table */
                        else if ((inst->active_lookup_tbl[byte].flags & MTB_PMBUS_CMD_FLAG_IS_PRESENT) != 0U)
#else
                        if ((inst->active_lookup_tbl[byte].flags & MTB_PMBUS_CMD_FLAG_IS_PRESENT) != 0U)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
                        {
                            MTB_PMBUS_LOG_DBG("Cmd [%x] is supported", byte);

                            /* Check if command is enabled */
                            if ((inst->active_lookup_tbl[byte].flags & MTB_PMBUS_CMD_FLAG_IS_ENABLED) != 0U)
                            {
                                /* Store the command position */
                                inst->cmd_pos = inst->active_lookup_tbl[byte].cmd_pos;
                                /* Send ACK on command code */
                                response = true;

                                /* Call the command callback */
                                if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                                {
                                    bool result = inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_MATCH,
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                                                                               (int32_t)inst->act_page,
#else
                                                                                               MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                                                                                               (int32_t)inst->act_phase,
#else
                                                                                               MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U))
                                                                                               0U, 0U);
#else
                                                                                               0U);
#endif /* #if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U)) */
                                    if (((inst->active_lookup_tbl[byte].flags & MTB_PMBUS_CMD_FLAG_IS_ENABLED) == 0U) ||
                                        !result)
                                    {
                                        /* Send NACK on command code if user disable it in the c */
                                        response = false;
                                    }
                                }

                                if (response)
                                {
                                    /* Update State Machine: Cmd code is received */
                                    inst->state |= MTB_PMBUS_STATE_CMD_MATCH;

                                    inst->cmd_code = byte;
                                    inst->active_lookup_tbl[inst->cmd_code].flags |= MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE;

    #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
                                    if ((inst->state & MTB_PMUBS_STATE_CMD_MATCH_EXT) != 0U)
                                    {
                                        inst->cmd_code_full = (uint16_t)MTB_PMBUS_CMD_CODE_EXT |
                                                              ((uint16_t)(inst->cmd_code) << MTB_PMBUS_CMD_EXT_POS);
                                    }
                                    else
    #endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
                                    {
                                        inst->cmd_code_full = inst->cmd_code;
                                    }

                                    inst->byte_requested = inst->active_cmd_tbl[inst->cmd_pos].data_size;

                                    if ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_BLOCK) != 0U)
                                    {
                                        inst->is_cmd_block = true;
                                    }
#if (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U)
                                    if (((inst->state & MTB_PMBUS_STATE_PAGE_PLUS_CMD) != 0U) ||
                                        ((inst->state & MTB_PMBUS_STATE_P2_PLUS_CMD) != 0U))
                                    {
                                        /* Check and adjust embedded cmd */
                                        response = mtb_pmbus_int_handle_p_plus_cmd(inst);
                                        MTB_PMBUS_LOG_DBG("Cmd [%x] is embedded and %s", byte,
                                                          (response ? "accepted" : "not valid"));
                                    }
                                    else
#endif /* #if (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U) */
                                    {
                                        MTB_PMBUS_LOG_DBG("Cmd [%x] is accepted", byte);
                                    }
                                }
                                /* Command is ignored */
                                else
                                {
                                    inst->state |= MTB_PMBUS_CMD_IS_IGNORED;
                                    inst->errors |= MTB_PMBUS_ERR_UNSUPPORTED_CMD;
                                    MTB_PMBUS_LOG_WRN("Cmd [%x] is not enabled (Callback)", byte);
                                }
                            }
                            else
                            {
                                inst->state |= MTB_PMBUS_CMD_IS_IGNORED;
                                inst->errors |= MTB_PMBUS_ERR_UNSUPPORTED_CMD;
                                MTB_PMBUS_LOG_WRN("Cmd [%x] is not enabled", byte);
                            }
                        }
                        else
                        {
                            inst->errors |= MTB_PMBUS_ERR_UNSUPPORTED_CMD;
                            MTB_PMBUS_LOG_WRN("Cmd [%x] is not supported", byte);
                        }
                    }
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
                    else
                    {
                        inst->state |= MTB_PMBUS_CMD_IS_IGNORED;
                        MTB_PMBUS_LOG_DBG("Cmd [%x] is ignored", byte);
                    }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
                }
            }
            else
            {
                /* Unsupported command */
            }
        }
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
        if ((inst->state & MTB_PMBUS_STATE_PEC_IS_EXECUTED) == 0U)
        {
            mtb_pmbus_int_crc_calc(inst, byte);
        }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
    }

    return response;
}


void mtb_pmbus_handle_restart(mtb_pmbus_stc_t *inst)
{
    MTB_PMBUS_LOG_DBG("RE");

    /* The Re-Start condition can be handled only if the target address
     * are received
     */
    if ((inst->state & MTB_PMBUS_STATE_TARGET_ADDR_MATCH_1) != 0U)
    {
        /* Reset the number of requested byte for case when the command supports read and process
           call and
         * gets the just read request
         */
        if (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_RD) != 0U)
            && ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) != 0U))
        {
            if (inst->byte_received == 0U)
            {
                MTB_PMBUS_LOG_DBG("Reset byte_requested");
                inst->byte_requested = 0U;
            }
        }
    }
    else
    {
#if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U))
        if (!inst->cfg->enable_pmbus)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */
        {
            /* If the Target Address is not received, set the state to prevent
             * the Group Command Protocol to be processed in the next transfer
             */
            inst->state |= MTB_PMBUS_STATE_TARGET_ADDR_NOT_MATCH;
        }
    }
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
    if (((inst->state & MTB_PMBUS_STATE_ZONE_READ) != 0U) && ((inst->state & MTB_PMBUS_STATE_DIR_RD) != 0U))
    {
        if (!inst->zone_is_read)
        {
            if ((inst->errors & MTB_PMBUS_ERR_ARB_LOST) == 0U)
            {
                if ((inst->state & MTB_PMBUS_STATE_ZONE_READ_STATUS) == 0U)
                {
                    if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                    {
                        (void)inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_READ_DONE,
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                                                           (int32_t)inst->act_page,
#else
                                                                           MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                                                                           (int32_t)inst->act_phase,
#else
                                                                           MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U))
                                                                           0xFFU, 0U);
#else
                                                                           0xFFU);
#endif /* #if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U)) */
                    }
                }
                inst->zone_pages++;
            }
            else
            {
                inst->errors &= ~MTB_PMBUS_ERR_ARB_LOST;
                if ((inst->state & MTB_PMBUS_STATE_ZONE_RESP_CONTINUE) == 0U)
                {
                    inst->zone_pages++;
                }
                else
                {
                    inst->state |= MTB_PMBUS_STATE_ZONE_RETRY_RESP;
                }
            }
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
            if ((inst->state & MTB_PMBUS_STATE_PEC_IS_EXECUTED) != 0U)
            {
                inst->state &= ~MTB_PMBUS_STATE_PEC_IS_EXECUTED;
            }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
        }
    }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

    /* Always clear the direction */
    inst->state &= ~(MTB_PMBUS_STATE_DIR_WR | MTB_PMBUS_STATE_DIR_RD);
}


void mtb_pmbus_handle_tx(mtb_pmbus_stc_t *inst, uint8_t **data_ptr, uint16_t *size)
{
    MTB_PMBUS_LOG_DBG("TX");

    /* Update the pointer to external buffer */
    *data_ptr = inst->int_buff;
    /* Set the default size */
    *size = 1U;

    if (((inst->state & MTB_PMBUS_STATE_DIR_RD) != 0U) && (inst->errors == 0U)
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
        && !inst->zone_is_read
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
        )
    {
        if (inst->byte_sent == 0U)
        {
            inst->byte_sent = inst->byte_to_send;
            *size = inst->byte_to_send;

            for (uint16_t i = 0U; i < inst->byte_to_send; i++)
            {
                MTB_PMBUS_LOG_DBG("RD Byte:%x", inst->int_buff[i]);
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
                mtb_pmbus_int_crc_calc(inst, inst->int_buff[i]);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
            }
        }
        else
        {
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
            if ((inst->state & MTB_PMBUS_STATE_PEC_IS_EXECUTED) == 0U)
            {
                if (inst->cfg->enable_pec)
                {
                    inst->int_buff[0U] = inst->crc;
                    inst->state |= MTB_PMBUS_STATE_PEC_IS_EXECUTED;
                }
                else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
            {
                inst->int_buff[0U] = MTB_PMBUS_EXTRA_BYTE_VAL;
                inst->errors |= MTB_PMBUS_ERR_RD_TOO_MANY_BYTES;
                MTB_PMBUS_LOG_WRN("Too many bytes is requested");
            }
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
        }
        else
        {
            inst->int_buff[0U] = MTB_PMBUS_EXTRA_BYTE_VAL;
            inst->errors |= MTB_PMBUS_ERR_RD_TOO_MANY_BYTES;
            MTB_PMBUS_LOG_WRN("Too many bytes is requested (PEC)");
        }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
        }
    }
    else
    {
        inst->int_buff[0U] = MTB_PMBUS_EXTRA_BYTE_VAL;
    }
}


void mtb_pmbus_handle_stop(mtb_pmbus_stc_t *inst)
{
    MTB_PMBUS_LOG_DBG("STOP");

#if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U))
    if (!inst->cfg->enable_pmbus)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */
    {
        /* Reset the state to allow the Group Command Protocol to be processed
         * in the next transfer after the Stop condition.
         */
        inst->state &= ~MTB_PMBUS_STATE_TARGET_ADDR_NOT_MATCH;
    }

    /* The Stop condition can be handled only if any supported addresses
     * are received
     */
    if ((inst->state & (
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
             MTB_PMBUS_STATE_ZONE_WRITE |
             MTB_PMBUS_STATE_ZONE_READ |
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
#if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)
             MTB_PMBUS_STATE_ALERT_RESP_ADDR |
#endif /* #if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U) */
#if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U))
             MTB_PMBUS_STATE_GENERAL_CALL_ADDR |
#endif /* #if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)) */
             MTB_PMBUS_STATE_TARGET_ADDR_MATCH_1)) != 0U)
    {
        /* Check if error is present */
        if (inst->errors == 0U)
        {
            if ((inst->state & MTB_PMBUS_CMD_IS_IGNORED) == 0U)
            {
                if (((inst->state & MTB_PMBUS_STATE_CMD_MATCH) == 0U)
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
                    && ((inst->state & MTB_PMBUS_STATE_ZONE_READ_STATUS) == 0U)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
                    )
                {
#if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)
                    if ((inst->state & MTB_PMBUS_STATE_ALERT_RESP_ADDR) != 0U)
                    {
                        if (inst->byte_sent == MTB_PMBUS_ALERT_ADDR_RESPONSE_SIZE)
                        {
                            if (inst->cfg->gen_callback != NULL)
                            {
                                inst->cfg->gen_callback(MTB_PMBUS_ALERT_RESPONSE_ADDR_EVENT);
                            }

                            if (inst->smbalert_mode == MTB_PMBUS_SMBALERT_MODE_AUTO)
                            {
                                MTB_PMBUS_LOG_DBG("Clear SMBALERT signal");
                                mtb_pmbus_smbalert_clear_signal(inst);
                            }
                        }
                    }
                    else
#endif /* #if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U) */
                    {
                        if ((inst->state & MTB_PMBUS_STATE_RECEIVED_BYTE_PROT) != 0U)
                        {
                            /* Receive byte protocol */
                            if (mtb_pmbus_hal_is_tx_cmpl(inst))
                            {
                                if (inst->cfg->gen_callback != NULL)
                                {
                                    inst->cfg->gen_callback(MTB_PMBUS_RECEIVE_BYTE_CMPLT_EVENT);
                                }
                            }
                            else
                            {
                                if (inst->cfg->gen_callback != NULL)
                                {
                                    inst->cfg->gen_callback(MTB_PMBUS_QUICK_CMD_RD_EVENT);
                                }
                            }
                        }
                        else
                        {
                            if (inst->cfg->gen_callback != NULL)
                            {
                                inst->cfg->gen_callback(MTB_PMBUS_QUICK_CMD_WR_EVENT);
                            }
                        }
                    }
                }
                else
                {
                    if ((inst->state & MTB_PMBUS_STATE_DIR_RD) != 0U)
                    {
                        if (!mtb_pmbus_hal_is_tx_cmpl(inst))
                        {
                            inst->errors |= MTB_PMBUS_ERR_RD_LESS_BYTES;
                            MTB_PMBUS_LOG_DBG("Less bytes are sent");
                        }
                        else
                        {
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
                            if ((inst->state & MTB_PMBUS_STATE_IMPL_CMD) != 0U)
                            {
                                (void)mtb_pmbus_int_impl_cmd_handler(inst, MTB_PMBUS_CMD_READ_DONE);
                            }
                            else
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
                            {
                                /* Call the command callback */
                                if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                                {
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
                                    if ((inst->state & MTB_PMBUS_STATE_ZONE_READ_STATUS) == 0U)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
                                    {
                                        /* Ignore the return status as it is not applicable for this
                                           event */
                                        (void)inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_READ_DONE,
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                                                                           (int32_t)inst->act_page,
#else
                                                                                           MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                                                                                           (int32_t)inst->act_phase,
#else
                                                                                           MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U))
                                                                                           0U, 0U);
#else
                                                                                           0U);
#endif /* #if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U)) */
                                    }
                                    inst->active_lookup_tbl[inst->cmd_code].flags |= MTB_PMBUS_CMD_FLAG_IS_TRANSMITTED;
                                }
                            }
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
                            if (((inst->state & MTB_PMBUS_STATE_ZONE_READ) != 0U) &&
                                ((inst->state & MTB_PMBUS_STATE_ZONE_READ_STATUS) != 0U)
                                && !inst->zone_is_read)
                            {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                (void)inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_CMPLT, inst->zone_sts_mask,
                                                               inst->act_page);
#else
                                (void)inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_CMPLT, inst->zone_sts_mask,
                                                               MTB_PMBUS_NO_PAGE_PHASE);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                            }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
                        }
                    }
                    else
                    {
                        /* The handler of the case when the command supports read and process call,
                           but gets a write request. */
                        if (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_RD) != 0U)
                            && ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) !=
                                0U))
                        {
                            inst->errors |= MTB_PMBUS_ERR_REQ_WR_FOR_PC_ONLY;
                            MTB_PMBUS_LOG_WRN("Cmd [%x] doesn't support write transaction",
                                              inst->active_cmd_tbl[inst->cmd_pos].cmd_code);
                        }
                        else
                        {
                            /* Check if all requested data is received */
                            if (inst->byte_received == inst->byte_requested)
                            {
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
                                /* Store data */
                                if ((inst->state & MTB_PMBUS_STATE_IMPL_CMD) != 0U)
                                {
                                    (void)mtb_pmbus_int_impl_cmd_handler(inst, MTB_PMBUS_CMD_WRITE_DONE);
                                }
                                else
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
                                {
#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
                                    defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
                                    /* PASSKEY writes must use PEC (mandatory from Security Level 0
                                       up).
                                     * Reject the write if PEC is enabled but was not executed. */
                                    if ((inst->cmd_code_full == MTB_PMBUS_PASSKEY_CMD_CODE) &&
                                        inst->cfg->enable_pec &&
                                        ((inst->state & MTB_PMBUS_STATE_PEC_IS_EXECUTED) == 0U))
                                    {
                                        inst->errors |= MTB_PMBUS_ERR_PEC_REQUIRED;
                                        MTB_PMBUS_LOG_WRN("PASSKEY write rejected: PEC is mandatory");
                                    }
                                    else
#endif /* #if MTB_PMBUS_SUPPORT_PEC */
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */
                                    {
                                        mtb_pmbus_int_update_data(inst);
                                        /* Call the command callback */
                                        if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                                        {
                                            /* Ignore the return status as it is not applicable for
                                               this
                                               event */
                                            (void)inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_WRITE_DONE,
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                                                                               (int32_t)inst->act_page,
#else
                                                                                               MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                                                                                               (int32_t)inst->act_phase,
#else
                                                                                               MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U))
                                                                                               0U, 0U);
#else
                                                                                               0U);
#endif /* #if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U)) */
                                            inst->active_lookup_tbl[inst->cmd_code].flags |=
                                                MTB_PMBUS_CMD_FLAG_IS_TRANSMITTED;
                                        }
#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
                                        defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
                                        /* ACL Write Once [2]: auto-lock after first successful
                                           write
                                         * (normal write STOP path) */
                                        if ((inst->errors == 0U) &&
                                            ((inst->acl_table[inst->cmd_code] & MTB_PMBUS_ACL_BIT_WRITE_ONCE) != 0U))
                                        {
                                            inst->acl_table[inst->cmd_code] |= MTB_PMBUS_ACL_BIT_WRITE_ACCESS;
                                            inst->active_lookup_tbl[inst->cmd_code].flags |=
                                                MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED;
                                            MTB_PMBUS_LOG_DBG("Write Once: cmd [%x] write-locked", inst->cmd_code);
                                        }
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */
                                    } /* end PEC-mandatory else block */
                                }
                            }
                            else
                            {
                                inst->errors |= MTB_PMBUS_ERR_WR_LESS_BYTES;
                                MTB_PMBUS_LOG_WRN("Less bytes are received");
                            }
                        }
                    }
                    if (inst->errors == 0U)
                    {
                        /* Temporarily clear state to allow user use command APIs inside of this
                           callback */
                        uint32_t state_temp = inst->state;
                        inst->state = 0U;
                        inst->active_lookup_tbl[inst->cmd_code].flags &= (uint8_t)(~MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE);
                        /* Call the command callback */
                        if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                        {
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
                            if ((state_temp & MTB_PMBUS_STATE_ZONE_READ_STATUS) == 0U)
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
                            {
                                (void)inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_DONE,
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                                                                   (int32_t)inst->act_page,
#else
                                                                                   MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                                                                                   (int32_t)inst->act_phase,
#else
                                                                                   MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U))
                                                                                   0U, 0U);
#else
                                                                                   0U);
#endif /* #if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U)) */
                            }
                        }
                        /* Restore state */
                        inst->state = state_temp;
                    }
                }
            }
        }

        if (inst->errors != 0U)
        {
#if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)
            /* Clear the arbitration lost event for alert response address */
            if ((inst->state & MTB_PMBUS_STATE_ALERT_RESP_ADDR) != 0U)
            {
                inst->errors &= ~MTB_PMBUS_ERR_ARB_LOST;
            }
#endif /* #if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U) */

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
            if ((inst->state & MTB_PMBUS_STATE_ZONE_READ) != 0U)
            {
                if (inst->errors == MTB_PMBUS_ERR_ARB_LOST)
                {
                    inst->errors &= ~MTB_PMBUS_ERR_ARB_LOST;
                }
            }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

            /* Clear the arbitration lost event if the MTB_PMBUS_STATE_RECEIVED_BYTE_PROT
             * is set, as these two conditions means that Quick Command with Read direction
             * is occurred on the Bus */
            if ((inst->errors == MTB_PMBUS_ERR_ARB_LOST) &&
                ((inst->state & MTB_PMBUS_STATE_RECEIVED_BYTE_PROT) != 0U) && (!mtb_pmbus_hal_is_tx_cmpl(inst)))
            {
                inst->errors &= ~MTB_PMBUS_ERR_ARB_LOST;
                if (inst->cfg->gen_callback != NULL)
                {
                    inst->cfg->gen_callback(MTB_PMBUS_QUICK_CMD_RD_EVENT);
                }
            }
        }

        mtb_pmbus_int_reset_state(inst);
    }
}


void mtb_pmbus_handle_arb_lost(mtb_pmbus_stc_t *inst)
{
    inst->errors |= MTB_PMBUS_ERR_ARB_LOST;

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
    if ((inst->state & MTB_PMBUS_STATE_ZONE_READ) != 0U)
    {
        if (!inst->zone_is_read)
        {
            if ((inst->state & MTB_PMBUS_STATE_ZONE_READ_STATUS) == 0U)
            {
                if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                {
                    (void)inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_ARB_LOST,
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                                                       (int32_t)inst->act_page,
#else
                                                                       MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                                                                       (int32_t)inst->act_phase,
#else
                                                                       MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U))
                                                                       0xFFU, 0U);
#else
                                                                       0xFFU);
#endif /* #if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U)) */
                }
            }
            else
            {
                if (inst->cfg->zone_callback != NULL)
                {
                    (void)inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_ARB_LOST, inst->zone_sts_mask,
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                                   inst->act_page
#else
                                                   MTB_PMBUS_NO_PAGE_PHASE
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                                                   );
                }
            }
        }
    }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
}


void mtb_pmbus_handle_bus_error(mtb_pmbus_stc_t *inst)
{
    MTB_PMBUS_LOG_WRN("Bus error");
    inst->errors |= MTB_PMBUS_ERR_BUS_ERROR;
    mtb_pmbus_int_reset_state(inst);
}


#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
void mtb_pmbus_handle_timeout(mtb_pmbus_stc_t *inst)
{
    MTB_PMBUS_LOG_WRN("25 ms Timeout");
    inst->errors |= MTB_PMBUS_ERR_TIMEOUT;
    mtb_pmbus_int_reset_state(inst);
}
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */

#if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U))
void mtb_pmbus_handle_hnp(mtb_pmbus_stc_t *inst, mtb_pmbus_host_notify_events_t event)
{
    mtb_pmbus_host_notify_events_t event_local = event;

    if ((inst->state & MTB_PMBUS_STATE_HOST_NOTIFY) != 0U)
    {
        if ((event_local == MTB_PMBUS_HOST_NOTIFY_ERROR) || (event_local == MTB_PMBUS_HOST_NOTIFY_ARB_LOST))
        {
            mtb_pmbus_hal_hnp_abort(inst);
        }

        if (inst->cfg->hnp_callback != NULL)
        {
            inst->cfg->hnp_callback(event_local);
        }

        mtb_pmbus_int_reset_state(inst);
    }
}
#endif /* #if(defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) */

static void mtb_pmbus_int_update_data(mtb_pmbus_stc_t *inst)
{
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
    int32_t page =
        ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) ==
         0U) ? MTB_PMBUS_NO_PAGE_PHASE : (int32_t)inst->act_page;
#else
    int32_t page = MTB_PMBUS_NO_PAGE_PHASE;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
    int32_t phase =
        ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) ==
         0U) ? MTB_PMBUS_NO_PAGE_PHASE : (int32_t)inst->act_phase;
#else
    int32_t phase = MTB_PMBUS_NO_PAGE_PHASE;
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */

#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) && (defined(MTB_PMBUS_PHASES_NUM) && \
    (MTB_PMBUS_PHASES_NUM != 0U))
    if ((page == (int32_t)MTB_PMBUS_PAGE_ALL) && (phase == (int32_t)MTB_PMBUS_PHASE_ALL))
    {
        for (int32_t i = 0; i < (int32_t)inst->cfg->num_pages; i++)
        {
            for (int32_t j = 0; j < (int32_t)inst->cfg->num_phases; j++)
            {
                MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", i, j);
                (void)mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, i, j,
                                                        inst->int_buff + (uint8_t)(inst->is_cmd_block),
                                                        (uint32_t)(inst->byte_received) -
                                                        (uint32_t)(inst->is_cmd_block));
            }
        }
    }
    else if (page == (int32_t)MTB_PMBUS_PAGE_ALL)
    {
        for (int32_t i = 0; (i < (int32_t)inst->cfg->num_pages); i++)
        {
            MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", i, phase);
            (void)mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, i, phase,
                                                    inst->int_buff + (uint8_t)(inst->is_cmd_block),
                                                    (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
        }
    }
    else if (phase == (int32_t)MTB_PMBUS_PHASE_ALL)
    {
        for (int32_t i = 0; i < (int32_t)inst->cfg->num_phases; i++)
        {
            MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", page, i);
            (void)mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, page, i,
                                                    inst->int_buff + (uint8_t)(inst->is_cmd_block),
                                                    (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
        }
    }
    else
#elif (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
    if (page == (int32_t)MTB_PMBUS_PAGE_ALL)
    {
        for (int32_t i = 0; i < (int32_t)inst->cfg->num_pages; i++)
        {
            MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", i, phase);
            (void)mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, i, phase,
                                                    inst->int_buff + (uint8_t)(inst->is_cmd_block),
                                                    (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
        }
    }
    else
#elif (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
    if (phase == (int32_t)MTB_PMBUS_PHASE_ALL)
    {
        for (int32_t i = 0; i < (int32_t)inst->cfg->num_phases; i++)
        {
            MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", page, i);
            (void)mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, page, i,
                                                    inst->int_buff + (uint8_t)(inst->is_cmd_block),
                                                    (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
        }
    }
    else
#endif /* if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) &&
          (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
    {
#if ((defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) && (defined(MTB_PMBUS_PAGES_NUM) && \
        (MTB_PMBUS_PAGES_NUM != 0U)))
        if (((inst->state & MTB_PMBUS_STATE_ZONE_WRITE) != 0U) && (inst->cfg->num_pages != 0U))
        {
            for (int32_t i = 0; i < (int32_t)inst->cfg->num_pages; i++)
            {
                if ((inst->act_write_zone == inst->write_zones[i]) ||
                    ((inst->act_write_zone == MTB_PMBUS_ZONE_ALL) && (inst->write_zones[i] != MTB_PMBUS_ZONE_NONE)))
                {
                    MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", i, phase);
                    (void)mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, i, phase,
                                                            inst->int_buff + (uint8_t)(inst->is_cmd_block),
                                                            (uint32_t)(inst->byte_received) -
                                                            (uint32_t)(inst->is_cmd_block));
                }
            }
        }
        else
#endif \
        /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) &&
           (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
        {
            MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", page, phase);
            (void)mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, page, phase,
                                                    inst->int_buff + (uint8_t)(inst->is_cmd_block),
                                                    (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
        }
    }
}


#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
__STATIC_INLINE void mtb_pmbus_int_crc_calc(mtb_pmbus_stc_t *inst, uint8_t byte)
{
    if ((inst->cfg->enable_pec) && !((bool)inst->errors))
    {
        inst->crc = mtb_pmbus_int_crc8_update(inst->crc, byte);
    }
}
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
static void mtb_pmbus_int_reverse_byte_order(uint8_t *array, uint32_t length)
{
    uint8_t temp;
    for (uint16_t i = 0U; i < (length / 2U); i++)
    {
        temp = array[i];
        array[i] = array[length - 1U - i];
        array[length - 1U - i] = temp;
    }
}
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
static bool mtb_pmbus_int_impl_cmd_handler(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    bool status = false;

    switch (inst->cmd_code)
    {
#if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)
        case MTB_PMBUS_IMPL_CMD_REVISION_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_rev(inst, event);
            break;
        }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)
        case MTB_PMBUS_IMPL_CMD_CAPABILITY_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_capability(inst, event);
            break;
        }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U)
        case MTB_PMBUS_IMPL_CMD_QUERY_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_query(inst, event);
            break;
        }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U)
        case MTB_PMBUS_IMPL_CMD_PAGE_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_page(inst, event);
            break;
        }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U)
        case MTB_PMBUS_IMPL_CMD_PHASE_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_phase(inst, event);
            break;
        }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U)
        case MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_zone_cfg(inst, event);
            break;
        }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U)
        case MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_zone_act(inst, event);
            break;
        }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE != 0U)
        case MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_page_plus_write(inst, event);
            break;
        }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ != 0U)
        case MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_page_plus_read(inst, event);
            break;
        }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE != 0U)
        case MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_p2_plus_write(inst, event);
            break;
        }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ != 0U)
        case MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_p2_plus_read(inst, event);
            break;
        }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ != 0U) */

        default:
            /* Do nothing */
            break;
    }

#if (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U)
    if ((((inst->state & MTB_PMBUS_STATE_PAGE_PLUS_CMD) != 0U) ||
         ((inst->state & MTB_PMBUS_STATE_P2_PLUS_CMD) != 0U)) && (event == MTB_PMBUS_CMD_MATCH))
    {
        /* Check and adjust embedded cmd */
        status = mtb_pmbus_int_handle_p_plus_cmd(inst);
        MTB_PMBUS_LOG_DBG("Cmd [%x] is embedded and %s", inst->cmd_code, (status ? "accepted" : "not valid"));
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U) */

    return status;
}
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)
static bool mtb_pmbus_int_impl_cmd_rev(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    if (MTB_PMBUS_CMD_READ_REQ == event)
    {
        /* Only one PMBus revision is supported in this Middleware version */
        if (inst->cfg->revision == MTB_PMBUS_REVISION_1_4)
        {
            /* Middleware supports only the same revision for Part 1 and Part 2 specification */
            inst->int_buff[0U] = MTB_PMBUS_REV_1_4;
            inst->byte_to_send = MTB_PMBUS_IMPL_CMD_REVISION_SIZE;
        }
        else if (inst->cfg->revision == MTB_PMBUS_REVISION_1_5)
        {
            inst->int_buff[0U] = MTB_PMBUS_REV_1_5;
            inst->byte_to_send = MTB_PMBUS_IMPL_CMD_REVISION_SIZE;
        }
        else
        {
            /* Unknown revision — return no data */
        }
    }

    /* Always return true */
    return true;
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)
static bool mtb_pmbus_int_impl_cmd_capability(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    if (MTB_PMBUS_CMD_READ_REQ == event)
    {
        inst->int_buff[0U] =
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
            ((uint8_t)(inst->cfg->enable_pec) << MTB_PMBUS_CAPABILITY_PEC_POS) |
#else
            (0U << MTB_PMBUS_CAPABILITY_PEC_POS) |
#endif
            ((uint8_t)(inst->cfg->speed) << MTB_PMBUS_CAPABILITY_SPEED_POS) |
#if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U))
            ((inst->cfg->enable_smbalert ? 1U : 0U) << MTB_PMBUS_CAPABILITY_SMBALERT_POS) |
#else
            (0U << MTB_PMBUS_CAPABILITY_SMBALERT_POS) |
#endif /* #if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) */
            ((uint8_t)(inst->cfg->enable_ieee_format) << MTB_PMBUS_CAPABILITY_FORMAT_POS);
        inst->byte_to_send = MTB_PMBUS_IMPL_CMD_CAPABILITY_SIZE;
    }

    /* Always return true */
    return true;
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U)
static bool mtb_pmbus_int_impl_cmd_query(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    bool status = true;

    if (MTB_PMBUS_CMD_READ_REQ == event)
    {
        /* Read the command code from internal buffer */
        uint8_t cmd_code = inst->int_buff[1U];
        /* Check if command is present in command table */
        bool cmd_is_present = ((inst->cmd_lookup_tbl[cmd_code].flags & MTB_PMBUS_CMD_FLAG_IS_PRESENT) != 0U);
        /* Set block count to 1 */
        inst->int_buff[0U] = 0x01U;
        inst->int_buff[1U] = ((uint8_t)(cmd_is_present) << MTB_PMBUS_QUERY_CMD_SUP_POS) |
                             (cmd_is_present ?
                              (uint8_t)(
                                  (((uint16_t)((inst->cfg->cmd_table[inst->cmd_lookup_tbl[cmd_code].cmd_pos].cmd_cap &
                                                MTB_PMBUS_CMD_CAP_DIR_WR) != 0U) << MTB_PMBUS_QUERY_CMD_SUP_WR_POS) |
                                   ((uint8_t)((inst->cfg->cmd_table[inst->cmd_lookup_tbl[cmd_code].cmd_pos].cmd_cap &
                                               MTB_PMBUS_CMD_CAP_DIR_RD) != 0U) << MTB_PMBUS_QUERY_CMD_SUP_RD_POS) |
                                   ((inst->cfg->cmd_table[inst->cmd_lookup_tbl[cmd_code].cmd_pos].cmd_cap >>
                                     MTB_PMBUS_CMD_CAP_FORMAT_POS) << MTB_PMBUS_QUERY_CMD_FORMAT_POS))
                                  ) : 0U);
        inst->byte_to_send = MTB_PMBUS_IMPL_CMD_QUERY_SIZE;
    }

    return status;
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U)
static bool mtb_pmbus_int_impl_cmd_page(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    bool status = true;

    if (MTB_PMBUS_CMD_READ_REQ == event)
    {
        inst->int_buff[0U] = inst->act_page;
        inst->byte_to_send = MTB_PMBUS_IMPL_CMD_PAGE_SIZE;
    }
    else if (MTB_PMBUS_CMD_WRITE_DONE == event)
    {
        if ((inst->int_buff[0U] < inst->cfg->num_pages) || (inst->int_buff[0U] == MTB_PMBUS_PAGE_ALL))
        {
#if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U))
            if (inst->cfg->page_callback != NULL)
            {
                uint8_t ret = inst->cfg->page_callback(MTB_PMBUS_CMD_PAGE, true, inst->int_buff[0U]);
                status = (ret == 0U) ? false : true;
            }
            if (status)
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U)) */
            {
                inst->act_page = inst->int_buff[0U];
                MTB_PMBUS_LOG_DBG("New PAGE [%x]", inst->act_page);
            }
        }
        else
        {
            /* If Page number is out of scope, then drop the data */
            inst->errors |= MTB_PMBUS_ERR_INVALID_PAGE;
            MTB_PMBUS_LOG_WRN("The PAGE num is out of range [%x]", inst->int_buff[0U]);
        }
    }
    else
    {
        /* Unhandled event */
    }

    return status;
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U)
static bool mtb_pmbus_int_impl_cmd_phase(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    bool status = true;

    if (MTB_PMBUS_CMD_READ_REQ == event)
    {
        inst->int_buff[0U] = inst->act_phase;
        inst->byte_to_send = MTB_PMBUS_IMPL_CMD_PHASE_SIZE;
    }
    else if (MTB_PMBUS_CMD_WRITE_DONE == event)
    {
        if ((inst->int_buff[0U] < inst->cfg->num_phases) || (inst->int_buff[0U] == MTB_PMBUS_PHASE_ALL))
        {
#if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U))
            if (inst->cfg->phase_callback != NULL)
            {
                uint8_t ret = inst->cfg->phase_callback(MTB_PMBUS_CMD_PHASE, true, inst->int_buff[0U]);
                status = (ret == 0U) ? false : true;
            }
            if (status)
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U)) */
            {
                inst->act_phase = inst->int_buff[0U];
                MTB_PMBUS_LOG_DBG("New PHASE [%x]", inst->act_phase);
            }
        }
        else
        {
            /* If Phase number is out of scope, then drop the data */
            inst->errors |= MTB_PMBUS_ERR_INVALID_PHASE;
            MTB_PMBUS_LOG_WRN("The PHASE num is out of range [%x]", inst->int_buff[0U]);
        }
    }
    else
    {
        /* Unhandled event */
    }

    return status;
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U)
static bool mtb_pmbus_int_impl_cmd_zone_cfg(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    bool status = true;

    if (MTB_PMBUS_CMD_READ_REQ == event)
    {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
        inst->int_buff[0U] = inst->write_zones[inst->act_page];
        inst->int_buff[1U] = inst->read_zones[inst->act_page];
#else
        inst->int_buff[0U] = inst->write_zones[0U];
        inst->int_buff[1U] = inst->read_zones[0U];
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
        inst->byte_to_send = MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_SIZE;
    }
    else if (MTB_PMBUS_CMD_WRITE_BYTE == event)
    {
        /* The first byte is received */
        if (inst->byte_received == 0U)
        {
            if ((inst->int_buff[0U] <= MTB_PMBUS_ZONE_MAX_NUM) || (inst->int_buff[0U] == MTB_PMBUS_ZONE_NONE))
            {
#if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U))
                if (inst->cfg->zone_callback != NULL)
                {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                    uint8_t ret =
                        inst->cfg->zone_callback(MTB_PMBUS_ZONE_CONFIG_WR, inst->int_buff[0U], inst->act_page);
#else
                    uint8_t ret = inst->cfg->zone_callback(MTB_PMBUS_ZONE_CONFIG_WR, inst->int_buff[0U], 0U);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                    status = (ret == 0U) ? false : true;
                }
#endif /* #if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U)) */
            }
            else
            {
                /* If Write Zone number is out of scope, then drop the data */
                inst->errors |= MTB_PMBUS_ERR_ZONE_VALUE_OUT_OF_RANGE;
                MTB_PMBUS_LOG_WRN("The WRITE ZONE num is out of range [%x]", inst->int_buff[0U]);
            }
        }
        /* The second byte is received */
        else if (inst->byte_received == 1U)
        {
            if ((inst->int_buff[1U] <= MTB_PMBUS_ZONE_MAX_NUM) || (inst->int_buff[1U] == MTB_PMBUS_ZONE_NONE))
            {
#if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U))
                if (inst->cfg->zone_callback != NULL)
                {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                    uint8_t ret =
                        inst->cfg->zone_callback(MTB_PMBUS_ZONE_CONFIG_RD, inst->int_buff[1U], inst->act_page);
#else
                    uint8_t ret = inst->cfg->zone_callback(MTB_PMBUS_ZONE_CONFIG_RD, inst->int_buff[1U], 0U);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                    status = (ret == 0U) ? false : true;
                }
                if (status)
#endif /* #if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U)) */
                {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                    inst->write_zones[inst->act_page] = inst->int_buff[0U];
                    MTB_PMBUS_LOG_DBG("New WRITE ZONE [%x] for page [%x]", inst->int_buff[0U], inst->act_page);
                    inst->read_zones[inst->act_page] = inst->int_buff[1U];
                    MTB_PMBUS_LOG_DBG("New READ ZONE [%x] for page [%x]", inst->int_buff[1U], inst->act_page);
#else
                    inst->write_zones[0U] = inst->int_buff[0U];
                    MTB_PMBUS_LOG_DBG("New WRITE ZONE [%x]", inst->int_buff[0U]);
                    inst->read_zones[0U] = inst->int_buff[1U];
                    MTB_PMBUS_LOG_DBG("New READ ZONE [%x]", inst->int_buff[1U]);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                }
            }
            else
            {
                /* If Write Zone number is out of scope, then drop the data */
                inst->errors |= MTB_PMBUS_ERR_ZONE_VALUE_OUT_OF_RANGE;
                MTB_PMBUS_LOG_WRN("The READ ZONE num is out of range [%x]", inst->int_buff[0U]);
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing, not supported event for this command */
    }

    return status;
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U)
static bool mtb_pmbus_int_impl_cmd_zone_act(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    bool status = true;

    if (MTB_PMBUS_CMD_READ_REQ == event)
    {
        inst->int_buff[0U] = inst->act_write_zone;
        inst->int_buff[1U] = inst->act_read_zone;
        inst->byte_to_send = MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_SIZE;
    }
    else if (MTB_PMBUS_CMD_WRITE_BYTE == event)
    {
        /* The first byte is received */
        if (inst->byte_received == 0U)
        {
            if ((inst->int_buff[0U] <= MTB_PMBUS_ZONE_MAX_NUM) || (inst->int_buff[0U] == MTB_PMBUS_ZONE_ALL))
            {
#if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U))
                if (inst->cfg->zone_callback != NULL)
                {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                    uint8_t ret =
                        inst->cfg->zone_callback(MTB_PMBUS_ZONE_ACTIVE_WR, inst->int_buff[0U], inst->act_page);
#else
                    uint8_t ret = inst->cfg->zone_callback(MTB_PMBUS_ZONE_ACTIVE_WR, inst->int_buff[0U], 0U);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                    status = (ret == 0U) ? false : true;
                }
#endif /* #if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U)) */
            }
            else
            {
                /* If Write Zone number is out of scope, then drop the data */
                inst->errors |= MTB_PMBUS_ERR_ZONE_VALUE_OUT_OF_RANGE;
                MTB_PMBUS_LOG_WRN("The WRITE ZONE num is out of range [%x]", inst->int_buff[0U]);
            }
        }
        /* The second byte is received */
        else if (inst->byte_received == 1U)
        {
            if ((inst->int_buff[1U] <= MTB_PMBUS_ZONE_MAX_NUM) || (inst->int_buff[1U] == MTB_PMBUS_ZONE_ALL))
            {
#if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U))
                if (inst->cfg->zone_callback != NULL)
                {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                    uint8_t ret =
                        inst->cfg->zone_callback(MTB_PMBUS_ZONE_ACTIVE_RD, inst->int_buff[1U], inst->act_page);
#else
                    uint8_t ret = inst->cfg->zone_callback(MTB_PMBUS_ZONE_ACTIVE_RD, inst->int_buff[1U], 0U);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                    status = (ret == 0U) ? false : true;
                }
                if (status)
#endif /* #if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U)) */
                {
                    inst->act_write_zone  = inst->int_buff[0U];
                    MTB_PMBUS_LOG_DBG("New active WRITE ZONE [%x]", inst->int_buff[0U]);
                    inst->act_read_zone  = inst->int_buff[1U];
                    MTB_PMBUS_LOG_DBG("New active READ ZONE [%x]", inst->int_buff[1U]);
                }
            }
            else
            {
                /* If Write Zone number is out of scope, then drop the data */
                inst->errors |= MTB_PMBUS_ERR_ZONE_VALUE_OUT_OF_RANGE;
                MTB_PMBUS_LOG_WRN("The READ ZONE num is out of range [%x]", inst->int_buff[0U]);
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing, not supported event for this command */
    }

    return status;
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U) */

#if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS != 0U)
static bool mtb_pmbus_int_impl_cmd_page_plus_common(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event, bool is_write)
{
    bool status = true;

    if (MTB_PMBUS_CMD_WRITE_BYTE == event)
    {
        if (inst->int_buff[0U] < MTB_PMBUS_PAGE_PLUS_MIN_BLOCK_SIZE)
        {
            status = false;
            MTB_PMBUS_LOG_WRN("PAGE_PLUS_%s command, block count is less than 2: %x",
                              is_write ? "WRITE" : "READ", inst->int_buff[0U]);
        }
        else if (inst->byte_received == MTB_PMBUS_PAGE_PLUS_SUP_DATA_SIZE)
        {
            if (!((inst->int_buff[1U] < inst->cfg->num_pages) || (inst->int_buff[1U] == MTB_PMBUS_PAGE_ALL)))
            {
                /* If Page number is out of scope, then drop the data */
                inst->errors |= MTB_PMBUS_ERR_INVALID_PAGE;
                MTB_PMBUS_LOG_WRN("The PAGE num is out of range [%x]", inst->int_buff[1U]);
            }
            else
            {
#if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U))
                if (inst->cfg->page_callback != NULL)
                {
                    uint8_t ret = inst->cfg->page_callback(MTB_PMBUS_CMD_PAGE_PLUS, is_write, inst->int_buff[1U]);
                    status = (ret == 0U) ? false : true;
                }
                if (status)
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U)) */
                {
                    /* Store active page and replace it with the new one */
                    inst->state |= MTB_PMBUS_STATE_PAGE_PLUS_CMD;
                    MTB_PMBUS_LOG_DBG("PAGE_PLUS_%s COMMAND - SET STATE",
                                      is_write ? "WRITE" : "READ");
                    inst->stored_page = inst->act_page;
                    inst->act_page    = inst->int_buff[1U];
                    MTB_PMBUS_LOG_DBG("PAGE_PLUS_%s command, new PAGE [%x]",
                                      is_write ? "WRITE" : "READ", inst->act_page);
                    /* Reset the state for upcoming command */
                    inst->state &= ~(MTB_PMBUS_STATE_CMD_MATCH | MTB_PMBUS_STATE_IMPL_CMD);
                    inst->active_cmd_tbl = inst->cfg->cmd_table;
                    inst->active_lookup_tbl = inst->cmd_lookup_tbl;
                    inst->is_cmd_block = false;
                }
            }
        }
        else
        {
            /* Do nothing */
        }
    }

    return status;
}
#endif /* #if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE != 0U)
static bool mtb_pmbus_int_impl_cmd_page_plus_write(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    return mtb_pmbus_int_impl_cmd_page_plus_common(inst, event, true);
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ != 0U)
static bool mtb_pmbus_int_impl_cmd_page_plus_read(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    return mtb_pmbus_int_impl_cmd_page_plus_common(inst, event, false);
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ != 0U) */

#if (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U)
static bool mtb_pmbus_int_impl_cmd_p2_plus_common(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event, bool is_write)
{
    bool status = true;
#if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U))
    bool status_page = true;
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U)) */
#if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U))
    bool status_phase = true;
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U)) */

    if (MTB_PMBUS_CMD_WRITE_BYTE == event)
    {
        if (inst->int_buff[0U] < MTB_PMBUS_P2_PLUS_MIN_BLOCK_SIZE)
        {
            status = false;
            MTB_PMBUS_LOG_WRN("P2_PLUS_%s command, block count is less than 3: %x",
                              is_write ? "WRITE" : "READ", inst->int_buff[0U]);
        }
        else if (inst->byte_received == MTB_PMBUS_P2_PLUS_SUP_DATA_SIZE)
        {
            if (!((inst->int_buff[2U] < inst->cfg->num_phases) || (inst->int_buff[2U] == MTB_PMBUS_PHASE_ALL)))
            {
                /* If Phase number is out of scope, then drop the data */
                inst->errors |= MTB_PMBUS_ERR_INVALID_PHASE;
                MTB_PMBUS_LOG_WRN("The PHASE num is out of range [%x]", inst->int_buff[2U]);
            }
            else if (!((inst->int_buff[1U] < inst->cfg->num_pages) || (inst->int_buff[1U] == MTB_PMBUS_PAGE_ALL)))
            {
                /* If Page number is out of scope, then drop the data */
                inst->errors |= MTB_PMBUS_ERR_INVALID_PAGE;
                MTB_PMBUS_LOG_WRN("The PAGE num is out of range [%x]", inst->int_buff[1U]);
            }
            else
            {
                /* Store active page and phase and replace them with the new ones */
                inst->state |= MTB_PMBUS_STATE_P2_PLUS_CMD;
                MTB_PMBUS_LOG_DBG("P2_PLUS_%s COMMAND - SET STATE",
                                  is_write ? "WRITE" : "READ");
#if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U))
                if (inst->cfg->page_callback != NULL)
                {
                    uint8_t ret = inst->cfg->page_callback(MTB_PMBUS_CMD_PAGE_P2_PLUS, is_write, inst->int_buff[1U]);
                    status_page = (ret == 0U) ? false : true;
                }
                if (status_page)
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U)) */
                {
                    inst->stored_page = inst->act_page;
                    inst->act_page    = inst->int_buff[1U];
                    MTB_PMBUS_LOG_DBG("P2_PLUS_%s command, new PAGE [%x]",
                                      is_write ? "WRITE" : "READ", inst->act_page);
                }
#if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U))
                if (inst->cfg->phase_callback != NULL)
                {
                    uint8_t ret = inst->cfg->phase_callback(MTB_PMBUS_CMD_PHASE_P2_PLUS, is_write, inst->int_buff[2U]);
                    status_phase = (ret == 0U) ? false : true;
                }
                if (status_phase)
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U)) */
                {
                    inst->stored_phase = inst->act_phase;
                    inst->act_phase    = inst->int_buff[2U];
                    MTB_PMBUS_LOG_DBG("P2_PLUS_%s command, new PHASE [%x]",
                                      is_write ? "WRITE" : "READ", inst->act_phase);
                }
                /* Reset the state for upcoming command */
                inst->state &= ~(MTB_PMBUS_STATE_CMD_MATCH | MTB_PMBUS_STATE_IMPL_CMD);
                inst->active_cmd_tbl = inst->cfg->cmd_table;
                inst->active_lookup_tbl = inst->cmd_lookup_tbl;
                inst->is_cmd_block = false;
            }
        }
        else
        {
            /* Do nothing */
        }
    }

    return (status
#if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U))
            && status_page
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U)) */
#if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U))
            && status_phase
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U)) */
            );
}
#endif /* #if (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE != 0U)
static bool mtb_pmbus_int_impl_cmd_p2_plus_write(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    return mtb_pmbus_int_impl_cmd_p2_plus_common(inst, event, true);
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ != 0U)
static bool mtb_pmbus_int_impl_cmd_p2_plus_read(mtb_pmbus_stc_t *inst, mtb_pmbus_cmd_events_t event)
{
    return mtb_pmbus_int_impl_cmd_p2_plus_common(inst, event, false);
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ != 0U) */

#if (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U)
static bool mtb_pmbus_int_handle_p_plus_cmd(mtb_pmbus_stc_t *inst)
{
    bool status = false;
#if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS != 0U)
    bool is_page_plus = ((inst->state & MTB_PMBUS_STATE_PAGE_PLUS_CMD) != 0U);

    if (is_page_plus)
    {
        if ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) == 0U)
        {
            /* If the command does not support PAGE, then it is not allowed to use PAGE_PLUS command
             */
            inst->errors |= MTB_PMBUS_ERR_UNSUPPORTED_CMD;
            MTB_PMBUS_LOG_WRN("Cmd [%x] does not support PAGE and cannot be used with PAGE_PLUS command",
                              inst->active_cmd_tbl[inst->cmd_pos].cmd_code);
        }
        else if (inst->is_cmd_block &&
                 ((inst->int_buff[0] - MTB_PMBUS_PAGE_PLUS_MIN_BLOCK_SIZE) >
                  inst->active_cmd_tbl[inst->cmd_pos].data_size))
        {
            /* Byte count exceed length for embedded block command */
            inst->errors |= MTB_PMBUS_ERR_BYTE_COUNT_TOO_BIG;
            MTB_PMBUS_LOG_WRN("Count byte [%x] exceed length for embedded cmd",
                              inst->int_buff[0] - MTB_PMBUS_PAGE_PLUS_MIN_BLOCK_SIZE);
        }
        else
        {
            status = true;
        }
    }
    else
#endif /* #if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS != 0U) */
    {
#if (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U)
        if (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) == 0U) ||
            ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) == 0U))
        {
            /* If the command does not support PAGE and PHASE, then it is not allowed to use P2_PLUS
               command */
            inst->errors |= MTB_PMBUS_ERR_UNSUPPORTED_CMD;
            MTB_PMBUS_LOG_WRN("Cmd [%x] does not support PAGE or PHASE and cannot be used with P2_PLUS command",
                              inst->active_cmd_tbl[inst->cmd_pos].cmd_code);
        }
        else if (inst->is_cmd_block &&
                 ((inst->int_buff[0] - MTB_PMBUS_P2_PLUS_MIN_BLOCK_SIZE) >
                  inst->active_cmd_tbl[inst->cmd_pos].data_size))
        {
            /* Byte count exceed length for embedded block command */
            inst->errors |= MTB_PMBUS_ERR_BYTE_COUNT_TOO_BIG;
            MTB_PMBUS_LOG_WRN("Count byte [%x] exceed length for embedded cmd",
                              inst->int_buff[0] - MTB_PMBUS_P2_PLUS_MIN_BLOCK_SIZE);
        }
        else
        {
            status = true;
        }
#endif /* #if (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U) */
    }

    if (status)
    {
        if (!inst->is_cmd_block)
        {
            inst->byte_received = 0U;
        }
        else
        {
#if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS != 0U)
            if (is_page_plus)
            {
                inst->byte_requested = (uint16_t)inst->int_buff[0U] - MTB_PMBUS_PAGE_PLUS_SUP_DATA_SIZE;
            }
            else
#endif /* #if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS != 0U) */
            {
#if (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U)
                inst->byte_requested = (uint16_t)inst->int_buff[0U] - MTB_PMBUS_P2_PLUS_SUP_DATA_SIZE;
#endif /* #if (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U) */
            }
            inst->byte_received = 1U;
        }
    }

    return status;
}
#endif /* #if (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U) */

void mtb_pmbus_int_reset_state(mtb_pmbus_stc_t *inst)
{
    if ((inst->cfg->errors_callback != NULL) && (inst->errors != 0U))
    {
#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
        bool is_cmd_ext = (((inst->state & MTB_PMUBS_STATE_CMD_MATCH_EXT) != 0U) ? true : false);
        inst->cfg->errors_callback(inst->errors, inst->cmd_code, is_cmd_ext);
#else
        inst->cfg->errors_callback(inst->errors, inst->cmd_code, false);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
    }

#if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS != 0U)
    if ((inst->state & MTB_PMBUS_STATE_PAGE_PLUS_CMD) != 0U)
    {
        /* If the PAGE_PLUS command was active, then restore the page */
        inst->act_page = inst->stored_page;
        MTB_PMBUS_LOG_DBG("PAGE_PLUS command, restore PAGE [%x]", inst->act_page);
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS != 0U) */
#if (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U)
    if ((inst->state & MTB_PMBUS_STATE_P2_PLUS_CMD) != 0U)
    {
        /* If the P2_PLUS command was active, then restore the page and phase */
        inst->act_page = inst->stored_page;
        inst->act_phase = inst->stored_phase;
        MTB_PMBUS_LOG_DBG("P2_PLUS command, restore PAGE [%x] PHASE [%x]", inst->act_page, inst->act_phase);
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U) */

#if ((defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)))
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
    if ((inst->state & MTB_PMBUS_STATE_ZONE_READ) != 0U)
    {
        inst->act_page = inst->stored_page;
    }
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */


    if (
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
        (0U == (inst->state & MTB_PMBUS_STATE_IMPL_CMD)) &&
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
        (NULL != inst->active_lookup_tbl))
    {
        if (0U != (inst->active_lookup_tbl[inst->cmd_code].flags & (uint8_t)MTB_PMBUS_CMD_FLAG_IS_PAUSED))
        {
            /* Resume TX to release the bus */
            (void)mtb_pmbus_hal_resume_tx(inst);
            MTB_PMBUS_LOG_DBG("Resuming paused command [%x]", inst->cmd_code);
        }
        /* Reset the command active and paused flags */
        inst->active_lookup_tbl[inst->cmd_code].flags &= ~((uint8_t)MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE |
                                                           (uint8_t)MTB_PMBUS_CMD_FLAG_IS_PAUSED);
    }

    /* Reset state machine */
    inst->state = 0U;
    inst->byte_received = 0U;
    inst->byte_sent = 0U;
    inst->cmd_pos = 0U;
    inst->cmd_code = 0U;
    inst->byte_requested = 0U;
    inst->byte_to_send = 0U;
    inst->errors = 0U;
    inst->is_cmd_block = false;
    inst->is_pause_allowed = false;
    inst->cmd_code_full = 0U;
    inst->active_cmd_tbl = NULL;
    inst->active_lookup_tbl = NULL;
#if defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)
    inst->crc = 0U;
#endif /* #if defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
    inst->cmd_ctrl_byte = 0U;
    inst->zone_sts_mask = 0U;
    inst->zone_pages = 0U;
    inst->num_available_zone = 0U;
    inst->zone_is_read = false;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

    /* Add empty line to logs */
    MTB_PMBUS_LOG_DBG("\n\r");
}


void mtb_pmbus_int_cmd_handle_tx(mtb_pmbus_stc_t *inst)
{
    /* The handler of the case when the command supports write and process call, but gets a read
       request. */
    if (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_WR) != 0U)
        && ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) != 0U)
        && (inst->byte_received == 0U))
    {
        inst->errors |= MTB_PMBUS_ERR_REQ_RD_FOR_PC_ONLY;
        MTB_PMBUS_LOG_WRN("Cmd [%x] doesn't support read transaction", inst->active_cmd_tbl[inst->cmd_pos].cmd_code);
    }
    /* The handler of the case when the command is write-only (no DIR_RD, no DIR_PROCESS_CALL) */
    else if (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_RD) == 0U)
             && ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) == 0U))
    {
        inst->errors |= MTB_PMBUS_ERR_REQ_RD_FOR_WR_ONLY;
        MTB_PMBUS_LOG_WRN("Cmd [%x] doesn't support read transaction", inst->active_cmd_tbl[inst->cmd_pos].cmd_code);
    }
    else
    {
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
        if ((inst->state & MTB_PMBUS_STATE_IMPL_CMD) != 0U)
        {
            (void)mtb_pmbus_int_impl_cmd_handler(inst, MTB_PMBUS_CMD_READ_REQ);
        }
        else
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
        {
            uint8_t size = 0U;
            /* Select the page count */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
            int32_t page =
                ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) ==
                 0U) ? MTB_PMBUS_NO_PAGE_PHASE : (int32_t)inst->act_page;
#else
            int32_t page = MTB_PMBUS_NO_PAGE_PHASE;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
            int32_t phase =
                ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) ==
                 0U) ? MTB_PMBUS_NO_PAGE_PHASE : (int32_t)inst->act_phase;
#else
            int32_t phase = MTB_PMBUS_NO_PAGE_PHASE;
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */

            /* Call the command callback */
            if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
            {
                /* Ignore the return status as it is not applicable for this event */
                (void)inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_READ_REQ,
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                                                   (int32_t)inst->act_page,
#else
                                                                   MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                                                                   (int32_t)inst->act_phase,
#else
                                                                   MTB_PMBUS_NO_PAGE_PHASE,
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U))
                                                                   0U, 0U);
#else
                                                                   0U);
#endif /* #if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U)) */
            }

            if (inst->is_cmd_block)
            {
                (void)mtb_pmbus_cmd_get_transfer_size_ext_isr(inst, inst->cmd_code_full, page, phase, &size);
                inst->int_buff[0U] = size;
            }
            inst->byte_to_send =
                inst->is_cmd_block ? ((uint16_t)(size) +
                                      MTB_PMBUS_CMD_BLOCK_EXTRA_BYTE) : (uint16_t)inst->active_cmd_tbl[inst->cmd_pos].
                data_size;
            /* Clear the sent bytes number, required only for Zone Read */
            inst->byte_sent = 0U;

            MTB_PMBUS_LOG_DBG("Read data for page [%" PRIi32 "] phase [%" PRIi32 "]", page, phase);
            (void)mtb_pmbus_cmd_read_data_ext_isr(inst, inst->cmd_code_full, page, phase,
                                                  inst->int_buff + (uint8_t)(inst->is_cmd_block),
                                                  (uint32_t)(inst->byte_to_send) - (uint32_t)(inst->is_cmd_block));
        }
    }
}


#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
static uint8_t mtb_pmbus_int_get_num_available_zone(mtb_pmbus_stc_t *inst)
{
    uint8_t num_zones = 0U;
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
    if (inst->cfg->num_pages > 0U)
    {
        for (uint8_t i = 0U; i < inst->cfg->num_pages; i++)
        {
            if ((inst->read_zones[i] == inst->act_read_zone) ||
                ((inst->act_read_zone == MTB_PMBUS_ZONE_ALL) && (inst->read_zones[i] != MTB_PMBUS_ZONE_NONE)))
            {
                num_zones++;
            }
        }
    }
    else
    {
        if ((inst->read_zones[0U] == inst->act_read_zone) ||
            ((inst->act_read_zone == MTB_PMBUS_ZONE_ALL) && (inst->read_zones[0U] != MTB_PMBUS_ZONE_NONE)))
        {
            num_zones = 1U;
        }
    }
#else
    if ((inst->read_zones[0U] == inst->act_read_zone) ||
        ((inst->act_read_zone == MTB_PMBUS_ZONE_ALL) && (inst->read_zones[0U] != MTB_PMBUS_ZONE_NONE)))
    {
        num_zones = 1U;
    }
#endif // if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))

    return num_zones;
}


#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
static uint8_t mtb_pmbus_int_get_next_page2actzone(mtb_pmbus_stc_t *inst)
{
    uint8_t next_page = 0U;
    uint8_t i = 0U;
    if (inst->zone_pages > 0U)
    {
        i = inst->act_page + 1U; /* Start from the next page */
    }

    for (; i < inst->cfg->num_pages; i++)
    {
        if ((inst->read_zones[i] == inst->act_read_zone) ||
            ((inst->act_read_zone == MTB_PMBUS_ZONE_ALL) && (inst->read_zones[i] != MTB_PMBUS_ZONE_NONE)))
        {
            next_page = i;
            break;
        }
    }
    return next_page;
}
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
