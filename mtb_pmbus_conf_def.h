/***************************************************************************//**
* \file mtb_pmbus_conf_def.h
* \version 1.0
*
* Default compile time options configurations.
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

#ifndef MTB_PMBUS_CONF_DEF_H
#define MTB_PMBUS_CONF_DEF_H

/**
 * \addtogroup group_pmbus_common_macro_conf
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

#if !defined (MTB_PMBUS_ENABLE_TIMEOUT) || defined (MTB_PMBUS_DOXYGEN)
/** Enable the timeout error when the clock line is held low for 25 ms.
 * The timeout is a mandatory feature of the SMBus specification, so
 * it is recommended to disable it only during debugging.
 *
 * Default: 1U (Enabled)
 */
    #define MTB_PMBUS_ENABLE_TIMEOUT            (1U)
#endif /* #ifndef MTB_PMBUS_ENABLE_TIMEOUT */

/** \} group_pmbus_common_macro_conf */

/**
 * \addtogroup group_pmbus_trgt_macro_conf
 * \{
 */
#if !defined (MTB_PMBUS_SUPPORT_PMBUS) || defined (MTB_PMBUS_DOXYGEN)
/** Enable or disable PMBus support
 *
 * Default: 1U (Enabled)
 *
 * The APIs will be not available if this option is disabled:
 * - \ref mtb_pmbus_stc_config_t::enable_pmbus
 *
 * \note This option must be enabled to use Zone protocols (\ref MTB_PMBUS_SUPPORT_ZONE),
 * Extended commands (\ref MTB_PMBUS_SUPPORT_EXT_CMD), Pages (\ref MTB_PMBUS_PAGES_NUM),
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

#if !defined (MTB_PMBUS_SUPPORT_HOST_NOTIFY) || defined (MTB_PMBUS_DOXYGEN)
/** Enable or disable Host Notify Protocol
 *
 * Default: 0U (Disabled)
 *
 * The APIs will be not available if this option is disabled:
 * - \ref mtb_pmbus_send_host_notify
 * - \ref mtb_pmbus_stc_config_t::hnp_callback
 */
    #define MTB_PMBUS_SUPPORT_HOST_NOTIFY          (0U)
#endif /* #ifndef (MTB_PMBUS_SUPPORT_HOST_NOTIFY) */

#if !defined (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) || defined (MTB_PMBUS_DOXYGEN)
/** Enable or disable Host Notify Protocol to support Controller Host mode
 *
 * Default: 0U (Disabled)
 *
 * The APIs will be not available if this option is disabled:
 * - \ref mtb_pmbus_ctrl_hnp_get_trgt_addr
 * - \ref mtb_pmbus_ctrl_hnp_get_trgt_data
 * - \ref mtb_pmbus_ctrl_cfg_t::callback_hnp
 */
    #define MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY          (0U)
#endif /* #ifndef (MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY) */

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

#if !defined (MTB_PMBUS_SUPPORT_EXT_CMD) || defined (MTB_PMBUS_DOXYGEN)
/** Enable or disable Extended commands
 *
 * Default: 0U (Disabled)
 *
 * The APIs will be not available if this option is disabled:
 * - \ref mtb_pmbus_stc_config_t::enable_ext_cmd
 * - \ref mtb_pmbus_stc_config_t::ext_cmd_table
 * - \ref mtb_pmbus_stc_config_t::ext_cmd_num
 * - \ref MTB_PMBUS_CONV_CMD_EXT_CMD_FORMAT
 *
 */
    #define MTB_PMBUS_SUPPORT_EXT_CMD           (0U)
#endif /* #ifndef (MTB_PMBUS_SUPPORT_EXT_CMD) */

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

#if !defined (MTB_PMBUS_SUPPORT_SECURITY) || defined (MTB_PMBUS_DOXYGEN)
/** Enable or disable PMBus Security feature support.
 *
 * When enabled, the Middleware enforces the Security Level defined by
 * \ref MTB_PMBUS_SEC_LEVEL and exposes the related APIs.
 *
 * Default: 0U (Disabled)
 */
    #define MTB_PMBUS_SUPPORT_SECURITY          (0U)
#endif /* #ifndef MTB_PMBUS_SUPPORT_SECURITY */

#if !defined (MTB_PMBUS_SEC_LEVEL) || defined (MTB_PMBUS_DOXYGEN)
/** Select the PMBus Security Level.
 *
 * Supported values:
 * - 0 — Security Level 0 (supported, except for the FW update)
 * - 1 — Security Level 1 (not currently supported)
 * - 2 — Security Level 2 (not currently supported)
 * - 3 — Security Level 3 (not currently supported)
 *
 * Default: 0U (Level 0)
 */
    #define MTB_PMBUS_SEC_LEVEL                 (0U)
#endif /* #ifndef MTB_PMBUS_SEC_LEVEL */

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

#if !defined (MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV) || defined (MTB_PMBUS_DOXYGEN)
/** When enabled, the application can control the page selection
 * initiated by the controller using the Page callback.
 *
 * The APIs will be not available if option is disabled:
 * - \ref mtb_pmbus_cmd_events_t::MTB_PMBUS_CMD_PAGE
 * - \ref mtb_pmbus_cmd_events_t::MTB_PMBUS_CMD_PAGE_PLUS
 * - \ref mtb_pmbus_cmd_events_t::MTB_PMBUS_CMD_PAGE_P2_PLUS
 * - \ref mtb_pmbus_stc_config_t::page_callback
 *
 * Default: 0U (Disabled)
 */
    #define MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV     (0U)
#endif /* #ifndef MTB_PMBUS_IMPL_CMD_PAGE_SEL_ADV */

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

#if !defined (MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV) || defined (MTB_PMBUS_DOXYGEN)
/** When enabled, the application can control the phase selection
 * initiated by the controller using the Phase callback.
 *
 * The APIs will be not available if option is disabled:
 * - \ref mtb_pmbus_cmd_events_t::MTB_PMBUS_CMD_PHASE
 * - \ref mtb_pmbus_cmd_events_t::MTB_PMBUS_CMD_PHASE_P2_PLUS
 * - \ref mtb_pmbus_stc_config_t::phase_callback
 *
 * Default: 0U (Disabled)
 */
    #define MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV    (0U)
#endif /* #ifndef MTB_PMBUS_IMPL_CMD_PHASE_SEL_ADV */

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

#if !defined (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) || defined (MTB_PMBUS_DOXYGEN)
/** When enabled, the command callback also provides a sequence number for
 * each received byte.
 *
 * This option only applies to the following event:
 * - \ref mtb_pmbus_cmd_events_t::MTB_PMBUS_CMD_WRITE_BYTE
 *
 * Default: 0U (Disabled)
 */
    #define MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV   (0U)
#endif /* #if !defined (MTB_PMBUS_ENABLE_CMD_CALLBACK_ADV) */

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

#if !defined (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE) || defined (MTB_PMBUS_DOXYGEN)
/** Enable or disable automatic response on PAGE_PLUS_WRITE (0x05) command
 *
 * Default: 1U (Enabled)
 *
 * Also, enable this command in the run-time configuration:
 * \ref mtb_pmbus_stc_config_t::impl_cmd_mask
 *
 * \note MTB_PMBUS_PAGES_NUM must be greater than 0 to use this command.
 */
    #define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE  (1U)
#endif /* #if !defined (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE) */

#if !defined (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ) || defined (MTB_PMBUS_DOXYGEN)
/** Enable or disable automatic response on PAGE_PLUS_READ (0x06) command
 *
 * Default: 1U (Enabled)
 *
 * Also, enable this command in the run-time configuration:
 * \ref mtb_pmbus_stc_config_t::impl_cmd_mask
 *
 * \note MTB_PMBUS_PAGES_NUM must be greater than 0 to use this command.
 */
    #define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ   (1U)
#endif /* #if !defined (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ) */

#if !defined (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE) || defined (MTB_PMBUS_DOXYGEN)
/** Enable or disable automatic response on P2_PLUS_WRITE (0x09) command
 *
 * Default: 1U (Enabled)
 *
 * Also, enable this command in the run-time configuration:
 * \ref mtb_pmbus_stc_config_t::impl_cmd_mask
 *
 * \note Both \ref MTB_PMBUS_PAGES_NUM and \ref MTB_PMBUS_PHASES_NUM must be greater than 0 to use
 * this command.
 */
    #define MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE    (1U)
#endif /* #if !defined (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE) */

#if !defined (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ) || defined (MTB_PMBUS_DOXYGEN)
/** Enable or disable automatic response on P2_PLUS_READ (0x0A) command
 *
 * Default: 1U (Enabled)
 *
 * Also, enable this command in the run-time configuration:
 * \ref mtb_pmbus_stc_config_t::impl_cmd_mask
 *
 * \note Both \ref MTB_PMBUS_PAGES_NUM and \ref MTB_PMBUS_PHASES_NUM must be greater than 0 to use
 * this command.
 */
    #define MTB_PMBUS_IMPL_CMD_P2_PLUS_READ     (1U)
#endif /* #if !defined (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ) */

/** \} group_pmbus_trgt_macro_conf */

/* Number of implemented commands */
#define MTB_PMBUS_IMPL_CMD_NUM                   (MTB_PMBUS_IMPL_CMD_REVISION + MTB_PMBUS_IMPL_CMD_CAPABILITY + \
                                                  MTB_PMBUS_IMPL_CMD_QUERY + MTB_PMBUS_IMPL_CMD_PAGE + \
                                                  MTB_PMBUS_IMPL_CMD_PHASE + MTB_PMBUS_IMPL_CMD_ZONE_CONFIG + \
                                                  MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE + \
                                                  MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE + MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ + \
                                                  MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE + MTB_PMBUS_IMPL_CMD_P2_PLUS_READ)

/* Internal macros for configuration conditions to improve readability */
#if ((defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE != 0U)) || \
    (defined(MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ != 0U)))
    #define MTB_PMBUS_IMPL_CMD_PAGE_PLUS            (1U)
#else
    #define MTB_PMBUS_IMPL_CMD_PAGE_PLUS            (0U)
#endif

#if ((defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE != 0U)) || \
    (defined(MTB_PMBUS_IMPL_CMD_P2_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ != 0U)))
    #define MTB_PMBUS_IMPL_CMD_P2_PLUS              (1U)
#else
    #define MTB_PMBUS_IMPL_CMD_P2_PLUS              (0U)
#endif

#define MTB_PMBUS_IMPL_CMD_PX_PLUS                  \
    ((MTB_PMBUS_IMPL_CMD_PAGE_PLUS != 0U) || (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U))


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
    (MTB_PMBUS_SUPPORT_EXT_CMD != 0U) || \
    (MTB_PMBUS_PAGES_NUM > 0U) || \
    (MTB_PMBUS_PHASES_NUM > 0U) || \
    (MTB_PMBUS_IMPL_CMD_NUM > 0U)))
    #error \
    "MTB_PMBUS_SUPPORT_PMBUS must be enabled to use Zone protocols, Extended commands, Pages, Phases or implemented commands"
#endif /* #if (MTB_PMBUS_SUPPORT_PMBUS == 0U) && ... */

#if ((MTB_PMBUS_SUPPORT_ZONE == 0U) && \
    ((MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U) || \
    (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U)))
    #error "MTB_PMBUS_SUPPORT_ZONE must be enabled to use implemented commands for Zone protocols"
#endif /* #if (MTB_PMBUS_SUPPORT_ZONE == 0U) && ... */

#if ((MTB_PMBUS_PAGES_NUM == 0U) && \
    ((MTB_PMBUS_IMPL_CMD_PAGE != 0U) || \
    (MTB_PMBUS_IMPL_CMD_PX_PLUS != 0U)))
    #error "MTB_PMBUS_PAGES_NUM must be greater than 0 to use implemented commands for Pages"
#endif /* #if (MTB_PMBUS_PAGES_NUM == 0U) && ... */

#if ((MTB_PMBUS_PHASES_NUM == 0U) && \
    ((MTB_PMBUS_IMPL_CMD_PHASE != 0U) || \
    (MTB_PMBUS_IMPL_CMD_P2_PLUS != 0U)))
    #error "MTB_PMBUS_PHASES_NUM must be greater than 0 to use implemented commands for Phases"
#endif /* #if (MTB_PMBUS_PHASES_NUM == 0U) && ... */

#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY != 0U)) && \
    (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC == 0U))
    #error "MTB_PMBUS_SUPPORT_SECURITY requires MTB_PMBUS_SUPPORT_PEC to be enabled"
#endif

#if (defined(MTB_PMBUS_SUPPORT_SECURITY) && (MTB_PMBUS_SUPPORT_SECURITY == 0U)) && \
    (defined(MTB_PMBUS_SEC_LEVEL) && (MTB_PMBUS_SEC_LEVEL != 0U))
    #error "MTB_PMBUS_SEC_LEVEL must be 0 when MTB_PMBUS_SUPPORT_SECURITY is disabled"
#endif
#endif /* MTB_PMBUS_CONF_DEF_H */
