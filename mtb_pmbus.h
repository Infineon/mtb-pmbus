/***************************************************************************//**
* \file mtb_pmbus.h
* \version 1.0
*
* Provides API declarations for the PMBus Middleware.
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

/**
 * \defgroup group_pmbus_trgt_api                    Target API
 * \{
 *      \defgroup group_pmbus_trgt_macro_conf        Compile Time Options Macros
 *      \defgroup group_pmbus_trgt_macro             Macros
 *      \defgroup group_pmbus_trgt_functions         Functions
 *      \defgroup group_pmbus_trgt_data_structs      Data Structures
 *      \defgroup group_pmbus_trgt_enums             Enumerated Types
 *      \defgroup group_pmbus_trgt_hal_apis          Hardware Specific APIs
 * \}
 *
 * \defgroup group_pmbus_ctrl_api                    Controller API
 * \{
 *      \defgroup group_pmbus_ctrl_functions         Functions
 *      \defgroup group_pmbus_ctrl_data_structs      Data Structures
 *      \defgroup group_pmbus_ctrl_enums             Enumerated Types
 *      \defgroup group_pmbus_ctrl_hal_apis          Hardware Specific APIs
 * \}
 *
 * \defgroup group_pmbus_common_api                  Common API
 * \{
 *      \defgroup group_pmbus_common_macro_conf      Compile Time Options Macros
 *      \defgroup group_pmbus_macro_log              Logging level Macros
 *      \defgroup group_pmbus_helper_func            Helper Functions
 *      \defgroup group_pmbus_common_hal_apis        Common Hardware Specific APIs
 * \}
 *
 * \page group_pmbus_changelog Changelog
 *
 * <table class="doxtable">
 *   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
 *   <tr>
 *     <td rowspan="1">1.3.0</td>
 *     <td>Add support for the WRITE_PROTECT, ACCESS_CONTROL and PASSKEY commands.</td>
 *     <td>New functionality</td>
 *   </tr>
 *   <tr>
 *     <td rowspan="4">1.2.0</td>
 *     <td>
 *       <ul>
 *         <li><b>Extended Commands Support</b>
 *         (see \ref extended-commands-support)
 *           <ul>
 *             <li> \ref MTB_PMBUS_SUPPORT_EXT_CMD </li>
 *             <li> \ref MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT </li>
 *             <li> \ref mtb_pmbus_cmd_ext_enable_disable </li>
 *           </ul>
 *         </li>
 *         <li><b>Support TGS timeout handling</b>
 *         (see \ref timeout-handling)
 *           <ul>
 *             <li> \ref MTB_PMBUS_HAL_USE_TGS </li>
 *           </ul>
 *         </li>
 *         <li><b>New pre-implemented commands</b>
 *         (see \ref implemented-commands)
 *           <ul>
 *             <li> \ref MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE </li>
 *             <li> \ref MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ </li>
 *             <li> \ref MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE </li>
 *             <li> \ref MTB_PMBUS_IMPL_CMD_P2_PLUS_READ </li>
 *             <li> \ref MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_EN </li>
 *             <li> \ref MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_EN </li>
 *             <li> \ref MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_EN </li>
 *             <li> \ref MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_EN </li>
 *           </ul>
 *         </li>
 *         <li><b>Host Notify Protocol support</b>
 *         (see \ref host-notify-protocol)
 *           <ul>
 *             <li> \ref mtb_pmbus_send_host_notify </li>
 *             <li> \ref mtb_pmbus_handle_host_notify_t </li>
 *           </ul>
 *         </li>
 *         <li><b>Exit ISR feature:</b>
 *           <ul>
 *             <li> \ref mtb_pmbus_cmd_exit_isr </li>
 *             <li> \ref mtb_pmbus_cmd_complete_transfer </li>
 *             <li> \ref mtb_pmbus_cmd_process_call_wr_done </li>
 *           </ul>
 *         </li>
 *         <li>New Zone event \ref MTB_PMBUS_ZONE_READ_STATUS_ARB_LOST </li>
 *         <li>The return value of the command callback is now handled for the \ref
 * MTB_PMBUS_CMD_MATCH event.</li>
 *       </ul>
 *     </td>
 *     <td>Expanding functionality</td>
 *   </tr>
 *   <tr>
 *     <td>
 *       <ul>
 *         <li>Fixed mixing of the Zone Read command and status events.</li>
 *         <li>Fixed the Zone Read issue when using the block read protocol.</li>
 *         <li>Fixed the update of a missing page after an arbitration lost during the Zone Read
 * transfer.</li>
 *         <li>Fixed the issue when the next command after Zone Read is NACKed.</li>
 *         <li>The Arbitration Lost error does not occur during Zone Read.</li>
 *         <li>Fixed the silicon freeze when a command callback is not implemented.</li>
 *         <li>Fixed the issue: \ref MTB_PMBUS_STATUS_CMD_IS_ACTIVE is not cleared.</li>
 *         <li>Added validation to ensure the runtime PAGES and PHASES settings match the command
 * capability settings.</li>
 *       </ul>
 *     </td>
 *     <td>Bug fixing</td>
 *   </tr>
 *   <tr>
 *     <td>
 *       <ul>
 *         <li>Updated section "Quick Start Guide"</li>
 *         <li>Added a diagram for the Events visualization for the Zone Read command transfer.</li>
 *         <li>General documentation improvements</li>
 *       </ul>
 *     </td>
 *     <td>Documentation enhancements</td>
 *   </tr>
 *   <tr>
 *     <td>
 *       <ul>
 *         <li>Controller Mode</li>
 *         <li>Solution personality and PMBus-Configurator support</li>
 *       </ul>
 *     </td>
 *     <td>New functionality</td>
 *   </tr>
 *   <tr>
 *     <td rowspan="1">1.0.0</td>
 *     <td>Initial version</td>
 *     <td></td>
 *   </tr>
 * </table>
 *
 */

#ifndef MTB_PMBUS_H
#define MTB_PMBUS_H

#include "mtb_pmbus_trgt.h"
#include "mtb_pmbus_ctrl.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*                          Middleware Version
*******************************************************************************/

/** PMBus middleware major version */
#define MTB_PMBUS_MW_VERSION_MAJOR                (1U)

/** PMBus middleware minor version */
#define MTB_PMBUS_MW_VERSION_MINOR                (3U)

/** PMBus middleware version */
#define MTB_PMBUS_MW_VERSION                      (13U)


/**
 * \addtogroup group_pmbus_helper_func
 * \{
 */

/**
 * @brief Converts the argument "linear11" to a floating point and returns it.
 *
 * @param linear_11 A number in the LINEAR11 format.
 * @return float32_t The LINEAR11 parameter converted to a floating point.
 */
float32_t mtb_pmbus_lin11_to_float(uint16_t linear_11);

/**
 * @brief Takes the argument "floatvar" (a floating point number) and converts
 * it to a 16-bit LINEAR11 value (11-bit mantissa + 5-bit exponent), which it returns.
 *
 * @param float_var  A floating point number.
 * @return uint16_t float_var converted to LINEAR11
 *
 * @note Conversion to LINEAR11 may result in the loss of precision, because
 * LINEAR11 uses less bits to represent the floating point number and mantissa is not normalized.
 */
uint16_t mtb_pmbus_float_to_lin11(float32_t float_var);

/**
 * @brief Converts argument LINEAR16 to a floating point and returns it.
 *
 * @param linear_16 The 16-bit mantissa of a LINEAR16 number.
 * @param int_exp The 5-bit exponent of a LINEAR16 number. Packed in the
 * lower 5 bits. 2's Complement.
 * @return float32_t The parameters converted to a floating point.
 */
float32_t mtb_pmbus_lin16_to_float(uint16_t linear_16, int8_t int_exp);

/**
 * @brief Takes the argument "floatvar" (a floating point number) and converts
 * it to a 16-bit LINEAR16 value (16-bit mantissa), which it returns.
 *
 * @param float_var A floating point number to be converted to LINEAR16.
 * @param out_exp User-provided 5-bit exponent to use in the conversion.
 * @return uint16_t The parameters converted to LINEAR16.
 *
 * @note Conversion to LINEAR16 may result in loss of precision, because
 * LINEAR16 use less bits to represent the floating point number and mantissa is not normalized.
 * @note Providing mismatched exponent lead to incorrect conversion:
 * @code
 *      Incorrect conversion examples:
 *      float_var = 10.5f, out_exp = 0     -> 0x000B LIN16 mantissa -> 11.0f when converted back to
 * float with the same exponent.
 *      float_var = 65535.0f, out_exp = -1 -> 0xFFFE LIN16 mantissa -> 32767.0f when converted back
 * to float with the same exponent.
 *      Correct conversion examples:
 *      float_var = 10.5f, out_exp = -1    -> 0x0015 LIN16 mantissa -> 10.5f when converted back to
 * float with the same exponent.
 *      float_var = 65535.0f, out_exp = 0  -> 0xFFFF LIN16 mantissa -> 65535.0f when converted back
 * to float with the same exponent.
 * @endcode
 */
uint16_t mtb_pmbus_float_to_lin16(float32_t float_var, int8_t out_exp);

/** \} group_pmbus_helper_func */

#ifdef __cplusplus
}
#endif
#endif /* MTB_PMBUS_H */
