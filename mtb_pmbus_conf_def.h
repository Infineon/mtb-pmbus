/***************************************************************************//**
* \file mtb_pmbus_conf_def.h
* \version 1.0
*
* Default compile time options configurations.
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

#ifndef MTB_PMBUS_CONF_DEF_H
#define MTB_PMBUS_CONF_DEF_H

/**
* \addtogroup group_pmbus_macro
* \{
*/

/**
* \defgroup group_pmbus_macro_conf Compile Time Options
* \{
*/

#include "mtb_pmbus_log_level.h"
#include "mtb_pmbus_conf.h"

#if !defined (MTB_PMBUS_LOG_LEVEL) || defined (MTB_PMBUS_DOXYGEN)
    /** Select the Log Level
     *
     * Default: \ref MTB_PMBUS_LOG_LEVEL_WARNING
     */
    #define MTB_PMBUS_LOG_LEVEL                 (MTB_PMBUS_LOG_LEVEL_WARNING)
#endif /* #ifndef (MTB_PMBUS_LOG_LEVEL) */

#if !defined (MTB_PMBUS_CUSTOM_LOG) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable the custom log
     *
     * Default: 0U (Disabled)
     */
    #define MTB_PMBUS_CUSTOM_LOG                (0U)
#endif /* #ifndef (MTB_PMBUS_CUSTOM_LOG) */

#if !defined (MTB_PMBUS_SUPPORT_PMBUS) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable PMBus support
     *
     * Default: 1U (Enabled)
     *
     * The APIs will be not available if this option is disabled:
     * - \ref mtb_pmbus_stc_config_t::enable_pmbus
     *
     * \note This option must be enabled to use Zone protocols (\ref MTB_PMBUS_SUPPORT_ZONE), 
     * Pages (\ref MTB_PMBUS_PAGES_NUM), 
     * Phases (\ref MTB_PMBUS_PHASES_NUM) or any implemented commands. If this option is disabled,
     * all these features must also be disabled (set to 0) to avoid compilation errors.
     */
    #define MTB_PMBUS_SUPPORT_PMBUS             (1U)
#endif /* #ifndef (MTB_PMBUS_SUPPORT_PMBUS) */

#if !defined (MTB_PMBUS_SUPPORT_ZONE) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable Zone Write and Zone Read support.
     *
     * Default: 1U (Enabled)
     *
     * The APIs will be not available if this option is disabled:
     * - \ref mtb_pmbus_stc_config_t::enable_zone
     * - \ref mtb_pmbus_stc_config_t::zone_callback
     * - \ref mtb_pmbus_get_read_zone
     * - \ref mtb_pmbus_get_write_zone
     * - \ref mtb_pmbus_get_active_zones
     * - \ref mtb_pmbus_set_default_zones
     * 
     */
    #define MTB_PMBUS_SUPPORT_ZONE              (1U)
#endif /* #ifndef (MTB_PMBUS_SUPPORT_ZONE) */

#if !defined (MTB_PMBUS_SUPPORT_SMBALERT) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable SMBus Alert support
     *
     * Default: 1U (Enabled)
     *
     * The APIs will be not available if this option is disabled:
     * - \ref mtb_pmbus_stc_config_t::enable_smbalert
     * - \ref mtb_pmbus_smbalert_config_mode
     * - \ref mtb_pmbus_smbalert_set_signal
     * - \ref mtb_pmbus_smbalert_clear_signal
     */
    #define MTB_PMBUS_SUPPORT_SMBALERT          (1U)
#endif /* #ifndef (MTB_PMBUS_SUPPORT_SMBALERT) */

#if !defined (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable General Call support
     *
     * Default: 1U (Enabled)
     *
     * The APIs will be not available if this option is disabled:
     * - \ref mtb_pmbus_stc_config_t::enable_gen_call_addr
     */
    #define MTB_PMBUS_SUPPORT_GEN_CALL_ADDR     (1U)
#endif /* #ifndef (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) */

#if !defined (MTB_PMBUS_SUPPORT_PEC) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable PEC
     *
     * Default: 1U (Enabled)
     *
     * The APIs will be not available if this option is disabled:
     * - \ref mtb_pmbus_stc_config_t::enable_pec
     */
    #define MTB_PMBUS_SUPPORT_PEC               (1U)
#endif /* #ifndef (MTB_PMBUS_SUPPORT_PEC) */

#if !defined (MTB_PMBUS_PAGES_NUM) || defined (MTB_PMBUS_DOXYGEN)
    /** Set the maximum number of pages. The valid range is 0x0-0x20.
     * If set to 0x0, the pages are disabled.
     *
     * Default: 0x10U
     *
     * The APIs will be not available if this option is disabled:
     * - \ref mtb_pmbus_stc_config_t::num_pages
     * - \ref mtb_pmbus_get_active_page
     */
    #define MTB_PMBUS_PAGES_NUM                 (0x10U)
#endif /* #ifndef (MTB_PMBUS_PAGES_NUM) */

#if !defined (MTB_PMBUS_PHASES_NUM) || defined (MTB_PMBUS_DOXYGEN)
    /** Set the maximum number of phases. The valid range is 0x0-0x80.
     * If set to 0x0, the phases are disabled.
     *
     * Default: 0x10U
     *
     * The APIs will be not available if this option is disabled:
     * - \ref mtb_pmbus_stc_config_t::num_phases
     * - \ref mtb_pmbus_get_active_phase
     */
    #define MTB_PMBUS_PHASES_NUM                (0x10U)
#endif /* #ifndef (MTB_PMBUS_PHASES_NUM) */

#if !defined (MTB_PMBUS_ENABLE_CONFIG_CHECK) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable the checking of configuration parameters (recommended
     * only for debug purposes). The checking of configuration parameters
     * is performed in the \ref mtb_pmbus_init function. Enabling this option
     * will increase the code size.
     *
     * Default: 1U (Enabled)
     *
     */
    #define MTB_PMBUS_ENABLE_CONFIG_CHECK       (1U)
#endif /* #ifndef (MTB_PMBUS_ENABLE_CONFIG_CHECK) */

#if !defined (MTB_PMBUS_CONFIG_FULL_CHECK) || defined (MTB_PMBUS_DOXYGEN)
    /** Define the level of the configuration parameter checking:
     *  0U: Minimal checking, only critical parameters (e.g., to avoid hard faults).
     *  1U: Full checking, includes all parameter validations.
     *
     * Default: 1U (Full checking)
     */
    #define MTB_PMBUS_CONFIG_FULL_CHECK         (1U)
#endif /* #ifndef MTB_PMBUS_CONFIG_FULL_CHECK */

#if !defined (MTB_PMBUS_ENABLE_TIMEOUT) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable the timeout error when the clock line is held low for 25 ms.
     * The timeout is a mandatory feature of the SMBus specification, so
     * it is recommended to disable it only during debugging.
     *
     * Default: 1U (Enabled)
     */
    #define MTB_PMBUS_ENABLE_TIMEOUT            (1U)
#endif /* #ifndef MTB_PMBUS_ENABLE_TIMEOUT */

#if !defined (MTB_PMBUS_ZONE_MAX_NUM) || defined (MTB_PMBUS_DOXYGEN)
    /** Define the range for Zone values. By default, the range
     * 0x00 - 0x7FU is used for normal active zones. But the range
     * can be extended by redefining this macro to cover the reserved for
     * PMBus product manufacturer's values up to 0xBF.
     *
     * Default: 0x7FU
     */
    #define MTB_PMBUS_ZONE_MAX_NUM              (0x7FU)
#endif /* #ifndef MTB_PMBUS_ZONE_MAX_NUM */

#if !defined (MTB_PMBUS_ZONE_SEL_ADV) || defined (MTB_PMBUS_DOXYGEN)
    /** When enabled, the application can control the zone selection
     * initiated by the controller using the Zone callback.
     *
     * The APIs will be not available if option is disabled:
     * - \ref mtb_pmbus_zone_events_t::MTB_PMBUS_ZONE_CONFIG_RD
     * - \ref mtb_pmbus_zone_events_t::MTB_PMBUS_ZONE_CONFIG_WR
     * - \ref mtb_pmbus_zone_events_t::MTB_PMBUS_ZONE_ACTIVE_RD
     * - \ref mtb_pmbus_zone_events_t::MTB_PMBUS_ZONE_ACTIVE_WR
     *
     * Default: 0U (Disabled)
     */
    #define MTB_PMBUS_ZONE_SEL_ADV              (0U)
#endif /* #ifndef MTB_PMBUS_ZONE_SEL_ADV */

#if !defined (MTB_PMBUS_IMPL_CMD_REVISION) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable the automatic response on the PMBUS_REVISION (0x98) command
     *
     * Default: 1U (Enabled)
     *
     * Also, enable this command in the run-time configuration:
      \ref mtb_pmbus_stc_config_t::impl_cmd_mask
     */
    #define MTB_PMBUS_IMPL_CMD_REVISION         (1U)
#endif /* #if !defined (MTB_PMBUS_IMPL_CMD_REVISION) */

#if !defined (MTB_PMBUS_IMPL_CMD_CAPABILITY) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable the automatic response on CAPABILITY (0x19) command
     *
     * Default: 1U (Enabled)
     *
     * Also, enable this command in the run-time configuration:
     * \ref mtb_pmbus_stc_config_t::impl_cmd_mask
     */
    #define MTB_PMBUS_IMPL_CMD_CAPABILITY       (1U)
#endif /* #if !defined (MTB_PMBUS_IMPL_CMD_CAPABILITY) */

#if !defined (MTB_PMBUS_IMPL_CMD_QUERY) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable automatic response on QUERY (0x1A) command
     *
     * Default: 1U (Enabled)
     *
     * Also, enable this command in the run-time configuration:
     * \ref mtb_pmbus_stc_config_t::impl_cmd_mask
     */
    #define MTB_PMBUS_IMPL_CMD_QUERY            (1U)
#endif /* #if !defined (MTB_PMBUS_IMPL_CMD_QUERY) */

#if !defined (MTB_PMBUS_IMPL_CMD_PAGE) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable automatic response on PAGE (0x00) command
     *
     * Default: 1U (Enabled)
     *
     * Also, enable this command in the run-time configuration:
     * \ref mtb_pmbus_stc_config_t::impl_cmd_mask
     *
     * \note \ref MTB_PMBUS_PAGES_NUM must be greater than 0 to use this command.
     */
    #define MTB_PMBUS_IMPL_CMD_PAGE             (1U)
#endif /* #if !defined (MTB_PMBUS_IMPL_CMD_PAGE) */

#if !defined (MTB_PMBUS_IMPL_CMD_PHASE) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable automatic response on PHASE (0x04) command
     *
     * Default: 1U (Enabled)
     *
     * Also, enable this command in the run-time configuration:
     * \ref mtb_pmbus_stc_config_t::impl_cmd_mask
     *
     * \note \ref MTB_PMBUS_PHASES_NUM must be greater than 0 to use this command.
     */
    #define MTB_PMBUS_IMPL_CMD_PHASE            (1U)
#endif /* #if !defined (MTB_PMBUS_IMPL_CMD_PHASE) */

#if !defined (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable automatic response on ZONE_CONFIG (0x07) command
     *
     * Default: 1U (Enabled)
     *
     * Also, enable this command in the run-time configuration:
     * \ref mtb_pmbus_stc_config_t::impl_cmd_mask
     *
     * \note \ref MTB_PMBUS_SUPPORT_ZONE must be enabled to use this command.
     */
    #define MTB_PMBUS_IMPL_CMD_ZONE_CONFIG      (1U)
#endif /* #if !defined (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) */

#if !defined (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable or disable automatic response on ZONE_ACTIVE (0x08) command
     *
     * Default: 1U (Enabled)
     *
     * Also, enable this command in the run-time configuration:
     * \ref mtb_pmbus_stc_config_t::impl_cmd_mask
     *
     * \note \ref MTB_PMBUS_SUPPORT_ZONE must be enabled to use this command.
     */
    #define MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE      (1U)
#endif /* #if !defined (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) */

/** \} group_pmbus_macro_conf */

/** \} group_pmbus_macro */

/* Number of implemented commands */
#define MTB_PMBUS_IMPL_CMD_NUM                   (MTB_PMBUS_IMPL_CMD_REVISION + MTB_PMBUS_IMPL_CMD_CAPABILITY + \
                                                  MTB_PMBUS_IMPL_CMD_QUERY + MTB_PMBUS_IMPL_CMD_PAGE + \
                                                  MTB_PMBUS_IMPL_CMD_PHASE + MTB_PMBUS_IMPL_CMD_ZONE_CONFIG + \
                                                  MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE)

/* Compile time options validation */
#if (MTB_PMBUS_PAGES_NUM > 0x20U)
    #error "MTB_PMBUS_PAGES_NUM must be in the range 0x0-0x20"
#endif /* #if (MTB_PMBUS_PAGES_NUM > 0x20U) */

#if (MTB_PMBUS_PHASES_NUM > 0x80U)
    #error "MTB_PMBUS_PHASES_NUM must be in the range 0x0-0x80"
#endif /* #if (MTB_PMBUS_PHASES_NUM > 0x80U) */

#if (MTB_PMBUS_ZONE_MAX_NUM > 0xBFU)
    #error "MTB_PMBUS_ZONE_MAX_NUM must be in the range 0x00-0xBF"
#endif /* #if (MTB_PMBUS_ZONE_MAX_NUM > 0xBFU) */

#if ((MTB_PMBUS_SUPPORT_PMBUS == 0U) && \
    ((MTB_PMBUS_SUPPORT_ZONE != 0U) || \
     (MTB_PMBUS_PAGES_NUM > 0U) || \
     (MTB_PMBUS_PHASES_NUM > 0U) || \
     (MTB_PMBUS_IMPL_CMD_NUM > 0U)))
    #error "MTB_PMBUS_SUPPORT_PMBUS must be enabled to use Zone protocols, Pages, Phases or implemented commands"
#endif /* #if (MTB_PMBUS_SUPPORT_PMBUS == 0U) && ... */

#if ((MTB_PMBUS_SUPPORT_ZONE == 0U) && \
    ((MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U) || \
     (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U)))
    #error "MTB_PMBUS_SUPPORT_ZONE must be enabled to use implemented commands for Zone protocols"
#endif /* #if (MTB_PMBUS_SUPPORT_ZONE == 0U) && ... */

#if ((MTB_PMBUS_PAGES_NUM == 0U) && \
     (MTB_PMBUS_IMPL_CMD_PAGE != 0U))
    #error "MTB_PMBUS_PAGES_NUM must be greater than 0 to use implemented commands for Pages"
#endif /* #if (MTB_PMBUS_PAGES_NUM == 0U) && ... */

#if ((MTB_PMBUS_PHASES_NUM == 0U) && \
     (MTB_PMBUS_IMPL_CMD_PHASE != 0U))
    #error "MTB_PMBUS_PHASES_NUM must be greater than 0 to use implemented commands for Phases"
#endif /* #if (MTB_PMBUS_PHASES_NUM == 0U) && ... */

#endif /* MTB_PMBUS_CONF_DEF_H */
