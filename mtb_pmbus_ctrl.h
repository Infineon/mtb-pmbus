/***************************************************************************//**
* \file mtb_pmbus_ctrl.h
* \version 1.0
*
* Provides API declarations for the PMBus controller.
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


#ifndef MTB_PMBUS_CTRL_H
#define MTB_PMBUS_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mtb_pmbus_conf_def.h"
#include "mtb_pmbus_log.h"
#include "mtb_pmbus_ctrl_hal.h"
#include "mtb_pmbus_pec.h"

/**
 * \addtogroup group_pmbus_ctrl_enums
 * \{
 */
/** Controller statuses */
typedef enum
{
    /** Correct status, No error */
    MTB_PMBUS_CTRL_STATUS_SUCCESS = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 0U),
    /** The provided command code is incorrect */
    MTB_PMBUS_CTRL_STATUS_BAD_PARAM = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 1U),
    /** The controller is busy */
    MTB_PMBUS_CTRL_STATUS_IS_BUSY = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 2U),
    /** The controller is ready to send data */
    MTB_PMBUS_CTRL_STATUS_IS_READY = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 3U),
    /** The unknown error */
    MTB_PMBUS_CTRL_STATUS_UNKNOWN_ERR = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 4U),
    /** The I2C Bus is busy */
    MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 5U),
    /** Timeout */
    MTB_PMBUS_CTRL_STATUS_TIMEOUT = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 6U),
    /** The last transfer finished with an error. The controller is ready to send data */
    MTB_PMBUS_CTRL_STATUS_TRANSFER_ERR = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MIDDLEWARE_PMBUS, 7U)
} mtb_pmbus_ctrl_status_t;

/** Controller events */
typedef enum
{
    /** The Transfer is completed */
    MTB_PMBUS_CTRL_TRANSFER_DONE    = 0U,
    /** The incorrect PEC is received */
    MTB_PMBUS_CTRL_CORRUPTED_DATA   = 1U,
    /** The Target sends NACK to Address */
    MTB_PMBUS_CTRL_TARGET_NACK_ADDR = 2U,
    /** A bus error occurred on the I2C bus */
    MTB_PMBUS_CTRL_BUS_ERR          = 3U,
    /** A timeout is detected */
    MTB_PMBUS_CTRL_TIMEOUT          = 4U,
    /** Target sends NACK to the data byte */
    MTB_PMBUS_CTRL_TARGET_NACK_BYTE = 5U,
    /** Target sends NACK to the command code */
    MTB_PMBUS_CTRL_TARGET_NACK_CMD  = 6U,
    /** Controller arbitration lost */
    MTB_PMBUS_CTRL_ARB_LOST         = 7U,
    /** Controller abort starts */
    MTB_PMBUS_CTRL_ABORT_START      = 8U,
    /** The received block count is too big */
    MTB_PMBUS_CTRL_BLOCK_COUNT_TOO_BIG  = 9U
} mtb_pmbus_ctrl_events_t;

#if (defined(MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U)) || \
    defined (MTB_PMBUS_DOXYGEN)
/** Host Notify events
 * \note This enum is only available when \ref MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY is enabled at
 * compile time options.
 */
typedef enum
{
    /** The Host Notify transfer successful complete */
    MTB_PMBUS_CTRL_HOST_NOTIFY_COMPLETE            = 0U,
    /** The Host Notify request received */
    MTB_PMBUS_CTRL_HOST_NOTIFY_REQUEST_RECEIVED    = 1U,
    /** An error occurred during Host Notify transfer */
    MTB_PMBUS_CTRL_HOST_NOTIFY_ERROR               = 2U
} mtb_pmbus_ctrl_host_notify_events_t;
#endif \
    /* #if (defined(MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY !=
       0U)) || defined (MTB_PMBUS_DOXYGEN) */

/** Events for HW initialization */
typedef enum
{
    /** Initialize the HW resources */
    MTB_PMBUS_CTRL_HW_RESOURCES_INIT    = 0U,
    /** Enable the I2C HW */
    MTB_PMBUS_CTRL_HW_RESOURCES_ENABLE  = 1U,
    /** Disable the I2C HW */
    MTB_PMBUS_CTRL_HW_RESOURCES_DISABLE = 2U
} mtb_pmbus_ctrl_hw_resources_ctrl_action_t;

/** \} group_pmbus_ctrl_enums */

/**
 * \addtogroup group_pmbus_ctrl_data_structs
 * \{
 */
/** Provides the typedef for the callback function called in ISR to
 * notify the user about occurrences of \ref mtb_pmbus_ctrl_events_t
 */
typedef void (*mtb_pmbus_ctrl_handle_events_t)(mtb_pmbus_ctrl_events_t event);

/** Provides the typedef for the callback function called in
 * \ref mtb_pmbus_ctrl_enable and \ref mtb_pmbus_ctrl_disable to perform
 * enabling/disabling HW resources.
 */
typedef void (*mtb_pmbus_ctrl_hw_resources_ctrl_t)(mtb_pmbus_ctrl_hw_resources_ctrl_action_t action);

/** Provides the typedef for the callback function called inside
 * the middleware to enable/disable PMBus controller related interrupts.
 */
typedef void (*mtb_pmbus_ctrl_hw_isr_ctrl_t)(void);

#if (defined(MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U)) || \
    defined (MTB_PMBUS_DOXYGEN)
/** Provides the typedef for the callback function called in ISR to
 * notify the user about occurrences of \ref mtb_pmbus_ctrl_host_notify_events_t
 */
typedef void (*mtb_pmbus_ctrl_handle_host_notify_t)(mtb_pmbus_ctrl_host_notify_events_t event);
#endif \
    /* #if (defined(MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY !=
       0U)) || defined (MTB_PMBUS_DOXYGEN) */

/** Controller configuration structure */
typedef struct
{
    /** The pointer to events callback */
    mtb_pmbus_ctrl_handle_events_t callback_events;
#if (defined(MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U)) || \
    defined (MTB_PMBUS_DOXYGEN)
    /** The pointer to Host Notify events callback */
    mtb_pmbus_ctrl_handle_host_notify_t callback_hnp;
#endif \
    /* #if (defined(MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY !=
       0U)) || defined (MTB_PMBUS_DOXYGEN) */
    /** The pointer to HAL config structure */
    mtb_pmbus_ctrl_stc_config_hal_t *hal_cfg;
    /** The pointer to the user callback for initializing/enabling/disabling the I2C Hardware */
    mtb_pmbus_ctrl_hw_resources_ctrl_t callback_hw;
    /** The pointer to the user callback for enabling I2C HW interrupts */
    mtb_pmbus_ctrl_hw_isr_ctrl_t callback_isr_enable;
    /** The pointer to the user callback for disabling I2C HW interrupts */
    mtb_pmbus_ctrl_hw_isr_ctrl_t callback_isr_disable;
} mtb_pmbus_ctrl_cfg_t;

/** Transfer configuration structure */
typedef struct
{
    /** The pointer to the data buffer */
    uint8_t *data;
    /** The target address */
    uint8_t addr;
    /** The size of the write part of the transfer */
    uint16_t wr_size;
    /** The size of the read part of the transfer */
    uint16_t rd_size;
    /** True - execute the stop condition at the end of transfer, False - Otherwise */
    bool execute_stop;
} mtb_pmbus_ctrl_stc_transfer_cfg_t;

/** Controller instance structure */
typedef struct
{
    /** \cond INTERNAL */
    mtb_pmbus_ctrl_cfg_t *cfg;
    uint32_t transfer_cap;
    uint32_t state;
    uint32_t errors;
    uint8_t buffer[512U];
    uint8_t pec;
    uint8_t *user_data;
    uint32_t *block_buff_size;
    uint16_t byte_received;
    uint16_t byte_requested;
    mtb_pmbus_ctrl_stc_transfer_cfg_t transfer_cfg;
    /** \endcond */
} mtb_pmbus_ctrl_stc_t;

/** \} group_pmbus_ctrl_data_structs */

/**
 * \addtogroup group_pmbus_ctrl_functions
 * \{
 */

/**
 * @defgroup group_pmbus_ctrl_init_func Initialization Functions
 * \{
 */

/**
 * @brief Initialize the PMBus Instance in Controller mode
 *
 * @param inst The pointer to the PMBus controller instance structure
 * @param cfg The pointer to the PMBus controller configuration structure
 * @return \ref mtb_pmbus_ctrl_status_t
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_init(mtb_pmbus_ctrl_stc_t *inst, mtb_pmbus_ctrl_cfg_t *cfg);

/**
 * @brief Enable PMBus instance in Controller mode
 *
 * After calling this function, the controller is ready to perform transfers
 *
 * @param inst The pointer to the PMBus controller instance structure
 */
void mtb_pmbus_ctrl_enable(mtb_pmbus_ctrl_stc_t *inst);

/**
 * @brief Disable PMBus instance in Controller mode
 *
 * After calling this function, the controller stops operation
 *
 * @param inst The pointer to the PMBus controller instance structure
 */
void mtb_pmbus_ctrl_disable(mtb_pmbus_ctrl_stc_t *inst);

/** \} group_pmbus_ctrl_init_func */

/**
 * @defgroup group_pmbus_ctrl_status_func Status Functions
 * \{
 */

/**
 * @brief Check if the PMBus bus is busy
 *
 * This function checks the hardware status of the PMBus bus to determine if it's
 * currently occupied by another controller or if there's ongoing communication.
 *
 * @param inst The pointer to the PMBus controller instance structure. Must not be NULL.
 *
 * @return bool Bus status:
 *         - true: The PMBus bus is currently busy (occupied by another controller or ongoing
 * transfer)
 *         - false: The PMBus bus is free and available for new transfers
 *
 * @note This is a non-blocking function that returns the current bus state immediately.
 */
bool mtb_pmbus_ctrl_bus_is_busy(mtb_pmbus_ctrl_stc_t *inst);

/**
 * @brief Get the status of the PMBus controller
 *
 * This function checks the internal state and error flags of the PMBus controller
 * to determine its current operational status. It provides information about whether
 * the controller is ready for new operations, currently busy with a transfer,
 * or has encountered an error.
 *
 * @param inst The pointer to the PMBus controller instance structure.
 *
 * @return @ref mtb_pmbus_ctrl_status_t The current status of the controller:
 *         - MTB_PMBUS_CTRL_STATUS_IS_READY: The controller is ready to accept new transfers
 *         - MTB_PMBUS_CTRL_STATUS_IS_BUSY: The controller is currently processing a transfer
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: The invalid parameter (inst is NULL)
 *
 * @note This function performs a non-blocking status check of the controller internal state.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_get_status(mtb_pmbus_ctrl_stc_t *inst);

/**
 * @brief Wait for the transfer completion with a timeout
 *
 * This function polls the PMBus controller status until the transfer is complete
 * or the specified timeout expires. It checks the controller status every microsecond
 * and returns when the controller becomes ready or when a timeout occurs.
 *
 * @param inst The pointer to the PMBus controller instance structure. Must not be NULL.
 * @param timeout The timeout value in microseconds. The function will wait up to this
 *                many microseconds for the transfer to complete. A value of 0 will
 *                perform a single status check without waiting.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_IS_READY: Transfer completed
 *         - MTB_PMBUS_CTRL_STATUS_TIMEOUT: A timeout occurred before completion
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameter (inst is NULL)
 *
 * @note This function uses a blocking wait with 1 microsecond polling interval.
 * @note The function will delay for 1 microsecond between each status check.
 * @note If timeout is 0, the function performs only one status check without delay.
 *
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_wait_cmpl(mtb_pmbus_ctrl_stc_t *inst, uint32_t timeout);

/** \} group_pmbus_ctrl_status_func */

/**
 * @defgroup group_pmbus_ctrl_transfer_func Transfer Functions
 * \{
 */

/**
 * @brief Execute a generic PMBus transfer
 *
 * It accepts a transfer configuration structure containing all transfer
 * parameters, allowing the application to construct custom protocol sequences.
 *
 * The function handles low-level bus operations and the error detection while giving the
 * application full control over:
 * - Target device address
 * - Payload format for write
 * - Write data size (number of bytes to write)
 * - Read data size (number of bytes to read)
 * - Stop condition control (whether to generate STOP at the end of the transfer)
 *
 * Transfer behavior based on parameter combinations:
 * - wr_size > 0, rd_size = 0, execute_stop = true: Write-only transfer with STOP
 * - wr_size > 0, rd_size = 0, execute_stop = false: Write-only transfer without STOP
 * - wr_size > 0, rd_size > 0, execute_stop = true: Write followed by Repeated START and read, then
 * STOP
 * - wr_size > 0, rd_size > 0, execute_stop = false: Write followed by Repeated START and read,
 * without STOP
 * - wr_size = 0, rd_size > 0, execute_stop = true: Read-only transfer with STOP
 * - wr_size = 0, rd_size > 0, execute_stop = false: Read-only transfer without STOP
 *
 * @note When both wr_size and rd_size are non-zero, the provided data buffer must be
 * large enough to accommodate the larger of the two sizes (size >= max(wr_size, rd_size)),
 * as read data will overwrite the write data in the same buffer.
 *
 * @note When using this generic transfer API, the application is responsible for proper
 * protocol sequencing and data formatting according to the SMBus/PMBus specification.
 *
 * <b>Example: PMBus Group Command Protocol</b>
 *
 * The Group Command protocol allows sending the same command to multiple devices
 * atomically. This can be achieved by calling this function multiple times with
 * execute_stop set to false for all transfers except the last one. For example,
 * to set VOUT_COMMAND on three devices (0x20, 0x21, 0x22), execute three transfers
 * with only the final transfer generating a STOP condition, and so ensures all devices
 * receive the command and update their outputs simultaneously.
 *
 * @param inst The pointer to the PMBus controller instance structure
 * @param cfg The pointer to the transfer configuration structure (\ref
 * mtb_pmbus_ctrl_stc_transfer_cfg_t)
 * @return \ref mtb_pmbus_ctrl_status_t
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_execute_transfer(mtb_pmbus_ctrl_stc_t *inst,
                                                        mtb_pmbus_ctrl_stc_transfer_cfg_t *cfg);

/**
 * @brief Execute SMBus Quick Command protocol
 *
 * This function performs the SMBus Quick Command Protocol, which sends only the
 * write-direction bit of the target device address write-direction bit.
 *
 * The function constructs a complete transaction packet including:
 * - Device address with W bit
 *
 * Transaction format:
 * @code [S] [ADDR+W] [A] [P] @endcode
 *
 * Where: S=Start, P=Stop, A=ACK, W=Write bit
 *
 * @note Quick Command with Read direction (ADDR+R) is not supported due to
 *       HW limitations. See \ref quick-command-with-read-direction-limitation-controller-mode
 *       "Quick Command with Read Direction Limitation" for details.
 *
 * @param inst The pointer to the PMBus Controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the W bit.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_quick_cmd(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr);

/**
 * @brief Execute SMBus Receive Byte protocol
 *
 * This function performs the SMBus Receive Byte protocol, which reads a single byte
 * of data from the target device.
 *
 * The function constructs a complete transaction packet including:
 * - Data payload reception (1 byte)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+R] [A] [Data Byte] [N] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+R] [A] [Data Byte] [A] [PEC] [N] [P] @endcode
 *
 * Where: S=Start, P=Stop, A=ACK, N=NACK, R=Read bit
 *
 * @param inst The pointer to the PMBus Controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param data The pointer to a 1-byte buffer to store received data.
 *             Must point to valid memory with at least 1 byte.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and verification.
 *            When enabled, verifies CRC-8 checksum for error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_received_byte(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint8_t *data,
                                                        bool pec);

/**
 * @brief Execute SMBus Send Byte protocol
 *
 * This function performs the SMBus Send Byte protocol, which sends a single byte
 * of data to the target device.
 *
 * The function constructs a complete transaction packet including:
 * - Data payload (1 byte)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [Data Byte] [A] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [Data Byte] [A] [PEC] [A] [P] @endcode
 *
 * Where: S=Start, P=Stop, A=ACK, W=Write bit
 *
 * @param inst The pointer to the PMBus controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param data The pointer to a 1-byte data value to send.
 *             Must point to valid memory with at least 1 byte.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and transmission.
 *            When enabled, adds CRC-8 checksum for error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_send_byte(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint8_t *data, bool pec);

/**
 * @brief Execute SMBus Write Byte protocol
 *
 * This function performs the SMBus Write Byte protocol, which sends a command code
 * followed by 1 byte of data to the target device.
 *
 * The function constructs a complete transaction packet including:
 * - Command code (1 byte)
 * - Data payload (1 byte)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Data Byte] [A] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Data Byte] [A] [PEC] [A] [P] @endcode
 *
 * Where: S=Start, P=Stop, A=ACK, W=Write bit
 *
 * @param inst The pointer to the PMBus controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data The pointer to a 1-byte data value to write.
 *             Must point to valid memory with at least 1 byte.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and transmission.
 *            When enabled, adds CRC-8 checksum for error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_write_byte(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                     uint8_t *data, bool pec);

/**
 * @brief Execute SMBus Read Byte protocol
 *
 * This function performs the SMBus Read Byte protocol, which sends a command code
 * to the target device and then reads 1 byte of data from it.
 *
 * The function constructs the complete transaction packet including:
 * - Command code (1 byte)
 * - Data payload reception (1 byte)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Sr] [ADDR+R] [A] [Data Byte] [N] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Sr] [ADDR+R] [A] [Data Byte] [A] [PEC] [N] [P] @endcode
 *
 * Where: S=Start, Sr=Repeated Start, P=Stop, A=ACK, N=NACK, W=Write bit, R=Read bit
 *
 * @param inst The pointer to the PMBus Controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data Pointer to a 1-byte buffer to store the read data.
 *             Must point to valid memory with at least 1 byte.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and verification.
 *            When enabled, verifies CRC-8 checksum for error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_read_byte(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                    uint8_t *data, bool pec);

/**
 * @brief Execute SMBus Write Word protocol
 *
 * This function performs the SMBus Write Word protocol, which sends a command code
 * followed by 2 bytes of data to the target device.
 *
 * The function constructs a complete transaction packet including:
 * - Command code (1 byte)
 * - Data payload (2 bytes, LSB first)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Data Byte Low] [A] [Data Byte High] [A] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Data Byte Low] [A] [Data Byte High] [A] [PEC] [A] [P] @endcode
 *
 * Where: S=Start, P=Stop, A=ACK, W=Write bit
 *
 * @param inst The pointer to the PMBus Controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data The pointer to a 2-byte array containing the data to write.
 *             Must point to valid memory with at least 2 bytes.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and transmission.
 *            When enabled, adds CRC-8 checksum for the error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 *
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_write_word(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                     uint8_t *data, bool pec);

/**
 * @brief Execute SMBus Read Word protocol
 *
 * This function performs the SMBus Read Word protocol, which sends a command code
 * to the target device and then reads 2 bytes of data from it.
 *
 * The function constructs a complete transaction packet including:
 * - Command code (1 byte)
 * - Data payload reception (2 bytes, Data Byte Low first)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Sr] [ADDR+R] [A] [Data Byte Low] [A] ...
 *       ... [Data Byte High] [N] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Sr] [ADDR+R] [A] [Data Byte Low] [A] ...
 *       ... [Data Byte High] [A] [PEC] [N] [P] @endcode
 *
 * Where: S=Start, Sr=Repeated Start, P=Stop, A=ACK, N=NACK, W=Write bit, R=Read bit
 *
 * @param inst The pointer to the PMBus Controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data The pointer to a 2-byte buffer to store read data.
 *             Must point to valid memory with at least 2 bytes.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and verification.
 *            When enabled, verifies CRC-8 checksum for error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_read_word(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                    uint8_t *data, bool pec);

/**
 * @brief Execute SMBus Process Call protocol
 *
 * This function performs a SMBus Process Call protocol, which sends a command code
 * followed by 2 bytes of data to a target device, and then reads 2 bytes of data back.
 * This is a combined write-read operation in a single transaction.
 *
 * The function constructs the complete transaction packet including:
 * - Command code (1 byte)
 * - Data payload transmission (2 bytes, Data Byte Low first)
 * - Data payload reception (2 bytes, Data Byte Low first)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Data Byte Low] [A] [Data Byte High] [A] ...
 *       ... [Sr] [ADDR+R] [A] [Data Byte Low] [A] [Data Byte High] [N] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Data Byte Low] [A] [Data Byte High] [A] ...
 *       ... [Sr] [ADDR+R] [A] [Data Byte Low] [A] [Data Byte High] [A] [PEC] [N] [P] @endcode
 *
 * Where: S=Start, Sr=Repeated Start, P=Stop, A=ACK, N=NACK, W=Write bit, R=Read bit
 *
 * @param inst The pointer to the PMBus Controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data The pointer to the 4-byte buffer. The first 2 bytes contain data to send,
 *             and the last 2 bytes will be overwritten with the received data.
 *             Must point to valid memory with at least 4 bytes.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and verification.
 *            When enabled, verifies CRC-8 checksum for error detection on the read part only.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 * @note The received data will overwrite bytes 2-3 of the data buffer upon completion.
 * @note PEC verification is only applied to the read portion of the transaction.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_process_call(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                       uint8_t *data, bool pec);

/**
 * @brief Execute SMBus Block Write protocol
 *
 * This function performs the SMBus Block Write protocol, which sends a command code
 * followed by a byte count and a variable number of data bytes to a target device.
 *
 * The function constructs a complete transaction packet including:
 * - Command code (1 byte)
 * - Byte count (1 byte, indicating number of data bytes to follow)
 * - Data payload (0 to 255 bytes)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Byte Count] [A] [Data Byte 0] [A] ...
 *       ... [Data Byte 1] [A] ... [Data Byte N-1] [A] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Byte Count] [A] [Data Byte 0] [A] ...
 *       ... [Data Byte 1] [A] ... [Data Byte N-1] [A] [PEC] [A] [P] @endcode
 *
 * Where: S=Start, P=Stop, A=ACK, W=Write bit, N=Number of data bytes (1-255)
 *
 * @param inst The pointer to the PMBus Controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data The pointer to the data buffer containing bytes to write.
 *             Must point to valid memory with at least 'size' bytes.
 * @param size Number of bytes to write (0 to 255 bytes).
 *             This value is automatically sent as the byte count in the transaction.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and transmission.
 *            When enabled, adds CRC-8 checksum for error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers, size out of range)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 * @note The byte count is automatically included in the transaction and should not be part of the
 * data buffer.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_block_write(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                      uint8_t *data, uint32_t size, bool pec);

/**
 * @brief Execute SMBus Block Read protocol
 *
 * This function performs the SMBus Block Read protocol, which sends a command code
 * to the target device and then reads a variable number of data bytes from it.
 * The target device first responds with a byte count indicating how many data bytes will follow.
 *
 * The function constructs a complete transaction packet including:
 * - Command code (1 byte)
 * - Byte count reception (1 byte, indicating number of data bytes that follow)
 * - Data payload reception (0 to 255 bytes, as specified by byte count)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Sr] [ADDR+R] [A] [Byte Count] [A] ...
 *       ... [Data Byte 0] [A] [Data Byte 1] [A] ... [Data Byte N-1] [N] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Sr] [ADDR+R] [A] [Byte Count] [A] ...
 *       ... [Data Byte 0] [A] [Data Byte 1] [A] ... [Data Byte N-1] [A] [PEC] [N] [P] @endcode
 *
 * Where: S=Start, Sr=Repeated Start, P=Stop, A=ACK, N=NACK, W=Write bit, R=Read bit
 *
 * @param inst The pointer to the PMBus Controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data The pointer to the data buffer to store read data.
 *             Must point to valid memory with at least the size specified in the size parameter.
 *             The byte count is not stored in this buffer, only actual data bytes.
 * @param size The pointer to the variable containing the maximum buffer size on the input and
 * actual read size on the output.
 *             Input: The maximum number of bytes that can be stored in the data buffer.
 *             Output: The actual number of bytes read (as reported by the target byte count).
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and verification.
 *            When enabled, verifies CRC-8 checksum for error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data and size parameters must remain valid until the transaction completes.
 * @note The byte count received from the target determines the actual number of data bytes read.
 * @note If the target byte count exceeds the maximum buffer size,
 *       the Controller will NACK the block count byte.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_block_read(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                     uint8_t *data, uint32_t *size, bool pec);

/**
 * @brief Execute SMBus Block Write-Block Read Process Call protocol
 *
 * This function performs the SMBus Block Write-Block Read Process Call protocol, which sends
 * command code followed by a byte count and variable number of data bytes to the target device,
 * and then reads the variable number of data bytes back. This is a combined block write-block read
 * operation in a single transaction.
 *
 * The function constructs a complete transaction packet including:
 * - Command code (1 byte)
 * - Write byte count (1 byte, indicating number of data bytes to write)
 * - Write data payload (0 to 255 bytes)
 * - Read byte count reception (1 byte, indicating number of data bytes that follow)
 * - Read data payload reception (0 to 255 bytes, as specified by read byte count)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Write Byte Count] [A] ...
 *       ... [Write Data 0] [A] ... [Write Data N-1] [A] ... [Sr] [ADDR+R] [A] ...
 *       ... [Read Byte Count] [A] [Read Data 0] [A] ... [Read Data M-1] [N] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Write Byte Count] [A] ...
 *       ... [Write Data 0] [A] ... [Write Data N-1] [A] ...
 *       ... [Sr] [ADDR+R] [A] [Read Byte Count] [A] ...
 *       ... [Read Data 0] [A] ... [Read Data M-1] [A] [PEC] [N] [P] @endcode
 *
 * Where: S=Start, Sr=Repeated Start, P=Stop, A=ACK, N=NACK, W=Write bit, R=Read bit
 *
 * @param inst The pointer to the PMBus controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data The pointer to the data buffer containing write data followed by space for read data.
 *             The first size_wr bytes contain the data to write.
 *             The remaining buffer space will be overwritten with received data.
 *             Must point to valid memory with at least (size_wr + maximum expected read size)
 * bytes.
 * @param size_wr Number of bytes to write (0 to 255 bytes). Must be less than or equal to the size
 * of the data buffer.
 *               This value is automatically sent as a write byte count in the transaction.
 * @param size The pointer to the variable containing the maximum read buffer size on the input and
 * actual read size on the output.
 *             Input: The size of the buffer space.
 *             Output: The actual number of bytes read (as reported by the target's read byte
 * count).
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and verification.
 *            When enabled, verifies CRC-8 checksum for error detection on the read part only.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers, size_wr out of
 * range)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data and size parameters must remain valid until the transaction completes.
 * @note The write byte count is automatically included in the transaction and shall not be part of
 * the data buffer.
 * @note The read byte count received from the target determines the actual number of data bytes
 * read.
 * @note The received data will overwrite the buffer starting at offset size_wr upon completion.
 * @note If the target's read byte count exceeds the remaining buffer size,
 *       the Controller will NACK the read byte count.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_block_process_call(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr,
                                                             uint32_t cmd_code, uint8_t *data, uint32_t size_wr,
                                                             uint32_t *size, bool pec);

/**
 * @brief Execute SMBus Write 32 protocol
 *
 * This function performs a SMBus Write 32 Protocol, which sends command code
 * followed by 4 bytes of data to a target device.
 *
 * The function constructs the complete transaction packet including:
 * - Command code (1 byte)
 * - Data payload (4 bytes)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Data Byte 0] [A] [Data Byte 1] [A] ...
 *       ... [Data Byte 2] [A] [Data Byte 3] [A] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Data Byte 0] [A] [Data Byte 1] [A] ...
 *       ... [Data Byte 2] [A] [Data Byte 3] [A] [PEC] [A] [P] @endcode
 *
 * Where: S=Start, P=Stop, A=ACK, W=Write bit
 *
 * @param inst The pointer to the PMBus controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data The pointer to a 4-byte array containing data to write.
 *             Must point to valid memory with at least 4 bytes.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and transmission.
 *            When enabled, adds CRC-8 checksum for error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_write_32(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                   uint8_t *data, bool pec);

/**
 * @brief Execute SMBus Write 64 protocol
 *
 * This function performs the SMBus Write 64 Protocol, which sends command code
 * followed by 8 bytes of data to the target device.
 *
 * The function constructs a complete transaction packet including:
 * - Command code (1 byte)
 * - Data payload (8 bytes)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Data Byte 0] [A] [Data Byte 1] [A] ...
 *       ... [Data Byte 2] [A] [Data Byte 3] [A] [Data Byte 4] [A] [Data Byte 5] [A] ...
 *       ... [Data Byte 6] [A] [Data Byte 7] [A] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Data Byte 0] [A] [Data Byte 1] [A] ...
 *       ... [Data Byte 2] [A] [Data Byte 3] [A] [Data Byte 4] [A] [Data Byte 5] [A] ...
 *       ... [Data Byte 6] [A] [Data Byte 7] [A] [PEC] [A] [P] @endcode
 *
 * Where: S=Start, P=Stop, A=ACK, W=Write bit
 *
 * @param inst The pointer to the PMBus controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data The pointer to an 8-byte array containing data to write.
 *             Must point to valid memory with at least 8 bytes.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and transmission.
 *            When enabled, adds CRC-8 checksum for error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_write_64(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                   uint8_t *data, bool pec);

/**
 * @brief Execute SMBus Read 32 protocol
 *
 * This function performs the SMBus Read 32 protocol, which sends a command code
 * to the target device and then reads 4 bytes of data from it.
 *
 * The function constructs a complete transaction packet including:
 * - Command code (1 byte)
 * - Data payload reception (4 bytes)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Sr] [ADDR+R] [A] [Data Byte 0] [A] ...
 *       ... [Data Byte 1] [A] [Data Byte 2] [A] [Data Byte 3] [N] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Sr] [ADDR+R] [A] [Data Byte 0] [A] ...
 *       ... [Data Byte 1] [A] [Data Byte 2] [A] [Data Byte 3] [A] [PEC] [N] [P] @endcode
 *
 * Where: S=Start, Sr=Repeated Start, P=Stop, A=ACK, N=NACK, W=Write bit, R=Read bit
 *
 * @param inst The pointer to the PMBus controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data The pointer to the 4-byte buffer to store the read data.
 *             Must point to valid memory with at least 4 bytes.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and verification.
 *            When enabled, verifies CRC-8 checksum for error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t Status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error code as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_read_32(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                  uint8_t *data, bool pec);

/**
 * @brief Execute PMBus Read 64 protocol
 *
 * This function performs the PMBus Read 64 protocol, which sends command code
 * to the target device and then reads 8 bytes of data from it.
 *
 * The function constructs the complete transaction packet including:
 * - Command code (1 byte)
 * - Data payload reception (8 bytes)
 * - Optional PEC (Packet Error Code) for data integrity verification
 *
 * Transaction format without PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Sr] [ADDR+R] [A] [Data Byte 0] [A] ...
 *       ... [Data Byte 1] [A] [Data Byte 2] [A] [Data Byte 3] [A] [Data Byte 4] [A] ...
 *       ... [Data Byte 5] [A] [Data Byte 6] [A] [Data Byte 7] [N] [P] @endcode
 * Transaction format with PEC:
 * @code [S] [ADDR+W] [A] [CMD] [A] [Sr] [ADDR+R] [A] [Data Byte 0] [A] ...
 *       ... [Data Byte 1] [A] [Data Byte 2] [A] [Data Byte 3] [A] [Data Byte 4] [A] ...
 *       ... [Data Byte 5] [A] [Data Byte 6] [A] [Data Byte 7] [A] [PEC] [N] [P] @endcode
 *
 * Where: S=Start, Sr=Repeated Start, P=Stop, A=ACK, N=NACK, W=Write bit, R=Read bit
 *
 * @param inst The pointer to the PMBus controller instance structure.
 * @param addr Target device 7-bit I2C address (0x00 to 0x7F).
 *             The function automatically handles the R/W bit.
 * @param cmd_code Command code to send.
 * @param data The pointer to the 8-byte buffer to store read data.
 *             Must point to valid memory with at least 8 bytes.
 * @param pec Enable (true) or disable (false) Packet Error Code calculation and verification.
 *            When enabled, verifies CRC-8 checksum for the error detection.
 *
 * @return @ref mtb_pmbus_ctrl_status_t The status of the operation:
 *         - MTB_PMBUS_CTRL_STATUS_SUCCESS: Transaction initiated successfully
 *         - MTB_PMBUS_CTRL_STATUS_BAD_PARAM: Invalid parameters (NULL pointers)
 *         - MTB_PMBUS_CTRL_STATUS_BUS_IS_BUSY: I2C bus is currently busy
 *         - Other error codes as defined in @ref mtb_pmbus_ctrl_status_t
 *
 * @note This function is non-blocking. Use @ref mtb_pmbus_ctrl_wait_cmpl()
 *       to determine when the transaction completes.
 * @note The data parameter must remain valid until the transaction completes.
 */
mtb_pmbus_ctrl_status_t mtb_pmbus_ctrl_ex_read_64(mtb_pmbus_ctrl_stc_t *inst, uint8_t addr, uint32_t cmd_code,
                                                  uint8_t *data, bool pec);

/** \} group_pmbus_ctrl_transfer_func */

/**
 * @defgroup group_pmbus_ctrl_isr_func Interrupt Functions
 * \{
 */

/**
 * @brief PMBus controller interrupt service routine
 *
 * This function must be called from the I2C interrupt handler
 *
 * @param inst The pointer to the PMBus controller instance structure
 */
void mtb_pmbus_ctrl_isr(mtb_pmbus_ctrl_stc_t *inst);

#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) || defined (MTB_PMBUS_DOXYGEN)
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief PMBus controller timer interrupt service routine
 *
 * This function must be called from the TCPWM interrupt handler
 *
 * \note This function is only available when \ref MTB_PMBUS_ENABLE_TIMEOUT
 * and \ref MTB_PMBUS_HAL_USE_TCPWM are enabled at compile time options.
 *
 * @param inst The pointer to the PMBus controller instance structure
 */
void mtb_pmbus_ctrl_timer_isr(mtb_pmbus_ctrl_stc_t *inst);
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) || defined (MTB_PMBUS_DOXYGEN) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) || defined (MTB_PMBUS_DOXYGEN) */

#if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY != 0U)) || \
    defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief Return target address that sent Host Notify request
 *
 * This function must be called in the \ref mtb_pmbus_ctrl_handle_host_notify_t
 * when a \ref MTB_PMBUS_CTRL_HOST_NOTIFY_COMPLETE event occurs.
 *
 * \note This function is only available when \ref MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY is
 * enabled at compile time options.
 *
 * @param inst The pointer to the PMBus controller instance structure
 *
 * @return The 7-bit I2C address of the target device that sent the Host Notify request
 */
uint8_t mtb_pmbus_ctrl_hnp_get_trgt_addr(mtb_pmbus_ctrl_stc_t *inst);

/**
 * @brief Return data that target sent during Host Notify
 *
 * This function must be called in the \ref mtb_pmbus_ctrl_handle_host_notify_t
 * when a \ref MTB_PMBUS_CTRL_HOST_NOTIFY_COMPLETE event occurs.
 *
 * \note This function is only available when \ref MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY is
 * enabled at compile time options.
 *
 * @param inst The pointer to the PMBus controller instance structure
 *
 * @return The 16-bit data sent by the target device during Host Notify
 */
uint16_t mtb_pmbus_ctrl_hnp_get_trgt_data(mtb_pmbus_ctrl_stc_t *inst);
#endif \
    /* #if (defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY !=
       0U)) || defined (MTB_PMBUS_DOXYGEN) */

/** \} group_pmbus_ctrl_isr_func */

/** \} group_pmbus_ctrl_functions */


#ifdef __cplusplus
}
#endif
#endif /* MTB_PMBUS_CTRL_H */
