/***************************************************************************//**
* \file mtb_pmbus_ctrl_int.h
* \version 1.0
*
* Provides internal definition for PMBUS Controller.
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

#ifndef MTB_PMBUS_CTRL_INT_H
#define MTB_PMBUS_CTRL_INT_H

#include "mtb_pmbus.h"
#include <string.h>

#define MTB_PMBUS_CTRL_EVENTS_WR_DONE           (0x01U)
#define MTB_PMBUS_CTRL_EVENTS_RD_DONE           (0x02U)
#define MTB_PMBUS_CTRL_EVENTS_ERR               (0x04U)

/* Controller error codes */
/* Received PEC does not match */
#define MTB_PMBUS_CTRL_ERR_CORRUPTED_DATA               (0x01U)
/* The Target NACKed the address */
#define MTB_PMBUS_CTRL_ERR_NACK_ADDR                    (0x02U)
/* The Target NACKed the data */
#define MTB_PMBUS_CTRL_ERR_NACK_DATA                    (0x04U)
/* The Target NACKed the command code */
#define MTB_PMBUS_CTRL_ERR_NACK_CMD                     (0x08U)
/* Bus error is happened on the I2C bus */
#define MTB_PMBUS_CTRL_ERR_BUS_ERROR                    (0x10U)
/* Timeout is detected */
#define MTB_PMBUS_CTRL_ERR_TIMEOUT                      (0x20U)
/* Controller arbitration lost */
#define MTB_PMBUS_CTRL_ERR_ARB_LOST                     (0x40U)
/* Controller abort start */
#define MTB_PMBUS_CTRL_ERR_ABORT_START                  (0x80U)
/* Received block count is too big */
#define MTB_PMBUS_CTRL_ERR_BLOCK_COUNT                  (0x100U)

#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
void mtb_pmbus_ctrl_handle_timeout(mtb_pmbus_ctrl_stc_t *inst);
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */

bool mtb_pmbus_ctrl_handle_rx(mtb_pmbus_ctrl_stc_t *inst, uint8_t byte);
void mtb_pmbus_ctrl_events_handler(mtb_pmbus_ctrl_stc_t *inst, uint32_t events);

mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_hal_write(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint8_t *data, uint32_t size,
                                                 bool stop);
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_hal_read(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint8_t *data, uint32_t size,
                                                bool stop);
void mtb_pmbus_ctrl_hal_init(mtb_pmbus_ctrl_stc_t *inst);
bool mtb_pmbus_ctrl_hal_is_bus_busy(mtb_pmbus_ctrl_stc_t *inst);
void mtb_pmbus_hal_delay_us(mtb_pmbus_ctrl_stc_t *inst, uint32_t delay);
void mtb_pmbus_ctrl_hal_set_errors(mtb_pmbus_ctrl_stc_t *inst);
uint32_t mtb_pmbus_ctrl_hal_get_status(mtb_pmbus_ctrl_stc_t *inst);
uint32_t mtb_pmbus_ctrl_hal_get_byte_count(mtb_pmbus_ctrl_stc_t *inst);
void mtb_pmbus_ctrl_hal_abort_write(mtb_pmbus_ctrl_stc_t *inst);
void mtb_pmbus_ctrl_hal_abort_read(mtb_pmbus_ctrl_stc_t *inst);
#if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U))
void mtb_pmbus_ctrl_handle_hnp(mtb_pmbus_ctrl_stc_t *inst, mtb_pmbus_ctrl_host_notify_events_t event);
void mtb_pmbus_ctrl_hal_hnp_clr_write_status(mtb_pmbus_ctrl_stc_t *inst);
void mtb_pmbus_ctrl_hal_hnp_resetup_buf(mtb_pmbus_ctrl_stc_t *inst);
uint8_t mtb_pmbus_ctrl_hal_hnp_get_trgt_addr(mtb_pmbus_ctrl_stc_t *inst);
uint16_t mtb_pmbus_ctrl_hal_hnp_get_trgt_data(mtb_pmbus_ctrl_stc_t *inst);
bool mtb_pmbus_ctrl_hal_hnp_transfer_is_ok(mtb_pmbus_ctrl_stc_t *inst);
#endif /* #if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U)) */
#endif /* MTB_PMBUS_CTRL_INT_H */
