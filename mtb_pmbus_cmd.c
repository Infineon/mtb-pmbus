/***************************************************************************//**
* \file mtb_pmbus.c
* \version 1.0
*
* Provides command APIs implementation for the PMBus Middleware.
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

#define CMD_BUFF_SIZE(code)                     (CMD_IS_BLOCK((code)) ? ((uint32_t)(CMD_DATA_SIZE((code))) + MTB_PMBUS_CMD_BLOCK_EXTRA_BYTE) : \
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


__STATIC_INLINE void mtb_pmbus_int_get_table(mtb_pmbus_stc_t * inst, uint32_t * code, mtb_pmbus_stc_config_cmd_t ** cmd_table_ptr,
                                         mtb_pmbus_cmd_lookup_tbl_t ** cmd_lookup_tbl);
static mtb_pmbus_status_t mtb_pmbus_int_cmd_arg_is_valid(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page,
                                                     int32_t phase, uint32_t size, bool skip_pp);

/* Get the tables based on command code */
__STATIC_INLINE void mtb_pmbus_int_get_table(mtb_pmbus_stc_t * inst, uint32_t * code, mtb_pmbus_stc_config_cmd_t ** cmd_table_ptr,
                                mtb_pmbus_cmd_lookup_tbl_t ** cmd_lookup_tbl)
{
    (void)code;
    *cmd_table_ptr = inst->cfg->cmd_table;
    *cmd_lookup_tbl = inst->cmd_lookup_tbl;
}

/* Check if arguments are valid */
static mtb_pmbus_status_t mtb_pmbus_int_cmd_arg_is_valid(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page,
                                                         int32_t phase, uint32_t size, bool skip_pp)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_SUCCESS;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

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
        if ((((!CMD_IS_PAGED(code)) && (page >= 0)) || (page > ((int32_t)inst->cfg->num_pages - 1)) || (CMD_IS_PAGED(code) && (page == MTB_PMBUS_NO_PAGE_PHASE))) && !skip_pp)
        {
            MTB_PMBUS_LOG_WRN("The page %" PRIu32 " is not valid for command %" PRIx32,
                              page, code_tmp);
            status = MTB_PMBUS_STATUS_INVALID_PAGE;
        }
        else
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
        {
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
            if ((((!CMD_IS_PHASED(code)) && (phase >= 0)) || (phase > ((int32_t)inst->cfg->num_phases - 1)) || (CMD_IS_PHASED(code) && (phase == MTB_PMBUS_NO_PAGE_PHASE))) && !skip_pp)
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
    (void) page;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM == 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM == 0U))
    (void) phase;
    (void) skip_pp;
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM == 0U)) */
    return status;
}

void mtb_pmbus_cmd_update_data_ext_isr(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page,
                                                 int32_t phase, uint8_t * data, uint32_t data_size)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    /* Find the position of first element in data array */
    uint8_t *data_buff_start = CMD_DATA_START_POS(code_tmp, page, phase);
    if (CMD_IS_BLOCK(code))
    {
        /* Store the updated size in the first variable */
        *data_buff_start = (uint8_t)data_size;
        (void) memcpy(data_buff_start + MTB_PMBUS_CMD_BLOCK_EXTRA_BYTE, data, data_size);
    }
    else
    {
        (void) memcpy(data_buff_start, data, data_size);
    }
}

mtb_pmbus_status_t mtb_pmbus_cmd_update_data_ext(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page,
                                                 int32_t phase, uint8_t * data, uint32_t data_size)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;
    uint32_t code_tmp = code;

    CY_ASSERT(NULL != inst);

    /* Disable I2C interrupt */
    inst->cfg->hw_config->disable_hw_irq_callback();

    if (NULL != data)
    {
        mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

        /* Check if requested data is valid */
        status = mtb_pmbus_int_cmd_arg_is_valid(inst, code_tmp, page, phase, data_size, false);
        if (status == MTB_PMBUS_STATUS_SUCCESS)
        {
            /* Check if current command is transferring at this moment of time */
            if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) == 0U)
            {
                mtb_pmbus_cmd_update_data_ext_isr(inst, code_tmp, page, phase, data, data_size);
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

void mtb_pmbus_cmd_update_data_isr(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * data, uint32_t data_size)
{
    mtb_pmbus_cmd_update_data_ext_isr(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, data, data_size);
}

mtb_pmbus_status_t mtb_pmbus_cmd_update_data(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * data,
                                             uint32_t data_size)
{
    return mtb_pmbus_cmd_update_data_ext(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, data, data_size);
}

void mtb_pmbus_cmd_read_data_ext_isr(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page,
                                                 int32_t phase, uint8_t * data, uint32_t data_size)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    /* Find the position of first element in data array */
    uint8_t *data_buff_start =  CMD_DATA_START_POS(code_tmp, page, phase);

    if (CMD_IS_BLOCK(code_tmp))
    {
        (void) memcpy(data, data_buff_start + MTB_PMBUS_CMD_BLOCK_EXTRA_BYTE, data_size);
    }
    else
    {
        (void) memcpy(data, data_buff_start, data_size);
    }
}

mtb_pmbus_status_t mtb_pmbus_cmd_read_data_ext(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page,
                                               int32_t phase, uint8_t * data, uint32_t data_size)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    /* Disable I2C interrupt */
    inst->cfg->hw_config->disable_hw_irq_callback();

    if (NULL != data)
    {
        /* Check if requested data is valid */
        status = mtb_pmbus_int_cmd_arg_is_valid(inst, code_tmp, page, phase, data_size, false);
        if (status == MTB_PMBUS_STATUS_SUCCESS)
        {
            mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

            /* Check if current command is transferring at this moment of time */
            if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) == 0U)
            {
                mtb_pmbus_cmd_read_data_ext_isr(inst, code_tmp, page, phase, data, data_size);
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

void mtb_pmbus_cmd_read_data_isr(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * data, uint32_t data_size)
{
    mtb_pmbus_cmd_read_data_ext_isr(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, data, data_size);
}

mtb_pmbus_status_t mtb_pmbus_cmd_read_data(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * data,
                                           uint32_t data_size)
{
    return mtb_pmbus_cmd_read_data_ext(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, data, data_size);
}

void mtb_pmbus_cmd_get_transfer_size_ext_isr(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page,
                                                 int32_t phase, uint8_t * size)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

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

mtb_pmbus_status_t mtb_pmbus_cmd_get_transfer_size_ext(mtb_pmbus_stc_t * inst, uint32_t code,  int32_t page,
                                                       int32_t phase, uint8_t * size)
{
    mtb_pmbus_status_t status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    /* Disable I2C interrupt */
    inst->cfg->hw_config->disable_hw_irq_callback();

    if (NULL != size)
    {
        /* Check if requested data is valid */
        status = mtb_pmbus_int_cmd_arg_is_valid(inst, code_tmp, page, phase, 0U, false);
        if (status == MTB_PMBUS_STATUS_SUCCESS)
        {
            mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

            /* Check if current command is transferring at this moment of time */
            if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) == 0U)
            {
                mtb_pmbus_cmd_get_transfer_size_ext_isr(inst, code_tmp, page, phase, size);
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

void mtb_pmbus_cmd_get_transfer_size_isr(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * size)
{
    mtb_pmbus_cmd_get_transfer_size_ext_isr(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, size);
}

mtb_pmbus_status_t mtb_pmbus_cmd_get_transfer_size(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * size)
{
    return mtb_pmbus_cmd_get_transfer_size_ext(inst, code, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, size);
}

void mtb_pmbus_cmd_is_active_isr(mtb_pmbus_stc_t * inst, uint32_t code, bool * status)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    *status = ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) != 0U) ? true : false;
}

mtb_pmbus_status_t mtb_pmbus_cmd_is_active(mtb_pmbus_stc_t * inst, uint32_t code, bool * status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    /* Check if requested data is valid */
    ret_status = mtb_pmbus_int_cmd_arg_is_valid(inst, code_tmp, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, 0U, true);
    if (ret_status == MTB_PMBUS_STATUS_SUCCESS)
    {
        mtb_pmbus_cmd_is_active_isr(inst, code_tmp, status);
    }

    return ret_status;
}

mtb_pmbus_status_t mtb_pmbus_cmd_get_status(mtb_pmbus_stc_t * inst, uint32_t code, bool * status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    /* Check if requested data is valid */
    ret_status = mtb_pmbus_int_cmd_arg_is_valid(inst, code_tmp, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, 0U, true);
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

void mtb_pmbus_cmd_enable_disable_isr(mtb_pmbus_stc_t * inst, uint32_t code, bool status)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

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

mtb_pmbus_status_t mtb_pmbus_cmd_enable_disable(mtb_pmbus_stc_t * inst, uint32_t code, bool status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    /* Check if requested data is valid */
    ret_status = mtb_pmbus_int_cmd_arg_is_valid(inst, code_tmp, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, 0U, true);
    if (ret_status == MTB_PMBUS_STATUS_SUCCESS)
    {
        mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

        CY_ASSERT(NULL != cmd_table_ptr);
        CY_ASSERT(NULL != cmd_lookup_tbl);

        inst->cfg->hw_config->disable_hw_irq_callback();

        /* Check if current command is transferring at this moment of time */
        if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) == 0U)
        {
            mtb_pmbus_cmd_enable_disable_isr(inst, code_tmp, status);
        }
        else
        {
            ret_status = MTB_PMBUS_STATUS_CMD_IS_ACTIVE;
        }

        inst->cfg->hw_config->enable_hw_irq_callback();
    }

    return ret_status;
}

mtb_pmbus_status_t mtb_pmbus_cmd_all_enable_disable(mtb_pmbus_stc_t * inst, bool status)
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
            (void) mtb_pmbus_cmd_enable_disable(inst, inst->cfg->cmd_table[i].cmd_code, status);
        }
    }
    else
    {
        ret_status = MTB_PMBUS_STATUS_IS_BUSY;
    }

    inst->cfg->hw_config->enable_hw_irq_callback();

    return ret_status;
}

void mtb_pmbus_cmd_wr_protect_isr(mtb_pmbus_stc_t * inst, uint32_t code, bool status)
{
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

    mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

    if (status)
    {
        cmd_lookup_tbl[code_tmp].flags |= MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED;
    }
    else
    {
        cmd_lookup_tbl[code_tmp].flags &= (uint8_t)(~MTB_PMBUS_CMD_FLAG_IS_WR_PROTECTED);
    }
}

mtb_pmbus_status_t mtb_pmbus_cmd_wr_protect(mtb_pmbus_stc_t * inst, uint32_t code, bool status)
{
    mtb_pmbus_status_t ret_status = MTB_PMBUS_STATUS_BAD_PARAM;
    uint32_t code_tmp = code;
    mtb_pmbus_stc_config_cmd_t * cmd_table_ptr = NULL;
    mtb_pmbus_cmd_lookup_tbl_t * cmd_lookup_tbl = NULL;

    CY_ASSERT(NULL != inst);

    /* Check if requested data is valid */
    ret_status = mtb_pmbus_int_cmd_arg_is_valid(inst, code_tmp, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, 0U, true);
    if (ret_status == MTB_PMBUS_STATUS_SUCCESS)
    {
        mtb_pmbus_int_get_table(inst, &code_tmp, &cmd_table_ptr, &cmd_lookup_tbl);

        CY_ASSERT(NULL != cmd_table_ptr);
        CY_ASSERT(NULL != cmd_lookup_tbl);

        inst->cfg->hw_config->disable_hw_irq_callback();

        if ((cmd_lookup_tbl[code_tmp].flags & MTB_PMBUS_CMD_FLAG_IS_COM_ACTIVE) == 0U)
        {
            mtb_pmbus_cmd_wr_protect_isr(inst, code_tmp, status);
        }
        else
        {
            ret_status = MTB_PMBUS_STATUS_CMD_IS_ACTIVE;
        }

        inst->cfg->hw_config->enable_hw_irq_callback();
    }

    return ret_status;
}

mtb_pmbus_status_t mtb_pmbus_cmd_all_wr_protect(mtb_pmbus_stc_t * inst, bool status)
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
            (void) mtb_pmbus_cmd_wr_protect(inst, inst->cfg->cmd_table[i].cmd_code, status);
        }
    }
    else
    {
        ret_status = MTB_PMBUS_STATUS_IS_BUSY;
    }

    inst->cfg->hw_config->enable_hw_irq_callback();

    return ret_status;
}
