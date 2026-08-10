/***************************************************************************//**
* \file mtb_pmbus_trgt_hal.c
* \version 1.0
*
* Provides implementation of the HAL layer for the PMBus Target.
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
#include <stdint.h>

#define MTB_PMBUS_SMBALERT_SET                  (0U)
#define MTB_PMBUS_SMBALERT_CLEAR                (1U)


#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
void mtb_pmbus_timer_isr(mtb_pmbus_stc_t *inst)
{
    uint32_t int_timer_mask = Cy_TCPWM_GetInterruptStatusMasked(inst->cfg->hw_config->hal_config->timeout_tcpwm_base,
                                                                inst->cfg->hw_config->hal_config->timeout_tcpwm_cntnum);

    if ((int_timer_mask & CY_TCPWM_INT_ON_CC0) != 0U)
    {
        mtb_pmbus_handle_timeout(inst);
    }
    /* Clear the interrupt */
    Cy_TCPWM_ClearInterrupt(inst->cfg->hw_config->hal_config->timeout_tcpwm_base,
                            inst->cfg->hw_config->hal_config->timeout_tcpwm_cntnum,
                            int_timer_mask);
}
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */

CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 11.8', 11, \
                             'Casting a volatile pointer to non-volatile is required in an interrupt context to manipulate a user-provided mtb_pmbus_stc_t object. Volatile is preserved during the variables lifecycle to prevent compiler optimization.');
static volatile mtb_pmbus_stc_t *trgt_inst = NULL;
void mtb_pmbus_i2c_isr(mtb_pmbus_stc_t *inst)
{
    mtb_pmbus_stc_t *old_inst = (mtb_pmbus_stc_t *)trgt_inst;

    trgt_inst = inst;
#if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U))
    Cy_SCB_I2C_Interrupt(inst->cfg->hw_config->hal_config->hw_ptr, inst->cfg->hw_config->hal_config->pdl_i2c_context);
#else
    Cy_SCB_I2C_SlaveInterrupt(inst->cfg->hw_config->hal_config->hw_ptr,
                              inst->cfg->hw_config->hal_config->pdl_i2c_context);
#endif /* #if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) */
    trgt_inst = old_inst;
}


void mtb_pmbus_hal_scb_event_handler(uint32_t event);
cy_en_scb_i2c_command_t mtb_pmbus_hal_scb_addr_handler(uint32_t event);
cy_en_scb_i2c_command_t mtb_pmbus_hal_scb_byte_handler(uint8_t byte_received);


void mtb_pmbus_hal_scb_event_handler(uint32_t event)
{
#if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U))
    if ((CY_SCB_I2C_MASTER_ERR_EVENT & event) != 0U)
    {
        mtb_pmbus_handle_hnp((mtb_pmbus_stc_t *)trgt_inst, MTB_PMBUS_HOST_NOTIFY_ERROR);
    }

    if ((CY_SCB_I2C_MASTER_ARB_LOST_EVENT & event) != 0U)
    {
        mtb_pmbus_handle_hnp((mtb_pmbus_stc_t *)trgt_inst, MTB_PMBUS_HOST_NOTIFY_ARB_LOST);
    }

    if ((CY_SCB_I2C_MASTER_WR_CMPLT_EVENT & event) != 0U)
    {
        mtb_pmbus_handle_hnp((mtb_pmbus_stc_t *)trgt_inst, MTB_PMBUS_HOST_NOTIFY_COMPLETE);
    }
#endif /* #if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) */

    if ((event & CY_SCB_I2C_SLAVE_ERR_EVENT) != 0U)
    {
        mtb_pmbus_handle_bus_error((mtb_pmbus_stc_t *)trgt_inst);
    }

    if ((event & CY_SCB_I2C_SLAVE_ARB_LOST_EVENT) != 0U)
    {
        mtb_pmbus_handle_arb_lost((mtb_pmbus_stc_t *)trgt_inst);
        Cy_SCB_I2C_SlaveAbortRead(trgt_inst->cfg->hw_config->hal_config->hw_ptr,
                                  trgt_inst->cfg->hw_config->hal_config->pdl_i2c_context);
        (void)Cy_SCB_I2C_SlaveClearReadStatus(trgt_inst->cfg->hw_config->hal_config->hw_ptr,
                                              trgt_inst->cfg->hw_config->hal_config->pdl_i2c_context);
    }

    if ((event & CY_SCB_I2C_SLAVE_RESTART_EVENT) != 0U)
    {
        mtb_pmbus_handle_restart((mtb_pmbus_stc_t *)trgt_inst);
    }

    if ((event & CY_SCB_I2C_SLAVE_STOP_ANY_EVENT) != 0U)
    {
        mtb_pmbus_handle_stop((mtb_pmbus_stc_t *)trgt_inst);
        if (!mtb_pmbus_hal_is_tx_cmpl((mtb_pmbus_stc_t *)trgt_inst))
        {
            Cy_SCB_I2C_SlaveAbortRead(trgt_inst->cfg->hw_config->hal_config->hw_ptr,
                                      trgt_inst->cfg->hw_config->hal_config->pdl_i2c_context);
            (void)Cy_SCB_I2C_SlaveClearReadStatus(trgt_inst->cfg->hw_config->hal_config->hw_ptr,
                                                  trgt_inst->cfg->hw_config->hal_config->pdl_i2c_context);
        }
    }

#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U))
    if ((event & (CY_SCB_I2C_SLAVE_TIMEOUT0_EVENT
                  | CY_SCB_I2C_SLAVE_TIMEOUT1_EVENT
                  | CY_SCB_I2C_SLAVE_TIMEOUT2_EVENT)) != 0U)
    {
        mtb_pmbus_handle_timeout((mtb_pmbus_stc_t *)trgt_inst);
    }
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U))  */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */

    if ((event & (CY_SCB_I2C_SLAVE_RD_BUF_EMPTY_EVENT
                  | CY_SCB_I2C_SLAVE_READ_EVENT)) != 0U)
    {
        mtb_pmbus_handle_tx((mtb_pmbus_stc_t *)trgt_inst, &trgt_inst->cfg->hw_config->hal_config->hal_read_buf_ptr,
                            &trgt_inst->cfg->hw_config->hal_config->hal_read_buf_size);
        Cy_SCB_I2C_SlaveConfigReadBuf(trgt_inst->cfg->hw_config->hal_config->hw_ptr,
                                      trgt_inst->cfg->hw_config->hal_config->hal_read_buf_ptr,
                                      trgt_inst->cfg->hw_config->hal_config->hal_read_buf_size,
                                      trgt_inst->cfg->hw_config->hal_config->pdl_i2c_context);
    }
}


cy_en_scb_i2c_command_t mtb_pmbus_hal_scb_addr_handler(uint32_t event)
{
    bool status = false;
    uint32_t local_event = event;
#if (defined (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U))
    if ((local_event & CY_SCB_I2C_GENERAL_CALL_EVENT) != 0U)
    {
        (void)Cy_SCB_ReadRxFifo(trgt_inst->cfg->hw_config->hal_config->hw_ptr);
        status = mtb_pmbus_handle_gen_call((mtb_pmbus_stc_t *)trgt_inst);
        local_event = 0U;
    }
#endif /* #if (defined (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)) */

    if ((local_event & CY_SCB_I2C_ADDR_IN_FIFO_EVENT) != 0U)
    {
        uint8_t device_address = (uint8_t)Cy_SCB_ReadRxFifo(trgt_inst->cfg->hw_config->hal_config->hw_ptr);
        status = mtb_pmbus_handle_addr((mtb_pmbus_stc_t *)trgt_inst, device_address);
    }

    return status ? CY_SCB_I2C_ACK : CY_SCB_I2C_NAK;
}


cy_en_scb_i2c_command_t mtb_pmbus_hal_scb_byte_handler(uint8_t byte_received)
{
    bool status = false;
    status = mtb_pmbus_handle_rx((mtb_pmbus_stc_t *)trgt_inst, byte_received);
    Cy_SCB_I2C_SlaveConfigWriteBuf(trgt_inst->cfg->hw_config->hal_config->hw_ptr,
                                   trgt_inst->cfg->hw_config->hal_config->hal_write_buf, MTB_PMBUS_HAL_WRITE_BUF_SIZE,
                                   trgt_inst->cfg->hw_config->hal_config->pdl_i2c_context);

    return status ? CY_SCB_I2C_ACK : CY_SCB_I2C_NAK;
}


void mtb_pmbus_hal_init(mtb_pmbus_stc_t *inst)
{
    if ((inst->cfg->hw_config != NULL) &&
        (inst->cfg->hw_config->hal_config != NULL) &&
        (inst->cfg->hw_config->hal_config->hw_ptr != NULL) &&
        (inst->cfg->hw_config->hal_config->pdl_i2c_context != NULL))
    {
        (void)memset(inst->cfg->hw_config->hal_config->hal_write_buf, (int32_t)0U, MTB_PMBUS_HAL_WRITE_BUF_SIZE);

        Cy_SCB_I2C_SlaveConfigWriteBuf(inst->cfg->hw_config->hal_config->hw_ptr,
                                       inst->cfg->hw_config->hal_config->hal_write_buf, MTB_PMBUS_HAL_WRITE_BUF_SIZE,
                                       inst->cfg->hw_config->hal_config->pdl_i2c_context);
        Cy_SCB_I2C_RegisterEventCallback(inst->cfg->hw_config->hal_config->hw_ptr,
                                         (cy_cb_scb_i2c_handle_events_t)mtb_pmbus_hal_scb_event_handler,
                                         inst->cfg->hw_config->hal_config->pdl_i2c_context);

#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U))
        Cy_SCB_SetTgsInterruptMask(inst->cfg->hw_config->hal_config->hw_ptr, ((uint32_t)CY_SCB_TGS_INTR_0_UNDERFLOW
                                                                              | (uint32_t)CY_SCB_TGS_INTR_1_UNDERFLOW
                                                                              | (uint32_t)CY_SCB_TGS_INTR_2_UNDERFLOW
                                                                              ));
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */

        Cy_SCB_I2C_RegisterAddrCallback(inst->cfg->hw_config->hal_config->hw_ptr,
                                        (cy_cb_scb_i2c_handle_addr_t)mtb_pmbus_hal_scb_addr_handler,
                                        inst->cfg->hw_config->hal_config->pdl_i2c_context);

        Cy_SCB_I2C_RegisterSlaveByteReceivedCallback(inst->cfg->hw_config->hal_config->hw_ptr,
                                                     (cy_cb_scb_i2c_handle_byte_t)mtb_pmbus_hal_scb_byte_handler,
                                                     inst->cfg->hw_config->hal_config->pdl_i2c_context);
    }
}


CY_MISRA_BLOCK_END('MISRA C-2012 Rule 11.8');

/* Return true, if all data is send after last call of mtb_pmbus_handle_tx() */
bool mtb_pmbus_hal_is_tx_cmpl(mtb_pmbus_stc_t *inst)
{
    return Cy_SCB_IsTxComplete(inst->cfg->hw_config->hal_config->hw_ptr);
}


#if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U)
bool mtb_pmbus_hal_check_hw_cfg(const mtb_pmbus_stc_config_t *cfg)
{
    bool status = true;

    if (cfg->hw_config == NULL)
    {
        status = false;
        MTB_PMBUS_LOG_ERR("Hardware configuration (hw_config) is NULL");
    }
    else
    {
        if (cfg->hw_config->hal_config == NULL)
        {
            status = false;
            MTB_PMBUS_LOG_ERR("HAL configuration (hal_config) is NULL in hw_config");
        }

        if (cfg->hw_config->hw_resource_ctrl_callback == NULL)
        {
            status = false;
            MTB_PMBUS_LOG_ERR("The hardware resource control callback (hw_resource_ctrl_callback) is NULL in hw_config");
        }

        if (cfg->hw_config->enable_hw_irq_callback == NULL)
        {
            status = false;
            MTB_PMBUS_LOG_ERR("Enable IRQ callback (enable_hw_irq_callback) is NULL in hw_config");
        }

        if (cfg->hw_config->disable_hw_irq_callback == NULL)
        {
            status = false;
            MTB_PMBUS_LOG_ERR("Disable IRQ callback (disable_hw_irq_callback) is NULL in hw_config");
        }
    }

    return status;
}
#endif /* #if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U) */

#if (defined (MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U))
void mtb_pmbus_hal_smbalert_set(mtb_pmbus_stc_t *inst)
{
    if (inst->cfg->hw_config->hal_config->smbalert_port_addr != NULL)
    {
        Cy_GPIO_Write(inst->cfg->hw_config->hal_config->smbalert_port_addr,
                      inst->cfg->hw_config->hal_config->smbalert_pin_num,
                      MTB_PMBUS_SMBALERT_SET);
    }
}


void mtb_pmbus_hal_smbalert_clear(mtb_pmbus_stc_t *inst)
{
    if (inst->cfg->hw_config->hal_config->smbalert_port_addr != NULL)
    {
        Cy_GPIO_Write(inst->cfg->hw_config->hal_config->smbalert_port_addr,
                      inst->cfg->hw_config->hal_config->smbalert_pin_num,
                      MTB_PMBUS_SMBALERT_CLEAR);
    }
}
#endif /* #if (defined (MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) */

#if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U))
bool mtb_pmbus_hal_hnp_write(mtb_pmbus_stc_t *inst, uint8_t target_addr, uint32_t data)
{
    bool status = false;
    cy_stc_scb_i2c_master_xfer_config_t xfer_config = { 0 };

    inst->cfg->hw_config->hal_config->notify_data[0U] = target_addr;
    inst->cfg->hw_config->hal_config->notify_data[1U] = (uint8_t)(data & 0xFFU);
    inst->cfg->hw_config->hal_config->notify_data[2U] = (uint8_t)((data >> 8U) & 0xFFU);

    xfer_config.slaveAddress = MTB_PMBUS_HOST_NOTIFY_ADDR;
    xfer_config.buffer = inst->cfg->hw_config->hal_config->notify_data;
    xfer_config.bufferSize = MTB_PMBUS_HOST_NOTIFY_DATA_SIZE;
    xfer_config.xferPending = false;

    if (CY_SCB_I2C_SUCCESS == Cy_SCB_I2C_MasterWrite(inst->cfg->hw_config->hal_config->hw_ptr,
                                                     &xfer_config,
                                                     inst->cfg->hw_config->hal_config->pdl_i2c_context))
    {
        status = true;
    }

    return status;
}


void mtb_pmbus_hal_hnp_abort(mtb_pmbus_stc_t *inst)
{
    (void)Cy_SCB_I2C_MasterAbortWrite(inst->cfg->hw_config->hal_config->hw_ptr,
                                      inst->cfg->hw_config->hal_config->pdl_i2c_context);
}
#endif /* #if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) */

bool mtb_pmbus_hal_pause_tx(mtb_pmbus_stc_t *inst)
{
    return Cy_SCB_I2C_SlavePauseTransmit(inst->cfg->hw_config->hal_config->hw_ptr,
                                         inst->cfg->hw_config->hal_config->pdl_i2c_context);
}


bool mtb_pmbus_hal_resume_tx(mtb_pmbus_stc_t *inst)
{
    return Cy_SCB_I2C_SlaveResumeTransmit(inst->cfg->hw_config->hal_config->hw_ptr,
                                          inst->cfg->hw_config->hal_config->pdl_i2c_context);
}
