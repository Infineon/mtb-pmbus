/***************************************************************************//**
* \file mtb_pmbus_trgt_cmd.c
* \version 1.0
*
* Provides command APIs implementation for the PMBus Target Middleware.
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
#include "string.h"

#define CMD_DATA_PTR(code)                      ((uint8_t *)(cmd_table_ptr[cmd_lookup_tbl[code].cmd_pos].data_buf))
#define CMD_DATA_SIZE(code)                     (cmd_table_ptr[cmd_lookup_tbl[code].cmd_pos].data_size)
#define CMD_CAP(code)                           (cmd_table_ptr[cmd_lookup_tbl[code].cmd_pos].cmd_cap)

#define CMD_IS_BLOCK(code)                      ((CMD_CAP((code)) & MTB_PMBUS_CMD_CAP_BLOCK) != 0U)
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
#define CMD_IS_PAGED(code)                      ((CMD_CAP((code)) & MTB_PMBUS_CMD_CAP_PAGE) != 0U)
#else
#define CMD_IS_PAGED(code)                      (0U != 0U)
#endif
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
#define CMD_IS_PHASED(code)                     ((CMD_CAP((code)) & MTB_PMBUS_CMD_CAP_PHASE) != 0U)
#else
#define CMD_IS_PHASED(code)                     (0U != 0U)
#endif

#define CMD_BUFF_SIZE(code)                     \
    (CMD_IS_BLOCK((code)) ? ((uint32_t)(CMD_DATA_SIZE((code))) + MTB_PMBUS_CMD_BLOCK_EXTRA_BYTE) :                                             \
                                                 (uint32_t)(CMD_DATA_SIZE((code))))

#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
#define CMD_PHASES_IN_PAGE(code)                (((!CMD_IS_PHASED(code)) ? 1U : inst->cfg->num_phases))
#else
#define CMD_PHASES_IN_PAGE(code)                (1U)
#endif
#define CMD_PAGE_SHIFT(page)                    (((page) >= 0) ? ((uint32_t)page) : 0U)
#define CMD_PHASE_SHIFT(phase)                  (((phase) >= 0) ? ((uint32_t)phase) : 0U)

#define CMD_DATA_START_POS(code, page, phase)   (CMD_DATA_PTR((code)) + \
                                                ((CMD_PAGE_SHIFT((page)) * CMD_PHASES_IN_PAGE((code)) * CMD_BUFF_SIZE((code))) + \
                                                (CMD_PHASE_SHIFT((phase)) * CMD_BUFF_SIZE((code)))))


__STATIC_INLINE void mtb_pmbus_int_get_table(mtb_pmbus_stc_t *inst, uint32_t *code,
                                             mtb_pmbus_stc_config_cmd_t **cmd_table_ptr,
                                             mtb_pmbus_cmd_lookup_tbl_t **cmd_lookup_tbl);
static mtb_pmbus_status_t mtb_pmbus_int_cmd_arg_is_valid(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page,
                                                         int32_t phase, uint32_t size, bool skip_pp);

/* Get the tables based on command code */
__STATIC_INLINE void mtb_pmbus_int_get_table(mtb_pmbus_stc_t *inst, uint32_t *code,
                                             mtb_pmbus_stc_config_cmd_t **cmd_table_ptr,
                                             mtb_pmbus_cmd_lookup_tbl_t **cmd_lookup_tbl)
{
#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
    if (MTB_PMBUS_CMD_CODE_EXT == (MTB_PMBUS_CMD_EXT_MASK & *code))
    {
        *cmd_table_ptr = inst->cfg->ext_cmd_table;
        *cmd_lookup_tbl = inst->ext_cmd_lookup_tbl;
        *code = *code >> MTB_PMBUS_CMD_EXT_POS;
    }
    else
#else
    /* To avoid compiler warning */
    (void)code;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
    {
        *cmd_table_ptr = inst->cfg->cmd_table;
        *cmd_lookup_tbl = inst->cmd_lookup_tbl;
    }
}


/* Check if arguments are valid */
static mtb_pmbus_status_t mtb_pmbus_int_cmd_arg_is_valid(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page,
                                                         int32_t phase, uint32_t size, bool skip_pp)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_SUCCESS;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    CY_ASSERT(NULL != cmd_table_ptr);
    CY_ASSERT(NULL != cmd_lookup_tbl);

    /* Check if command is present */
    if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_PRESENT) == 0U)
    {
        MTB_PMBUS_LOG_WRN("The command with code %" PRIx32 " is not supported", code_tmp);
        status = MTB_PMBUS_STATUS_INVALID_CMD_CODE;
    }
    else
    {
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
        if ((((!CMD_IS_PAGED(code_tmp)) && (page >= 0)) || (page > ((int32_t)inst->cfg->num_pages - 1)) ||
             (CMD_IS_PAGED(code_tmp) && (page == MTB_PMBUS_NO_PAGE_PHASE))) && !skip_pp)
        {
            MTB_PMBUS_LOG_WRN("The page %" PRIu32 " is not valid for command %" PRIx32,
                              page, code_tmp);
            status = MTB_PMBUS_STATUS_INVALID_PAGE;
        }
        else
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
        {
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
            if ((((!CMD_IS_PHASED(code_tmp)) && (phase >= 0)) || (phase > ((int32_t)inst->cfg->num_phases - 1)) ||
                 (CMD_IS_PHASED(code_tmp) && (phase == MTB_PMBUS_NO_PAGE_PHASE))) && !skip_pp)
            {
                MTB_PMBUS_LOG_WRN("The phase %llu is not valid for command %llx",
                                  (int64_t)phase, (int64_t)code_tmp);
                MTB_PMBUS_LOG_WRN("The phase %" PRIu32 " is not valid for command %" PRIx32,
                                  phase, code_tmp);
                status = MTB_PMBUS_STATUS_INVALID_PHASE;
            }
            else
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
            {
                if (size > CMD_DATA_SIZE(code_tmp))
                {
                    MTB_PMBUS_LOG_WRN("The requested data size %" PRIu32 " is too big for command %" PRIx32,
                                      size, code_tmp);
                    status = MTB_PMBUS_STATUS_TOO_MANY_BYTES;
                }
            }
        }
    }
/* To avoid compiler warning */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM == 0U))
    (void)page;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM == 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM == 0U))
    (void)phase;
    (void)skip_pp;
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM == 0U)) */
    return status;
}


void mtb_pmbus_cmd_update_data_ext_isr(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page,
                                       int32_t phase, uint8_t *data, uint32_t data_size)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    /* Find the position of first element in data array */
    uint8_t *data_buff_start = CMD_DATA_START_POS(code_tmp, page, phase);
    if (CMD_IS_BLOCK(code_tmp))
    {
        /* Store the updated size in the first variable */
        *data_buff_start = (uint8_t)data_size;
        (void)memcpy(data_buff_start + MTB_PMBUS_CMD_BLOCK_EXTRA_BYTE, data, data_size);
    }
    else
    {
        (void)memcpy(data_buff_start, data, data_size);
    }
}


mtb_pmbus_status_t mtb_pmbus_cmd_update_data_ext(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page,
                                                 int32_t phase, uint8_t *data, uint32_t data_size)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;
    uint32_t code_tmp = code;

    CY_ASSERT(NULL != inst);

    /* Disable I2C interrupt */
    inst->cfg->hw_config->disable_hw_irq_callback();

    if (NULL != data)
    {
        /* Check if requested data is valid */
        status = mtb_pmbus_int_cmd_arg_is_valid(inst, code, page, phase, data_size, false);
        if (status == MTB_PMBUS_STATUS_SUCCESS)
        {
            mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

            /* Check if current command is transferring at this moment of time */
            if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) == 0U)
            {
                mtb_pmbus_cmd_update_data_ext_isr(inst, code, page, phase, data, data_size);
            }
            else
            {
                status = MTB_PMBUS_STATUS_CMD_IS_ACTIVE;
            }
        }
    }

    inst->cfg->hw_config->enable_hw_irq_callback();

    return status;
}


void mtb_pmbus_cmd_update_data_isr(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *data, uint32_t data_size)
{
    mtb_pmbus_cmd_update_data_ext_isr(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, data, data_size);
}


mtb_pmbus_status_t mtb_pmbus_cmd_update_data(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *data,
                                             uint32_t data_size)
{
    return mtb_pmbus_cmd_update_data_ext(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, data, data_size);
}


void mtb_pmbus_cmd_read_data_ext_isr(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page,
                                     int32_t phase, uint8_t *data, uint32_t data_size)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    /* Find the position of first element in data array */
    uint8_t *data_buff_start =  CMD_DATA_START_POS(code_tmp, page, phase);

    if (CMD_IS_BLOCK(code_tmp))
    {
        (void)memcpy(data, data_buff_start + MTB_PMBUS_CMD_BLOCK_EXTRA_BYTE, data_size);
    }
    else
    {
        (void)memcpy(data, data_buff_start, data_size);
    }
}


mtb_pmbus_status_t mtb_pmbus_cmd_read_data_ext(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page,
                                               int32_t phase, uint8_t *data, uint32_t data_size)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    /* Disable I2C interrupt */
    inst->cfg->hw_config->disable_hw_irq_callback();

    if (NULL != data)
    {
        /* Check if requested data is valid */
        status = mtb_pmbus_int_cmd_arg_is_valid(inst, code, page, phase, data_size, false);
        if (status == MTB_PMBUS_STATUS_SUCCESS)
        {
            mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

            /* Check if current command is transferring at this moment of time */
            if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) == 0U)
            {
                mtb_pmbus_cmd_read_data_ext_isr(inst, code, page, phase, data, data_size);
            }
            else
            {
                status = MTB_PMBUS_STATUS_CMD_IS_ACTIVE;
            }
        }
    }

    inst->cfg->hw_config->enable_hw_irq_callback();

    return status;
}


void mtb_pmbus_cmd_read_data_isr(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *data, uint32_t data_size)
{
    mtb_pmbus_cmd_read_data_ext_isr(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, data, data_size);
}


mtb_pmbus_status_t mtb_pmbus_cmd_read_data(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *data,
                                           uint32_t data_size)
{
    return mtb_pmbus_cmd_read_data_ext(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, data, data_size);
}


void mtb_pmbus_cmd_get_transfer_size_ext_isr(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page,
                                             int32_t phase, uint8_t *size)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    if (CMD_IS_BLOCK(code_tmp))
    {
        *size = *(CMD_DATA_START_POS(code_tmp, page, phase));
    }
    else
    {
        *size = CMD_DATA_SIZE(code_tmp);
    }
}


mtb_pmbus_status_t mtb_pmbus_cmd_get_transfer_size_ext(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page,
                                                       int32_t phase, uint8_t *size)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    /* Disable I2C interrupt */
    inst->cfg->hw_config->disable_hw_irq_callback();

    if (NULL != size)
    {
        /* Check if requested data is valid */
        status = mtb_pmbus_int_cmd_arg_is_valid(inst, code, page, phase, 0U, false);
        if (status == MTB_PMBUS_STATUS_SUCCESS)
        {
            mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

            /* Check if current command is transferring at this moment of time */
            if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) == 0U)
            {
                mtb_pmbus_cmd_get_transfer_size_ext_isr(inst, code, page, phase, size);
            }
            else
            {
                status = MTB_PMBUS_STATUS_CMD_IS_ACTIVE;
            }
        }
    }

    inst->cfg->hw_config->enable_hw_irq_callback();

    return status;
}


void mtb_pmbus_cmd_get_transfer_size_isr(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *size)
{
    mtb_pmbus_cmd_get_transfer_size_ext_isr(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, size);
}


mtb_pmbus_status_t mtb_pmbus_cmd_get_transfer_size(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *size)
{
    return mtb_pmbus_cmd_get_transfer_size_ext(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, size);
}


void mtb_pmbus_cmd_is_active_isr(mtb_pmbus_stc_t *inst, uint32_t code, bool *status)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    *status = ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) != 0U) ? true : false;
}


mtb_pmbus_status_t mtb_pmbus_cmd_is_active(mtb_pmbus_stc_t *inst, uint32_t code, bool *status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    /* Check if requested data is valid */
    ret_status = mtb_pmbus_int_cmd_arg_is_valid(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, 0U, true);
    if (ret_status == MTB_PMBUS_STATUS_SUCCESS)
    {
        mtb_pmbus_cmd_is_active_isr(inst, code, status);
    }

    return ret_status;
}


mtb_pmbus_status_t mtb_pmbus_cmd_get_status(mtb_pmbus_stc_t *inst, uint32_t code, bool *status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    /* Check if requested data is valid */
    ret_status = mtb_pmbus_int_cmd_arg_is_valid(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, 0U, true);
    if (ret_status == MTB_PMBUS_STATUS_SUCCESS)
    {
        mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

        CY_ASSERT(NULL != cmd_table_ptr);
        CY_ASSERT(NULL != cmd_lookup_tbl);

        /* Disable I2C interrupt to prevent status corruption */
        inst->cfg->hw_config->disable_hw_irq_callback();

        /* Check if current command is transferring at this moment of time */
        if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) == 0U)
        {
            *status = (cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_TRANSMITTED) != 0U;
            cmd_lookup_tbl[code_tmp].flags &= (uint8_t)(~MTB_PMBUS_CMD_FLAG_IS_TRANSMITTED);
        }
        else
        {
            ret_status = MTB_PMBUS_STATUS_CMD_IS_ACTIVE;
        }

        inst->cfg->hw_config->enable_hw_irq_callback();
    }

    return ret_status;
}


void mtb_pmbus_cmd_enable_disable_isr(mtb_pmbus_stc_t *inst, uint32_t code, bool status)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    if (status)
    {
        cmd_lookup_tbl[code_tmp].flags |= MTB_PMBUS_CMD_FLAG_IS_ENABLED;
    }
    else
    {
        cmd_lookup_tbl[code_tmp].flags &= (uint8_t)(~MTB_PMBUS_CMD_FLAG_IS_ENABLED);
    }
}


mtb_pmbus_status_t mtb_pmbus_cmd_enable_disable(mtb_pmbus_stc_t *inst, uint32_t code, bool status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    /* Check if requested data is valid */
    ret_status = mtb_pmbus_int_cmd_arg_is_valid(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, 0U, true);
    if (ret_status == MTB_PMBUS_STATUS_SUCCESS)
    {
        mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

        CY_ASSERT(NULL != cmd_table_ptr);
        CY_ASSERT(NULL != cmd_lookup_tbl);

        inst->cfg->hw_config->disable_hw_irq_callback();

        /* Check if current command is transferring at this moment of time */
        if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) == 0U)
        {
            mtb_pmbus_cmd_enable_disable_isr(inst, code, status);
        }
        else
        {
            ret_status = MTB_PMBUS_STATUS_CMD_IS_ACTIVE;
        }

        inst->cfg->hw_config->enable_hw_irq_callback();
    }

    return ret_status;
}


#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) && (defined(MTB_PMBUS_SUPPORT_PMBUS) && \
    (MTB_PMBUS_SUPPORT_PMBUS != 0U))
mtb_pmbus_status_t mtb_pmbus_cmd_ext_enable_disable(mtb_pmbus_stc_t *inst, bool status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_SUCCESS;

    CY_ASSERT(NULL != inst);

    inst->cfg->hw_config->disable_hw_irq_callback();

    if (!mtb_pmbus_is_busy(inst))
    {
        if (inst->cfg->enable_ext_cmd && inst->cfg->enable_pmbus)
        {
            inst->ext_cmd_en = status;
        }
    }
    else
    {
        ret_status = MTB_PMBUS_STATUS_IS_BUSY;
    }

    inst->cfg->hw_config->enable_hw_irq_callback();

    return ret_status;
}
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) &&
       (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */

mtb_pmbus_status_t mtb_pmbus_cmd_all_enable_disable(mtb_pmbus_stc_t *inst, bool status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_SUCCESS;

    CY_ASSERT(NULL != inst);

    inst->cfg->hw_config->disable_hw_irq_callback();

    if (!mtb_pmbus_is_busy(inst))
    {
        /* Enable/Disable all commands from main command table */
        for (uint16_t i = 0U; i < inst->cfg->cmd_num; i++)
        {
            /* Ignore the return status as the input parameters are always valid */
            (void)mtb_pmbus_cmd_enable_disable(inst, inst->cfg->cmd_table[i].cmd_code, status);
        }

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
        if (inst->cfg->enable_ext_cmd)
        {
            /* Enable/Disable the extended command */
            (void)mtb_pmbus_cmd_ext_enable_disable(inst, status);

            /* Enable/Disable all commands from extended table */
            for (uint16_t i = 0U; i < inst->cfg->ext_cmd_num; i++)
            {
                /* Ignore the return status as the input parameters are always valid */
                (void)mtb_pmbus_cmd_enable_disable(inst,
                                                   ((uint32_t)inst->cfg->ext_cmd_table[i].cmd_code <<
                                                    MTB_PMBUS_CMD_EXT_POS) | MTB_PMBUS_CMD_CODE_EXT, status);
            }
        }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
    }
    else
    {
        ret_status = MTB_PMBUS_STATUS_IS_BUSY;
    }

    inst->cfg->hw_config->enable_hw_irq_callback();

    return ret_status;
}


void mtb_pmbus_cmd_wr_protect_isr(mtb_pmbus_stc_t *inst, uint32_t code, bool status)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    if (status)
    {
        cmd_lookup_tbl[code_tmp].flags |= MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED;
    }
    else
    {
        cmd_lookup_tbl[code_tmp].flags &= (uint8_t)(~MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED);
    }

#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
    defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
    for (uint8_t i = 0U; i < MTB_PMBUS_IMPL_CMD_NUM; i++)
    {
        if ((inst->pre_impl_cmd_lookup_tbl[i].flags & MTB_PMBUS_CMD_FLAG_IS_PRESENT) != 0U)
        {
            if (inst->pre_impl_cmd_table[i].cmd_code == (uint8_t)code_tmp)
            {
                if (status)
                {
                    inst->pre_impl_cmd_lookup_tbl[i].flags |= MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED;
                }
                else
                {
                    inst->pre_impl_cmd_lookup_tbl[i].flags &=
                        (uint8_t)(~MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED);
                }
                break;
            }
        }
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */
}


mtb_pmbus_status_t mtb_pmbus_cmd_wr_protect(mtb_pmbus_stc_t *inst, uint32_t code, bool status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    /* Check if requested data is valid */
    ret_status = mtb_pmbus_int_cmd_arg_is_valid(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, 0U, true);
    if (ret_status == MTB_PMBUS_STATUS_SUCCESS)
    {
        mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

        CY_ASSERT(NULL != cmd_table_ptr);
        CY_ASSERT(NULL != cmd_lookup_tbl);

        inst->cfg->hw_config->disable_hw_irq_callback();

        if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) == 0U)
        {
            mtb_pmbus_cmd_wr_protect_isr(inst, code, status);
        }
        else
        {
            ret_status = MTB_PMBUS_STATUS_CMD_IS_ACTIVE;
        }

        inst->cfg->hw_config->enable_hw_irq_callback();
    }

    return ret_status;
}


mtb_pmbus_status_t mtb_pmbus_cmd_all_wr_protect(mtb_pmbus_stc_t *inst, bool status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_SUCCESS;

    CY_ASSERT(NULL != inst);

    inst->cfg->hw_config->disable_hw_irq_callback();

    if (!mtb_pmbus_is_busy(inst))
    {
        /* Provide or not provide the protection for all commands from main command table */
        for (uint16_t i = 0U; i < inst->cfg->cmd_num; i++)
        {
            /* Ignore the return status as the input parameters are always valid */
            (void)mtb_pmbus_cmd_wr_protect(inst, inst->cfg->cmd_table[i].cmd_code, status);
        }

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
        if (inst->cfg->enable_ext_cmd)
        {
            /* Provide or not provide the protection for all commands from extended table */
            for (uint16_t i = 0U; i < inst->cfg->ext_cmd_num; i++)
            {
                /* Ignore the return status as the input parameters are always valid */
                (void)mtb_pmbus_cmd_wr_protect(inst,
                                               ((uint32_t)inst->cfg->ext_cmd_table[i].cmd_code << MTB_PMBUS_CMD_EXT_POS) | MTB_PMBUS_CMD_CODE_EXT,
                                               status);
            }
        }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
    }
    else
    {
        ret_status = MTB_PMBUS_STATUS_IS_BUSY;
    }

    inst->cfg->hw_config->enable_hw_irq_callback();

    return ret_status;
}


/*******************************************************************************
* WRITE_PROTECT helpers
*******************************************************************************/
#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
    defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
/** Returns true if cmd_code is unprotected (writable) at the given WRITE_PROTECT level. */
__STATIC_INLINE bool mtb_pmbus_int_is_wp_unprotected(uint8_t cmd_code, uint8_t wp_val)
{
    bool result;

    if (mtb_pmbus_int_is_wp_exempt(cmd_code))
    {
        result = true;   /* Protect Locks: always writable */
    }
    else
    {
        switch (wp_val)
        {
            case MTB_PMBUS_WP_VAL_PROTECT_ALL:
                result = false;
                break;

            case MTB_PMBUS_WP_VAL_PROTECT_IMMEDIATE:
                result = (cmd_code == MTB_PMBUS_OPERATION_CMD_CODE);
                break;

            case MTB_PMBUS_WP_VAL_PROTECT_VOLATILE:
                result = ((cmd_code == MTB_PMBUS_OPERATION_CMD_CODE) ||
                          (cmd_code == MTB_PMBUS_ON_OFF_CONFIG_CMD_CODE) ||
                          ((cmd_code >= MTB_PMBUS_VOUT_FIRST_CMD_CODE) && (cmd_code <= MTB_PMBUS_VOUT_LAST_CMD_CODE)) ||
                          ((cmd_code >= MTB_PMBUS_VOUT_TRANS_FIRST_CMD_CODE) &&
                           (cmd_code <= MTB_PMBUS_VOUT_TRANS_LAST_CMD_CODE)));
                break;

            case MTB_PMBUS_WP_VAL_NO_PROTECTION:
                result = true;
                break;

            default:
                /* Undefined WRITE_PROTECT value — apply PROTECT_ALL for safety. */
                result = false;
                break;
        }
    }

    return result;
}


void mtb_pmbus_apply_write_protect_isr(mtb_pmbus_stc_t *inst, uint8_t value)
{
    /* Store the new value for read-back in the user callback */
    inst->write_protect_val = value;

    /* Apply IS_WR_PROTECTED flag to every command in the main table.
     * Most-restrictive-wins: WRITE_PROTECT and ACCESS_CONTROL bit [7] are
     * independent sources; set the flag if either source demands protection. */
    for (uint16_t i = 0U; i < inst->cfg->cmd_num; i++)
    {
        uint8_t code = inst->cfg->cmd_table[i].cmd_code;
        bool wp_protects  = !mtb_pmbus_int_is_wp_unprotected(code, value);
        bool acl_protects = (inst->acl_table[code] & MTB_PMBUS_ACL_BIT_WRITE_ACCESS) != 0U;
        mtb_pmbus_cmd_wr_protect_isr(inst, (uint32_t)code, wp_protects || acl_protects);
    }

#if (MTB_PMBUS_IMPL_CMD_NUM != 0U)
    /* Apply IS_WR_PROTECTED to pre-implemented commands that support write direction.
     * These are not in cmd_table so the loop above does not cover them.
     * ACL does not apply to pre-impl commands (no acl_table entry). */
    for (uint8_t i = 0U; i < MTB_PMBUS_IMPL_CMD_NUM; i++)
    {
        if ((inst->pre_impl_cmd_lookup_tbl[i].flags & MTB_PMBUS_CMD_FLAG_IS_PRESENT) != 0U)
        {
            uint8_t code = inst->pre_impl_cmd_table[i].cmd_code;
            uint32_t cap = inst->pre_impl_cmd_table[i].cmd_cap;
            if ((cap & (MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL)) != 0U)
            {
                bool protect = !mtb_pmbus_int_is_wp_unprotected(code, value);
                if (protect)
                {
                    inst->pre_impl_cmd_lookup_tbl[i].flags |= MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED;
                }
                else
                {
                    inst->pre_impl_cmd_lookup_tbl[i].flags &=
                        (uint8_t)(~MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED);
                }
            }
        }
    }
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) */

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
    if (inst->cfg->enable_ext_cmd)
    {
        /* Extended command codes: most-restrictive-wins between WRITE_PROTECT level
         * and ext_acl_table bit [7] (if ext_acl_table is provided). */
        bool protect_ext = (value != MTB_PMBUS_WP_VAL_NO_PROTECTION);
        for (uint16_t i = 0U; i < inst->cfg->ext_cmd_num; i++)
        {
            uint32_t ext_code = ((uint32_t)inst->cfg->ext_cmd_table[i].cmd_code
                                 << MTB_PMBUS_CMD_EXT_POS) | MTB_PMBUS_CMD_CODE_EXT;
            if (inst->cfg->ext_acl_table != NULL)
            {
                uint8_t pos = inst->ext_cmd_lookup_tbl[inst->cfg->ext_cmd_table[i].cmd_code].cmd_pos;
                bool acl_ext = (inst->cfg->ext_acl_table[pos] & MTB_PMBUS_ACL_BIT_WRITE_ACCESS) != 0U;
                mtb_pmbus_cmd_wr_protect_isr(inst, ext_code, protect_ext || acl_ext);
            }
            else
            {
                mtb_pmbus_cmd_wr_protect_isr(inst, ext_code, protect_ext);
            }
        }
    }
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
}


mtb_pmbus_status_t mtb_pmbus_apply_write_protect(mtb_pmbus_stc_t *inst, uint8_t value)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_IS_BUSY;

    CY_ASSERT(NULL != inst);

    inst->cfg->hw_config->disable_hw_irq_callback();

    if (!mtb_pmbus_is_busy(inst))
    {
        mtb_pmbus_apply_write_protect_isr(inst, value);
        ret_status = MTB_PMBUS_STATUS_SUCCESS;
    }

    inst->cfg->hw_config->enable_hw_irq_callback();

    return ret_status;
}
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */


/*******************************************************************************
* ACCESS_CONTROL ACL helpers
*******************************************************************************/
#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
    defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
uint8_t mtb_pmbus_get_acl(const mtb_pmbus_stc_t *inst, uint32_t cmd_code)
{
    uint8_t acl_byte = 0x00U;

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
    if (MTB_PMBUS_CMD_CODE_EXT == (MTB_PMBUS_CMD_EXT_MASK & cmd_code))
    {
        if (inst->cfg->ext_acl_table != NULL)
        {
            uint8_t ext_code = (uint8_t)((cmd_code >> MTB_PMBUS_CMD_EXT_POS) & 0xFFU);
            if ((inst->ext_cmd_lookup_tbl[ext_code].flags & MTB_PMBUS_CMD_FLAG_IS_PRESENT) != 0U)
            {
                acl_byte = inst->cfg->ext_acl_table[inst->ext_cmd_lookup_tbl[ext_code].cmd_pos];
            }
        }
    }
    else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
    {
        acl_byte = inst->acl_table[(uint8_t)cmd_code];
    }

    return acl_byte;
}


void mtb_pmbus_set_acl_isr(mtb_pmbus_stc_t *inst, uint32_t cmd_code, uint8_t acl_byte)
{
    bool rejected = false;

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
    if (MTB_PMBUS_CMD_CODE_EXT == (MTB_PMBUS_CMD_EXT_MASK & cmd_code))
    {
        if (inst->cfg->ext_acl_table != NULL)
        {
            uint8_t ext_code = (uint8_t)((cmd_code >> MTB_PMBUS_CMD_EXT_POS) & 0xFFU);
            if ((inst->ext_cmd_lookup_tbl[ext_code].flags & MTB_PMBUS_CMD_FLAG_IS_PRESENT) != 0U)
            {
                uint8_t pos = inst->ext_cmd_lookup_tbl[ext_code].cmd_pos;
                uint8_t current = inst->cfg->ext_acl_table[pos];
                if ((current & MTB_PMBUS_ACL_BIT_NEVER_AGAIN) != 0U)
                {
                    inst->errors |= MTB_PMBUS_ERR_ACL_WR_REJECTED;
                    MTB_PMBUS_LOG_WRN("ACL update for ext cmd [%x] rejected: Never Again bit set", ext_code);
                    rejected = true;
                }
                else if ((current & MTB_PMBUS_ACL_BIT_NO_MORE) != 0U)
                {
                    inst->errors |= MTB_PMBUS_ERR_ACL_WR_REJECTED;
                    MTB_PMBUS_LOG_WRN("ACL update for ext cmd [%x] rejected: No More bit set", ext_code);
                    rejected = true;
                }
                else
                {
                    inst->cfg->ext_acl_table[pos] = acl_byte;
                }
            }
        }
    }
    else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
    {
        uint8_t idx = (uint8_t)cmd_code;
        uint8_t current = inst->acl_table[idx];

        /* Never Again [0]: permanently locked */
        if ((current & MTB_PMBUS_ACL_BIT_NEVER_AGAIN) != 0U)
        {
            inst->errors |= MTB_PMBUS_ERR_ACL_WR_REJECTED;
            MTB_PMBUS_LOG_WRN("ACL update for cmd [%x] rejected: Never Again bit set", idx);
            rejected = true;
        }
        /* No More [1]: not alterable even when PASSKEY is unlocked */
        else if ((current & MTB_PMBUS_ACL_BIT_NO_MORE) != 0U)
        {
            inst->errors |= MTB_PMBUS_ERR_ACL_WR_REJECTED;
            MTB_PMBUS_LOG_WRN("ACL update for cmd [%x] rejected: No More bit set", idx);
            rejected = true;
        }
        else
        {
            inst->acl_table[idx] = acl_byte;
        }
    }

    /* Synchronize write-access restriction with the IS_WR_PROTECTED flag so that both
     * WRITE_PROTECT and ACCESS_CONTROL bit [7] can independently restrict writes
     * (most-restrictive-wins is preserved automatically). */
    if (!rejected)
    {
        mtb_pmbus_cmd_wr_protect_isr(inst, cmd_code,
                                     (acl_byte & MTB_PMBUS_ACL_BIT_WRITE_ACCESS) != 0U);
    }
}


mtb_pmbus_status_t mtb_pmbus_set_acl(mtb_pmbus_stc_t *inst, uint32_t cmd_code, uint8_t acl_byte)
{
    mtb_pmbus_status_t status;

    CY_ASSERT(NULL != inst);

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
    if (MTB_PMBUS_CMD_CODE_EXT == (MTB_PMBUS_CMD_EXT_MASK & cmd_code))
    {
        if (inst->cfg->ext_acl_table == NULL)
        {
            status = MTB_PMBUS_STATUS_BAD_PARAM;
        }
        else
        {
            uint8_t ext_code = (uint8_t)((cmd_code >> MTB_PMBUS_CMD_EXT_POS) & 0xFFU);
            if ((inst->ext_cmd_lookup_tbl[ext_code].flags & MTB_PMBUS_CMD_FLAG_IS_PRESENT) == 0U)
            {
                status = MTB_PMBUS_STATUS_BAD_PARAM;
            }
            else
            {
                status = MTB_PMBUS_STATUS_IS_BUSY;
            }
        }
    }
    else
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
    {
        /* Validate that the standard command code is registered */
        status = mtb_pmbus_int_cmd_arg_is_valid(inst, cmd_code,
                                                MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE,
                                                0U, true);
        if (status == MTB_PMBUS_STATUS_SUCCESS)
        {
            status = MTB_PMBUS_STATUS_IS_BUSY;
        }
    }

    if (status == MTB_PMBUS_STATUS_IS_BUSY)
    {
        if (!mtb_pmbus_is_busy(inst))
        {
            inst->cfg->hw_config->disable_hw_irq_callback();
            mtb_pmbus_set_acl_isr(inst, cmd_code, acl_byte);
            inst->cfg->hw_config->enable_hw_irq_callback();
            status = MTB_PMBUS_STATUS_SUCCESS;
        }
    }

    return status;
}


void mtb_pmbus_set_acl_from_nvm(mtb_pmbus_stc_t *inst, const uint8_t *nvm_acl, uint16_t count)
{
    CY_ASSERT(NULL != inst);
    CY_ASSERT(NULL != nvm_acl);

    uint16_t limit = (count > (uint16_t)MTB_PMBUS_CMD_MAX_NUM)
                     ? (uint16_t)MTB_PMBUS_CMD_MAX_NUM
                     : count;

    for (uint16_t i = 0U; i < limit; i++)
    {
        /* Write directly — NVM image is authoritative at power-on;
         * Never Again / No More guards are intentionally bypassed. */
        inst->acl_table[i] = nvm_acl[i];

        /* Sync IS_WR_PROTECTED flag for commands with bit [7] set */
        mtb_pmbus_cmd_wr_protect_isr(inst, (uint32_t)i,
                                     (nvm_acl[i] & MTB_PMBUS_ACL_BIT_WRITE_ACCESS) != 0U);
    }
}


/*******************************************************************************
* PASSKEY helpers
*******************************************************************************/
mtb_pmbus_passkey_state_t mtb_pmbus_get_passkey_state_isr(const mtb_pmbus_stc_t *inst)
{
    CY_ASSERT(NULL != inst);
    return inst->passkey_state;
}


uint8_t mtb_pmbus_get_passkey_read_byte_isr(const mtb_pmbus_stc_t *inst)
{
    CY_ASSERT(NULL != inst);
    uint8_t result;

    switch (inst->passkey_state)
    {
        case MTB_PMBUS_PASSKEY_ST_LOCKED:
            result = (uint8_t)(MTB_PMBUS_PASSKEY_STATE_BYTE_LOCKED_BASE |
                               (inst->passkey_fail_cnt & MTB_PMBUS_PASSKEY_FAIL_CNT_MASK));
            break;

        case MTB_PMBUS_PASSKEY_ST_LOCKED_OUT:
            result = MTB_PMBUS_PASSKEY_STATE_BYTE_LOCKED_OUT;
            break;

        case MTB_PMBUS_PASSKEY_ST_UNLOCKED:
            result = 0x00U;
            break;

        case MTB_PMBUS_PASSKEY_ST_SET_NOT_LOCKED:
            result = 0x00U;
            break;

        default:
            result = 0x00U;
            break;
    }

    return result;
}


void mtb_pmbus_passkey_transition_isr(mtb_pmbus_stc_t *inst, bool key_matched, bool is_zero_key)
{
    CY_ASSERT(NULL != inst);
    switch (inst->passkey_state)
    {
        case MTB_PMBUS_PASSKEY_ST_UNLOCKED:
            if (!is_zero_key)
            {
                /* First passkey write — move to SetNotLocked; application stores key */
                inst->passkey_state = MTB_PMBUS_PASSKEY_ST_SET_NOT_LOCKED;
            }
            /* Writing zero passkey when Unlocked has no defined effect; ignore */
            break;

        case MTB_PMBUS_PASSKEY_ST_SET_NOT_LOCKED:
            if (is_zero_key)
            {
                /* Cancel — return to Unlocked */
                inst->passkey_state = MTB_PMBUS_PASSKEY_ST_UNLOCKED;
            }
            else if (key_matched)
            {
                /* Confirmation write matches — transition to Locked */
                inst->passkey_state    = MTB_PMBUS_PASSKEY_ST_LOCKED;
                inst->passkey_fail_cnt = 0U;
            }
            else
            {
                /* Non-matching non-zero write in SetNotLocked state.
                 * This branch is unreachable in normal usage: the application must
                 * return false from MTB_PMBUS_CMD_WRITE_BYTE on the last data byte
                 * to NACK it; the MW then sets errors and suppresses CMD_WRITE_DONE,
                 * so this function is never called for the mismatch path. */
            }
            break;

        case MTB_PMBUS_PASSKEY_ST_LOCKED:
            if (key_matched && !is_zero_key)
            {
                /* Matching NVM passkey — first step of unlock (-> SetNotLocked) */
                inst->passkey_state = MTB_PMBUS_PASSKEY_ST_SET_NOT_LOCKED;
            }
            else
            {
                /* Non-matching — stealth; increment fail counter */
                inst->passkey_fail_cnt++;
                if (inst->passkey_fail_cnt >= MTB_PMBUS_PASSKEY_MAX_FAIL_CNT)
                {
                    inst->passkey_state = MTB_PMBUS_PASSKEY_ST_LOCKED_OUT;
                    inst->errors |= MTB_PMBUS_ERR_PASSKEY_LOCKED_OUT;
                }
            }
            break;

        case MTB_PMBUS_PASSKEY_ST_LOCKED_OUT:
            /* All writes silently accepted; no state changes */
            break;

        default:
            /* Unreachable: passkey_state is only assigned from mtb_pmbus_passkey_state_t values */
            break;
    }
}


void mtb_pmbus_passkey_force_locked_isr(mtb_pmbus_stc_t *inst, uint8_t fail_cnt)
{
    CY_ASSERT(NULL != inst);
    inst->passkey_state    = MTB_PMBUS_PASSKEY_ST_LOCKED;
    inst->passkey_fail_cnt = (fail_cnt >= MTB_PMBUS_PASSKEY_MAX_FAIL_CNT)
                              ? (MTB_PMBUS_PASSKEY_MAX_FAIL_CNT - 1U)
                              : fail_cnt;
}


mtb_pmbus_status_t mtb_pmbus_passkey_force_locked(mtb_pmbus_stc_t *inst, uint8_t fail_cnt)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_IS_BUSY;

    CY_ASSERT(NULL != inst);

    inst->cfg->hw_config->disable_hw_irq_callback();

    if (!mtb_pmbus_is_busy(inst))
    {
        mtb_pmbus_passkey_force_locked_isr(inst, fail_cnt);
        ret_status = MTB_PMBUS_STATUS_SUCCESS;
    }

    inst->cfg->hw_config->enable_hw_irq_callback();

    return ret_status;
}
#endif /* #if MTB_PMBUS_SUPPORT_SECURITY */


mtb_pmbus_status_t mtb_pmbus_cmd_process_call_wr_done(mtb_pmbus_stc_t *inst, bool *status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_BAD_PARAM;

    CY_ASSERT(NULL != inst);

    if (NULL != status)
    {
        *status = inst->is_pause_allowed;
        ret_status = MTB_PMBUS_STATUS_SUCCESS;
    }

    return ret_status;
}


mtb_pmbus_status_t mtb_pmbus_cmd_exit_isr(mtb_pmbus_stc_t *inst)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;

    CY_ASSERT(NULL != inst);

    if (inst->is_pause_allowed)
    {
        if (mtb_pmbus_hal_pause_tx(inst))
        {
            /* Enable communication pause flag */
            inst->active_lookup_tbl[inst->cmd_code].flags |= MTB_PMBUS_CMD_FLAG_IS_PAUSED;
            status = MTB_PMBUS_STATUS_SUCCESS;
        }
    }
    else
    {
        /* The requested action is aborted as the function called outside of WRITE_DONE event for
           Process Call */
        MTB_PMBUS_LOG_WRN("Pause is not allowed at current moment");
    }

    return status;
}


mtb_pmbus_status_t mtb_pmbus_cmd_complete_transfer(mtb_pmbus_stc_t *inst, uint32_t code)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
    mtb_pmbus_stc_config_cmd_t *cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t *cmd_lookup_tbl = NULL;
    uint32_t code_tmp = code;

    CY_ASSERT(NULL != inst);

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    status = mtb_pmbus_int_cmd_arg_is_valid(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, 0U, true);
    if (status == MTB_PMBUS_STATUS_SUCCESS)
    {
        if (0U != (cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_PAUSED))
        {
            if (0U == inst->errors)
            {
                mtb_pmbus_int_cmd_handle_tx(inst);
            }
            inst->cfg->hw_config->disable_hw_irq_callback();
            /* Disable I2C interrupt to ensure that status will be updated before TX response */
            if (mtb_pmbus_hal_resume_tx(inst))
            {
                cmd_lookup_tbl[code_tmp].flags &= (uint8_t)(~MTB_PMBUS_CMD_FLAG_IS_PAUSED);
            }
            else
            {
                MTB_PMBUS_LOG_ERR("Failed to resume TX transfer");
                status = MTB_PMBUS_STATUS_BAD_PARAM;
            }
            inst->cfg->hw_config->enable_hw_irq_callback();
        }
    }

    return status;
}


#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U) && \
    defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL >= 0U))
/*******************************************************************************
* Function Name: mtb_pmbus_sec_get_level
********************************************************************************
* Returns the encoded Security Level compiled into the Middleware.
*******************************************************************************/
uint8_t mtb_pmbus_sec_get_level(void)
{
    return (uint8_t)(1U << MTB_PMBUS_SEC_LEVEL);
}


/*******************************************************************************
* Function Name: mtb_pmbus_sec_check_l0_cmds
********************************************************************************
* Checks that all mandatory Security Level 0 commands are registered.
*******************************************************************************/
mtb_pmbus_status_t mtb_pmbus_sec_check_l0_cmds(mtb_pmbus_stc_t *inst, uint8_t *missing_mask)
{
    mtb_pmbus_status_t status;

    if (NULL == inst)
    {
        status = MTB_PMBUS_STATUS_BAD_PARAM;
    }
    else
    {
        uint8_t mask = 0U;

        if ((inst->cmd_lookup_tbl[MTB_PMBUS_PASSKEY_CMD_CODE].flags &
             MTB_PMBUS_CMD_FLAG_IS_PRESENT) == 0U)
        {
            mask |= MTB_PMBUS_L0_MISSING_PASSKEY;
        }
        if ((inst->cmd_lookup_tbl[MTB_PMBUS_ACCESS_CONTROL_CMD_CODE].flags &
             MTB_PMBUS_CMD_FLAG_IS_PRESENT) == 0U)
        {
            mask |= MTB_PMBUS_L0_MISSING_ACCESS_CONTROL;
        }
        if ((inst->cmd_lookup_tbl[MTB_PMBUS_WRITE_PROTECT_CMD_CODE].flags &
             MTB_PMBUS_CMD_FLAG_IS_PRESENT) == 0U)
        {
            mask |= MTB_PMBUS_L0_MISSING_WRITE_PROTECT;
        }

        if (NULL != missing_mask)
        {
            *missing_mask = mask;
        }

        status = (mask == 0U) ? MTB_PMBUS_STATUS_SUCCESS : MTB_PMBUS_STATUS_L0_CMDS_MISSING;
    }

    return status;
}
#endif /* #if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U)) */
