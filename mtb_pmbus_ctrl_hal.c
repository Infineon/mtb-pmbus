/***************************************************************************//**
* \file mtb_pmbus_ctrl_hal.c
* \version 1.0
*
* Provides implementation of the HAL layer for PMBus Controller.
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
#include <stdint.h>


/** Controller section */
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 11.8', 6,
                             'Casting a volatile pointer to non-volatile is required in an interrupt context to manipulate a user-provided mtb_pmbus_ctrl_stc_t object. Volatile is preserved during the variables lifecycle to prevent compiler optimization.');
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 8.9', 1,
                             'File scope necessary - variable shared across ISR and PDL-invoked callbacks');
static volatile mtb_pmbus_ctrl_stc_t *ctrl_inst = NULL;
CY_MISRA_BLOCK_END('MISRA C-2012 Rule 8.9');

void mtb_pmbus_ctrl_isr(mtb_pmbus_ctrl_stc_t *inst)
{
    mtb_pmbus_ctrl_stc_t *old_inst = (mtb_pmbus_ctrl_stc_t *)ctrl_inst;

    ctrl_inst = inst;
#if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U))
    Cy_SCB_I2C_Interrupt(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->pdl_i2c_context);
#else
    Cy_SCB_I2C_MasterInterrupt(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->pdl_i2c_context);
#endif /* #if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U)) */
    ctrl_inst = old_inst;
}


/* Internal function */
void mtb_pmbus_ctrl_hal_events_handler(uint32_t event);
cy_en_scb_i2c_command_t mtb_pmbus_ctrl_hal_scb_byte_handler(uint8_t byte_received);

void mtb_pmbus_ctrl_hal_events_handler(uint32_t event)
{
#if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U))
    if ((event & CY_SCB_I2C_SLAVE_ERR_EVENT) != 0U)
    {
        mtb_pmbus_ctrl_handle_hnp((mtb_pmbus_ctrl_stc_t *)ctrl_inst, MTB_PMBUS_CTRL_HOST_NOTIFY_ERROR);
    }

    if ((event & CY_SCB_I2C_SLAVE_WRITE_EVENT) != 0U)
    {
        mtb_pmbus_ctrl_handle_hnp((mtb_pmbus_ctrl_stc_t *)ctrl_inst, MTB_PMBUS_CTRL_HOST_NOTIFY_REQUEST_RECEIVED);
    }

    if ((event & CY_SCB_I2C_SLAVE_WR_CMPLT_EVENT) != 0U)
    {
        mtb_pmbus_ctrl_handle_hnp((mtb_pmbus_ctrl_stc_t *)ctrl_inst, MTB_PMBUS_CTRL_HOST_NOTIFY_COMPLETE);
    }
#endif /* #if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U)) */
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U))
    if ((event & (CY_SCB_I2C_MASTER_TIMEOUT0_EVENT
                  | CY_SCB_I2C_MASTER_TIMEOUT1_EVENT
                  | CY_SCB_I2C_MASTER_TIMEOUT2_EVENT)) != 0U)
    {
        mtb_pmbus_ctrl_handle_timeout((mtb_pmbus_ctrl_stc_t *)ctrl_inst);
    }
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U))  */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
    if ((event & CY_SCB_I2C_MASTER_ERR_EVENT) != 0U)
    {
        mtb_pmbus_ctrl_events_handler((mtb_pmbus_ctrl_stc_t *)ctrl_inst, MTB_PMBUS_CTRL_EVENTS_ERR);
    }

    if ((event & CY_SCB_I2C_MASTER_WR_CMPLT_EVENT) != 0U)
    {
        mtb_pmbus_ctrl_events_handler((mtb_pmbus_ctrl_stc_t *)ctrl_inst, MTB_PMBUS_CTRL_EVENTS_WR_DONE);
    }
    else if ((event & CY_SCB_I2C_MASTER_RD_CMPLT_EVENT) != 0U)
    {
        mtb_pmbus_ctrl_events_handler((mtb_pmbus_ctrl_stc_t *)ctrl_inst, MTB_PMBUS_CTRL_EVENTS_RD_DONE);
    }
    else
    {
        /* Do nothing for unsupported events */
    }
}


cy_en_scb_i2c_command_t mtb_pmbus_ctrl_hal_scb_byte_handler(uint8_t byte_received)
{
    return mtb_pmbus_ctrl_handle_rx((mtb_pmbus_ctrl_stc_t *)ctrl_inst, byte_received) ? CY_SCB_I2C_ACK
                                                                                      : CY_SCB_I2C_NAK;
}


CY_MISRA_BLOCK_END('MISRA C-2012 Rule 11.8');

#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
void mtb_pmbus_ctrl_timer_isr(mtb_pmbus_ctrl_stc_t *inst)
{
    uint32_t int_timer_mask = Cy_TCPWM_GetInterruptStatusMasked(inst->cfg->hal_cfg->timeout_tcpwm_base,
                                                                inst->cfg->hal_cfg->timeout_tcpwm_cntnum);

    if ((int_timer_mask & CY_TCPWM_INT_ON_CC0) != 0U)
    {
        mtb_pmbus_ctrl_handle_timeout(inst);
    }
    /* Clear the interrupt */
    Cy_TCPWM_ClearInterrupt(inst->cfg->hal_cfg->timeout_tcpwm_base,
                            inst->cfg->hal_cfg->timeout_tcpwm_cntnum,
                            int_timer_mask);
}
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */

void mtb_pmbus_hal_delay_us(mtb_pmbus_ctrl_stc_t *inst, uint32_t delay)
{
    (void)inst;
    uint16_t cycles = (uint16_t)(delay / (uint16_t)UINT16_MAX);
    uint16_t last_cycle = (uint16_t)(delay % (uint16_t)UINT16_MAX);
    while (cycles > 0U)
    {
        Cy_SysLib_DelayUs((uint16_t)UINT16_MAX);
        cycles--;
    }
    Cy_SysLib_DelayUs(last_cycle);
}


void mtb_pmbus_ctrl_hal_init(mtb_pmbus_ctrl_stc_t *inst)
{
    Cy_SCB_I2C_RegisterEventCallback(inst->cfg->hal_cfg->hw_ptr,
                                     (cy_cb_scb_i2c_handle_events_t)mtb_pmbus_ctrl_hal_events_handler,
                                     inst->cfg->hal_cfg->pdl_i2c_context);
    Cy_SCB_I2C_RegisterMasterByteReceivedCallback(inst->cfg->hal_cfg->hw_ptr,
                                                  (cy_cb_scb_i2c_handle_byte_t)mtb_pmbus_ctrl_hal_scb_byte_handler,
                                                  inst->cfg->hal_cfg->pdl_i2c_context);
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U))
    Cy_SCB_SetTgsInterruptMask(inst->cfg->hal_cfg->hw_ptr, ((uint32_t)CY_SCB_TGS_INTR_0_UNDERFLOW
                                                            | (uint32_t)CY_SCB_TGS_INTR_1_UNDERFLOW
                                                            | (uint32_t)CY_SCB_TGS_INTR_2_UNDERFLOW
                                                            ));
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */

#if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U))
    /* Setup buffer for the Host Notify */
    Cy_SCB_I2C_SlaveConfigWriteBuf(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->notify_data,
                                   MTB_PMBUS_HOST_NOTIFY_DATA_SIZE,
                                   inst->cfg->hal_cfg->pdl_i2c_context);
#endif /* #if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U)) */
}


bool mtb_pmbus_ctrl_hal_is_bus_busy(mtb_pmbus_ctrl_stc_t *inst)
{
    return Cy_SCB_I2C_IsBusBusy(inst->cfg->hal_cfg->hw_ptr);
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_hal_write(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint8_t *data, uint32_t size,
                                                 bool stop)
{
    mtb_pmbus_ctrl_status_t status;

    inst->cfg->hal_cfg->transfer_cfg.slaveAddress = addr;
    inst->cfg->hal_cfg->transfer_cfg.buffer = data;
    inst->cfg->hal_cfg->transfer_cfg.bufferSize = size;
    inst->cfg->hal_cfg->transfer_cfg.xferPending = !stop;

    cy_en_scb_i2c_status_t pdl_status = Cy_SCB_I2C_MasterWrite(inst->cfg->hal_cfg->hw_ptr,
                                                               &inst->cfg->hal_cfg->transfer_cfg,
                                                               inst->cfg->hal_cfg->pdl_i2c_context);

    if (CY_SCB_I2C_SUCCESS == pdl_status)
    {
        status = MTB_PMBUS_CTRL_STATUS_SUCCESS;
    }
    else if (CY_SCB_I2C_MASTER_NOT_READY == pdl_status)
    {
        status = MTB_PMBUS_CTRL_STATUS_IS_BUSY;
    }
    else
    {
        status = MTB_PMBUS_CTRL_STATUS_UNKNOWN_ERR;
    }

    return status;
}


mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_hal_read(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint8_t *data, uint32_t size,
                                                bool stop)
{
    mtb_pmbus_ctrl_status_t status;

    inst->cfg->hal_cfg->transfer_cfg.slaveAddress = addr;
    inst->cfg->hal_cfg->transfer_cfg.buffer = data;
    inst->cfg->hal_cfg->transfer_cfg.bufferSize = size;
    inst->cfg->hal_cfg->transfer_cfg.xferPending = !stop;

    cy_en_scb_i2c_status_t pdl_status = Cy_SCB_I2C_MasterRead(inst->cfg->hal_cfg->hw_ptr,
                                                              &inst->cfg->hal_cfg->transfer_cfg,
                                                              inst->cfg->hal_cfg->pdl_i2c_context);

    if (CY_SCB_I2C_SUCCESS == pdl_status)
    {
        status = MTB_PMBUS_CTRL_STATUS_SUCCESS;
    }
    else if (CY_SCB_I2C_MASTER_NOT_READY == pdl_status)
    {
        status = MTB_PMBUS_CTRL_STATUS_IS_BUSY;
    }
    else
    {
        status = MTB_PMBUS_CTRL_STATUS_UNKNOWN_ERR;
    }

    return status;
}


void mtb_pmbus_ctrl_hal_set_errors(mtb_pmbus_ctrl_stc_t *inst)
{
    uint32_t master_status =
        Cy_SCB_I2C_MasterGetStatus(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->pdl_i2c_context);
    if (0U != (master_status & CY_SCB_I2C_MASTER_ADDR_NAK))
    {
        inst->errors |= MTB_PMBUS_CTRL_ERR_NACK_ADDR;
    }
    else if (0U != (master_status & CY_SCB_I2C_MASTER_DATA_NAK))
    {
        uint32_t byte_transferred = Cy_SCB_I2C_MasterGetTransferCount(inst->cfg->hal_cfg->hw_ptr,
                                                                      inst->cfg->hal_cfg->pdl_i2c_context);
        if ((NULL != inst->transfer_cfg.data) && (byte_transferred > 0U))
        {
            MTB_PMBUS_LOG_DBG("Target NACKed %s byte: 0x%02X", (byte_transferred == 1U) ? "cmd" : "data",
                              inst->transfer_cfg.data[byte_transferred - 1U]);
        }
        inst->errors |= (byte_transferred == 1U) ? MTB_PMBUS_CTRL_ERR_NACK_CMD : MTB_PMBUS_CTRL_ERR_NACK_DATA;
    }
    else if (0U != (master_status & CY_SCB_I2C_MASTER_ARB_LOST))
    {
        inst->errors |= MTB_PMBUS_CTRL_ERR_ARB_LOST;
    }
    else if (0U != (master_status & CY_SCB_I2C_MASTER_BUS_ERR))
    {
        inst->errors |= MTB_PMBUS_CTRL_ERR_BUS_ERROR;
    }
    else if (0U != (master_status & CY_SCB_I2C_MASTER_ABORT_START))
    {
        inst->errors |= MTB_PMBUS_CTRL_ERR_ABORT_START;
    }
    else
    {
        /* Do nothing for unsupported events */
    }
}


uint32_t mtb_pmbus_ctrl_hal_get_status(mtb_pmbus_ctrl_stc_t *inst)
{
    return Cy_SCB_I2C_MasterGetStatus(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->pdl_i2c_context);
}


uint32_t mtb_pmbus_ctrl_hal_get_byte_count(mtb_pmbus_ctrl_stc_t *inst)
{
    return Cy_SCB_I2C_MasterGetTransferCount(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->pdl_i2c_context);
}


void mtb_pmbus_ctrl_hal_abort_write(mtb_pmbus_ctrl_stc_t *inst)
{
    Cy_SCB_I2C_MasterAbortWrite(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->pdl_i2c_context);
}


void mtb_pmbus_ctrl_hal_abort_read(mtb_pmbus_ctrl_stc_t *inst)
{
    Cy_SCB_I2C_MasterAbortRead(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->pdl_i2c_context);
}


#if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U))
void mtb_pmbus_ctrl_hal_hnp_clr_write_status(mtb_pmbus_ctrl_stc_t *inst)
{
    (void)Cy_SCB_I2C_SlaveClearWriteStatus(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->pdl_i2c_context);
}


void mtb_pmbus_ctrl_hal_hnp_resetup_buf(mtb_pmbus_ctrl_stc_t *inst)
{
    Cy_SCB_I2C_SlaveConfigWriteBuf(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->notify_data,
                                   MTB_PMBUS_HOST_NOTIFY_DATA_SIZE,
                                   inst->cfg->hal_cfg->pdl_i2c_context);
}


uint8_t mtb_pmbus_ctrl_hal_hnp_get_trgt_addr(mtb_pmbus_ctrl_stc_t *inst)
{
    return (uint8_t)inst->cfg->hal_cfg->notify_data[0U];
}


uint16_t mtb_pmbus_ctrl_hal_hnp_get_trgt_data(mtb_pmbus_ctrl_stc_t *inst)
{
    uint16_t data = 0U;

    data = (uint16_t)inst->cfg->hal_cfg->notify_data[1U];
    data |= ((uint16_t)inst->cfg->hal_cfg->notify_data[2U] << 8U);

    return data;
}


bool mtb_pmbus_ctrl_hal_hnp_transfer_is_ok(mtb_pmbus_ctrl_stc_t *inst)
{
    return ((((CY_SCB_I2C_SLAVE_WR_OVRFL | CY_SCB_I2C_SLAVE_RESTART) &
              Cy_SCB_I2C_SlaveGetStatus(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->pdl_i2c_context)) == 0U) &&
            (Cy_SCB_I2C_SlaveGetWriteTransferCount(inst->cfg->hal_cfg->hw_ptr, inst->cfg->hal_cfg->pdl_i2c_context) ==
             MTB_PMBUS_HOST_NOTIFY_DATA_SIZE));
}
#endif /* #if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U)) */
