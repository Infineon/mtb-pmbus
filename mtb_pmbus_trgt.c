/***************************************************************************//**
* \file mtb_pmbus_trgt.c
* \version 1.0
*
* Provides API declarations for the PMBus Target Middleware.
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

/* The default value for received byte protocol */
#define MTB_PMBUS_RECEIVED_BYTE_DEFAULT             (0xFFU)


/* Register Implemented commands */
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
static void mtb_pmbus_int_add_impl_command(mtb_pmbus_stc_t *inst);
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
#if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U)
static mtb_pmbus_status_t mtb_pmbus_int_check_hw_cfg(mtb_pmbus_stc_t *inst);
static mtb_pmbus_status_t mtb_pmbus_int_check_cmd_table(mtb_pmbus_stc_t *inst);
#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
static mtb_pmbus_status_t mtb_pmbus_int_check_ext_cmd_table(mtb_pmbus_stc_t *inst);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
static mtb_pmbus_status_t mtb_pmbus_int_check_cfg(mtb_pmbus_stc_t *inst);
#endif /* #if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U) */

mtb_pmbus_status_t mtb_pmbus_init(mtb_pmbus_stc_t *inst, mtb_pmbus_stc_config_t const *config)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) || (defined(MTB_PMBUS_PHASES_NUM) && \
    (MTB_PMBUS_PHASES_NUM != 0U))
    bool cmd_cap_error = false;
#endif \
    /* #if ((defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) ||
       (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))) */
    if ((inst != NULL) && (config != NULL))
    {
        inst->cfg = config;
#if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U)
        status = mtb_pmbus_int_check_cfg(inst);
        if (MTB_PMBUS_STATUS_SUCCESS == status)
#endif /* #if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U) */
        {
            mtb_pmbus_hal_init(inst);
            /* Fill the lookup tables with default value */
            for (uint16_t i = 0U; i < MTB_PMBUS_CMD_MAX_NUM; i++)
            {
                inst->cmd_lookup_tbl[i].flags = 0U;
#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) && (defined(MTB_PMBUS_SUPPORT_PMBUS) && \
                (MTB_PMBUS_SUPPORT_PMBUS != 0U))
                if (inst->cfg->enable_ext_cmd && inst->cfg->enable_pmbus)
                {
                    inst->ext_cmd_lookup_tbl[i].flags = 0U;
                }
#endif \
                /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) &&
                   (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */
            }

            /* Find and store commands position */
            for (uint16_t i = 0U; i < inst->cfg->cmd_num; i++)
            {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                if (((inst->cfg->cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) != 0U) && (inst->cfg->num_pages == 0U))
                {
                    MTB_PMBUS_LOG_ERR("The instance does not support paged commands, but cmd [%x] is paged", i);
                    cmd_cap_error = true;
                }
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                if (((inst->cfg->cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) != 0U) &&
                    (inst->cfg->num_phases == 0U))
                {
                    MTB_PMBUS_LOG_ERR("The instance does not support phased commands, but cmd [%x] is phased", i);
                    cmd_cap_error = true;
                }
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) || (defined(MTB_PMBUS_PHASES_NUM) && \
                (MTB_PMBUS_PHASES_NUM != 0U))
                if (cmd_cap_error)
                {
                    break;
                }
                else
#endif \
                /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) ||
                   (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
                {
                    uint8_t cmd_code = inst->cfg->cmd_table[i].cmd_code;
                    inst->cmd_lookup_tbl[cmd_code].cmd_pos = (uint8_t)i;
                    inst->cmd_lookup_tbl[cmd_code].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                           MTB_PMBUS_CMD_FLAG_IS_ENABLED;
                    MTB_PMBUS_LOG_DBG("Command %x is added to lookup table", cmd_code);
                }
            }

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) && (defined(MTB_PMBUS_SUPPORT_PMBUS) && \
            (MTB_PMBUS_SUPPORT_PMBUS != 0U))
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) || (defined(MTB_PMBUS_PHASES_NUM) && \
            (MTB_PMBUS_PHASES_NUM != 0U))
            if (!cmd_cap_error)
#endif \
            /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) ||
               (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
            {
                if (inst->cfg->enable_ext_cmd && inst->cfg->enable_pmbus)
                {
                    /* Find and store extended commands position */
                    for (uint16_t i = 0U; i < inst->cfg->ext_cmd_num; i++)
                    {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                        if (((inst->cfg->ext_cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) != 0U) &&
                            (inst->cfg->num_pages == 0U))
                        {
                            MTB_PMBUS_LOG_ERR("The instance does not support paged commands, but ext cmd [%x] is paged",
                                              i);
                            cmd_cap_error = true;
                        }
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                        if (((inst->cfg->ext_cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) != 0U) &&
                            (inst->cfg->num_phases == 0U))
                        {
                            MTB_PMBUS_LOG_ERR(
                                "The instance does not support phased commands, but ext cmd [%x] is phased", i);
                            cmd_cap_error = true;
                        }
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) || (defined(MTB_PMBUS_PHASES_NUM) && \
                        (MTB_PMBUS_PHASES_NUM != 0U))
                        if (cmd_cap_error)
                        {
                            break;
                        }
                        else
#endif \
                        /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) ||
                           (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
                        {
                            uint8_t cmd_code = inst->cfg->ext_cmd_table[i].cmd_code;
                            inst->ext_cmd_lookup_tbl[cmd_code].cmd_pos = (uint8_t)i;
                            inst->ext_cmd_lookup_tbl[cmd_code].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                                       MTB_PMBUS_CMD_FLAG_IS_ENABLED;
                            MTB_PMBUS_LOG_DBG("Ext command %x is added to lookup table", cmd_code);
                        }
                    }
                    /* Enable extended command by default */
                    inst->ext_cmd_en = true;
                }
                else
                {
                    inst->ext_cmd_en = false;
                }
            }
#endif \
            /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) &&
               (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) || (defined(MTB_PMBUS_PHASES_NUM) && \
            (MTB_PMBUS_PHASES_NUM != 0U))
            if (!cmd_cap_error)
#endif \
            /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) ||
               (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
            {
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U) && (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U))
                for (uint8_t i = 0U; i < MTB_PMBUS_IMPL_CMD_NUM; i++)
                {
                    inst->pre_impl_cmd_lookup_tbl[i].flags = 0U;
                }

                if ((inst->cfg->impl_cmd_mask != 0U) && inst->cfg->enable_pmbus)
                {
                    mtb_pmbus_int_add_impl_command(inst);
                }
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) && (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */
                /* Set page and phase to 0 */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                inst->act_page = 0U;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
                inst->act_phase = 0U;
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_IMPL_CMD_PX_PLUS) && (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U))
                inst->stored_page = 0U;
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_PX_PLUS) && (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U)) */
#if (defined(MTB_PMBUS_IMPL_CMD_P2_PLUS) && (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U))
                inst->stored_phase = 0U;
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_P2_PLUS) && (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U)) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) && (defined(MTB_PMBUS_SUPPORT_PMBUS) && \
                (MTB_PMBUS_SUPPORT_PMBUS != 0U))
                if (inst->cfg->enable_zone && inst->cfg->enable_pmbus)
                {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
                    if (inst->cfg->num_pages != 0U)
                    {
                        /* Set default Zones for all pages */
                        for (uint8_t i = 0U; i < inst->cfg->num_pages; i++)
                        {
                            /* Do not check the return status as the input parameters are valid */
                            (void)mtb_pmbus_set_default_zones(inst, MTB_PMBUS_ZONE_NONE, MTB_PMBUS_ZONE_NONE, i);
                        }
                    }
                    else
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
                    {
                        (void)mtb_pmbus_set_default_zones(inst, MTB_PMBUS_ZONE_NONE, MTB_PMBUS_ZONE_NONE, 0U);
                    }

                    /* Set active zones to 0 */
                    inst->act_read_zone = 0U;
                    inst->act_write_zone = 0U;
                }
#endif \
                /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) &&
                   (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) &&
                   (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */

#if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)
                /* Set default value for SMBALERT mode */
                if (inst->cfg->enable_smbalert)
                {
                    inst->smbalert_mode = MTB_PMBUS_SMBALERT_MODE_AUTO;
                }
#endif /* #if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U) */

                inst->received_byte = MTB_PMBUS_RECEIVED_BYTE_DEFAULT;

#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
                defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
                /* Initialize ACL table — 0x00 = all access permitted */
                (void)memset(inst->acl_table, 0, sizeof(inst->acl_table));
#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
                if ((config->ext_acl_table != NULL) && (config->ext_cmd_num > 0U))
                {
                    (void)memset(config->ext_acl_table, 0, config->ext_cmd_num);
                }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */

                /* PASSKEY state — power-on default is Unlocked.
                 * Applications that store a non-zero passkey in NVM must call
                 * mtb_pmbus_passkey_force_locked_isr() before enabling the PMBus HAL IRQ. */
                inst->passkey_state    = MTB_PMBUS_PASSKEY_ST_UNLOCKED;
                inst->passkey_fail_cnt = 0U;

                /* WRITE_PROTECT — power-on default is 0x00 (no protection) */
                inst->write_protect_val = MTB_PMBUS_WP_VAL_NO_PROTECTION;
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */

#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
                defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL == 0U))
                /* Verify all mandatory Security Level 0 commands are registered */
                status = mtb_pmbus_sec_check_l0_cmds(inst, NULL);
#else
                status = MTB_PMBUS_STATUS_SUCCESS;
#endif /* MTB_PMBUS_SUPPORT_SECURITY && MTB_PMBUS_SEC_LEVEL == 0 */
            }
        }
    }

    return status;
}


mtb_pmbus_status_t mtb_pmbus_enable(mtb_pmbus_stc_t *inst)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_SUCCESS;

    CY_ASSERT(NULL != inst);

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
#if defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)
    inst->crc = 0U;
#endif /* #if defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U) */
    inst->active_cmd_tbl = NULL;
    inst->active_lookup_tbl = NULL;
#if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)
    inst->smbalert_is_trig = false;
#endif /* #if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
    inst->cmd_ctrl_byte = 0U;
    inst->zone_sts_mask = 0U;
    inst->zone_pages = 0U;
    inst->num_available_zone = 0U;
    inst->zone_is_read = false;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

    /* Enable Interrupts */
    if (inst->cfg->hw_config->enable_hw_irq_callback != NULL)
    {
        inst->cfg->hw_config->enable_hw_irq_callback();
    }
    else
    {
        status = MTB_PMBUS_STATUS_BAD_PARAM;
        MTB_PMBUS_LOG_ERR("The enable interrupt callback is not provided. See enable_hw_irq_callback");
    }

    /* Enable I2C HW */
    if (inst->cfg->hw_config->hw_resource_ctrl_callback != NULL)
    {
        inst->cfg->hw_config->hw_resource_ctrl_callback(MTB_PMBUS_HW_RESOURCES_ENABLE);
    }
    else
    {
        status = MTB_PMBUS_STATUS_BAD_PARAM;
        MTB_PMBUS_LOG_ERR("The HW resource control callback is not provided. See hw_resource_ctrl_callback");
    }

    return status;
}


mtb_pmbus_status_t mtb_pmbus_disable(mtb_pmbus_stc_t *inst)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_SUCCESS;

    CY_ASSERT(NULL != inst);

    /* Disable Interrupts */
    if (inst->cfg->hw_config->disable_hw_irq_callback != NULL)
    {
        inst->cfg->hw_config->disable_hw_irq_callback();
    }
    else
    {
        status = MTB_PMBUS_STATUS_BAD_PARAM;
        MTB_PMBUS_LOG_ERR("The disable interrupt callback is not provided. See disable_hw_irq_callback");
    }

    /* Disable I2C HW */
    if (inst->cfg->hw_config->hw_resource_ctrl_callback != NULL)
    {
        inst->cfg->hw_config->hw_resource_ctrl_callback(MTB_PMBUS_HW_RESOURCES_DISABLE);
    }
    else
    {
        status = MTB_PMBUS_STATUS_BAD_PARAM;
        MTB_PMBUS_LOG_ERR("The HW resource control callback is not provided. See hw_resource_ctrl_callback");
    }

    return status;
}


void mtb_pmbus_set_received_byte(mtb_pmbus_stc_t *inst, uint8_t byte)
{
    CY_ASSERT(NULL != inst);

    inst->received_byte = byte;
}


#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
uint8_t mtb_pmbus_get_active_page(mtb_pmbus_stc_t *inst)
{
    CY_ASSERT(NULL != inst);

    return inst->act_page;
}
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */

#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
uint8_t mtb_pmbus_get_active_phase(mtb_pmbus_stc_t *inst)
{
    CY_ASSERT(NULL != inst);

    return inst->act_phase;
}
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */

#if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)
void mtb_pmbus_smbalert_config_mode(mtb_pmbus_stc_t *inst, mtb_pmbus_smbalert_mode_t mode)
{
    CY_ASSERT(NULL != inst);

    if (inst->cfg->enable_smbalert)
    {
        inst->smbalert_mode = mode;
    }
}


void mtb_pmbus_smbalert_set_signal(mtb_pmbus_stc_t *inst)
{
    CY_ASSERT(NULL != inst);

    if (inst->cfg->enable_smbalert)
    {
        inst->smbalert_is_trig = true;
        mtb_pmbus_hal_smbalert_set(inst);
    }
}


void mtb_pmbus_smbalert_clear_signal(mtb_pmbus_stc_t *inst)
{
    CY_ASSERT(NULL != inst);

    if (inst->cfg->enable_smbalert)
    {
        inst->smbalert_is_trig = false;
        mtb_pmbus_hal_smbalert_clear(inst);
    }
}
#endif /* #if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U) */

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) && (defined(MTB_PMBUS_SUPPORT_PMBUS) && \
    (MTB_PMBUS_SUPPORT_PMBUS != 0U))
mtb_pmbus_status_t mtb_pmbus_set_default_zones(mtb_pmbus_stc_t *inst, uint32_t zone_read, uint32_t zone_write,
                                               uint32_t page)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
#if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)
    uint32_t temp_page = page;
#else
    uint32_t temp_page = 0U;
    (void)page;
#endif /* #if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U) */

    CY_ASSERT(NULL != inst);

    if (inst->cfg->enable_zone && inst->cfg->enable_pmbus)
    {
#if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)
        /* Always set page parameter to zero if instance does not support pages */
        if (inst->cfg->num_pages == 0U)
        {
            temp_page = 0U;
        }

        if ((temp_page >= inst->cfg->num_pages) && (inst->cfg->num_pages != 0U))
        {
            MTB_PMBUS_LOG_ERR("Page [%" PRIx32 "] is not supported", temp_page);
            status = MTB_PMBUS_STATUS_INVALID_PAGE;
        }
        else
#endif /* #if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U) */
        {
            inst->read_zones[temp_page] = (uint8_t)zone_read;
            inst->write_zones[temp_page] = (uint8_t)zone_write;
            status = MTB_PMBUS_STATUS_SUCCESS;
        }
    }
    else
    {
        MTB_PMBUS_LOG_ERR("Zones are not supported. Enable enable_zone and enable_pmbus in the configuration");
    }

    return status;
}


mtb_pmbus_status_t mtb_pmbus_get_active_zones(mtb_pmbus_stc_t *inst, uint32_t *zone_read, uint32_t *zone_write)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;

    CY_ASSERT(NULL != inst);

    if (inst->cfg->enable_zone && inst->cfg->enable_pmbus)
    {
        if ((zone_read != NULL) && (zone_write != NULL))
        {
            *zone_read = inst->act_read_zone;
            *zone_write = inst->act_write_zone;
            status = MTB_PMBUS_STATUS_SUCCESS;
        }
    }
    else
    {
        MTB_PMBUS_LOG_ERR("Zones are not supported. Enable enable_zone and enable_pmbus in the configuration");
    }

    return status;
}


mtb_pmbus_status_t mtb_pmbus_get_read_zone(mtb_pmbus_stc_t *inst, uint32_t *zone, uint32_t page)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
#if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)
    uint32_t temp_page = page;
#else
    uint32_t temp_page = 0U;
    (void)page;
#endif /* #if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U) */

    CY_ASSERT(NULL != inst);

    if (inst->cfg->enable_zone && inst->cfg->enable_pmbus)
    {
#if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)
        /* Always set page parameter to zero if instance does not support pages */
        if (inst->cfg->num_pages == 0U)
        {
            temp_page = 0U;
        }

        if ((temp_page >= inst->cfg->num_pages) && (inst->cfg->num_pages != 0U))
        {
            MTB_PMBUS_LOG_ERR("Page [%" PRIx32 "] is not supported", temp_page);
            status = MTB_PMBUS_STATUS_INVALID_PAGE;
        }
        else
#endif /* #if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U) */
        {
            if (zone != NULL)
            {
                *zone = inst->read_zones[temp_page];
                status = MTB_PMBUS_STATUS_SUCCESS;
            }
        }
    }
    else
    {
        MTB_PMBUS_LOG_ERR("Zones are not supported. Enable enable_zone and enable_pmbus in the configuration");
    }

    return status;
}


mtb_pmbus_status_t mtb_pmbus_get_write_zone(mtb_pmbus_stc_t *inst, uint32_t *zone, uint32_t page)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
#if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)
    uint32_t temp_page = page;
#else
    uint32_t temp_page = 0U;
    (void)page;
#endif /* #if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U) */

    CY_ASSERT(NULL != inst);

    if (inst->cfg->enable_zone && inst->cfg->enable_pmbus)
    {
#if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)
        /* Always set page parameter to zero if instance does not support pages */
        if (inst->cfg->num_pages == 0U)
        {
            temp_page = 0U;
        }

        if ((temp_page >= inst->cfg->num_pages) && (inst->cfg->num_pages != 0U))
        {
            MTB_PMBUS_LOG_ERR("Page [%" PRIx32 "] is not supported", temp_page);
            status = MTB_PMBUS_STATUS_INVALID_PAGE;
        }
        else
#endif /* #if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U) */
        {
            if (zone != NULL)
            {
                *zone = inst->write_zones[temp_page];
                status = MTB_PMBUS_STATUS_SUCCESS;
            }
        }
    }
    else
    {
        MTB_PMBUS_LOG_ERR("Zones are not supported. Enable enable_zone and enable_pmbus in the configuration");
    }

    return status;
}
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) &&
       (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */

#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
static void mtb_pmbus_int_add_impl_command(mtb_pmbus_stc_t *inst)
{
    uint8_t cmd_position = 0U;
#if (MTB_PMBUS_IMPL_CMD_REVISION == 1U)
    if ((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_REVISION_EN) != 0U)
    {
        inst->pre_impl_cmd_table[cmd_position].cmd_code = MTB_PMBUS_IMPL_CMD_REVISION_CODE;
        inst->pre_impl_cmd_table[cmd_position].cmd_cap  = MTB_PMBUS_IMPL_CMD_REVISION_CAP;
        inst->pre_impl_cmd_table[cmd_position].data_buf = NULL;
        inst->pre_impl_cmd_table[cmd_position].data_size = MTB_PMBUS_IMPL_CMD_REVISION_SIZE;
        inst->pre_impl_cmd_table[cmd_position].callback = NULL;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                            MTB_PMBUS_CMD_FLAG_IS_ENABLED;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].cmd_pos = cmd_position;

        MTB_PMBUS_LOG_DBG("Pre-impl PMBUS_REVISION cmd %x is added", MTB_PMBUS_IMPL_CMD_REVISION_CODE);
        cmd_position++;
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_REVISION == 1U) */

#if (MTB_PMBUS_IMPL_CMD_CAPABILITY == 1U)
    if ((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_CAPABILITY_EN) != 0U)
    {
        inst->pre_impl_cmd_table[cmd_position].cmd_code = MTB_PMBUS_IMPL_CMD_CAPABILITY_CODE;
        inst->pre_impl_cmd_table[cmd_position].cmd_cap  = MTB_PMBUS_IMPL_CMD_CAPABILITY_CAP;
        inst->pre_impl_cmd_table[cmd_position].data_buf = NULL;
        inst->pre_impl_cmd_table[cmd_position].data_size = MTB_PMBUS_IMPL_CMD_CAPABILITY_SIZE;
        inst->pre_impl_cmd_table[cmd_position].callback = NULL;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                            MTB_PMBUS_CMD_FLAG_IS_ENABLED;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].cmd_pos = cmd_position;

        MTB_PMBUS_LOG_DBG("Pre-impl CAPABILITY cmd %x is added", MTB_PMBUS_IMPL_CMD_CAPABILITY_CODE);
        cmd_position++;
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_CAPABILITY == 1U) */

#if (MTB_PMBUS_IMPL_CMD_QUERY == 1U)
    if ((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_QUERY_EN) != 0U)
    {
        inst->pre_impl_cmd_table[cmd_position].cmd_code = MTB_PMBUS_IMPL_CMD_QUERY_CODE;
        inst->pre_impl_cmd_table[cmd_position].cmd_cap  = MTB_PMBUS_IMPL_CMD_QUERY_CAP;
        inst->pre_impl_cmd_table[cmd_position].data_buf = NULL;
        inst->pre_impl_cmd_table[cmd_position].data_size = MTB_PMBUS_IMPL_CMD_QUERY_SIZE;
        inst->pre_impl_cmd_table[cmd_position].callback = NULL;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                            MTB_PMBUS_CMD_FLAG_IS_ENABLED;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].cmd_pos = cmd_position;

        MTB_PMBUS_LOG_DBG("Pre-impl QUERY cmd %x is added", MTB_PMBUS_IMPL_CMD_QUERY_CODE);
        cmd_position++;
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_QUERY == 1U) */

#if (MTB_PMBUS_IMPL_CMD_PAGE == 1U)
    if (((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_PAGE_EN) != 0U) && (inst->cfg->num_pages > 0U))
    {
        inst->pre_impl_cmd_table[cmd_position].cmd_code = MTB_PMBUS_IMPL_CMD_PAGE_CODE;
        inst->pre_impl_cmd_table[cmd_position].cmd_cap  = MTB_PMBUS_IMPL_CMD_PAGE_CAP;
        inst->pre_impl_cmd_table[cmd_position].data_buf = NULL;
        inst->pre_impl_cmd_table[cmd_position].data_size = MTB_PMBUS_IMPL_CMD_PAGE_SIZE;
        inst->pre_impl_cmd_table[cmd_position].callback = NULL;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                            MTB_PMBUS_CMD_FLAG_IS_ENABLED;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].cmd_pos = cmd_position;

        MTB_PMBUS_LOG_DBG("Pre-impl PAGE cmd %x is added", MTB_PMBUS_IMPL_CMD_PAGE_CODE);
        cmd_position++;
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_PAGE == 1U) */

#if (MTB_PMBUS_IMPL_CMD_PHASE == 1U)
    if (((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_PHASE_EN) != 0U) && (inst->cfg->num_phases > 0U))
    {
        inst->pre_impl_cmd_table[cmd_position].cmd_code = MTB_PMBUS_IMPL_CMD_PHASE_CODE;
        inst->pre_impl_cmd_table[cmd_position].cmd_cap  = MTB_PMBUS_IMPL_CMD_PHASE_CAP;
        inst->pre_impl_cmd_table[cmd_position].data_buf = NULL;
        inst->pre_impl_cmd_table[cmd_position].data_size = MTB_PMBUS_IMPL_CMD_PHASE_SIZE;
        inst->pre_impl_cmd_table[cmd_position].callback = NULL;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                            MTB_PMBUS_CMD_FLAG_IS_ENABLED;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].cmd_pos = cmd_position;

        MTB_PMBUS_LOG_DBG("Pre-impl PHASE cmd %x is added", MTB_PMBUS_IMPL_CMD_PHASE_CODE);
        cmd_position++;
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_PHASE == 1U) */

#if (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG == 1U)
    if (((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_EN) != 0U) && (inst->cfg->enable_zone))
    {
        inst->pre_impl_cmd_table[cmd_position].cmd_code = MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_CODE;
        inst->pre_impl_cmd_table[cmd_position].cmd_cap  = MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_CAP;
        inst->pre_impl_cmd_table[cmd_position].data_buf = NULL;
        inst->pre_impl_cmd_table[cmd_position].data_size = MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_SIZE;
        inst->pre_impl_cmd_table[cmd_position].callback = NULL;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                            MTB_PMBUS_CMD_FLAG_IS_ENABLED;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].cmd_pos = cmd_position;

        MTB_PMBUS_LOG_DBG("Pre-impl ZONE_CONFIG cmd %x is added", MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_CODE);
        cmd_position++;
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG == 1U) */

#if (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE == 1U)
    if (((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_EN) != 0U) && (inst->cfg->enable_zone))
    {
        inst->pre_impl_cmd_table[cmd_position].cmd_code = MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_CODE;
        inst->pre_impl_cmd_table[cmd_position].cmd_cap  = MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_CAP;
        inst->pre_impl_cmd_table[cmd_position].data_buf = NULL;
        inst->pre_impl_cmd_table[cmd_position].data_size = MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_SIZE;
        inst->pre_impl_cmd_table[cmd_position].callback = NULL;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                            MTB_PMBUS_CMD_FLAG_IS_ENABLED;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].cmd_pos = cmd_position;

        MTB_PMBUS_LOG_DBG("Pre-impl ZONE_ACTIVE cmd %x is added", MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_CODE);
        cmd_position++;
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE == 1U) */

#if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE == 1U)
    if (((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_EN) != 0U) && (inst->cfg->num_pages > 0U))
    {
        inst->pre_impl_cmd_table[cmd_position].cmd_code = MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_CODE;
        inst->pre_impl_cmd_table[cmd_position].cmd_cap  = MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_CAP;
        inst->pre_impl_cmd_table[cmd_position].data_buf = NULL;
        inst->pre_impl_cmd_table[cmd_position].data_size = MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_SIZE;
        inst->pre_impl_cmd_table[cmd_position].callback = NULL;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                            MTB_PMBUS_CMD_FLAG_IS_ENABLED;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].cmd_pos = cmd_position;

        MTB_PMBUS_LOG_DBG("Pre-impl PAGE_PLUS_WRITE cmd %x is added", MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_CODE);
        cmd_position++;
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE == 1U) */

#if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ == 1U)
    if (((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_EN) != 0U) && (inst->cfg->num_pages > 0U))
    {
        inst->pre_impl_cmd_table[cmd_position].cmd_code = MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_CODE;
        inst->pre_impl_cmd_table[cmd_position].cmd_cap  = MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_CAP;
        inst->pre_impl_cmd_table[cmd_position].data_buf = NULL;
        inst->pre_impl_cmd_table[cmd_position].data_size = MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_SIZE;
        inst->pre_impl_cmd_table[cmd_position].callback = NULL;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                            MTB_PMBUS_CMD_FLAG_IS_ENABLED;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].cmd_pos = cmd_position;

        MTB_PMBUS_LOG_DBG("Pre-impl PAGE_PLUS_READ cmd %x is added", MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_CODE);
        cmd_position++;
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ == 1U) */

#if (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE == 1U)
    if (((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_EN) != 0U) && (inst->cfg->num_pages > 0U) &&
        (inst->cfg->num_phases > 0U))
    {
        inst->pre_impl_cmd_table[cmd_position].cmd_code = MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_CODE;
        inst->pre_impl_cmd_table[cmd_position].cmd_cap  = MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_CAP;
        inst->pre_impl_cmd_table[cmd_position].data_buf = NULL;
        inst->pre_impl_cmd_table[cmd_position].data_size = MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_SIZE;
        inst->pre_impl_cmd_table[cmd_position].callback = NULL;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                            MTB_PMBUS_CMD_FLAG_IS_ENABLED;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].cmd_pos = cmd_position;

        MTB_PMBUS_LOG_DBG("Pre-impl P2_PLUS_WRITE cmd %x is added", MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_CODE);
        cmd_position++;
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE == 1U) */

#if (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ == 1U)
    if (((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_EN) != 0U) && (inst->cfg->num_pages > 0U) &&
        (inst->cfg->num_phases > 0U))
    {
        inst->pre_impl_cmd_table[cmd_position].cmd_code = MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_CODE;
        inst->pre_impl_cmd_table[cmd_position].cmd_cap  = MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_CAP;
        inst->pre_impl_cmd_table[cmd_position].data_buf = NULL;
        inst->pre_impl_cmd_table[cmd_position].data_size = MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_SIZE;
        inst->pre_impl_cmd_table[cmd_position].callback = NULL;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].flags = MTB_PMBUS_CMD_FLAG_IS_PRESENT |
                                                            MTB_PMBUS_CMD_FLAG_IS_ENABLED;
        inst->pre_impl_cmd_lookup_tbl[cmd_position].cmd_pos = cmd_position;

        MTB_PMBUS_LOG_DBG("Pre-impl P2_PLUS_READ cmd %x is added", MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_CODE);
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ == 1U) */
}
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */

#if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U)
/* Function checks the validity of hardware configuration parameters */
static mtb_pmbus_status_t mtb_pmbus_int_check_hw_cfg(mtb_pmbus_stc_t *inst)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
    if (mtb_pmbus_hal_check_hw_cfg(inst->cfg))
    {
        status = MTB_PMBUS_STATUS_SUCCESS;
    }

    return status;
}


static mtb_pmbus_status_t mtb_pmbus_int_check_cmd_table(mtb_pmbus_stc_t *inst)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_SUCCESS;

    if ((inst->cfg->cmd_num > 0U) && (inst->cfg->cmd_table == NULL))
    {
        status = MTB_PMBUS_STATUS_BAD_PARAM;
        MTB_PMBUS_LOG_ERR("Command table is NULL, but cmd_num is greater than 0");
    }
    else if (inst->cfg->cmd_table != NULL)
    {
#if defined(MTB_PMBUS_CONFIG_FULL_CHECK) && (MTB_PMBUS_CONFIG_FULL_CHECK == 1U)
        if (inst->cfg->cmd_num == 0U)
        {
            status = MTB_PMBUS_STATUS_BAD_PARAM;
            MTB_PMBUS_LOG_ERR("cmd_table is provided, but cmd_num is 0");
        }
#endif /* #if defined(MTB_PMBUS_CONFIG_FULL_CHECK) && (MTB_PMBUS_CONFIG_FULL_CHECK == 1U) */
        for (uint32_t i = 0U; i < inst->cfg->cmd_num; i++)
        {
            bool terminate_loop = false;

            if ((inst->cfg->cmd_table[i].data_size > 0U) && (inst->cfg->cmd_table[i].data_buf == NULL))
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("Command %x has non-zero data_size but data_buf is NULL",
                                  inst->cfg->cmd_table[i].cmd_code);
                terminate_loop = true;
            }

#if defined(MTB_PMBUS_CONFIG_FULL_CHECK) && (MTB_PMBUS_CONFIG_FULL_CHECK == 1U)
            if (((inst->cfg->cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_WR) != 0U) &&
                ((inst->cfg->cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_RD) != 0U) &&
                ((inst->cfg->cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) != 0U))
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("Command %x has forbidden combination of DIR capabilities",
                                  inst->cfg->cmd_table[i].cmd_code);
                terminate_loop = true;
            }
#if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)
            if (((inst->cfg->cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) != 0U) &&
                (inst->cfg->num_pages == 0U))
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("Command %x has PAGE_CMD capability, but instance does not support pages",
                                  inst->cfg->cmd_table[i].cmd_code);
                terminate_loop = true;
            }
#endif /* #if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U) */
#if defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)
            if (((inst->cfg->cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) != 0U) &&
                (inst->cfg->num_phases == 0U))
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("Command %x has PHASE_CMD capability, but instance does not support phases",
                                  inst->cfg->cmd_table[i].cmd_code);
                terminate_loop = true;
            }
#endif /* #if defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U) */
#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
            if (inst->cfg->enable_ext_cmd && (MTB_PMBUS_CMD_CODE_EXT == inst->cfg->cmd_table[i].cmd_code))
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("enable_ext_cmd is enabled, but there is 0xFE command in cmd_table");
                terminate_loop = true;
            }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
#endif /* #if defined(MTB_PMBUS_CONFIG_FULL_CHECK) && (MTB_PMBUS_CONFIG_FULL_CHECK == 1U) */

            if (terminate_loop)
            {
                break;
            }
        }
    }
    else
    {
        /* Command table is not provided */
    }

    return status;
}


#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
static mtb_pmbus_status_t mtb_pmbus_int_check_ext_cmd_table(mtb_pmbus_stc_t *inst)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_SUCCESS;

    if (inst->cfg->enable_ext_cmd)
    {
        if (inst->cfg->ext_cmd_num == 0U)
        {
            status = MTB_PMBUS_STATUS_BAD_PARAM;
            MTB_PMBUS_LOG_ERR("Ext commands are enabled, but ext_cmd_num is 0");
        }

        if (inst->cfg->ext_cmd_table == NULL)
        {
            status = MTB_PMBUS_STATUS_BAD_PARAM;
            MTB_PMBUS_LOG_ERR("Ext commands are enabled, but ext_cmd_table is NULL");
        }
        else
        {
            for (uint32_t i = 0U; i < inst->cfg->ext_cmd_num; i++)
            {
                bool terminate_loop = false;

                if ((inst->cfg->ext_cmd_table[i].data_size > 0U) && (inst->cfg->ext_cmd_table[i].data_buf == NULL))
                {
                    status = MTB_PMBUS_STATUS_BAD_PARAM;
                    MTB_PMBUS_LOG_ERR("Ext command %x has non-zero data_size but data_buf is NULL",
                                      inst->cfg->ext_cmd_table[i].cmd_code);
                    terminate_loop = true;
                }

#if defined(MTB_PMBUS_CONFIG_FULL_CHECK) && (MTB_PMBUS_CONFIG_FULL_CHECK == 1U)
                if (((inst->cfg->ext_cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_WR) != 0U) &&
                    ((inst->cfg->ext_cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_RD) != 0U) &&
                    ((inst->cfg->ext_cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL) != 0U))
                {
                    status = MTB_PMBUS_STATUS_BAD_PARAM;
                    MTB_PMBUS_LOG_ERR("Ext command %x has forbidden combination of DIR capabilities",
                                      inst->cfg->ext_cmd_table[i].cmd_code);
                    terminate_loop = true;
                }
#if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)
                if (((inst->cfg->ext_cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_PAGE) != 0U) &&
                    (inst->cfg->num_pages == 0U))
                {
                    status = MTB_PMBUS_STATUS_BAD_PARAM;
                    MTB_PMBUS_LOG_ERR("Ext command %x has PAGE_CMD capability, but instance does not support pages",
                                      inst->cfg->ext_cmd_table[i].cmd_code);
                    terminate_loop = true;
                }
#endif /* #if defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U) */
#if defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)
                if (((inst->cfg->ext_cmd_table[i].cmd_cap & MTB_PMBUS_CMD_CAP_PHASE) != 0U) &&
                    (inst->cfg->num_phases == 0U))
                {
                    status = MTB_PMBUS_STATUS_BAD_PARAM;
                    MTB_PMBUS_LOG_ERR("Ext command %x has PHASE_CMD capability, but instance does not support phases",
                                      inst->cfg->ext_cmd_table[i].cmd_code);
                    terminate_loop = true;
                }
#endif /* #if defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U) */
#endif /* #if defined(MTB_PMBUS_CONFIG_FULL_CHECK) && (MTB_PMBUS_CONFIG_FULL_CHECK == 1U) */

                if (terminate_loop)
                {
                    break;
                }
            }
        }
    }
    else
    {
#if defined(MTB_PMBUS_CONFIG_FULL_CHECK) && (MTB_PMBUS_CONFIG_FULL_CHECK == 1U)
        if (inst->cfg->ext_cmd_num != 0U)
        {
            status = MTB_PMBUS_STATUS_BAD_PARAM;
            MTB_PMBUS_LOG_ERR("Ext commands are disabled, but ext_cmd_num is not 0");
        }

        if (inst->cfg->ext_cmd_table != NULL)
        {
            status = MTB_PMBUS_STATUS_BAD_PARAM;
            MTB_PMBUS_LOG_ERR("Ext commands are disabled, but ext_cmd_table is not NULL");
        }
#endif /* #if defined(MTB_PMBUS_CONFIG_FULL_CHECK) && (MTB_PMBUS_CONFIG_FULL_CHECK == 1U) */
    }

    return status;
}
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */

/* Function checks the validity of configuration parameters */
static mtb_pmbus_status_t mtb_pmbus_int_check_cfg(mtb_pmbus_stc_t *inst)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_SUCCESS;

    status = mtb_pmbus_int_check_hw_cfg(inst);

#if defined(MTB_PMBUS_CONFIG_FULL_CHECK) && (MTB_PMBUS_CONFIG_FULL_CHECK == 1U)
    if (status == MTB_PMBUS_STATUS_SUCCESS)
    {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
        if (inst->cfg->num_pages > MTB_PMBUS_PAGES_NUM)
        {
            status = MTB_PMBUS_STATUS_BAD_PARAM;
            MTB_PMBUS_LOG_ERR("Total number of pages %x is more than MTB_PMBUS_PAGES_NUM %x", inst->cfg->num_pages,
                              MTB_PMBUS_PAGES_NUM);
        }
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */

#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
        if (inst->cfg->num_phases > MTB_PMBUS_PHASES_NUM)
        {
            status = MTB_PMBUS_STATUS_BAD_PARAM;
            MTB_PMBUS_LOG_ERR("Total number of phases %x is more than MTB_PMBUS_PHASES_NUM %x", inst->cfg->num_phases,
                              MTB_PMBUS_PHASES_NUM);
        }
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
        if (inst->cfg->enable_zone && (inst->cfg->zone_callback == NULL))
        {
            status = MTB_PMBUS_STATUS_BAD_PARAM;
            MTB_PMBUS_LOG_ERR("Zone protocol is enabled, but zone_callback is not set");
        }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
#if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U))
        if (inst->cfg->enable_pmbus)
        {
#if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U)
            if ((inst->cfg->num_pages != 0U) &&
                ((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_PAGE_EN) == 0U))
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("num_pages is not zero, but implemented PAGE command is disabled in impl_cmd_mask.");
            }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U)
            if ((inst->cfg->num_phases != 0U) &&
                ((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_PHASE_EN) == 0U))
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("num_phases is not zero, but implemented PHASE command is disabled in impl_cmd_mask.");
            }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U) */

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
            if (inst->cfg->enable_zone)
            {
#if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U)
                if ((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_EN) == 0U)
                {
                    status = MTB_PMBUS_STATUS_BAD_PARAM;
                    MTB_PMBUS_LOG_ERR(
                        "enable_zone is set, but implemented ZONE CONFIG command is disabled in impl_cmd_mask.");
                }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U) */
#if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U)
                if ((inst->cfg->impl_cmd_mask & MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_EN) == 0U)
                {
                    status = MTB_PMBUS_STATUS_BAD_PARAM;
                    MTB_PMBUS_LOG_ERR(
                        "enable_zone is set, but implemented ZONE ACTIVE command is disabled in impl_cmd_mask.");
                }
#endif /* #if defined(MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U) */
            }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
        }
        else
        {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
            if (inst->cfg->num_pages > 0U)
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("num_pages is not zero, but PMBus is disabled");
            }
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
            if (inst->cfg->num_phases > 0U)
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("num_phases is not zero, but PMBus is disabled");
            }
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
            if (inst->cfg->enable_zone)
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("enable_zone is set, but PMBus is disabled");
            }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
#if defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)
            if (inst->cfg->enable_ext_cmd)
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("enable_ext_cmd is set, but PMBus is disabled");
            }
#endif /* defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U) */
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
            if (inst->cfg->impl_cmd_mask != 0U)
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
                MTB_PMBUS_LOG_ERR("impl_cmd_mask is set, but PMBus is disabled");
            }
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
        }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */
    }
#endif /* #if defined(MTB_PMBUS_CONFIG_FULL_CHECK) && (MTB_PMBUS_CONFIG_FULL_CHECK == 1U) */

    if (status == MTB_PMBUS_STATUS_SUCCESS)
    {
        status = mtb_pmbus_int_check_cmd_table(inst);
    }

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
    if (status == MTB_PMBUS_STATUS_SUCCESS)
    {
        status = mtb_pmbus_int_check_ext_cmd_table(inst);
    }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */

    return status;
}
#endif /* #if defined(MTB_PMBUS_ENABLE_CONFIG_CHECK) && (MTB_PMBUS_ENABLE_CONFIG_CHECK != 0U) */

#if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U))
mtb_pmbus_status_t mtb_pmbus_send_host_notify(mtb_pmbus_stc_t *inst, uint32_t data)
{
    MTB_PMBUS_LOG_DBG("Start Host Notify");
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_IS_BUSY;
    uint32_t hn_data = data;

    if (mtb_pmbus_is_busy(inst))
    {
        MTB_PMBUS_LOG_ERR("Host Notify failed: Instance is busy");
    }
    else
    {
        if (mtb_pmbus_hal_hnp_write(inst, inst->cfg->address, hn_data))
        {
            status = MTB_PMBUS_STATUS_SUCCESS;
            inst->state |= MTB_PMBUS_STATE_HOST_NOTIFY;
        }
    }
    return status;
}
#endif /* #if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) */

bool mtb_pmbus_is_busy(mtb_pmbus_stc_t *inst)
{
    return (inst->state != 0U);
}
