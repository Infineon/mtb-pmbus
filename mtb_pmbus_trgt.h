/***************************************************************************//**
* \file mtb_pmbus_trgt.h
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

#ifndef MTB_PMBUS_TRGT_H
#define MTB_PMBUS_TRGT_H

#include "mtb_pmbus_conf_def.h"
#include "mtb_pmbus_log.h"
#include "mtb_pmbus_trgt_hal.h"
#include "mtb_pmbus_pec.h"

#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Internal */
#define MTB_PMBUS_CMD_MAX_NUM                   (256U)
#define MTB_PMBUS_CMD_CAP_FORMAT_POS            (6U)
#define MTB_PMBUS_BLOCK_COUNT_BYTE_SIZE         (1U)
#define MTB_PMBUS_ZONE_READ_PAGE_STATUS_SIZE    (2U)
#define MTB_PMBUS_HOST_NOTIFY_ADDR              (0x08U)

/**
 * \addtogroup group_pmbus_trgt_macro
 * \{
 */
/** Use this macro for \ref mtb_pmbus_cmd_update_data_ext, \ref mtb_pmbus_cmd_read_data_ext,
 * \ref mtb_pmbus_cmd_get_transfer_size_ext as input parameters for page or phase fields in case
 * if command is not paged or/and not phased.
 */
#define MTB_PMBUS_NO_PAGE_PHASE                 (-1)

/** The maximum command size in bytes */
#define MTB_PMBUS_CMD_MAX_SIZE                  (255U)

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** Macro to convert command code to extended command format.
 *
 * \note This macro is only available when \ref MTB_PMBUS_SUPPORT_EXT_CMD is enabled at compile time
 * options.
 */
#define MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT(code)      ((code << 0x08) | 0xFE)
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) || defined
       (MTB_PMBUS_DOXYGEN) */

/**
 * \defgroup group_pmbus_macro_cmd_cap Command capabilities macro
 * Command Capabilities setting rules:
 * - The \ref MTB_PMBUS_CMD_CAP_DIR_WR, \ref MTB_PMBUS_CMD_CAP_DIR_RD,
 * \ref MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL, and \ref MTB_PMBUS_CMD_CAP_BLOCK
 * are bitwise macros, and a few of them can be applied to one command.
 * However, it is forbidden to combine \ref MTB_PMBUS_CMD_CAP_DIR_WR,
 * \ref MTB_PMBUS_CMD_CAP_DIR_RD, and \ref MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL
 * together. Only two of them can be selected for one command.
 * - Additionally, each command must have one of the formats selected.
 * The formats cannot be combined. The selected format only impacts the
 * response of implemented QUERY command if the QUERY command is enabled.
 *
 * Valid combinations of capabilities:
 * - MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_FORMAT_16_BIT
 * - MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL | MTB_PMBUS_CMD_CAP_BLOCK |
 * MTB_PMBUS_CMD_CAP_FORMAT_DIR_MODE
 * - MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL | MTB_PMBUS_CMD_CAP_FORMAT_8_BIT
 *
 * Not valid combination of capabilities:
 * - MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL |
 * MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM: \ref MTB_PMBUS_CMD_CAP_DIR_WR, \ref MTB_PMBUS_CMD_CAP_DIR_RD and
 * \ref MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL can not be selected for one command
 * - MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD: At least one numeric format must be
 * selected
 *
 * \{
 */

/** The command supports write direction */
#define MTB_PMBUS_CMD_CAP_DIR_WR                (0x1U)
/** The command supports read direction */
#define MTB_PMBUS_CMD_CAP_DIR_RD                (0x2U)
/** The command supports Process Call */
#define MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL      (0x4U)
/** The command supports Block Write/Read protocols */
#define MTB_PMBUS_CMD_CAP_BLOCK                 (0x8U)
/** The command is paged */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) || MTB_PMBUS_DOXYGEN
#define MTB_PMBUS_CMD_CAP_PAGE                  (0x10U)
#else
#define MTB_PMBUS_CMD_CAP_PAGE                  (0U)
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) || MTB_PMBUS_DOXYGEN */
/** The command is phased */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) || MTB_PMBUS_DOXYGEN
#define MTB_PMBUS_CMD_CAP_PHASE                 (0x20U)
#else
#define MTB_PMBUS_CMD_CAP_PHASE                 (0U)
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) || MTB_PMBUS_DOXYGEN */
/** The command uses LINEAR11 or ULINEAR16 numeric format  */
#define MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16      (0U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses 16 bit signed number */
#define MTB_PMBUS_CMD_CAP_FORMAT_16_BIT         (1U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses IEEE Half Precision Floating Point Format */
#define MTB_PMBUS_CMD_CAP_FORMAT_IEEE           (2U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses Direct Mode Format */
#define MTB_PMBUS_CMD_CAP_FORMAT_DIR_MODE       (3U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses 8 bit unsigned */
#define MTB_PMBUS_CMD_CAP_FORMAT_8_BIT          (4U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses VID Mode Format */
#define MTB_PMBUS_CMD_CAP_FORMAT_VID_MODE       (5U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses Manufacturer specific format */
#define MTB_PMBUS_CMD_CAP_FORMAT_MFC_SPEC       (6U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command does not use any numeric data  */
#define MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM         (7U << MTB_PMBUS_CMD_CAP_FORMAT_POS)

/** \} group_pmbus_macro_cmd_cap */

/**
 * \defgroup group_pmbus_macro_error_evt Error events
 * \{
 */

/** Controller attempts to read too many bytes */
#define MTB_PMBUS_ERR_RD_TOO_MANY_BYTES         (0x01U)
/** Controller attempts to write too many bytes */
#define MTB_PMBUS_ERR_WR_TOO_MANY_BYTES         (0x02U)
/** Received command is unsupported or disabled */
#define MTB_PMBUS_ERR_UNSUPPORTED_CMD           (0x04U)
/** Controller writes less byte than expected */
#define MTB_PMBUS_ERR_WR_LESS_BYTES             (0x08U)
/** Controller read less byte than expected */
#define MTB_PMBUS_ERR_RD_LESS_BYTES             (0x10U)
/** Bus reset timeout occurred */
#define MTB_PMBUS_ERR_TIMEOUT                   (0x20U)
/** Received PEC does not match */
#define MTB_PMBUS_ERR_CORRUPTED_DATA            (0x40U)
/** Controller attempts to write data after the second address match */
#define MTB_PMBUS_ERR_WR_AFTER_SECOND_ADDR      (0x80U)
/** The received count byte exceeds the maximum command size.
 * Applicable only for Block Protocols
 */
#define MTB_PMBUS_ERR_BYTE_COUNT_TOO_BIG        (0x100U)
/** The controller attempts to write data for the read only command */
#define MTB_PMBUS_ERR_REQ_WR_FOR_RD_ONLY        (0x200U)
/** The Middleware loses Arbitration. This error is not reported
 * in the following cases:
 * - Arbitration is lost during Target response on ARA
 * (Address Response Alert)
 * - Arbitration is lost during Zone Read Protocol
 *
 * In these cases, the lost Arbitration is not an error.
 */
#define MTB_PMBUS_ERR_ARB_LOST                  (0x400U)
/** The controller sets an unsupported page for the current target.
 * These errors can be triggered only if the PAGE command implemented
 * is enabled in \ref mtb_pmbus_stc_config_t::impl_cmd_mask.
 */
#define MTB_PMBUS_ERR_INVALID_PAGE              (0x800U)
/** The controller sets an unsupported phase for the current target.
 * These errors can be triggered only if the implemented PHASE command
 * is enabled in \ref mtb_pmbus_stc_config_t::impl_cmd_mask.
 */
#define MTB_PMBUS_ERR_INVALID_PHASE             (0x1000U)
/** Command callback from the application sends NACK after receiving the byte */
#define MTB_PMBUS_ERR_APP_NACK_BYTE             (0x2000U)
/** The error code indicating a write operation to a protected PMBus command */
#define MTB_PMBUS_ERR_WR_TO_PROT_CMD            (0x4000U)
/** An error is detected on the bus. Typically, it is an
 * unexpected Start or Stop condition.
 */
#define MTB_PMBUS_ERR_BUS_ERROR                 (0x8000U)
/** The protocol is started with Target Address with Read Direction.
 *
 * The error is only applicable for PMBus mode.
 */
#define MTB_PMBUS_ERR_ADDR_WITH_RD_BIT          (0x10000U)
/** An error is detected when a read request occurs for a command
 *  specified as write and the process call.
 */
#define MTB_PMBUS_ERR_REQ_RD_FOR_PC_ONLY        (0x20000U)
/** An error is detected when a write request occurs for a command
 *  specified as read and the process call.
 */
#define MTB_PMBUS_ERR_REQ_WR_FOR_PC_ONLY        (0x40000U)
/** The zone value assigned by the controller is out of range */
#define MTB_PMBUS_ERR_ZONE_VALUE_OUT_OF_RANGE   (0x80000U)
/** Controller request read transfer for the paged/phased command when
 * an active page/phase is set to 0xFF.
 */
#define MTB_PMBUS_ERR_READ_REQ_FOR_FF           (0x100000U)
/** The controller attempts to read data for the write-only command */
#define MTB_PMBUS_ERR_REQ_RD_FOR_WR_ONLY        (0x200000U)

/** \} group_pmbus_macro_error_evt */

/**
 * \defgroup group_pmbus_macro_impl_cmd Implemented Commands macro
 * \{
 */

/** Enable the PAGE (0x00) command
 * \note This command can be disabled only if \ref mtb_pmbus_stc_config_t::num_pages
 * is equal to 0 or related compile-time option is disabled.
 */
#define MTB_PMBUS_IMPL_CMD_PAGE_EN              (1U << 0U)
/** Enable the PHASE (0x04) command
 * \note This command can be disabled only if \ref mtb_pmbus_stc_config_t::num_phases
 * is equal to 0 or related compile-time option is disabled.
 */
#define MTB_PMBUS_IMPL_CMD_PHASE_EN             (1U << 1U)
/** Enable the CAPABILITY (0x19) command */
#define MTB_PMBUS_IMPL_CMD_CAPABILITY_EN        (1U << 2U)
/** Enable the QUERY (0x1A) command */
#define MTB_PMBUS_IMPL_CMD_QUERY_EN             (1U << 3U)
/** Enable the PMBUS_REVISION (0x98) command */
#define MTB_PMBUS_IMPL_CMD_REVISION_EN          (1U << 4U)
/** Enable the ZONE_CONFIG (0x07) command
 * \note This command can be disabled only if \ref mtb_pmbus_stc_config_t::enable_zone
 * is true or related compile-time option is disabled.
 */
#define MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_EN       (1U << 5U)
/** Enable the ZONE_ACTIVE (0x08) command
 * \note This command can be disabled only if \ref mtb_pmbus_stc_config_t::enable_zone
 * is true or related compile-time option is disabled.
 */
#define MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_EN       (1U << 6U)
/** Enable the PAGE_PLUS_WRITE (0x05) command */
#define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_EN   (1U << 7U)
/** Enable the PAGE_PLUS_READ (0x06) command */
#define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_EN    (1UL << 8U)
/** Enable the P2_PLUS_WRITE (0x09) command */
#define MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_EN     (1UL << 9U)
/** Enable the P2_PLUS_READ (0x0A) command */
#define MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_EN      (1UL << 10U)

/** \} group_pmbus_macro_impl_cmd */

/** \} group_pmbus_trgt_macro */

/**
 * \addtogroup group_pmbus_trgt_enums
 * \{
 */
/** Used to return the statuses of most PMBus APIs */
typedef enum
{
    /** Correct status. No error. */
    MTB_PMBUS_STATUS_SUCCESS = MTB_PMBUS_HAL_STATUS_SUCCESS,
    /** The provided command code is incorrect */
    MTB_PMBUS_STATUS_INVALID_CMD_CODE = MTB_PMBUS_HAL_STATUS_INVALID_CMD_CODE,
    /** The provided page is incorrect */
    MTB_PMBUS_STATUS_INVALID_PAGE = MTB_PMBUS_HAL_STATUS_INVALID_PAGE,
    /** The provided phase is incorrect */
    MTB_PMBUS_STATUS_INVALID_PHASE = MTB_PMBUS_HAL_STATUS_INVALID_PHASE,
    /** One or more of input parameters are invalid */
    MTB_PMBUS_STATUS_BAD_PARAM = MTB_PMBUS_HAL_STATUS_BAD_PARAM,
    /** The requested size is bigger than command size */
    MTB_PMBUS_STATUS_TOO_MANY_BYTES = MTB_PMBUS_HAL_STATUS_TOO_MANY_BYTES,
    /** The requested action is aborted for the command as this command takes part in communication
     */
    MTB_PMBUS_STATUS_CMD_IS_ACTIVE = MTB_PMBUS_HAL_STATUS_CMD_IS_ACTIVE,
    /** The requested action is aborted as the PMBus instance takes part in communication */
    MTB_PMBUS_STATUS_IS_BUSY = MTB_PMBUS_HAL_STATUS_IS_BUSY
} mtb_pmbus_status_t;

#if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** The modes for SMBALERT pin
 * \note This enum is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile time
 * options.
 */
typedef enum
{
    /** The user must manually clear SMBALERT signal.
     * Typically, in the Alert Response Address callback function.
     */
    MTB_PMBUS_SMBALERT_MODE_MANUAL = 0U,
    /** The middleware automatically clears SMBALERT signal
     * after receiving Alert Response Address.
     */
    MTB_PMBUS_SMBALERT_MODE_AUTO   = 1U
} mtb_pmbus_smbalert_mode_t;
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined
       (MTB_PMBUS_DOXYGEN) */

/** General events for PMBus Middleware */
typedef enum
{
    /** Quick command event with Read bit */
    MTB_PMBUS_QUICK_CMD_RD_EVENT       = 0U,

    /** Quick command event with Write bit */
    MTB_PMBUS_QUICK_CMD_WR_EVENT       = 1U,

    /** Receive byte is requested by the controller.
     * Use this event to update data byte before sending.
     * This event is triggered in two cases: when the controller
     * requests the Receive byte or when the controller sends Quick
     * command with Read bit. Use \ref MTB_PMBUS_RECEIVE_BYTE_CMPLT_EVENT
     * to determine if the controller requests Receive byte or
     * Quick command is executed.
     */
    MTB_PMBUS_RECEIVE_BYTE_EVENT       = 2U,

    /** Receive byte protocol is executed */
    MTB_PMBUS_RECEIVE_BYTE_CMPLT_EVENT = 3U,

    /** The target successfully sent its own address to Host.
     * This event occurs only when SMBALERT signal is pulled down
     * by this Target Device. If SMBALERT mode is manual ( \ref MTB_PMBUS_SMBALERT_MODE_MANUAL),
     * use this event to stop pulling down of SMBALERT signal.
     */
    MTB_PMBUS_ALERT_RESPONSE_ADDR_EVENT = 4U
} mtb_pmbus_events_t;

/** Command specific events */
typedef enum
{
    /** Command match event. Inside this event, the application can
     * protect or disable the command. In this case, the middleware sends
     * NACK after receiving the command code. If the callback returns false
     * for this event, the middleware will also send a NACK for the command,
     * but the command will not be disabled for next transactions.
     */
    MTB_PMBUS_CMD_MATCH         = 0U,
    /** The controller sends a data byte. Using this event,
     * the application may decide whether ACK or NACK will be sent
     * after receiving the byte.
     */
    MTB_PMBUS_CMD_WRITE_BYTE    = 1U,
    /** Write is completed. The application can read the received data
     * inside the callback by \ref mtb_pmbus_cmd_read_data_isr
     * or \ref mtb_pmbus_cmd_read_data_ext_isr. For complex
     * protocols, like Group Protocol, it is recommended to apply the
     * received value only when the \ref MTB_PMBUS_CMD_DONE event occurs on
     * the bus.
     */
    MTB_PMBUS_CMD_WRITE_DONE    = 2U,
    /** Read data is requested. The application can update the data
     * before sending it to the controller inside this callback by
     * \ref mtb_pmbus_cmd_update_data_isr or \ref mtb_pmbus_cmd_update_data_ext_isr
     */
    MTB_PMBUS_CMD_READ_REQ      = 3U,
    /** Read is completed. This event does not mean that transfer is completed.
     * When complex protocols are initiated on the bus, like Zone Read, the completion
     * of read request does not mean that the controller completes the transfer, so
     * it is recommended to consider the data receiving event on the controller side
     * only when \ref MTB_PMBUS_CMD_DONE is occurred.
     */
    MTB_PMBUS_CMD_READ_DONE     = 4U,
    /** The transfer for command is completed, STOP event occurs on the Bus.
     * The application can perform any actions over a command including enabling or
     * disabling all commands. Before this event, the middleware clears the active transfer
     * status.
     */
    MTB_PMBUS_CMD_DONE          = 5U,
    /** The command arbitration is lost. This event occurs when the target
     * loses arbitration during a command transfer.
     */
    MTB_PMBUS_CMD_ARB_LOST      = 6U,
#if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** The controller sets the new active page for PAGE command.
     * The requested active page can be accepted or rejected by setting the return value for
     * callback:
     * 0U - NACK is sent after receiving the requested value,
     * 1U - ACK is sent after receiving the requested value.
     * \note This enum value is only available when \ref MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV
     * is enabled at compile time options.
     */
    MTB_PMBUS_CMD_PAGE          = 7U,
    /** The controller sets the new active page for PAGE_PLUS_* commands.
     * The requested active page can be accepted or rejected by setting the return value for
     * callback:
     * 0U - NACK is sent after receiving the requested value,
     * 1U - ACK is sent after receiving the requested value.
     * \note This enum value is only available when \ref MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV
     * is enabled at compile time options.
     */
    MTB_PMBUS_CMD_PAGE_PLUS     = 8U,
    /** The controller sets the new active page for P2_PLUS_* commands.
     * The requested active page can be accepted or rejected by setting the return value for
     * callback:
     * 0U - NACK is sent after receiving the requested value,
     * 1U - ACK is sent after receiving the requested value.
     * \note This enum value is only available when \ref MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV
     * is enabled at compile time options.
     */
    MTB_PMBUS_CMD_PAGE_P2_PLUS  = 9U,
#endif \
    /* #if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U)) ||
       defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U)) || \
    defined (MTB_PMBUS_DOXYGEN)
    /** The controller sets the new active phase for PHASE command.
     * The requested active phase can be accepted or rejected by setting the return value for
     * callback:
     * 0U - NACK is sent after receiving the requested value,
     * 1U - ACK is sent after receiving the requested value.
     * \note This enum value is only available when \ref MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV
     * is enabled at compile time options.
     */
    MTB_PMBUS_CMD_PHASE         = 10U,
    /** The controller sets the new active phase for P2_PLUS_* commands.
     * The requested active phase can be accepted or rejected by setting the return value for
     * callback:
     * 0U - NACK is sent after receiving the requested value,
     * 1U - ACK is sent after receiving the requested value.
     * \note This enum value is only available when \ref MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV
     * is enabled at compile time options.
     */
    MTB_PMBUS_CMD_PHASE_P2_PLUS = 11U
#endif \
    /* #if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U))
    || defined (MTB_PMBUS_DOXYGEN) */
} mtb_pmbus_cmd_events_t;

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** Zone events
 * \note This enum is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time
 * options.
 */
typedef enum
{
    /** Zone Read protocol is completed */
    MTB_PMBUS_ZONE_READ_STATUS_CMPLT    = 0U,
    /** The Status byte is requested */
    MTB_PMBUS_ZONE_READ_STATUS_BYTE_REQ = 1U,
    /** The Word byte is requested */
    MTB_PMBUS_ZONE_READ_STATUS_WORD_REQ = 2U,
#if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** The controller assigns the new read zone for the whole instance or specific page.
     * The requested zone value can be accepted or rejected by setting the return value
     * for the callback: 0U - NACK is sent after receiving the requested value, 1U - ACK is sent
     * after receiving the requested
     * value.
     * \note This enum value is only available when \ref MTB_PMBUS_ZONE_SEL_ADV is enabled at
     * compile time options.
     */
    MTB_PMBUS_ZONE_CONFIG_RD = 3U,
    /** The controller assigns a new write zone for the whole instance or specific page.
     * The requested zone value can be accepted or rejected by setting the return value
     * for callback: 0U - NACK is sent after receiving the the requested value, 1U - ACK is sent
     * after receiving the requested
     * value.
     * \note This enum value is only available when \ref MTB_PMBUS_ZONE_SEL_ADV is enabled at
     * compile time options.
     */
    MTB_PMBUS_ZONE_CONFIG_WR = 4U,
    /** The controller selects the new active read zone.
     * The requested zone value can be accepted or rejected by setting the return value
     * for callback: 0U - the NACK is sent after receiving the requested value, 1U - the ACK is sent
     * after receiving the requested
     * value.
     * \note This enum value is only available when \ref MTB_PMBUS_ZONE_SEL_ADV is enabled at
     * compile time options.
     */
    MTB_PMBUS_ZONE_ACTIVE_RD = 5U,
    /** The controller selects the new active write zone.
     * The requested zone value can be accepted or rejected by setting the return value
     * for callback: 0U - NACK is sent after receiving the requested value, 1U - ACK is sent after
     * receiving the requested
     * value.
     * \note This enum value is only available when \ref MTB_PMBUS_ZONE_SEL_ADV is enabled at
     * compile time options.
     */
    MTB_PMBUS_ZONE_ACTIVE_WR = 6U,
#endif /* #if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
    /** The ZONE_READ arbitration lost during reading Status  */
    MTB_PMBUS_ZONE_READ_STATUS_ARB_LOST = 7U
} mtb_pmbus_zone_events_t;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN) */

#if (defined(MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** Host Notify events
 * \note This enum is only available when \ref MTB_PMBUS_SUPPORT_HOST_NOTIFY is enabled at compile
 * time options.
 */
typedef enum
{
    /** The Host Notify transfer successful complete */
    MTB_PMBUS_HOST_NOTIFY_COMPLETE    = 0U,
    /** The arbitration is lost during Host Notify transfer */
    MTB_PMBUS_HOST_NOTIFY_ARB_LOST    = 1U,
    /** An error occurred during Host Notify transfer */
    MTB_PMBUS_HOST_NOTIFY_ERROR       = 2U
} mtb_pmbus_host_notify_events_t;
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) ||
       defined (MTB_PMBUS_DOXYGEN) */

/** I2C HW actions */
typedef enum
{
    /** Enable the I2C HW */
    MTB_PMBUS_HW_RESOURCES_ENABLE  = 0U,
    /** Disable the I2C HW */
    MTB_PMBUS_HW_RESOURCES_DISABLE = 1U
} mtb_pmbus_hw_resources_ctrl_action_t;

#if (defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** Revision of the PMBus specification
 * \note This enum is only available when \ref MTB_PMBUS_IMPL_CMD_REVISION is enabled at compile
 * time options.
 */
typedef enum
{
    /** PMBus revision 1.4 */
    MTB_PMBUS_REVISION_1_4 = 0U
} mtb_pmbus_revision_t;
#endif \
    /* #if (defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)) || defined
       (MTB_PMBUS_DOXYGEN) */

#if (defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** The Bus Speed
 * \note This enum is only available when \ref MTB_PMBUS_IMPL_CMD_CAPABILITY is enabled at compile
 * time options.
 */
typedef enum
{
    /** The maximum supported bus speed is 100 kHz */
    MTB_PMBUS_SPEED_100  = 0U,
    /** The maximum supported bus speed is 400 kHz */
    MTB_PMBUS_SPEED_400  = 1U,
    /** The maximum supported bus speed is 1 MHz */
    MTB_PMBUS_SPEED_1000 = 2U
} mtb_pmbus_speed_t;
#endif \
    /* #if (defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)) ||
       defined (MTB_PMBUS_DOXYGEN) */
/** \} group_pmbus_trgt_enums */

/**
 * \addtogroup group_pmbus_trgt_data_structs
 * \{
 */

/** Provides the typedef for the callback function called in ISR to
 * notify the user about occurrences of \ref mtb_pmbus_events_t
 */
typedef void (*mtb_pmbus_handle_events_t)(mtb_pmbus_events_t event);

/** Provides the typedef for the callback function called in ISR to
 * notify the user about the occurrences of error events
 */
typedef void (*mtb_pmbus_handle_error_events_t)(uint32_t events, uint8_t cmd_code, bool cmd_is_ext);

#if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U))
/** Provides the typedef for the callback function with an extended set of parameters called in ISR
   to
 * notify the user about occurrences of \ref mtb_pmbus_events_t
 */
typedef bool (*mtb_pmbus_handle_cmd_events_ext_t)(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase,
                                                  uint8_t byte, uint8_t idx);
#else
/** Provides the typedef for the callback function called in ISR to
 * notify the user about occurrences of \ref mtb_pmbus_events_t
 */
typedef bool (*mtb_pmbus_handle_cmd_events_t)(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
#endif /* #if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U)) */

#if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** Provides the typedef for the callback function called in ISR to
 * notify the user about occurrences of \ref mtb_pmbus_cmd_events_t related to PAGE commands.
 * The requested active page can be accepted or rejected by setting the return value for callback:
 * 0U - NACK is sent after receiving the requested value,
 * 1U - ACK is sent after receiving the requested value.
 */
typedef uint8_t (*mtb_pmbus_handle_cmd_page_events_t)(mtb_pmbus_cmd_events_t event, bool is_write, int32_t page);
#endif \
    /* #if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U)) ||
       defined (MTB_PMBUS_DOXYGEN) */

#if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U)) || \
    defined (MTB_PMBUS_DOXYGEN)
/** Provides the typedef for the callback function called in ISR to
 * notify the user about occurrences of \ref mtb_pmbus_cmd_events_t related to PHASE commands.
 * The requested active phase can be accepted or rejected by setting the return value for callback:
 * 0U - NACK is sent after receiving the requested value,
 * 1U - ACK is sent after receiving the requested value.
 */
typedef uint8_t (*mtb_pmbus_handle_cmd_phase_events_t)(mtb_pmbus_cmd_events_t event, bool is_write, int32_t phase);
#endif \
    /* #if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U))
    || defined (MTB_PMBUS_DOXYGEN) */

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** Provides the typedef for the callback function called in ISR to
 * notify the user about occurrences of \ref mtb_pmbus_zone_events_t
 */
typedef uint8_t (*mtb_pmbus_handle_zone_events_t)(mtb_pmbus_zone_events_t event, uint8_t data_byte, int32_t page);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN) */

#if (defined(MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** Provides the typedef for the callback function called in ISR to
 * notify the user about occurrences of \ref mtb_pmbus_host_notify_events_t
 */
typedef void (*mtb_pmbus_handle_host_notify_t)(mtb_pmbus_host_notify_events_t event);
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) ||
       defined (MTB_PMBUS_DOXYGEN) */

/** Provides the typedef for the callback function called in
 * \ref mtb_pmbus_enable and \ref mtb_pmbus_disable to perform
 * enabling/disabling HW resources.
 */
typedef void (*mtb_pmbus_hw_resources_ctrl_t)(mtb_pmbus_hw_resources_ctrl_action_t action);

/** Provides the typedef for the callback function called inside
 * the middleware to enable/disable PMBus related interrupts.
 */
typedef void (*mtb_pmbus_hw_isr_ctrl_t)(void);

/** Command configuration structure */
typedef struct
{
    /** Command code */
    uint8_t cmd_code;
    /** Command capabilities, see \ref group_pmbus_macro_cmd_cap */
    uint16_t cmd_cap;
    /** The pointer to command data buffer */
    void *data_buf;
    /** The command data size */
    uint8_t data_size;
    /** Command callback */
#if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U))
    mtb_pmbus_handle_cmd_events_ext_t callback;
#else
    mtb_pmbus_handle_cmd_events_t callback;
#endif /* #if (defined(MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) && (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV != 0U)) */
} mtb_pmbus_stc_config_cmd_t;

/** Hardware configuration structure */
typedef struct
{
    /** The pointer to HAL configuration structure, see \ref mtb_pmbus_stc_config_hal_t */
    mtb_pmbus_stc_config_hal_t *hal_config;
    /** The pointer to the user callback for enabling/disabling the I2C Hardware */
    mtb_pmbus_hw_resources_ctrl_t hw_resource_ctrl_callback;
    /** The pointer to the user callback for enabling I2C HW interrupts */
    mtb_pmbus_hw_isr_ctrl_t enable_hw_irq_callback;
    /** The pointer to the user callback for disabling I2C HW interrupts */
    mtb_pmbus_hw_isr_ctrl_t disable_hw_irq_callback;
} mtb_pmbus_stc_config_hw_t;

/** Configuration structure */
typedef struct
{
    /** Pointer to hardware configuration structure */
    mtb_pmbus_stc_config_hw_t *hw_config;

    /** Target address */
    uint8_t address;

#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable PEC support.
     * - With PEC enabled, the middleware can handle the protocols with or
     * without PEC.
     * \note If the received and calculated PEC bytes do not match, the
     * middleware sends NACK to PEC byte
     * - With PEC disabled, the middleware supports only protocols without PEC.
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_PEC is enabled at compile time
     * options.
     */
    bool enable_pec;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable SMBALERT signal
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile
     * time options.
     */
    bool enable_smbalert;
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined
       (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable General Call address handling
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_GEN_CALL_ADDR is enabled at
     * compile time options.
     */
    bool enable_gen_call_addr;
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)) ||
       defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable PMBus support. If this option is disabled, only SMBUS features are supported
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_PMBUS is enabled at compile
     * time options.
     */
    bool enable_pmbus;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** The total number of pages. If set to 0, the pages are not supported
     * \note This field is only available when \ref MTB_PMBUS_PAGES_NUM is greater than 0U at
     * compile time options.
     */
    uint8_t num_pages;
#if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** The pointer to the user callback for page-related commands
     * \note This field is only available when \ref MTB_PMBUS_PAGES_NUM is enabled at compile time
     * options.
     */
    mtb_pmbus_handle_cmd_page_events_t page_callback;
#endif \
    /* #if (defined(MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV != 0U)) ||
       defined (MTB_PMBUS_DOXYGEN) */
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** The total number of phases. If set to 0, the phases are not supported
     * \note This field is only available when \ref MTB_PMBUS_PHASES_NUM is greater than 0U at
     * compile time options.
     */
    uint8_t num_phases;
#if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U)) || \
    defined (MTB_PMBUS_DOXYGEN)
    /** The pointer to the user callback for phase-related commands
     * \note This field is only available when \ref MTB_PMBUS_PHASES_NUM is enabled at compile time
     * options.
     */
    mtb_pmbus_handle_cmd_phase_events_t phase_callback;
#endif \
    /* #if (defined(MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) && (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV != 0U))
    || defined (MTB_PMBUS_DOXYGEN) */
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable Zone Write and Zone Read protocols
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile
     * time options.
     */
    bool enable_zone;

    /** The pointer to the user callback for zone protocol
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile
     * time options.
     */
    mtb_pmbus_handle_zone_events_t zone_callback;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN) */

#if (MTB_PMBUS_IMPL_CMD_NUM != 0U) || defined (MTB_PMBUS_DOXYGEN)
    /** List of implemented commands
     *
     * Use this field to define the list of the implemented commands
     * \ref group_pmbus_macro_impl_cmd
     * \note This field is only available when at least one preimplemented command is enabled at
     * compile time options.
     */
    uint32_t impl_cmd_mask;
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) || defined (MTB_PMBUS_DOXYGEN) */
    /** The pointer to commands table */
    mtb_pmbus_stc_config_cmd_t *cmd_table;
    /** Number of defined commands in commands table  */
    uint8_t cmd_num;

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable extended commands support
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_EXT_CMD is enabled at compile
     * time options.
     */
    bool enable_ext_cmd;
    /** Pointer to commands table of extended commands
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_EXT_CMD is enabled at compile
     * time options.
     */
    mtb_pmbus_stc_config_cmd_t *ext_cmd_table;
    /** Number of defined commands in extended commands table
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_EXT_CMD is enabled at compile
     * time options.
     */
    uint8_t ext_cmd_num;
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) || defined
       (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** The pointer to the user callback for host notify events */
    mtb_pmbus_handle_host_notify_t hnp_callback;
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) ||
       defined (MTB_PMBUS_DOXYGEN) */
    /** The pointer to the user callback for general events */
    mtb_pmbus_handle_events_t gen_callback;

    /** The pointer to the user callback for error events */
    mtb_pmbus_handle_error_events_t errors_callback;

#if (defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Revision of PMBus specification
     * \note This field is only available when \ref MTB_PMBUS_IMPL_CMD_REVISION is enabled at
     * compile time options.
     */
    mtb_pmbus_revision_t revision;
#endif \
    /* #if (defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)) || defined
       (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** The maximum supported speed
     * \note This field is only available when \ref MTB_PMBUS_IMPL_CMD_CAPABILITY is enabled at
     * compile time options.
     */
    mtb_pmbus_speed_t speed;
    /** If enabled the Numeric data is in IEEE Half Precision
     * Floating Point Format, otherwise in LINEAR11, ULINEAR16 or
     * DIRECT formats
     * \note This field is only available when \ref MTB_PMBUS_IMPL_CMD_CAPABILITY is enabled at
     * compile time options.
     */
    bool enable_ieee_format;
#endif \
    /* #if (defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)) ||
       defined (MTB_PMBUS_DOXYGEN) */
} mtb_pmbus_stc_config_t;

/** \cond INTERNAL */
typedef struct
{
    /** Command position in command table */
    uint8_t cmd_pos;
    /** Command flags */
    uint8_t flags;
} mtb_pmbus_cmd_lookup_tbl_t;
/** \endcond*/

/** Instance structure */
typedef struct
{
    /** \cond INTERNAL */
    mtb_pmbus_stc_config_t const *cfg;
    mtb_pmbus_cmd_lookup_tbl_t cmd_lookup_tbl[MTB_PMBUS_CMD_MAX_NUM];
#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
    mtb_pmbus_cmd_lookup_tbl_t ext_cmd_lookup_tbl[MTB_PMBUS_CMD_MAX_NUM];
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
    /*  Command table and look up table for implemented commands */
#if (MTB_PMBUS_IMPL_CMD_NUM > 0U)
    mtb_pmbus_stc_config_cmd_t pre_impl_cmd_table[MTB_PMBUS_IMPL_CMD_NUM];
    mtb_pmbus_cmd_lookup_tbl_t pre_impl_cmd_lookup_tbl[MTB_PMBUS_IMPL_CMD_NUM];
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM > 0U) */
    uint32_t state;
    uint32_t errors;
    uint16_t byte_received;
    uint16_t byte_requested; /* Applicable only for Block commands */
    uint16_t byte_sent;
    uint16_t byte_to_send;
    uint8_t cmd_pos;
    uint8_t cmd_code;
    uint16_t cmd_code_full;
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
    uint8_t crc;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
    /* Add additional byte for Block commands and 2 additional bytes for Zone Read */
    uint8_t int_buff[MTB_PMBUS_CMD_MAX_SIZE + MTB_PMBUS_BLOCK_COUNT_BYTE_SIZE + MTB_PMBUS_ZONE_READ_PAGE_STATUS_SIZE];
#else /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
    /* Add additional byte for Block commands */
    uint8_t int_buff[MTB_PMBUS_CMD_MAX_SIZE + MTB_PMBUS_BLOCK_COUNT_BYTE_SIZE];
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM > 0U))
    uint8_t act_page;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM > 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM > 0U))
    uint8_t act_phase;
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM > 0U)) */
#if ((defined(MTB_PMBUS_IMPL_CMD_PX_PLUS) && (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U)) || (defined(MTB_PMBUS_SUPPORT_ZONE) && \
    (MTB_PMBUS_SUPPORT_ZONE != 0U)))
    uint8_t stored_page;
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_PX_PLUS) && (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U)) */
#if (defined(MTB_PMBUS_IMPL_CMD_P2_PLUS) && (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U))
    uint8_t stored_phase;
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_P2_PLUS) && (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U)) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
    uint8_t read_zones[MTB_PMBUS_PAGES_NUM];
    uint8_t write_zones[MTB_PMBUS_PAGES_NUM];
#else /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
    uint8_t read_zones[1U];
    uint8_t write_zones[1U];
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
    uint8_t act_read_zone;
    uint8_t act_write_zone;
    uint8_t cmd_ctrl_byte;
    uint8_t zone_sts_mask;
    uint8_t zone_pages;
    uint8_t num_available_zone;
    bool zone_is_read;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
    bool    is_cmd_block;
    bool    is_pause_allowed;
#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U))
    bool    ext_cmd_en;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */
    mtb_pmbus_stc_config_cmd_t *active_cmd_tbl;
    mtb_pmbus_cmd_lookup_tbl_t *active_lookup_tbl;
    /* Byte for Received byte protocol */
    uint8_t received_byte;
#if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U))
    mtb_pmbus_smbalert_mode_t smbalert_mode;
    bool smbalert_is_trig;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) */
    /** \endcond */
} mtb_pmbus_stc_t;

/** \} group_pmbus_trgt_data_structs */

/**
 * \addtogroup group_pmbus_trgt_functions
 * \{
 */

/**
 * \defgroup group_pmbus_init_func Initialization Functions
 * \{
 */

/**
 * @brief Initialize the PMBus Middleware
 *
 * @param inst  The pointer to the PMBus instance structure
 * @param config The pointer to PMBus configuration structure
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_init(mtb_pmbus_stc_t *inst, mtb_pmbus_stc_config_t const *config);

/**
 * @brief Enable PMBus Middleware
 *
 * After calling of this function Middleware starts to response on Controller transfers
 *
 * @param inst The pointer to the PMBus instance structure
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_enable(mtb_pmbus_stc_t *inst);

/**
 * @brief Disable PMBus Middleware
 *
 * After calling of this function Middleware stops to response on Controller transfers
 *
 * @param inst The pointer to the PMBus instance structure
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_disable(mtb_pmbus_stc_t *inst);

/** \} group_pmbus_init_func */

/**
 * \defgroup group_pmbus_cmd_func Command Handler Functions
 * A variety of functions are provided to work with commands:
 * - Update or read the command data.
 * - Retrieve the last transfer size (applicable for Block protocols).
 * - Enable or disable commands.
 * - Protect commands against write operations.
 *
 * In this section, functions with the `isr` suffix are optimized for use during ISRs,
 * typically in command callback functions. APIs without the `isr` suffix return the
 * \ref MTB_PMBUS_STATUS_CMD_IS_ACTIVE status if the selected command is participating
 * in an active transfer.
 * To access data for specific pages or phases of commands, use APIs with the `ext` suffix.
 * \{
 */

/**
 * @brief Update the data buffer for the selected command with new data.
 *
 * For the Block Read and Block Write-Block Read Process Call protocols:
 * If the updated data size is smaller than the command size in the command
 * table, only updated bytes will be transferred.
 *
 * This function always updates data from the 0 position.
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * The function prevents updating the data content if the command is active, to avoid data
 * corruption. Therefore, using this function within a PMBus callback (executed inside an ISR)
 * will have no effect.
 *
 * Use this function only for not paged/phased commands.
 *
 * \warning Updating the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is updated
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and update order.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 *
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_update_data(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *data, uint32_t data_size);

/**
 * @brief Update the data buffer for the selected command with new data.
 *
 * This function is optimized for use during ISR and is recommended to be used in a command
 * callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * For the Block Read and Block Write-Block Read Process Call protocols:
 * If the updated data size is smaller than the command size in the command
 * table, only the updated bytes will be transferred.
 *
 * This function always updates data from position 0.
 *
 * Use this function only for non-paged/non-phased commands.
 *
 * \warning The updating data for active command can lead to data corruption.
 * \warning Updating the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is updated
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and update order.
 *
 * @param inst      Pointer to the PMBus instance structure
 * @param code      Command code
 * @param data      Pointer to the data array
 * @param data_size Size of the data array
 */
void mtb_pmbus_cmd_update_data_isr(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *data, uint32_t data_size);

/**
 * @brief Update the data buffer for the selected command, page and phase with the new data.
 *
 * For the Block Read and Block Write-Block Read Process Call protocols:
 * If the updated data size is smaller than the command size in the command
 * table, only updated bytes will be transferred.
 *
 * This function always updates data from the 0 position.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * \warning Updating the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is updated
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and update order.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page  The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_update_data_ext(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page, int32_t phase,
                                                 uint8_t *data, uint32_t data_size);

/**
 * @brief Update the data buffer for the selected command, page and phase with the new data.
 *
 * This function is optimized for use during ISR and is recommended to be used in a command
 * callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * For the Block Read and Block Write-Block Read Process Call protocols:
 * If the updated data size is smaller than the command size in the command
 * table, only updated bytes will be transferred.
 *
 * This function always update data from the 0 position.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * \warning The updating data for active command can lead to data corruption.
 * \warning Updating the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is updated
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and update order.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page  The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 */
void mtb_pmbus_cmd_update_data_ext_isr(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page, int32_t phase, uint8_t *data,
                                       uint32_t data_size);

/**
 * @brief Read the data from the internal buffer for selected command
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * The function prevents reading the data content if the command is active, to avoid data
 * corruption. Therefore, using this function within a PMBus callback (executed inside an ISR)
 * will have no effect.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * \warning Reading the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is accessed
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and access order.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_read_data(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *data, uint32_t data_size);

/**
 * @brief Read the data from the internal buffer for selected command
 *
 * This function is optimized for use during ISR and is recommended to be used in a command
 * callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * \warning The returned data for command which take part in active transfer can be
 * invalid.
 * \warning Reading the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is accessed
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and access order.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 */
void mtb_pmbus_cmd_read_data_isr(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *data, uint32_t data_size);

/**
 * @brief Read the data from the buffer for the selected command, page and phase.
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * The function prevents reading the data content if the command is active, to avoid data
 * corruption. Therefore, using this function within a PMBus callback (executed inside an ISR)
 * will have no effect.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * \warning Reading the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is accessed
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and access order.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_read_data_ext(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page, int32_t phase,
                                               uint8_t *data, uint32_t data_size);

/**
 * @brief Read the data from the buffer for the selected command, page and phase.
 *
 * This function is optimized for use during ISR and is recommended to be used in a command
 * callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * \warning The returned data for command which take part in active transfer can be
 * invalid.
 * \warning Reading the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is accessed
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and access order.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page  The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 */
void mtb_pmbus_cmd_read_data_ext_isr(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page, int32_t phase, uint8_t *data,
                                     uint32_t data_size);

/**
 * @brief Returns the number of bytes which is transferred during last transaction.
 *
 * For majority of protocol the size of read/write data equal to command size,
 * but for the Block Write-Block Read Process Call, Block Write, Block Read protocols,
 * the size of transfer can be smaller than the maximum size of command in command table.
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * The function prevents reading the transfer size if the command is active, to avoid data
 * corruption. Therefore, using this function within a PMBus callback (executed inside an ISR)
 * will have no effect.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param size The size of the last transaction
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_get_transfer_size(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *size);

/**
 * @brief Returns the number of bytes which is transferred during last transaction.
 *
 * This function is optimized for use during ISR and is recommended to be used in a command
 * callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * For the majority of protocols, the size of read/write data is equal to the command size,
 * but for the Block Write-Block Read Process Call, Block Write, Block Read protocols,
 * the size of transfer can be smaller than the maximum size of a command in the command table.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * \warning The returned transfer size for command which takes part in active transfer can be
 * invalid.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param size The size of the last transaction
 */
void mtb_pmbus_cmd_get_transfer_size_isr(mtb_pmbus_stc_t *inst, uint32_t code, uint8_t *size);

/**
 * @brief Returns the number of bytes which is transferred during last transaction.
 *
 * For the majority of protocols, the size of read/write data is equal to the command size,
 * but for the Block Write-Block Read Process Call, Block Write, Block Read protocols,
 * the size of transfer can be smaller than the maximum size of command in command table.
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * The function prevents reading the transfer size if the command is active, to avoid data
 * corruption. Therefore, using this function within a PMBus callback (executed inside an ISR)
 * will have no effect.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page  The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param size The pointer to transfer size
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_get_transfer_size_ext(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page,
                                                       int32_t phase, uint8_t *size);

/**
 * @brief Returns the number of bytes which is transferred during last transaction.
 *
 * This function is optimized for use during ISR and is recommended to be used in a command
 * callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * For the majority of protocols, the size of read/write data is equal to the command size,
 * but for the Block Write-Block Read Process Call, Block Write, Block Read protocols,
 * the size of transfer can be smaller than the maximum size of a command in the command table.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * \warning The returned transfer size for command which takes part in active transfer can be
 * invalid.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param size The pointer to transfer size
 */
void mtb_pmbus_cmd_get_transfer_size_ext_isr(mtb_pmbus_stc_t *inst, uint32_t code, int32_t page, int32_t phase,
                                             uint8_t *size);

/**
 * @brief Returns true if command participate in current transfer, otherwise returns false
 *
 * This function is optimized for use during ISR and is recommended to be used in a command
 * callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param status True if command is active, otherwise false
 */
void mtb_pmbus_cmd_is_active_isr(mtb_pmbus_stc_t *inst, uint32_t code, bool *status);

/**
 * @brief Returns true if command participate in current transfer, otherwise returns false
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param status True if command is active, otherwise false
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_is_active(mtb_pmbus_stc_t *inst, uint32_t code, bool *status);

/**
 * @brief Enable/Disable command. If the command is enabled, the PMBus Middleware sends
 * ACK bit after receiving the command code and transmits or receives data, otherwise the PMBus
 * sends
 * NACK after receiving the command code.
 *
 * The function can be used to enable/disable commands from both command tables:
 * main ( \ref mtb_pmbus_stc_config_t::cmd_table) and extended ( \ref
 * mtb_pmbus_stc_config_t::ext_cmd_table).
 *
 * To disable the extended command, use \ref mtb_pmbus_cmd_ext_enable_disable.
 *
 * This function is optimized for use during ISR and is recommended to be used in a command
 * callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * \warning Enabling/Disabling the command which take part in active transfer can lead
 * to unexpected behavior.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param status Set to true to enable command, set to false to disable command
 */
void mtb_pmbus_cmd_enable_disable_isr(mtb_pmbus_stc_t *inst, uint32_t code, bool status);

/**
 * @brief Enable/Disable command. If the command is enabled, the PMBus Middleware sends
 * ACK bit after receiving the command code and transmits or receives data, otherwise the PMBus
 * sends
 * NACK after receiving the command code.
 *
 * The function can be used to enable/disable commands from both command tables:
 * main ( \ref mtb_pmbus_stc_config_t::cmd_table) and extended ( \ref
 * mtb_pmbus_stc_config_t::ext_cmd_table).
 *
 * To disable the extended command, use \ref mtb_pmbus_cmd_ext_enable_disable.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param status Set to true to enable command, set to false to disable command
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_enable_disable(mtb_pmbus_stc_t *inst, uint32_t code, bool status);

#if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief Enable/Disable the extended command. If the command is enabled, the PMBus middleware sends
 * ACK bit after receiving the command code and transmits or receives data, otherwise the PMBus
 * sends
 * NACK after receiving the command code.
 *
 * When the extended command is disabled, none commands from the extended table will be transmitted.
 *
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_EXT_CMD is enabled at compile
 * time options.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param status Set to true to enable the command, set to false to disable the command
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_ext_enable_disable(mtb_pmbus_stc_t *inst, bool status);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_EXT_CMD) && (MTB_PMBUS_SUPPORT_EXT_CMD != 0U)) */

/**
 * @brief Enable/Disable all commands from both command tables: main ( \ref
 * mtb_pmbus_stc_config_t::cmd_table)
 * and extended ( \ref mtb_pmbus_stc_config_t::ext_cmd_table). If the command is enabled, the PMBus
 * middleware
 * sends ACK bit after receiving the command code and transmits or receives data, otherwise the
 * PMBus sends
 * NACK after receiving the command code.
 *
 * The function prevents enabling/disabling the commands if PMBus instance takes
 * part in communication.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param status Set to true to enable all the commands, set to false to disable all the commands
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_all_enable_disable(mtb_pmbus_stc_t *inst, bool status);

/**
 * @brief Returns true if a command is executed at least one time.
 *
 * Returns true if a command is executed at least one time (The controller executes transaction
 * for this command and transaction is completed). Returns false if a command never executed after
 * enabling of middleware or after the last calling of \ref mtb_pmbus_cmd_get_status() for this
 * command.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param status Status of command execution
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_get_status(mtb_pmbus_stc_t *inst, uint32_t code, bool *status);

/**
 * @brief Enable or disable write protection for a specific command.
 *
 * When write protection is enabled (status = true), the PMBus middleware
 * will NACK any write attempts to the specified command code.
 * When disabled (status = false), normal write access is allowed.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * \warning Enabling/Disabling write protection for the command which take part in active transfer
 * can lead
 * to unexpected behavior.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code for target command
 * @param status Set to true to enable write protection, false to disable
 */
void mtb_pmbus_cmd_wr_protect_isr(mtb_pmbus_stc_t *inst, uint32_t code, bool status);

/**
 * @brief Enable or disable write protection for a specific command.
 *
 * When write protection is enabled (status = true), the PMBus middleware
 * will NACK any write attempts to the specified command code.
 * When disabled (status = false), normal write access is allowed.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code for target command
 * @param status Set to true to enable write protection, false to disable
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_wr_protect(mtb_pmbus_stc_t *inst, uint32_t code, bool status);

/**
 * @brief Enables or disables write protection for all PMBus commands.
 *
 * This function sets the write protection status for all commands in the PMBus instance.
 *
 * The function prevents enabling/disabling the commands protection if PMBus instance takes
 * part in communication.
 *
 * @param inst   The pointer to the PMBus instance structure.
 * @param status Set to true to enable write protection, false to disable.
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_all_wr_protect(mtb_pmbus_stc_t *inst, bool status);

/**
 * @brief Requests to pause the Process Call protocol execution and exit from the ISR handler.
 *
 * This function must be called from within the command callback during the MTB_PMBUS_CMD_WRITE_DONE
 * event for Process Call protocols. It allows the application to defer the read response phase,
 * enabling time-consuming processing outside the ISR context.
 *
 * When called during the allowed window (\ref MTB_PMBUS_CMD_WRITE_DONE event), the function pauses
 * the I2C hardware transmission.
 * The middleware will suspend the transaction after the write phase completes, allowing the
 * application
 * to process the received data and prepare the response outside of ISR context. After processing,
 * the application must call \ref mtb_pmbus_cmd_complete_transfer() to resume and complete the
 * transaction.
 *
 * Usage pattern:
 * 1. Controller initiates Process Call write phase
 * 2. In MTB_PMBUS_CMD_WRITE_DONE callback, call mtb_pmbus_cmd_exit_isr()
 * 3. Exit ISR and process received data in main application context
 * 4. Update response data buffer
 * 5. Call \ref mtb_pmbus_cmd_complete_transfer() to resume and send response
 *
 * @param inst The pointer to the PMBus instance structure
 *
 * @return \ref mtb_pmbus_status_t Status of the operation:
 *         - MTB_PMBUS_STATUS_SUCCESS: Pause request was accepted and hardware transmission paused
 * successfully
 *         - MTB_PMBUS_STATUS_BAD_PARAM: Function was called outside the allowed context
 *           (not during MTB_PMBUS_CMD_WRITE_DONE event for Process Call) or hardware pause
 * operation failed
 *
 * @note This function should only be used with Process Call protocols
 * @note Must be called from within the MTB_PMBUS_CMD_WRITE_DONE event callback
 * @note If a command supports both write and process call capabilities
 *       (MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL), the user can call
 *       \ref mtb_pmbus_cmd_process_call_wr_done() function to determine whether the current
 *       MTB_PMBUS_CMD_WRITE_DONE event occurred for a Process Call transaction
 */
mtb_pmbus_status_t mtb_pmbus_cmd_exit_isr(mtb_pmbus_stc_t *inst);

/**
 * @brief Resumes a paused Process Call protocol transaction.
 *
 * This function must be called outside of ISR context after calling \ref mtb_pmbus_cmd_exit_isr()
 * to complete a Process Call protocol transaction. After the write phase completes and the
 * application processes the received data, this function resumes the I2C transaction and
 * sends the response data to the controller.
 *
 * To apply this function to an extended command, put parameter code into macros \ref
 * MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT.
 *
 * Usage pattern:
 * 1. In MTB_PMBUS_CMD_WRITE_DONE callback, call \ref mtb_pmbus_cmd_exit_isr() to pause the
 * transaction
 * 2. Exit ISR and process received data in the main application context
 * 3. Update the command buffer with response data
 * 4. Call mtb_pmbus_cmd_complete_transfer() to resume and complete the transaction
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code Command code of the paused Process Call command
 *
 * @return \ref mtb_pmbus_status_t Status of the operation:
 *         - MTB_PMBUS_STATUS_SUCCESS: Transaction resumed and completed successfully
 *         - MTB_PMBUS_STATUS_BAD_PARAM: The specified command is not in paused state
 *           (either not a Process Call or \ref mtb_pmbus_cmd_exit_isr() was not called)
 *
 * @note This function must be called outside of ISR/callback context
 * @note This function should only be used with Process Call protocols
 */
mtb_pmbus_status_t mtb_pmbus_cmd_complete_transfer(mtb_pmbus_stc_t *inst, uint32_t code);

/**
 * @brief Determines if the current MTB_PMBUS_CMD_WRITE_DONE event is for a Process Call protocol.
 *
 * This function checks whether the current MTB_PMBUS_CMD_WRITE_DONE event occurred
 * for a Process Call transaction. It is useful when a command supports both standard
 * write and Process Call capabilities (MTB_PMBUS_CMD_CAP_DIR_WR |
 * MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL),
 * allowing the application to distinguish between the two operation types.
 *
 * When a command supports both capabilities, the WRITE_DONE callback will be triggered
 * for both write and Process Call operations. This function returns true only when
 * the event is for a Process Call, indicating that:
 * - The pause mechanism is available (mtb_pmbus_cmd_exit_isr() can be called)
 * - A read phase will follow to send response data back to the controller
 *
 * @param inst The pointer to the PMBus instance structure
 * @param status Pointer to a boolean variable that will be updated with the result:
 *               - true: Current WRITE_DONE event is for a Process Call protocol
 *               - false: Current WRITE_DONE event is for a standard write operation
 *
 * @return @ref mtb_pmbus_status_t Status of the operation:
 *         - MTB_PMBUS_STATUS_SUCCESS: Status retrieved successfully
 *         - MTB_PMBUS_STATUS_BAD_PARAM: Invalid parameter (status pointer is NULL)
 *
 * @note This function should be called from within the MTB_PMBUS_CMD_WRITE_DONE callback
 */
mtb_pmbus_status_t mtb_pmbus_cmd_process_call_wr_done(mtb_pmbus_stc_t *inst, bool *status);

/** \} group_pmbus_cmd_func */

/**
 * \defgroup group_pmbus_zone_func Zone Functions
 * \{
 */

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief Returns assigned Read Zone to this target device
 *
 * If the instance does not support pages, then the @p page parameter is ignored
 * and the function returns the global zones assigned to this instance.
 *
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time
 * options.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param zone The pointer to variable for storing the Read Zone
 * @param page The page number for zone assignment
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_get_read_zone(mtb_pmbus_stc_t *inst, uint32_t *zone, uint32_t page);

/**
 * @brief Returns assigned Write Zone to this target device
 *
 * If the instance does not support pages, then the @p page parameter is ignored
 * and the function returns the global zones assigned to this instance.
 *
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time
 * options.
 *
 * @param inst  The pointer to the PMBus instance structure
 * @param zone The pointer to variable for storing the Write Zone
 * @param page The page number for zone assignment
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_get_write_zone(mtb_pmbus_stc_t *inst, uint32_t *zone, uint32_t page);

/**
 * @brief Returns the active zones
 *
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time
 * options.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param zone_read The pointer to variable for storing the active Read Zone
 * @param zone_write The pointer to variable for storing the active Write Zone
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_get_active_zones(mtb_pmbus_stc_t *inst, uint32_t *zone_read, uint32_t *zone_write);

/**
 * @brief Sets the default zones for the device. Use this function to change the default zones
 * during the initialization of the device: after \ref mtb_pmbus_init() and before \ref
 * mtb_pmbus_enable().
 *
 * By default the 0xFE is assigned to Read and Write Zones during initialization stage.
 *
 * If instance does not support pages, then the @p page parameter value is ignored
 * and zones are assigned globally for instance.
 *
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time
 * options.
 *
 * \warning This function does not check for invalid zone values. This is done for flexibility
 * purposes,
 * as there are several ranges of zone values.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param zone_read The Read Zone to assign to this device
 * @param zone_write The Write Zone to assign to this device
 * @param page The page number for zone assignment
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_set_default_zones(mtb_pmbus_stc_t *inst, uint32_t zone_read, uint32_t zone_write,
                                               uint32_t page);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN) */

/** \} group_pmbus_zone_func */

/**
 * \defgroup group_pmbus_page_phase_func Page/Phase Functions
 * \{
 */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM > 0U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief Returns the active page number
 *
 * \note This function is only available when \ref MTB_PMBUS_PAGES_NUM is greater than 0U at compile
 * time options.
 *
 * @param inst The pointer to the PMBus instance structure
 * @return uint8_t The active page number
 */
uint8_t mtb_pmbus_get_active_page(mtb_pmbus_stc_t *inst);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM > 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM > 0U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief Returns the active phase number
 *
 * \note This function is only available when \ref MTB_PMBUS_PHASES_NUM is greater than 0U at
 * compile time options.
 *
 * @param inst The pointer to the PMBus instance structure
 * @return uint8_t The active phase number
 */
uint8_t mtb_pmbus_get_active_phase(mtb_pmbus_stc_t *inst);
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM > 0U)) || defined (MTB_PMBUS_DOXYGEN) */
/** \} group_pmbus_page_phase_func */

/**
 * \defgroup group_pmbus_smbalert_func SMBALERT Functions
 * \{
 */

#if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief Sets the mode for SMBALERT pin
 *
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile
 * time options.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param mode The selected mode, \ref mtb_pmbus_smbalert_mode_t
 */
void mtb_pmbus_smbalert_config_mode(mtb_pmbus_stc_t *inst, mtb_pmbus_smbalert_mode_t mode);

/**
 * @brief Set low level for SMBALERT pin.
 *
 * Once the SMBALERT signal sets to 0, the Middleware starts to response on
 * Alert Response Address.
 *
 * This function does nothing if \ref mtb_pmbus_stc_config_t::enable_smbalert
 * is set to NULL.
 *
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile
 * time options.
 *
 * @param inst The pointer to the PMBus instance structure
 */
void mtb_pmbus_smbalert_set_signal(mtb_pmbus_stc_t *inst);

/**
 * @brief Set hign level for SMBALERT pin
 *
 * This function does nothing if \ref mtb_pmbus_stc_config_t::enable_smbalert
 * is set to NULL.
 *
 * The middleware stops response on Alert Response Address once this function is
 * called. If the SMBALERT mode is \ref MTB_PMBUS_SMBALERT_MODE_AUTO, the middleware
 * automatic clears the SMBALERT signal, otherwise clear the signal
 * manually in \ref mtb_pmbus_handle_events_t callback when \ref MTB_PMBUS_ALERT_RESPONSE_ADDR_EVENT
 * is triggered.
 *
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile
 * time options.
 *
 * @param inst The pointer to the PMBus instance structure
 */
void mtb_pmbus_smbalert_clear_signal(mtb_pmbus_stc_t *inst);
#endif \
    /* #if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined
       (MTB_PMBUS_DOXYGEN) */
/** \} group_pmbus_smbalert_func */

/**
 * \defgroup group_pmbus_miscellaneous_func Miscellaneous Functions
 * \{
 */

/**
 * @brief Set the data byte for the Receive Byte protocol. The middleware will
 * automatically send this byte when it is requested by the controller.
 *
 * \note If the Quick Command with Read direction and Received Byte protocols
 * must be used in the same application, set the first bit of the data byte
 * to 1. See \ref communication-protocols "Communication Protocols" for more information.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param byte The byte to send
 */
void mtb_pmbus_set_received_byte(mtb_pmbus_stc_t *inst, uint8_t byte);

#if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief Notifies an error to PMBus host as defined by Host Notify Protocol(HNP).
 * It starts data transmission to the host as a controller device. During transmission,
 * the PMBus instance becomes unavailable for operations as target. To check transmission
 * results, use \ref mtb_pmbus_handle_host_notify_t callback.
 *
 * \note It needs specific hardware settings. Refer to \ref scb-i2c-configuration
 *       "SCB I2C Configuration"
 *
 * @param inst The pointer to the PMBus instance structure
 * @param data The data to send
 * \note Only lower 16 bits are used.
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_send_host_notify(mtb_pmbus_stc_t *inst, uint32_t data);
#endif \
    /* #if (defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) ||
       defined (MTB_PMBUS_DOXYGEN) */

/**
 * @brief Checks if the PMBus instance is currently busy.
 *
 * This function determines whether the specified PMBus instance is engaged in communication.
 * The busy status is set once the PMBus instance receives any of the supported addresses (Own
 * Target Address, Alert Response Address, etc.) and cleared after a STOP condition. Use this
 * function together with \ref mtb_pmbus_disable to avoid disabling the PMBus instance during
 * active communication and to prevent bus errors.
 *
 * @param inst Pointer to the PMBus instance structure
 * @return true if the PMBus instance is busy; false otherwise.
 */
bool mtb_pmbus_is_busy(mtb_pmbus_stc_t *inst);

/** \} group_pmbus_miscellaneous_func */

/**
 * \defgroup group_pmbus_isr_func ISR Functions
 * \{
 */

/**
 * @brief PMBus Target interrupt service routine
 *
 * This function should be called from the I2C interrupt handler
 *
 * @param inst The pointer to the PMBus instance structure
 */
void mtb_pmbus_i2c_isr(mtb_pmbus_stc_t *inst);

#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) || defined (MTB_PMBUS_DOXYGEN)
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief PMBus Timer interrupt service routine
 *
 * This function should be called from the TCPWM interrupt handler
 *
 * \note This function is only available when \ref MTB_PMBUS_ENABLE_TIMEOUT
 * and \ref MTB_PMBUS_HAL_USE_TCPWM are enabled at compile time options.
 *
 * @param inst The pointer to the PMBus instance structure
 */
void mtb_pmbus_timer_isr(mtb_pmbus_stc_t *inst);
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) || defined (MTB_PMBUS_DOXYGEN) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) || defined (MTB_PMBUS_DOXYGEN) */

/** \} group_pmbus_isr_func */

/** \} group_pmbus_trgt_functions */

#ifdef __cplusplus
}
#endif
#endif /* MTB_PMBUS_TRGT_H */
