/***************************************************************************//**
* \file mtb_pmbus_ctrl.c
* \version 1.0
*
* Provides APIs for PMBus Controller.
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

#include "mtb_pmbus_ctrl_int.h"

#define MTB_PMBUS_CTRL_STATE_WR_RD_REQ          (0x01U)
#define MTB_PMBUS_CTRL_STATE_IS_ACTIVE          (0x02U)
#define MTB_PMBUS_CTRL_STATE_PEC_REQ            (0x04U)
#define MTB_PMBUS_CTRL_STATE_PROCESS_CALL       (0x08U)
#define MTB_PMBUS_CTRL_STATE_BLOCK_READ         (0x10U)
#define MTB_PMBUS_CTRL_STATE_GOT_BLOCK_COUNT    (0x20U)
#define MTB_PMBUS_CTRL_STATE_DIR_WR             (0x40U)
#define MTB_PMBUS_CTRL_STATE_DIR_RD             (0x80U)
#define MTB_PMBUS_CTRL_STATE_TRANS_NOT_COMPLT   (0x100U)

#define MTB_PMBUS_CTRL_RECEIVED_BYTE_SIZE           (1U)
#define MTB_PMBUS_CTRL_SEND_BYTE_SIZE               (1U)
#define MTB_PMBUS_CTRL_WRITE_BYTE_SIZE              (1U)
#define MTB_PMBUS_CTRL_READ_BYTE_SIZE               (1U)
#define MTB_PMBUS_CTRL_WRITE_WORD_SIZE              (2U)
#define MTB_PMBUS_CTRL_READ_WORD_SIZE               (2U)
#define MTB_PMBUS_CTRL_PROCESS_CALL_SIZE            (2U)
#define MTB_PMBUS_CTRL_WRITE_32_SIZE                (4U)
#define MTB_PMBUS_CTRL_READ_32_SIZE                 (4U)
#define MTB_PMBUS_CTRL_WRITE_64_SIZE                (8U)
#define MTB_PMBUS_CTRL_READ_64_SIZE                 (8U)

#define MTB_PMBUS_CTRL_PEC_SIZE                     (1U)
#define MTB_PMBUS_CTRL_CMD_SIZE                     (1U)
#define MTB_PMBUS_CTRL_BLOCK_COUNT_SIZE             (1U)
#define MTB_PMBUS_CTRL_CMD_PEC_SIZE                 (2U)
#define MTB_PMBUS_CTRL_CMD_BLOCK_COUNT_SIZE         (2U)

#define MTB_PMBUS_CTRL_ADDR_WR                      (0x01U)
#define MTB_PMBUS_CTRL_ADDR_RD                      (0x02U)
#define MTB_PMBUS_CTRL_ADDR_RD_BIT                  (0x01U)

#define MTB_PMBUS_CTRL_BLOCK_COUNT_MAX_SIZE         (255U)

static mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_execute_transfer_int(mtb_pmbus_ctrl_stc_t *inst,
                                                                   mtb_pmbus_ctrl_stc_transfer_cfg_t *cfg,
                                                                   bool skip_cfg);
static uint8_t mtb_pmbus_int_pec_calc(uint8_t *data, uint16_t size, uint8_t addr, uint8_t addr_type, uint8_t init_pec);
static void mtb_pmbus_ctrl_reset_state(mtb_pmbus_ctrl_stc_t *inst);

mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_init(mtb_pmbus_ctrl_stc_t *inst, mtb_pmbus_ctrl_cfg_t *cfg)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != cfg))
    {
        if ((NULL != cfg->callback_hw) && (NULL != cfg->callback_isr_enable) && (NULL != cfg->callback_isr_disable))
        {
            inst->cfg = cfg;
            inst->cfg->callback_hw(MTB_PMBUS_CTRL_HW_RESOURCES_INIT);

            mtb_pmbus_ctrl_hal_init(inst);

            status = MTB_PMBUS_CTRL_STATUS_SUCCESS;
        }
    }

    return status;
}


void mtb_pmbus_ctrl_enable(mtb_pmbus_ctrl_stc_t *inst)
{
    CY_ASSERT(NULL != inst);
    CY_ASSERT(NULL != inst->cfg);
    CY_ASSERT(NULL != inst->cfg->callback_hw);
    CY_ASSERT(NULL != inst->cfg->callback_isr_enable);

    /* Reset state */
    inst->errors = 0U;
    mtb_pmbus_ctrl_reset_state(inst);

    /* Enable HW */
    inst->cfg->callback_hw(MTB_PMBUS_CTRL_HW_RESOURCES_ENABLE);
    inst->cfg->callback_isr_enable();
}


void mtb_pmbus_ctrl_disable(mtb_pmbus_ctrl_stc_t *inst)
{
    CY_ASSERT(NULL != inst);
    CY_ASSERT(NULL != inst->cfg);
    CY_ASSERT(NULL != inst->cfg->callback_hw);
    CY_ASSERT(NULL != inst->cfg->callback_isr_disable);

    inst->cfg->callback_hw(MTB_PMBUS_CTRL_HW_RESOURCES_DISABLE);
    inst->cfg->callback_isr_disable();
}


bool mtb_pmbus_ctrl_bus_is_busy(mtb_pmbus_ctrl_stc_t *inst)
{
    CY_ASSERT(NULL != inst);

    return mtb_pmbus_ctrl_hal_is_bus_busy(inst);
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_execute_transfer(mtb_pmbus_ctrl_stc_t *inst,
                                                        mtb_pmbus_ctrl_stc_transfer_cfg_t *cfg)
{
    return mtb_pmbus_ctrl_execute_transfer_int(inst, cfg, false);
}


static mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_execute_transfer_int(mtb_pmbus_ctrl_stc_t *inst,
                                                                   mtb_pmbus_ctrl_stc_transfer_cfg_t *cfg,
                                                                   bool skip_cfg)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    bool generate_stop = true;

    if (NULL != inst)
    {
        if ((!mtb_pmbus_ctrl_bus_is_busy(inst)) ||
            (mtb_pmbus_ctrl_bus_is_busy(inst) && ((inst->state & MTB_PMBUS_CTRL_STATE_TRANS_NOT_COMPLT) != 0U)))
        {
            inst->state |= MTB_PMBUS_CTRL_STATE_IS_ACTIVE;

            /* Reset errors if any occurred previously */
            inst->errors = 0U;

            if ((inst->state & MTB_PMBUS_CTRL_STATE_TRANS_NOT_COMPLT) != 0U)
            {
                /* Clear the flag as we are starting a new transfer */
                inst->state &= ~MTB_PMBUS_CTRL_STATE_TRANS_NOT_COMPLT;
            }

            if (!skip_cfg)
            {
                inst->transfer_cfg.data = cfg->data;
                inst->transfer_cfg.addr = cfg->addr;
                inst->transfer_cfg.wr_size = cfg->wr_size;
                inst->transfer_cfg.rd_size = cfg->rd_size;
                inst->transfer_cfg.execute_stop = cfg->execute_stop;
                if (cfg->rd_size != 0U)
                {
                    inst->user_data = cfg->data;
                }
            }

            /* Check if read part of transfer is present */
            if ((inst->transfer_cfg.rd_size != 0U) && (inst->transfer_cfg.wr_size != 0U))
            {
                inst->state |= MTB_PMBUS_CTRL_STATE_WR_RD_REQ;
                generate_stop = false;
            }
            else if (inst->transfer_cfg.rd_size == 0U)
            {
                generate_stop = inst->transfer_cfg.execute_stop;
            }
            /* Read only direction */
            else
            {
                generate_stop = inst->transfer_cfg.execute_stop;
            }

            if ((inst->transfer_cfg.wr_size != 0U) ||
                ((inst->transfer_cfg.wr_size == 0U) && (inst->transfer_cfg.rd_size == 0U)))
            {
                inst->state |= MTB_PMBUS_CTRL_STATE_DIR_WR;
                status = mtb_pmbus_ctrl_hal_write(inst, inst->transfer_cfg.addr, inst->transfer_cfg.data,
                                                  inst->transfer_cfg.wr_size, generate_stop);
            }
            else
            {
                inst->state |= MTB_PMBUS_CTRL_STATE_DIR_RD;
                status = mtb_pmbus_ctrl_hal_read(inst, inst->transfer_cfg.addr, inst->buffer,
                                                 inst->transfer_cfg.rd_size, generate_stop);
            }

            if (status != MTB_PMBUS_CTRL_STATUS_SUCCESS)
            {
                /* Reset state in case of error */
                mtb_pmbus_ctrl_reset_state(inst);
            }
        }
        else
        {
            status = MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY;
        }
    }

    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_wait_cmpl(mtb_pmbus_ctrl_stc_t *inst, uint32_t timeout)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    uint32_t timeout_temp = timeout;

    if (NULL != inst)
    {
        while (timeout_temp > 0U)
        {
            status = mtb_pmbus_ctrl_get_status(inst);

            if (status != MTB_PMBUS_CTRL_STATUS_IS_BUSY)
            {
                break;
            }

            mtb_pmbus_hal_delay_us(inst, 1U);
            timeout_temp--;
        }

        /* Final check after timeout or if loop didn't run */
        if (timeout_temp == 0U)
        {
            status = mtb_pmbus_ctrl_get_status(inst);
            if (status == MTB_PMBUS_CTRL_STATUS_IS_BUSY)
            {
                status = MTB_PMBUS_CTRL_STATUS_TIMEOUT;
            }
        }
    }

    return status;
}


void mtb_pmbus_ctrl_events_handler(mtb_pmbus_ctrl_stc_t *inst, uint32_t events)
{
    bool reset_state = true;

    if ((events & MTB_PMBUS_CTRL_EVENTS_ERR) != 0U)
    {
        mtb_pmbus_ctrl_hal_set_errors(inst);
        if (inst->cfg->callback_events != NULL)
        {
            /* Check which errors have been set and call appropriate callbacks */
            if ((inst->errors & MTB_PMBUS_CTRL_ERR_NACK_ADDR) != 0U)
            {
                inst->cfg->callback_events(MTB_PMBUS_CTRL_TARGET_NACK_ADDR);
            }
            else if ((inst->errors & MTB_PMBUS_CTRL_ERR_NACK_CMD) != 0U)
            {
                inst->cfg->callback_events(MTB_PMBUS_CTRL_TARGET_NACK_CMD);
            }
            else if ((inst->errors & MTB_PMBUS_CTRL_ERR_NACK_DATA) != 0U)
            {
                inst->cfg->callback_events(MTB_PMBUS_CTRL_TARGET_NACK_BYTE);
            }
            else if ((inst->errors & MTB_PMBUS_CTRL_ERR_BUS_ERROR) != 0U)
            {
                inst->cfg->callback_events(MTB_PMBUS_CTRL_BUS_ERR);
            }
            else if ((inst->errors & MTB_PMBUS_CTRL_ERR_ARB_LOST) != 0U)
            {
                inst->cfg->callback_events(MTB_PMBUS_CTRL_ARB_LOST);
            }
            else if ((inst->errors & MTB_PMBUS_CTRL_ERR_ABORT_START) != 0U)
            {
                inst->cfg->callback_events(MTB_PMBUS_CTRL_ABORT_START);
            }
            else
            {
                /* Do nothing for unsupported errors */
            }
        }
    }
    else if ((events & MTB_PMBUS_CTRL_EVENTS_WR_DONE) != 0U)
    {
        if (inst->errors == 0U)
        {
            MTB_PMBUS_LOG_DBG("WR Done");
            inst->state &= ~MTB_PMBUS_CTRL_STATE_DIR_WR;
            if ((inst->state & MTB_PMBUS_CTRL_STATE_WR_RD_REQ) != 0U)
            {
                if (MTB_PMBUS_CTRL_STATUS_SUCCESS ==
                    mtb_pmbus_ctrl_hal_read(inst, inst->transfer_cfg.addr, inst->buffer, inst->transfer_cfg.rd_size,
                                            inst->transfer_cfg.execute_stop))
                {
                    /* Set read direction state */
                    inst->state |= MTB_PMBUS_CTRL_STATE_DIR_RD;
                    /* Read started successfully, don't reset state */
                    reset_state = false;
                }
                else
                {
                    /* Unexpected error is happened on the bus. The Controller should be always
                       available
                     * after Write part of transfer with no STOP
                     */
                    if (inst->cfg->callback_events != NULL)
                    {
                        inst->cfg->callback_events(MTB_PMBUS_CTRL_BUS_ERR);
                    }
                    inst->errors |= MTB_PMBUS_CTRL_ERR_BUS_ERROR;
                }
            }
            else
            {
                if (inst->transfer_cfg.execute_stop == true)
                {
                    if (inst->cfg->callback_events != NULL)
                    {
                        /* Write transfer completed successfully */
                        inst->cfg->callback_events(MTB_PMBUS_CTRL_TRANSFER_DONE);
                    }
                }
                else
                {
                    inst->state |= MTB_PMBUS_CTRL_STATE_TRANS_NOT_COMPLT;
                    /* Write transfer part with no STOP completed successfully, don't reset state */
                    reset_state = false;
                }
            }
        }
        else
        {
            mtb_pmbus_ctrl_hal_abort_write(inst);
        }

        /* Reset state for all paths except successful read initiation */
        if (reset_state)
        {
            mtb_pmbus_ctrl_reset_state(inst);
        }
    }
    else if ((events & MTB_PMBUS_CTRL_EVENTS_RD_DONE) != 0U)
    {
        if (inst->errors == 0U)
        {
            MTB_PMBUS_LOG_DBG("RD Done");
            if ((inst->state & MTB_PMBUS_CTRL_STATE_BLOCK_READ) != 0U)
            {
                /* Assert block buffer size is valid */
                CY_ASSERT(NULL != inst->block_buff_size);
            }

            if ((inst->state & MTB_PMBUS_CTRL_STATE_PEC_REQ) != 0U)
            {
                uint32_t pec_pos =
                    ((inst->state & MTB_PMBUS_CTRL_STATE_BLOCK_READ) !=
                     0U) ? (*inst->block_buff_size + MTB_PMBUS_CTRL_BLOCK_COUNT_SIZE)
                                                                                           : ((uint32_t)inst->
                                                                                              transfer_cfg.rd_size -
                                                                                              MTB_PMBUS_CTRL_PEC_SIZE);
                uint8_t calc_pec = mtb_pmbus_int_pec_calc(inst->buffer, (uint16_t)pec_pos, inst->transfer_cfg.addr,
                                                          MTB_PMBUS_CTRL_ADDR_RD, inst->pec);
                if (inst->buffer[pec_pos] != calc_pec)
                {
                    MTB_PMBUS_LOG_WRN("PEC error, received [%x], calculated [%x]", inst->buffer[pec_pos], calc_pec);
                    inst->errors |= MTB_PMBUS_CTRL_ERR_CORRUPTED_DATA;
                    if (inst->cfg->callback_events != NULL)
                    {
                        inst->cfg->callback_events(MTB_PMBUS_CTRL_CORRUPTED_DATA);
                    }
                }
                else
                {
                    /* PEC matched */
                    MTB_PMBUS_LOG_DBG("PEC matched [%x]", inst->buffer[pec_pos]);
                }
            }

            if (inst->errors == 0U)
            {
                /* Copy data to internal buffer */
                uint16_t size = 0U;
                if ((inst->state & MTB_PMBUS_CTRL_STATE_BLOCK_READ) != 0U)
                {
                    size = (uint16_t)(*inst->block_buff_size);
                }
                else
                {
                    size =
                        ((inst->state & MTB_PMBUS_CTRL_STATE_PEC_REQ) !=
                         0U) ? (inst->transfer_cfg.rd_size - MTB_PMBUS_CTRL_PEC_SIZE) : inst->transfer_cfg.rd_size;
                }

                uint16_t src_shift =
                    ((inst->state & MTB_PMBUS_CTRL_STATE_BLOCK_READ) != 0U) ? MTB_PMBUS_CTRL_BLOCK_COUNT_SIZE : 0U;

                uint16_t dst_shift = 0U;
                if ((inst->state & MTB_PMBUS_CTRL_STATE_PROCESS_CALL) != 0U)
                {
                    dst_shift =
                        ((inst->state & MTB_PMBUS_CTRL_STATE_BLOCK_READ) !=
                         0U) ? (inst->transfer_cfg.wr_size -
                                MTB_PMBUS_CTRL_CMD_BLOCK_COUNT_SIZE) : MTB_PMBUS_CTRL_WRITE_WORD_SIZE;
                }

                if (size > 0U)
                {
                    (void)memcpy(inst->user_data + dst_shift, inst->buffer + src_shift, size);
                }

                if (inst->transfer_cfg.execute_stop == true)
                {
                    if (inst->cfg->callback_events != NULL)
                    {
                        /* Read transfer completed successfully */
                        inst->cfg->callback_events(MTB_PMBUS_CTRL_TRANSFER_DONE);
                    }
                }
                else
                {
                    inst->state |= MTB_PMBUS_CTRL_STATE_TRANS_NOT_COMPLT;
                    /* Read transfer part with no STOP completed successfully, don't reset state */
                    reset_state = false;
                }
            }
        }
        else
        {
            mtb_pmbus_ctrl_hal_abort_read(inst);
            /* Error is already captured in get_status */
        }

        /* Reset state */
        /* For ZONE_READ case we need to call this reset only if execute_transfer for last transfer
           part is called with stop */
        if (reset_state)
        {
            mtb_pmbus_ctrl_reset_state(inst);
        }
    }
    else
    {
        /* All events should be already handled. The application should never go to this else */
    }
}


#if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U))
void mtb_pmbus_ctrl_handle_hnp(mtb_pmbus_ctrl_stc_t *inst, mtb_pmbus_ctrl_host_notify_events_t event)
{
    mtb_pmbus_ctrl_host_notify_events_t local_event = event;

    if (local_event == MTB_PMBUS_CTRL_HOST_NOTIFY_REQUEST_RECEIVED)
    {
        inst->state |= MTB_PMBUS_CTRL_STATE_IS_ACTIVE;

        if (inst->cfg->callback_hnp != NULL)
        {
            inst->cfg->callback_hnp(local_event);
        }
    }

    if ((local_event == MTB_PMBUS_CTRL_HOST_NOTIFY_COMPLETE) || (local_event == MTB_PMBUS_CTRL_HOST_NOTIFY_ERROR))
    {
        if (local_event == MTB_PMBUS_CTRL_HOST_NOTIFY_ERROR)
        {
            mtb_pmbus_ctrl_hal_hnp_clr_write_status(inst);
        }
        else
        {
            if (!mtb_pmbus_ctrl_hal_hnp_transfer_is_ok(inst))
            {
                local_event = MTB_PMBUS_CTRL_HOST_NOTIFY_ERROR;
                mtb_pmbus_ctrl_hal_hnp_clr_write_status(inst);
            }
        }

        if (inst->cfg->callback_hnp != NULL)
        {
            inst->cfg->callback_hnp(local_event);
        }

        /* Re-setup buffer for the Host Notify */
        mtb_pmbus_ctrl_hal_hnp_resetup_buf(inst);
        /* Reset state */
        mtb_pmbus_ctrl_reset_state(inst);
    }
}


uint8_t mtb_pmbus_ctrl_hnp_get_trgt_addr(mtb_pmbus_ctrl_stc_t *inst)
{
    uint8_t addr = 0U;

    if (inst != NULL)
    {
        addr = mtb_pmbus_ctrl_hal_hnp_get_trgt_addr(inst);
    }

    return addr;
}


uint16_t mtb_pmbus_ctrl_hnp_get_trgt_data(mtb_pmbus_ctrl_stc_t *inst)
{
    uint16_t data = 0U;

    if (inst != NULL)
    {
        data = mtb_pmbus_ctrl_hal_hnp_get_trgt_data(inst);
    }
    return data;
}
#endif /* #if(defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U)) */

mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_get_status(mtb_pmbus_ctrl_stc_t *inst)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;

    if (inst != NULL)
    {
        if ((inst->state & MTB_PMBUS_CTRL_STATE_IS_ACTIVE) != 0U)
        {
            if ((inst->state & MTB_PMBUS_CTRL_STATE_TRANS_NOT_COMPLT) != 0U)
            {
                if (inst->errors == 0U)
                {
                    status = MTB_PMBUS_CTRL_STATUS_IS_READY;
                }
                else
                {
                    /* Set transfer error status */
                    status = MTB_PMBUS_CTRL_STATUS_TRANSFER_ERR;
                }
            }
            else
            {
                status = MTB_PMBUS_CTRL_STATUS_IS_BUSY;
            }
        }
        else
        {
            if (inst->errors == 0U)
            {
                status = MTB_PMBUS_CTRL_STATUS_IS_READY;
            }
            else
            {
                /* Set transfer error status */
                status = MTB_PMBUS_CTRL_STATUS_TRANSFER_ERR;
            }
        }
    }

    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_quick_cmd(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if (NULL != inst)
    {
        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size = 0U;
        inst->transfer_cfg.rd_size = 0U;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_received_byte(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint8_t *data,
                                                        bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data))
    {
        inst->user_data = data;

        if (pec)
        {
            inst->pec = 0U;
            inst->state |= MTB_PMBUS_CTRL_STATE_PEC_REQ;
        }

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size = 0U;
        inst->transfer_cfg.rd_size =
            pec ? (MTB_PMBUS_CTRL_RECEIVED_BYTE_SIZE + MTB_PMBUS_CTRL_PEC_SIZE) : MTB_PMBUS_CTRL_RECEIVED_BYTE_SIZE;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_send_byte(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint8_t *data, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = data[0U];

        if (pec)
        {
            inst->buffer[1U] = mtb_pmbus_int_pec_calc(inst->buffer, MTB_PMBUS_CTRL_SEND_BYTE_SIZE, addr,
                                                      MTB_PMBUS_CTRL_ADDR_WR, 0U);
        }

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size =
            pec ? (MTB_PMBUS_CTRL_SEND_BYTE_SIZE + MTB_PMBUS_CTRL_PEC_SIZE) : MTB_PMBUS_CTRL_SEND_BYTE_SIZE;
        inst->transfer_cfg.rd_size = 0U;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_write_byte(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                     uint8_t *data, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;
        inst->buffer[1U] = data[0U];

        if (pec)
        {
            inst->buffer[2U] = mtb_pmbus_int_pec_calc(inst->buffer,
                                                      MTB_PMBUS_CTRL_WRITE_BYTE_SIZE + MTB_PMBUS_CTRL_CMD_SIZE, addr,
                                                      MTB_PMBUS_CTRL_ADDR_WR, 0U);
        }

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size =
            pec ? (MTB_PMBUS_CTRL_WRITE_BYTE_SIZE + MTB_PMBUS_CTRL_CMD_PEC_SIZE) : (MTB_PMBUS_CTRL_WRITE_BYTE_SIZE +
                                                                                    MTB_PMBUS_CTRL_CMD_SIZE);
        inst->transfer_cfg.rd_size = 0U;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_read_byte(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                    uint8_t *data, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;

        inst->user_data = data;

        if (pec)
        {
            /* Prepare the first part of PEC */
            inst->pec = mtb_pmbus_int_pec_calc(inst->buffer, MTB_PMBUS_CTRL_CMD_SIZE, addr, MTB_PMBUS_CTRL_ADDR_WR, 0U);
            inst->state |= MTB_PMBUS_CTRL_STATE_PEC_REQ;
        }

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size = MTB_PMBUS_CTRL_CMD_SIZE;
        inst->transfer_cfg.rd_size =
            pec ? (MTB_PMBUS_CTRL_READ_BYTE_SIZE + MTB_PMBUS_CTRL_PEC_SIZE) : MTB_PMBUS_CTRL_READ_BYTE_SIZE;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_write_word(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                     uint8_t *data, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;
        inst->buffer[1U] = data[0U];
        inst->buffer[2U] = data[1U];

        if (pec)
        {
            inst->buffer[3U] = mtb_pmbus_int_pec_calc(inst->buffer,
                                                      MTB_PMBUS_CTRL_WRITE_WORD_SIZE + MTB_PMBUS_CTRL_CMD_SIZE, addr,
                                                      MTB_PMBUS_CTRL_ADDR_WR, 0U);
        }

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size =
            pec ? (MTB_PMBUS_CTRL_WRITE_WORD_SIZE + MTB_PMBUS_CTRL_CMD_PEC_SIZE) : (MTB_PMBUS_CTRL_WRITE_WORD_SIZE +
                                                                                    MTB_PMBUS_CTRL_CMD_SIZE);
        inst->transfer_cfg.rd_size = 0U;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_read_word(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                    uint8_t *data, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;

        inst->user_data = data;

        if (pec)
        {
            /* Prepare the first part of PEC */
            inst->pec = mtb_pmbus_int_pec_calc(inst->buffer, MTB_PMBUS_CTRL_CMD_SIZE, addr, MTB_PMBUS_CTRL_ADDR_WR, 0U);
            inst->state |= MTB_PMBUS_CTRL_STATE_PEC_REQ;
        }

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size = MTB_PMBUS_CTRL_CMD_SIZE;
        inst->transfer_cfg.rd_size =
            pec ? (MTB_PMBUS_CTRL_READ_WORD_SIZE + MTB_PMBUS_CTRL_PEC_SIZE) : MTB_PMBUS_CTRL_READ_WORD_SIZE;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_process_call(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                       uint8_t *data, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;
        inst->buffer[1U] = data[0U];
        inst->buffer[2U] = data[1U];

        inst->user_data = data;

        if (pec)
        {
            /* Prepare the first part of PEC */
            inst->pec = mtb_pmbus_int_pec_calc(inst->buffer, MTB_PMBUS_CTRL_WRITE_WORD_SIZE + MTB_PMBUS_CTRL_CMD_SIZE,
                                               addr, MTB_PMBUS_CTRL_ADDR_WR, 0U);
            inst->state |= MTB_PMBUS_CTRL_STATE_PEC_REQ;
        }

        inst->state |= MTB_PMBUS_CTRL_STATE_PROCESS_CALL;

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size = MTB_PMBUS_CTRL_WRITE_WORD_SIZE + MTB_PMBUS_CTRL_CMD_SIZE;
        inst->transfer_cfg.rd_size =
            pec ? (MTB_PMBUS_CTRL_READ_WORD_SIZE + MTB_PMBUS_CTRL_PEC_SIZE) : MTB_PMBUS_CTRL_READ_WORD_SIZE;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_block_write(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                      uint8_t *data, uint32_t size, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data) && (size <= MTB_PMBUS_CTRL_BLOCK_COUNT_MAX_SIZE))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;
        inst->buffer[1U] = (uint8_t)size;
        for (uint32_t i = 0U; i < size; i++)
        {
            inst->buffer[2U + i] = data[i];
        }

        if (pec)
        {
            inst->buffer[2U + size] = mtb_pmbus_int_pec_calc(inst->buffer,
                                                             (uint16_t)size + (uint16_t)MTB_PMBUS_CTRL_CMD_BLOCK_COUNT_SIZE, addr, MTB_PMBUS_CTRL_ADDR_WR,
                                                             0U);
        }

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size =
            pec ? ((uint16_t)size + (uint16_t)MTB_PMBUS_CTRL_CMD_PEC_SIZE + (uint16_t)MTB_PMBUS_CTRL_BLOCK_COUNT_SIZE)
                                         : ((uint16_t)size + (uint16_t)MTB_PMBUS_CTRL_CMD_BLOCK_COUNT_SIZE);
        inst->transfer_cfg.rd_size = 0U;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_block_read(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                     uint8_t *data, uint32_t *size, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data) && (NULL != size))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;

        inst->user_data = data;
        inst->block_buff_size = size;

        if (pec)
        {
            /* Prepare the first part of PEC */
            inst->pec = mtb_pmbus_int_pec_calc(inst->buffer, MTB_PMBUS_CTRL_CMD_SIZE, addr, MTB_PMBUS_CTRL_ADDR_WR, 0U);
            inst->state |= MTB_PMBUS_CTRL_STATE_PEC_REQ;
        }

        inst->state |= MTB_PMBUS_CTRL_STATE_BLOCK_READ;

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size = MTB_PMBUS_CTRL_CMD_SIZE;
        /* Set maximum read size, it will be adjusted after receiving block count */
        inst->transfer_cfg.rd_size =
            pec ? (MTB_PMBUS_CTRL_BLOCK_COUNT_SIZE + MTB_PMBUS_CTRL_BLOCK_COUNT_MAX_SIZE + MTB_PMBUS_CTRL_PEC_SIZE)
                                         : (MTB_PMBUS_CTRL_BLOCK_COUNT_SIZE + MTB_PMBUS_CTRL_BLOCK_COUNT_MAX_SIZE);
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_block_process_call(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr,
                                                             uint32_t cmd_code, uint8_t *data, uint32_t size_wr,
                                                             uint32_t *size, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data) && (NULL != size) && (size_wr <= MTB_PMBUS_CTRL_BLOCK_COUNT_MAX_SIZE) &&
        (size_wr <= *size))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;
        inst->buffer[1U] = (uint8_t)size_wr;
        for (uint32_t i = 0U; i < size_wr; i++)
        {
            inst->buffer[2U + i] = data[i];
        }

        inst->user_data = data;
        inst->block_buff_size = size;

        if (pec)
        {
            inst->pec = mtb_pmbus_int_pec_calc(inst->buffer,
                                               (uint16_t)size_wr + (uint16_t)MTB_PMBUS_CTRL_CMD_BLOCK_COUNT_SIZE, addr,
                                               MTB_PMBUS_CTRL_ADDR_WR, 0U);
            inst->state |= MTB_PMBUS_CTRL_STATE_PEC_REQ;
        }

        inst->state |= MTB_PMBUS_CTRL_STATE_BLOCK_READ;
        inst->state |= MTB_PMBUS_CTRL_STATE_PROCESS_CALL;

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size = (uint16_t)size_wr + (uint16_t)MTB_PMBUS_CTRL_CMD_BLOCK_COUNT_SIZE;
        /* Set maximum read size, it will be adjusted after receiving block count */
        inst->transfer_cfg.rd_size =
            pec ? (MTB_PMBUS_CTRL_BLOCK_COUNT_SIZE + MTB_PMBUS_CTRL_BLOCK_COUNT_MAX_SIZE + MTB_PMBUS_CTRL_PEC_SIZE)
                                         : (MTB_PMBUS_CTRL_BLOCK_COUNT_SIZE + MTB_PMBUS_CTRL_BLOCK_COUNT_MAX_SIZE);
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_write_32(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                   uint8_t *data, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;
        inst->buffer[1U] = data[0U];
        inst->buffer[2U] = data[1U];
        inst->buffer[3U] = data[2U];
        inst->buffer[4U] = data[3U];

        if (pec)
        {
            inst->buffer[5U] = mtb_pmbus_int_pec_calc(inst->buffer,
                                                      MTB_PMBUS_CTRL_WRITE_32_SIZE + MTB_PMBUS_CTRL_CMD_SIZE, addr,
                                                      MTB_PMBUS_CTRL_ADDR_WR, 0U);
        }

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size =
            pec ? (MTB_PMBUS_CTRL_WRITE_32_SIZE + MTB_PMBUS_CTRL_CMD_PEC_SIZE) : (MTB_PMBUS_CTRL_WRITE_32_SIZE +
                                                                                  MTB_PMBUS_CTRL_CMD_SIZE);
        inst->transfer_cfg.rd_size = 0U;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_write_64(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                   uint8_t *data, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;
        inst->buffer[1U] = data[0U];
        inst->buffer[2U] = data[1U];
        inst->buffer[3U] = data[2U];
        inst->buffer[4U] = data[3U];
        inst->buffer[5U] = data[4U];
        inst->buffer[6U] = data[5U];
        inst->buffer[7U] = data[6U];
        inst->buffer[8U] = data[7U];

        if (pec)
        {
            inst->buffer[9U] = mtb_pmbus_int_pec_calc(inst->buffer,
                                                      MTB_PMBUS_CTRL_WRITE_64_SIZE + MTB_PMBUS_CTRL_CMD_SIZE, addr,
                                                      MTB_PMBUS_CTRL_ADDR_WR, 0U);
        }

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size =
            pec ? (MTB_PMBUS_CTRL_WRITE_64_SIZE + MTB_PMBUS_CTRL_CMD_PEC_SIZE) : (MTB_PMBUS_CTRL_WRITE_64_SIZE +
                                                                                  MTB_PMBUS_CTRL_CMD_SIZE);
        inst->transfer_cfg.rd_size = 0U;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_read_32(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                  uint8_t *data, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;

        inst->user_data = data;

        if (pec)
        {
            /* Prepare the first part of PEC */
            inst->pec = mtb_pmbus_int_pec_calc(inst->buffer, MTB_PMBUS_CTRL_CMD_SIZE, addr, MTB_PMBUS_CTRL_ADDR_WR, 0U);
            inst->state |= MTB_PMBUS_CTRL_STATE_PEC_REQ;
        }

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size = MTB_PMBUS_CTRL_CMD_SIZE;
        inst->transfer_cfg.rd_size =
            pec ? (MTB_PMBUS_CTRL_READ_32_SIZE + MTB_PMBUS_CTRL_PEC_SIZE) : MTB_PMBUS_CTRL_READ_32_SIZE;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_read_64(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                  uint8_t *data, bool pec)
{
    mtb_pmbus_ctrl_status_t status = MTB_PMBUS_CTRL_STATUS_BAD_PARAM;
    if ((NULL != inst) && (NULL != data))
    {
        /* Fill internal buffer with data */
        inst->buffer[0U] = (uint8_t)cmd_code;

        inst->user_data = data;

        if (pec)
        {
            /* Prepare the first part of PEC */
            inst->pec = mtb_pmbus_int_pec_calc(inst->buffer, MTB_PMBUS_CTRL_CMD_SIZE, addr, MTB_PMBUS_CTRL_ADDR_WR, 0U);
            inst->state |= MTB_PMBUS_CTRL_STATE_PEC_REQ;
        }

        inst->transfer_cfg.data = inst->buffer;
        inst->transfer_cfg.addr = addr;
        inst->transfer_cfg.wr_size = MTB_PMBUS_CTRL_CMD_SIZE;
        inst->transfer_cfg.rd_size =
            pec ? (MTB_PMBUS_CTRL_READ_64_SIZE + MTB_PMBUS_CTRL_PEC_SIZE) : MTB_PMBUS_CTRL_READ_64_SIZE;
        inst->transfer_cfg.execute_stop = true;

        status = mtb_pmbus_ctrl_execute_transfer_int(inst, NULL, true);
    }
    return status;
}


bool mtb_pmbus_ctrl_handle_rx(mtb_pmbus_ctrl_stc_t *inst, uint8_t byte)
{
    bool status = true;

    if (inst->errors == 0U)
    {
        if ((inst->state & MTB_PMBUS_CTRL_STATE_BLOCK_READ) != 0U)
        {
            /* Assert block buffer size is valid */
            CY_ASSERT(NULL != inst->block_buff_size);

            if ((inst->state & MTB_PMBUS_CTRL_STATE_GOT_BLOCK_COUNT) == 0U)
            {
                /* Calculate the remaining buffer size for block read/process call */
                bool buffer_size_valid = false;
                uint32_t remaining_buffer_size = 0U;
                if ((inst->state & MTB_PMBUS_CTRL_STATE_PROCESS_CALL) != 0U)
                {
                    /* For process call, subtract written size (excluding command and block count)
                     */
                    if (inst->transfer_cfg.wr_size >= MTB_PMBUS_CTRL_CMD_BLOCK_COUNT_SIZE)
                    {
                        uint32_t wr_size_offset = (uint32_t)inst->transfer_cfg.wr_size -
                                                  (uint32_t)MTB_PMBUS_CTRL_CMD_BLOCK_COUNT_SIZE;
                        if ((*inst->block_buff_size) >= wr_size_offset)
                        {
                            remaining_buffer_size = (*inst->block_buff_size) - wr_size_offset;
                            buffer_size_valid = true;
                        }
                    }
                }
                else
                {
                    /* For block read, use the entire buffer size */
                    remaining_buffer_size = (*inst->block_buff_size);
                    buffer_size_valid = true;
                }

                if (buffer_size_valid && ((uint32_t)byte <= remaining_buffer_size))
                {
                    /* Update the state machine after getting block count */
                    *inst->block_buff_size = (uint32_t)byte;
                    inst->byte_requested =
                        ((inst->state & MTB_PMBUS_CTRL_STATE_PEC_REQ) !=
                         0U) ? ((uint16_t)byte + (uint16_t)MTB_PMBUS_CTRL_PEC_SIZE)
                                                                                                : (uint16_t)byte;
                    inst->state |= MTB_PMBUS_CTRL_STATE_GOT_BLOCK_COUNT;
                    MTB_PMBUS_LOG_DBG("Block count = %d", byte);
                    inst->byte_received = 0U;

                    if (inst->byte_requested == 0U)
                    {
                        /* Block count is 0 and PEC isn't requested - end transfer */
                        status = false;
                    }
                }
                else
                {
                    /* Block count is bigger than read/process call buffer size */
                    MTB_PMBUS_LOG_WRN("Block count is too big: %d", byte);
                    inst->errors |= MTB_PMBUS_CTRL_ERR_BLOCK_COUNT;
                    if (inst->cfg->callback_events != NULL)
                    {
                        inst->cfg->callback_events(MTB_PMBUS_CTRL_BLOCK_COUNT_TOO_BIG);
                    }
                    status = false;
                }
            }
            else
            {
                /* Receive data bytes for block read/process call */
                inst->byte_received++;
                if (inst->byte_received == inst->byte_requested)
                {
                    /* All bytes are received */
                    status = false;
                    /* Put last byte into the buffer */
                    inst->buffer[inst->byte_received] = byte;
                }
            }
        }
    }
    else
    {
        /* An error has occurred, stop receiving bytes */
        status = false;
    }

    return status;
}


static uint8_t mtb_pmbus_int_pec_calc(uint8_t *data, uint16_t size, uint8_t addr, uint8_t addr_type, uint8_t init_pec)
{
    uint8_t pec = init_pec;

    uint8_t addr_byte =
        ((addr << 1U) | (((addr_type & MTB_PMBUS_CTRL_ADDR_WR) != 0U) ? 0U : MTB_PMBUS_CTRL_ADDR_RD_BIT));
    pec = mtb_pmbus_int_crc8_update(pec, addr_byte);

    for (uint16_t i = 0U; i < size; i++)
    {
        pec = mtb_pmbus_int_crc8_update(pec, data[i]);
    }

    return pec;
}


static void mtb_pmbus_ctrl_reset_state(mtb_pmbus_ctrl_stc_t *inst)
{
    CY_ASSERT(NULL != inst);
    /* Reset state */
    inst->transfer_cap = 0U;
    inst->state = 0U;
    inst->pec = 0U;
    inst->user_data = NULL;
    inst->block_buff_size = NULL;
    inst->byte_requested = 0U;
    inst->byte_received = 0U;
    inst->transfer_cfg.data = NULL;
    inst->transfer_cfg.addr = 0U;
    inst->transfer_cfg.wr_size = 0U;
    inst->transfer_cfg.rd_size = 0U;
    inst->transfer_cfg.execute_stop = true;
}


#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
void mtb_pmbus_ctrl_handle_timeout(mtb_pmbus_ctrl_stc_t *inst)
{
    inst->errors |= MTB_PMBUS_CTRL_ERR_TIMEOUT;
    if (0U != (inst->state & MTB_PMBUS_CTRL_STATE_DIR_WR))
    {
        mtb_pmbus_ctrl_hal_abort_write(inst);
    }
    else if (0U != (inst->state & MTB_PMBUS_CTRL_STATE_DIR_RD))
    {
        mtb_pmbus_ctrl_hal_abort_read(inst);
    }
    else
    {
        /* Do nothing if the controller is not busy */
    }

    if (inst->cfg->callback_events != NULL)
    {
        inst->cfg->callback_events(MTB_PMBUS_CTRL_TIMEOUT);
    }
    mtb_pmbus_ctrl_reset_state(inst);
}
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
