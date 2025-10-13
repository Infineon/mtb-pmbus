/***************************************************************************//**
* \file mtb_pmbus_isr.c
* \version 1.0
*
* Provides ISR handler for the PMBus Middleware.
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

#include "mtb_pmbus_int.h"

/* The Target Address is matched in the first time */
#define MTB_PMBUS_STATE_TARGET_ADDR_MATCH_1     (0x01U)
/* The Target Address is matched in the second time. The transfer direction is changed */
#define MTB_PMBUS_STATE_TARGET_ADDR_MATCH_2     (0x02U)
/* The Received byte protocol is initiated by Controller */
#define MTB_PMBUS_STATE_RECEIVED_BYTE_PROT      (0x04U)
/* The received command is matched */
#define MTB_PMBUS_STATE_CMD_MATCH               (0x08U)
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
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
#if defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)
/* The General Call address is received */
#define MTB_PMBUS_STATE_GENERAL_CALL_ADDR       (0x40000U)
#endif /* #if defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U) */
/* The command should be ignored */
#define MTB_PMBUS_CMD_IS_IGNORED                (0x80000U)

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

#define MTB_PMBUS_ACK_BIT                       (true)
#define MTB_PMBUS_NACK_BIT                      (false)

#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
__STATIC_INLINE void mtb_pmbus_int_crc_calc(mtb_pmbus_stc_t * inst, uint8_t byte);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */

static void mtb_pmbus_int_update_data(mtb_pmbus_stc_t * inst);

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
static void mtb_pmbus_int_reverse_byte_order(uint8_t *array, size_t length);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
static bool mtb_pmbus_int_impl_cmd_handler(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event);
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)
static bool mtb_pmbus_int_impl_cmd_rev(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)
static bool mtb_pmbus_int_impl_cmd_capability(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U)
static bool mtb_pmbus_int_impl_cmd_query(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U)
static bool mtb_pmbus_int_impl_cmd_page(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U)
static bool mtb_pmbus_int_impl_cmd_phase(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U)
static bool mtb_pmbus_int_impl_cmd_zone_cfg(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U)
static bool mtb_pmbus_int_impl_cmd_zone_act(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event);
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U) */

void mtb_pmbus_int_cmd_handle_tx(mtb_pmbus_stc_t * inst);
void mtb_pmbus_int_reset_state(mtb_pmbus_stc_t * inst);

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
static uint8_t mtb_pmbus_int_get_num_available_zone(mtb_pmbus_stc_t * inst);
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
static uint8_t mtb_pmbus_int_get_next_page2actzone(mtb_pmbus_stc_t * inst);
static void mtb_pmbus_int_set_active_zone_page(mtb_pmbus_stc_t * inst, bool store_page);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */


#if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U))
bool mtb_pmbus_handle_gen_call(mtb_pmbus_stc_t * inst)
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

bool mtb_pmbus_handle_addr(mtb_pmbus_stc_t * inst, uint8_t addr)
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
                /* The handler of the case when PEC was executed in the Process Call Write Word part */
                if (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) != 0U) &&
                    ((inst->state & MTB_PMBUS_STATE_PEC_IS_EXECUTED) != 0U))
                {
                    inst->errors &= ~MTB_PMBUS_ERR_CORRUPTED_DATA;
                    inst->errors |= MTB_PMBUS_ERR_WR_TOO_MANY_BYTES;
                    MTB_PMBUS_LOG_WRN("Controller attempts to write too many bytes(Process Call with PEC in Write Word part) [%d]", inst->byte_received + 1U);
                }
                else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
                {
                    /* Store the received data in case of process call */
                    if ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) != 0U)
                    {
                        /* Check if all requested data is received */
                        if (inst->byte_received == inst->byte_requested)
                        {
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
                            if ((inst->state & MTB_PMBUS_STATE_IMPL_CMD) != 0U)
                            {
                                (void) mtb_pmbus_int_impl_cmd_handler(inst, MTB_PMBUS_CMD_WRITE_DONE);
                            }
                            else
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
                            {
                                mtb_pmbus_int_update_data(inst);
                                /* Call the command callback */
                                if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                                {
                                    (void) inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_WRITE_DONE,
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
                                                                                        0U);
                                }
                            }
                        }
                        else
                        {
                            inst->errors |= MTB_PMBUS_ERR_WR_LESS_BYTES;
                            MTB_PMBUS_LOG_WRN("Less bytes are received");
                        }
                    }

                    if (!(
                    #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                        (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) != 0U) && (inst->act_page == MTB_PMBUS_PAGE_ALL))
                    #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                        || (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) != 0U) && (inst->act_phase == MTB_PMBUS_PHASE_ALL))
                    #endif
                    #elif (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                        (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) != 0U) && (inst->act_phase == MTB_PMBUS_PHASE_ALL))
                    #else
                        0U != 0U
                    #endif
                    ))
                    {
                        mtb_pmbus_int_cmd_handle_tx(inst);
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
             * applicable only for received byte protocol or Quick command*/
            if (dir)
            {
#if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U))
                if (inst->cfg->enable_pmbus)
                {
                    inst->errors |= MTB_PMBUS_ERR_ADDR_WITH_RD_BIT;
                    MTB_PMBUS_LOG_WRN("The protocol can not be started with Target Address and RD direction. (Only for PMBus)");
                }
                else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */
                {
                    MTB_PMBUS_LOG_DBG("Received byte protocol");
                    inst->state |= MTB_PMBUS_STATE_DIR_RD | MTB_PMBUS_STATE_RECEIVED_BYTE_PROT;

                    /* Call the User callback to update data before sending */
                    if(inst->cfg->gen_callback != NULL)
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
#if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)
    else if (((addr & MTB_PMBUS_ADDR_MASK) == MTB_PMBUS_ALERT_RESPONSE_ADDR) && inst->smbalert_is_trig && inst->cfg->enable_smbalert)
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
    else if (((addr & MTB_PMBUS_ADDR_MASK) == MTB_PMBUS_ZONE_READ_ADDR) && inst->cfg->enable_zone && inst->cfg->enable_pmbus)
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
                if (inst->zone_pages > 0U)
                {
                    mtb_pmbus_int_set_active_zone_page(inst, false);
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
                                    zone_status = inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_BYTE_REQ, inst->zone_sts_mask, inst->act_page);
#else
                                    zone_status = inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_BYTE_REQ, inst->zone_sts_mask, MTB_PMBUS_NO_PAGE_PHASE);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                                }
                                else
                                {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                    zone_status = inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_WORD_REQ, inst->zone_sts_mask, inst->act_page);
#else
                                    zone_status = inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_WORD_REQ, inst->zone_sts_mask, MTB_PMBUS_NO_PAGE_PHASE);
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
#endif/* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */                        
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
                                mtb_pmbus_int_reverse_byte_order(inst->int_buff, inst->byte_to_send);
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
                            if (inst->cfg->num_pages > 0U)
                            {
                                inst->int_buff[inst->byte_to_send - 2U] = ((inst->cfg->address << 1U) | 0x01U);
                                inst->int_buff[inst->byte_to_send - 1U] = inst->act_page;
                            }
                            else
                            {
                                inst->int_buff[inst->byte_to_send - 2U] = (inst->cfg->address << 1U);
                                inst->int_buff[inst->byte_to_send - 1U] = 0xFFU;
                            }
#else
                            inst->int_buff[inst->byte_to_send - 2U] = (inst->cfg->address << 1U);
                            inst->int_buff[inst->byte_to_send - 1U] = 0xFFU;
#endif/* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
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
                mtb_pmbus_int_set_active_zone_page(inst, true);
            }
#endif/* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */

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
bool mtb_pmbus_handle_rx(mtb_pmbus_stc_t * inst, uint8_t byte)
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
            if(inst->byte_received > 2U)
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
                if ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & (MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL)) != 0U)
                {
                    /* Check if command is protected */
                    if ((inst->active_lookup_tbl[inst->cmd_code].flags & MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED) == 0U)
                    {
                        /* Check if it is a Count byte*/
                        if (inst->is_cmd_block && ((inst->state & MTB_PMBUS_STATE_GOT_BYTE_COUNT) == 0U))
                        {
                            /* Check if byte Count does not exceed cmd length */
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
                        else
                        {
                            /* Check if all data is already received */
                            if (inst->byte_received == inst->byte_requested)
                            {
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
                                /* Check if PEC is supported */
                                if (inst->cfg->enable_pec)
                                {
                                    if (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) != 0U) &&
                                    ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_WR) == 0U))
                                    {
                                        inst->errors |= MTB_PMBUS_ERR_WR_TOO_MANY_BYTES;
                                        MTB_PMBUS_LOG_WRN("Controller attempts to write too many bytes(Process Call with PEC in Write Word part) [%d]", inst->byte_received + 1U);
                                    }
                                    else if ((inst->state & MTB_PMBUS_STATE_PEC_IS_EXECUTED) == 0U)
                                    {
                                        /* Check if PEC is correct */
                                        if(byte == inst->crc)
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
                                        MTB_PMBUS_LOG_WRN("Controller attempts to write too many bytes (PEC) [%d]", inst->byte_received + 1U);
                                    }
                                }
                                else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
                                {
                                    inst->errors |= MTB_PMBUS_ERR_WR_TOO_MANY_BYTES;
                                    MTB_PMBUS_LOG_WRN("Controller attempts to write too many bytes [%d]", inst->byte_received + 1U);
                                }
                            }
                            else
                            {
                                /* Store byte into internal buffer to use it by pre-implemented commands */
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
                                                                                                   byte);
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
                            if ((inst->act_write_zone == inst->write_zones[i]) || ((inst->act_write_zone == MTB_PMBUS_ZONE_ALL) && (inst->write_zones[i] != MTB_PMBUS_ZONE_NONE)))
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
                        if ((inst->act_write_zone == inst->write_zones[0U]) || ((inst->act_write_zone == MTB_PMBUS_ZONE_ALL) && (inst->write_zones[0U] != MTB_PMBUS_ZONE_NONE)))
                        {
                            pages_is_act = true;
                            MTB_PMBUS_LOG_DBG("Target has active zone [%x]", inst->act_write_zone);
                        }
                    }
                }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) && (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                /* Check if implemented commands are present */
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
                if ((inst->cfg->impl_cmd_mask != 0U) && inst->cfg->enable_pmbus)
                {
                    /* Find the implemented command */
                    for (uint8_t i = 0U; i < MTB_PMBUS_IMPL_CMD_NUM; i++)
                    {
                        if ((inst->pre_impl_cmd_table[i].cmd_code == byte) && 
                           ((inst->pre_impl_cmd_lookup_tbl[i].flags & (MTB_PMBUS_CMD_FLAG_IS_PRESENT | MTB_PMBUS_CMD_FLAG_IS_ENABLED)) == 
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

                                (void) mtb_pmbus_int_impl_cmd_handler(inst, MTB_PMBUS_CMD_MATCH);

                                /* Send ACK */
                                response = true;
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
                    if(((inst->state & MTB_PMBUS_STATE_ZONE_WRITE) == 0U) || (pages_is_act))
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
                    {
                        if ((inst->active_lookup_tbl[byte].flags & MTB_PMBUS_CMD_FLAG_IS_PRESENT) != 0U)
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
                                    (void) inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_MATCH,
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
                                                                                        0U);
                                    if ((inst->active_lookup_tbl[byte].flags & MTB_PMBUS_CMD_FLAG_IS_ENABLED) == 0U)
                                    {
                                        /* Send NACK on command code if user disable it in the c */
                                        response = false;
                                    }
                                }

                                if (response)
                                {
                                    /* Update State Machine: Cmd code is received */
                                    inst->state |= MTB_PMBUS_STATE_CMD_MATCH;

                                    inst->active_lookup_tbl[inst->cmd_code].flags |= MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE;
                                    
                                    inst->cmd_code = byte;
                                    inst->cmd_code_full = inst->cmd_code;

                                    inst->byte_requested = inst->active_cmd_tbl[inst->cmd_pos].data_size;

                                    if ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_BLOCK) != 0U)
                                    {
                                        inst->is_cmd_block = true;
                                    }
                                    {
                                        MTB_PMBUS_LOG_DBG("Cmd [%x] is accepted", byte);
                                    }
                                }
                                /* Command is ignored */
                                else
                                {
                                    inst->state |= MTB_PMBUS_CMD_IS_IGNORED;
                                    MTB_PMBUS_LOG_WRN("Cmd [%x] is not enabled (Callback)", byte);

                                }
                            }
                            else
                            {
                                inst->state |= MTB_PMBUS_CMD_IS_IGNORED;
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

void mtb_pmbus_handle_restart(mtb_pmbus_stc_t * inst)
{
    MTB_PMBUS_LOG_DBG("RE");

    /* The Re-Start condition can be handled only if the target address
     * are received
     */
    if ((inst->state & MTB_PMBUS_STATE_TARGET_ADDR_MATCH_1) != 0U)
    {
        /* Reset the number of requested byte for case when the command supports read and process call and
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
    #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                (void)inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_CMPLT, inst->zone_sts_mask, inst->act_page);
    #else
                (void)inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_CMPLT, inst->zone_sts_mask, MTB_PMBUS_NO_PAGE_PHASE);
    #endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                inst->zone_pages++;
            }
            else
            {
                inst->errors &= ~MTB_PMBUS_ERR_ARB_LOST;
                if ((inst->state & MTB_PMBUS_STATE_ZONE_RESP_CONTINUE) == 0U)
                {
                    inst->zone_pages++;               
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
    inst->state &= ~ (MTB_PMBUS_STATE_DIR_WR | MTB_PMBUS_STATE_DIR_RD);
}

void mtb_pmbus_handle_tx(mtb_pmbus_stc_t * inst, uint8_t ** data_ptr, uint16_t * size)
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

void mtb_pmbus_handle_stop(mtb_pmbus_stc_t * inst)
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
                            if(inst->cfg->gen_callback != NULL)
                            {
                                inst->cfg->gen_callback(MTB_PMBUS_ALERT_RESPONSE_ADDR_EVENT);
                            }

                            if(inst->smbalert_mode == MTB_PMBUS_SMBALERT_MODE_AUTO)
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
                                if(inst->cfg->gen_callback != NULL)
                                {
                                    inst->cfg->gen_callback(MTB_PMBUS_RECEIVE_BYTE_CMPLT_EVENT);
                                }
                            }
                            else
                            {
                                if(inst->cfg->gen_callback != NULL)
                                {
                                    inst->cfg->gen_callback(MTB_PMBUS_QUICK_CMD_RD_EVENT);
                                }
                            }
                        }
                        else
                        {
                            if(inst->cfg->gen_callback != NULL)
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
                                (void) mtb_pmbus_int_impl_cmd_handler(inst, MTB_PMBUS_CMD_READ_DONE);
                            }
                            else
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
                            {
                                /* Call the command callback */
                                if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                                {
                                    /* Ignore the return status as it is not applicable for this event */
                                    (void) inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_READ_DONE,
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
                                                                                        0U);
                                    inst->active_lookup_tbl[inst->cmd_code].flags |= MTB_PMBUS_CMD_FLAG_IS_TRANSMITTED;
                                }
                            }
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
                            if (((inst->state & MTB_PMBUS_STATE_ZONE_READ) != 0U) && !inst->zone_is_read)
                            {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                                (void)inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_CMPLT, inst->zone_sts_mask, inst->act_page);
#else
                                (void)inst->cfg->zone_callback(MTB_PMBUS_ZONE_READ_STATUS_CMPLT, inst->zone_sts_mask, MTB_PMBUS_NO_PAGE_PHASE);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                            }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
                        }
                    }
                    else
                    {
                        /* The handler of the case when the command supports read and process call, but gets a write request. */
                        if (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_RD) != 0U)
                            && ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) != 0U))
                        {
                            inst->errors |= MTB_PMBUS_ERR_REQ_WR_FOR_PC_ONLY;
                            MTB_PMBUS_LOG_WRN("Cmd [%x] doesn't support write transaction", inst->active_cmd_tbl[inst->cmd_pos].cmd_code);
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
                                    (void) mtb_pmbus_int_impl_cmd_handler(inst, MTB_PMBUS_CMD_WRITE_DONE);
                                }
                                else
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
                                {
                                    mtb_pmbus_int_update_data(inst);
                                    /* Call the command callback */
                                    if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                                    {
                                        /* Ignore the return status as it is not applicable for this event */
                                        (void) inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_WRITE_DONE,
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
                                                                                            0U);
                                        inst->active_lookup_tbl[inst->cmd_code].flags |= MTB_PMBUS_CMD_FLAG_IS_TRANSMITTED;
                                    }
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
                        inst->active_lookup_tbl[inst->cmd_code].flags &= (uint8_t)(~MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE);
                        /* Clear state to allow user use command APIs inside of this callback */
                        inst->state = 0U;
                        /* Call the command callback */
                        if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
                        {
                            (void) inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_DONE,
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
                                                                                0U);
                        }
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

            /* Clear the arbitration lost event if the MTB_PMBUS_STATE_RECEIVED_BYTE_PROT
                * is set, as these two conditions means that Quick Command with Read direction
                * is occurred on the Bus */
            if ((inst->errors == MTB_PMBUS_ERR_ARB_LOST) && ((inst->state & MTB_PMBUS_STATE_RECEIVED_BYTE_PROT) != 0U) && (!mtb_pmbus_hal_is_tx_cmpl(inst)))
            {
                inst->errors &= ~MTB_PMBUS_ERR_ARB_LOST;
                if(inst->cfg->gen_callback != NULL)
                {
                    inst->cfg->gen_callback(MTB_PMBUS_QUICK_CMD_RD_EVENT);
                }
            }
        }

        mtb_pmbus_int_reset_state(inst);
    }
}

void mtb_pmbus_handle_arb_lost(mtb_pmbus_stc_t * inst)
{
    inst->errors |= MTB_PMBUS_ERR_ARB_LOST;

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
    if ((inst->state & MTB_PMBUS_STATE_ZONE_READ) != 0U)
    {
        if (!inst->zone_is_read)
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
                                                        0xFFU);
        }
    }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */ 
}

void mtb_pmbus_handle_bus_error(mtb_pmbus_stc_t * inst)
{
    MTB_PMBUS_LOG_WRN("Bus error");
    inst->errors |= MTB_PMBUS_ERR_BUS_ERROR;
    mtb_pmbus_int_reset_state(inst);
}

#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) 

void mtb_pmbus_handle_timeout(mtb_pmbus_stc_t * inst)
{
    MTB_PMBUS_LOG_WRN("25 ms Timeout");
    inst->errors |= MTB_PMBUS_ERR_TIMEOUT;
    mtb_pmbus_int_reset_state(inst);
}
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */

static void mtb_pmbus_int_update_data(mtb_pmbus_stc_t * inst)
{
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
    int32_t page = ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) == 0U) ? MTB_PMBUS_NO_PAGE_PHASE : (int32_t)inst->act_page;
#else
    int32_t page = MTB_PMBUS_NO_PAGE_PHASE;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
    int32_t phase = ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) == 0U) ? MTB_PMBUS_NO_PAGE_PHASE : (int32_t)inst->act_phase;
#else
    int32_t phase = MTB_PMBUS_NO_PAGE_PHASE;
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */

#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) && (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
    if ((page == (int32_t)MTB_PMBUS_PAGE_ALL) && (phase == (int32_t)MTB_PMBUS_PHASE_ALL))
    {
        for(int32_t i = 0; i < (int32_t)inst->cfg->num_pages; i++)
        {
            for(int32_t j = 0; j < (int32_t)inst->cfg->num_phases; j++)
            {
                MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", i, j);
                (void) mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, i, j, inst->int_buff + (uint8_t)(inst->is_cmd_block), (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
            }
        }
    }
    else if (page == (int32_t)MTB_PMBUS_PAGE_ALL)
    {
        for(int32_t i = 0; (i < (int32_t)inst->cfg->num_pages); i++)
        {
            MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", i, phase);
            (void) mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, i, phase, inst->int_buff + (uint8_t)(inst->is_cmd_block), (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
        }
    }
    else if (phase == (int32_t)MTB_PMBUS_PHASE_ALL)
    {
        for(int32_t i = 0; i < (int32_t)inst->cfg->num_phases; i++)
        {
            MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", page, i);
            (void) mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, page, i, inst->int_buff + (uint8_t)(inst->is_cmd_block), (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
        }
    }
    else
#elif (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
    if (page == (int32_t)MTB_PMBUS_PAGE_ALL)
    {
        for(int32_t i = 0; i < (int32_t)inst->cfg->num_pages; i++)
        {
            MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", i, phase);
            (void) mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, i, phase, inst->int_buff + (uint8_t)(inst->is_cmd_block), (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
        }
    }
    else
#elif (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
    if (phase == (int32_t)MTB_PMBUS_PHASE_ALL)
    {
        for(int32_t i = 0; i < (int32_t)inst->cfg->num_phases; i++)
        {
            MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", page, i);
            (void) mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, page, i, inst->int_buff + (uint8_t)(inst->is_cmd_block), (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
        }
    }
    else
#endif
    {
#if ((defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) && (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)))
        if (((inst->state & MTB_PMBUS_STATE_ZONE_WRITE) != 0U) && (inst->cfg->num_pages != 0U))
        {
            for(int32_t i = 0; i < (int32_t)inst->cfg->num_pages; i++)
            {
                if ((inst->act_write_zone == inst->write_zones[i]) || ((inst->act_write_zone == MTB_PMBUS_ZONE_ALL) && (inst->write_zones[i] != MTB_PMBUS_ZONE_NONE)))
                {
                    MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", i, phase);
                    (void) mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, i, phase, inst->int_buff + (uint8_t)(inst->is_cmd_block), (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
                }
            }
        }
        else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) && (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
        {
            MTB_PMBUS_LOG_DBG("Store data for page [%" PRIi32 "] phase [%" PRIi32 "]", page, phase);
            (void) mtb_pmbus_cmd_update_data_ext_isr(inst, inst->cmd_code_full, page, phase, inst->int_buff + (uint8_t)(inst->is_cmd_block), (uint32_t)(inst->byte_received) - (uint32_t)(inst->is_cmd_block));
        }
    }
}

#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
__STATIC_INLINE void mtb_pmbus_int_crc_calc(mtb_pmbus_stc_t * inst, uint8_t byte)
{
    /*******************************************************************************
    * Pre-computed CRC-8 data. Each byte represents the CRC-8 value for the given
    * index. For example, the CRC-8 computation for 0x80 is 0x89 (i.e. crc_table[0x80]
    * yields 0x89).
    *******************************************************************************/
    static const uint8_t crc_table[] =
    {
        0x00u,0x07u,0x0Eu,0x09u,0x1Cu,0x1Bu,0x12u,0x15u,0x38u,0x3Fu,0x36u,0x31u,0x24u,0x23u,0x2Au,0x2Du,
        0x70u,0x77u,0x7Eu,0x79u,0x6Cu,0x6Bu,0x62u,0x65u,0x48u,0x4Fu,0x46u,0x41u,0x54u,0x53u,0x5Au,0x5Du,
        0xE0u,0xE7u,0xEEu,0xE9u,0xFCu,0xFBu,0xF2u,0xF5u,0xD8u,0xDFu,0xD6u,0xD1u,0xC4u,0xC3u,0xCAu,0xCDu,
        0x90u,0x97u,0x9Eu,0x99u,0x8Cu,0x8Bu,0x82u,0x85u,0xA8u,0xAFu,0xA6u,0xA1u,0xB4u,0xB3u,0xBAu,0xBDu,

        0xC7u,0xC0u,0xC9u,0xCEu,0xDBu,0xDCu,0xD5u,0xD2u,0xFFu,0xF8u,0xF1u,0xF6u,0xE3u,0xE4u,0xEDu,0xEAu,
        0xB7u,0xB0u,0xB9u,0xBEu,0xABu,0xACu,0xA5u,0xA2u,0x8Fu,0x88u,0x81u,0x86u,0x93u,0x94u,0x9Du,0x9Au,
        0x27u,0x20u,0x29u,0x2Eu,0x3Bu,0x3Cu,0x35u,0x32u,0x1Fu,0x18u,0x11u,0x16u,0x03u,0x04u,0x0Du,0x0Au,
        0x57u,0x50u,0x59u,0x5Eu,0x4Bu,0x4Cu,0x45u,0x42u,0x6Fu,0x68u,0x61u,0x66u,0x73u,0x74u,0x7Du,0x7Au,

        0x89u,0x8Eu,0x87u,0x80u,0x95u,0x92u,0x9Bu,0x9Cu,0xB1u,0xB6u,0xBFu,0xB8u,0xADu,0xAAu,0xA3u,0xA4u,
        0xF9u,0xFEu,0xF7u,0xF0u,0xE5u,0xE2u,0xEBu,0xECu,0xC1u,0xC6u,0xCFu,0xC8u,0xDDu,0xDAu,0xD3u,0xD4u,
        0x69u,0x6Eu,0x67u,0x60u,0x75u,0x72u,0x7Bu,0x7Cu,0x51u,0x56u,0x5Fu,0x58u,0x4Du,0x4Au,0x43u,0x44u,
        0x19u,0x1Eu,0x17u,0x10u,0x05u,0x02u,0x0Bu,0x0Cu,0x21u,0x26u,0x2Fu,0x28u,0x3Du,0x3Au,0x33u,0x34u,

        0x4Eu,0x49u,0x40u,0x47u,0x52u,0x55u,0x5Cu,0x5Bu,0x76u,0x71u,0x78u,0x7Fu,0x6Au,0x6Du,0x64u,0x63u,
        0x3Eu,0x39u,0x30u,0x37u,0x22u,0x25u,0x2Cu,0x2Bu,0x06u,0x01u,0x08u,0x0Fu,0x1Au,0x1Du,0x14u,0x13u,
        0xAEu,0xA9u,0xA0u,0xA7u,0xB2u,0xB5u,0xBCu,0xBBu,0x96u,0x91u,0x98u,0x9Fu,0x8Au,0x8Du,0x84u,0x83u,
        0xDEu,0xD9u,0xD0u,0xD7u,0xC2u,0xC5u,0xCCu,0xCBu,0xE6u,0xE1u,0xE8u,0xEFu,0xFAu,0xFDu,0xF4u,0xF3u
    };

    if ((inst->cfg->enable_pec) && !((bool)inst->errors))
    {
        inst->crc ^= byte;
        inst->crc = crc_table[inst->crc];
    }
}
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
static void mtb_pmbus_int_reverse_byte_order(uint8_t *array, size_t length)
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
static bool mtb_pmbus_int_impl_cmd_handler(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event)
{
    bool status = false;

    switch (inst->cmd_code)
    {
#if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)
        case MTB_PMBUS_IMPL_CMD_REVISION_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_rev(inst, event);
        }
        break;
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)
        case MTB_PMBUS_IMPL_CMD_CAPABILITY_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_capability(inst, event);
        }
        break;
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U)
        case MTB_PMBUS_IMPL_CMD_QUERY_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_query(inst, event);
        }
        break;
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U)
        case MTB_PMBUS_IMPL_CMD_PAGE_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_page(inst, event);
        }
        break;
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U)
        case MTB_PMBUS_IMPL_CMD_PHASE_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_phase(inst, event);
        }
        break;
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U)
        case MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_zone_cfg(inst, event);
        }
        break;
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U)
        case MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_CODE:
        {
            status = mtb_pmbus_int_impl_cmd_zone_act(inst, event);
        }
        break;
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U) */
        default:
            /* Do nothing */
        break;
    }

    return status;
}
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)
static bool mtb_pmbus_int_impl_cmd_rev(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event)
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
    }

    /* Always return true */
    return true;
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)
static bool mtb_pmbus_int_impl_cmd_capability(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event)
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
static bool mtb_pmbus_int_impl_cmd_query(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event)
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
static bool mtb_pmbus_int_impl_cmd_page(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event)
{
    if (MTB_PMBUS_CMD_READ_REQ == event)
    {
        inst->int_buff[0U] = inst->act_page;
        inst->byte_to_send = MTB_PMBUS_IMPL_CMD_PAGE_SIZE; 
    }
    else if (MTB_PMBUS_CMD_WRITE_DONE == event)
    {
        if ((inst->int_buff[0U] < inst->cfg->num_pages) || (inst->int_buff[0U] == MTB_PMBUS_PAGE_ALL))
        {
            inst->act_page = inst->int_buff[0U];
            MTB_PMBUS_LOG_DBG("New PAGE [%x]", inst->act_page);
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

    /* Always return true */
    return true;
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U)
static bool mtb_pmbus_int_impl_cmd_phase(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event)
{
    if (MTB_PMBUS_CMD_READ_REQ == event)
    {
        inst->int_buff[0U] = inst->act_phase;
        inst->byte_to_send = MTB_PMBUS_IMPL_CMD_PHASE_SIZE; 
    }
    else if (MTB_PMBUS_CMD_WRITE_DONE == event)
    {
        if ((inst->int_buff[0U] < inst->cfg->num_phases) || (inst->int_buff[0U] == MTB_PMBUS_PHASE_ALL))
        {
            inst->act_phase = inst->int_buff[0U];
            MTB_PMBUS_LOG_DBG("New PHASE [%x]", inst->act_phase);
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

    /* Always return true */
    return true;
}
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U) */

#if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U)
static bool mtb_pmbus_int_impl_cmd_zone_cfg(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event)
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
                    uint8_t ret = inst->cfg->zone_callback(MTB_PMBUS_ZONE_CONFIG_WR, inst->int_buff[0U], inst->act_page);
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
                    uint8_t ret = inst->cfg->zone_callback(MTB_PMBUS_ZONE_CONFIG_RD, inst->int_buff[1U], inst->act_page);
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
static bool mtb_pmbus_int_impl_cmd_zone_act(mtb_pmbus_stc_t * inst, mtb_pmbus_cmd_events_t event)
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
                    uint8_t ret = inst->cfg->zone_callback(MTB_PMBUS_ZONE_ACTIVE_WR, inst->int_buff[0U], inst->act_page);
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
                    uint8_t ret = inst->cfg->zone_callback(MTB_PMBUS_ZONE_ACTIVE_RD, inst->int_buff[1U], inst->act_page);
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

void mtb_pmbus_int_reset_state(mtb_pmbus_stc_t * inst)
{
    if ((inst->cfg->errors_callback != NULL) && (inst->errors != 0U))
    {
        inst->cfg->errors_callback(inst->errors, inst->cmd_code, false);
    }

#if ((defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)))
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
    if ((inst->state & MTB_PMBUS_STATE_ZONE_READ) != 0U)
    {
        inst->act_page = inst->stored_page;
    }
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

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
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

    /* Add empty line to logs */
    MTB_PMBUS_LOG_DBG("\n\r");
}

void mtb_pmbus_int_cmd_handle_tx(mtb_pmbus_stc_t * inst)
{
    /* The handler of the case when the command supports write and process call, but gets a read request. */
    if (((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_WR) != 0U)
            && ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) != 0U)
            && (inst->byte_received == 0U))
    {
        inst->errors |= MTB_PMBUS_ERR_REQ_RD_FOR_PC_ONLY;
        MTB_PMBUS_LOG_WRN("Cmd [%x] doesn't support read transaction", inst->active_cmd_tbl[inst->cmd_pos].cmd_code);
    }
    else
    {
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
        if ((inst->state & MTB_PMBUS_STATE_IMPL_CMD) != 0U)
        {
            (void) mtb_pmbus_int_impl_cmd_handler(inst, MTB_PMBUS_CMD_READ_REQ);
        }
        else
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
        {
            uint8_t size = 0U;
            /* Select the page count */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
            int32_t page = ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) == 0U) ? MTB_PMBUS_NO_PAGE_PHASE : (int32_t)inst->act_page;
#else
            int32_t page = MTB_PMBUS_NO_PAGE_PHASE;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
            int32_t phase = ((inst->active_cmd_tbl[inst->cmd_pos].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) == 0U) ? MTB_PMBUS_NO_PAGE_PHASE : (int32_t)inst->act_phase;
#else
            int32_t phase = MTB_PMBUS_NO_PAGE_PHASE;
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */

            /* Call the command callback */
            if (inst->active_cmd_tbl[inst->cmd_pos].callback != NULL)
            {
                /* Ignore the return status as it is not applicable for this event */
                (void) inst->active_cmd_tbl[inst->cmd_pos].callback(MTB_PMBUS_CMD_READ_REQ,
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
                                                                    0U);
            }

            if (inst->is_cmd_block)
            {
                (void) mtb_pmbus_cmd_get_transfer_size_ext(inst, inst->cmd_code_full, page, phase, &size);
                inst->int_buff[0U] = size;
            }
            inst->byte_to_send =  inst->is_cmd_block ? ((uint16_t)(size) + MTB_PMBUS_CMD_BLOCK_EXTRA_BYTE) : (uint16_t)inst->active_cmd_tbl[inst->cmd_pos].data_size;
            /* Clear the sent bytes number, required only for Zone Read */
            inst->byte_sent = 0U;

            MTB_PMBUS_LOG_DBG("Read data for page [%" PRIi32 "] phase [%" PRIi32 "]", page, phase);
            (void) mtb_pmbus_cmd_read_data_ext_isr(inst, inst->cmd_code_full, page, phase, inst->int_buff + (uint8_t)(inst->is_cmd_block), (uint32_t)(inst->byte_to_send) - (uint32_t)(inst->is_cmd_block));
        }
    }
}

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
static uint8_t mtb_pmbus_int_get_num_available_zone(mtb_pmbus_stc_t * inst)
{
    uint8_t num_zones = 0U;
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
    if (inst->cfg->num_pages > 0U)
    {
        for (uint8_t i = 0U; i < inst->cfg->num_pages; i++)
        {
            if ((inst->read_zones[i] == inst->act_read_zone) || ((inst->act_read_zone == MTB_PMBUS_ZONE_ALL) && (inst->read_zones[i] != MTB_PMBUS_ZONE_NONE)))
            {
                num_zones++;
            }
        }
    }
    else
    {
        if ((inst->read_zones[0U] == inst->act_read_zone) || ((inst->act_read_zone == MTB_PMBUS_ZONE_ALL) && (inst->read_zones[0U] != MTB_PMBUS_ZONE_NONE)))
        {
            num_zones = 1U;
        }
    }
#else
    if ((inst->read_zones[0U] == inst->act_read_zone) || ((inst->act_read_zone == MTB_PMBUS_ZONE_ALL) && (inst->read_zones[0U] != MTB_PMBUS_ZONE_NONE)))
    {
        num_zones = 1U;
    }
#endif
    
    return num_zones;
}
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
static uint8_t mtb_pmbus_int_get_next_page2actzone(mtb_pmbus_stc_t * inst)
{
    uint8_t next_page = 0U;
    uint8_t i = 0U;
    if (inst->zone_pages > 0U)
    {
        i = inst->act_page + 1U; /* Start from the next page */
    }
    
    for (; i < inst->cfg->num_pages; i++)
    {
        if ((inst->read_zones[i] == inst->act_read_zone) || ((inst->act_read_zone == MTB_PMBUS_ZONE_ALL) && (inst->read_zones[i] != MTB_PMBUS_ZONE_NONE)))
        {
            next_page = i;
            break;
        }
    }
    return next_page;
}

static void mtb_pmbus_int_set_active_zone_page(mtb_pmbus_stc_t * inst, bool store_page)
{
    if (store_page)
    {
        inst->stored_page = inst->act_page; /* Store the current page */
    }
    inst->act_page = mtb_pmbus_int_get_next_page2actzone(inst); /* Set the next page for the active zone */
    MTB_PMBUS_LOG_DBG("Set active page [%x] for zone [%x]", inst->act_page, inst->act_read_zone);
}
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
