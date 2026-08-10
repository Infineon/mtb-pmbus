/***************************************************************************//**
* \file mtb_pmbus_trgt_cmd_conf.h
* \version 1.0
*
* Pre-implemented commands info for PMBUS Target.
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

#ifndef MTB_PMBUS_TRGT_CMD_CONF_H
#define MTB_PMBUS_TRGT_CMD_CONF_H

#include "mtb_pmbus.h"

#if (defined (MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U))
    #define MTB_PMBUS_IMPL_CMD_REVISION_CODE    (0x98U)
    #define MTB_PMBUS_IMPL_CMD_REVISION_CAP     (MTB_PMBUS_CMD_CAP_DIR_RD)
    #define MTB_PMBUS_IMPL_CMD_REVISION_SIZE    (1U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U))
    #define MTB_PMBUS_IMPL_CMD_CAPABILITY_CODE  (0x19U)
    #define MTB_PMBUS_IMPL_CMD_CAPABILITY_CAP   (MTB_PMBUS_CMD_CAP_DIR_RD)
    #define MTB_PMBUS_IMPL_CMD_CAPABILITY_SIZE  (1U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U))
    #define MTB_PMBUS_IMPL_CMD_QUERY_CODE       (0x1AU)
    #define MTB_PMBUS_IMPL_CMD_QUERY_CAP        (MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL | MTB_PMBUS_CMD_CAP_BLOCK)
    #define MTB_PMBUS_IMPL_CMD_QUERY_SIZE       (2U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_QUERY) && (MTB_PMBUS_IMPL_CMD_QUERY != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U))
    #define MTB_PMBUS_IMPL_CMD_PAGE_CODE        (0x00U)
    #define MTB_PMBUS_IMPL_CMD_PAGE_CAP         (MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_DIR_WR)
    #define MTB_PMBUS_IMPL_CMD_PAGE_SIZE        (1U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_PAGE) && (MTB_PMBUS_IMPL_CMD_PAGE != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U))
    #define MTB_PMBUS_IMPL_CMD_PHASE_CODE       (0x04U)
    #define MTB_PMBUS_IMPL_CMD_PHASE_CAP        (MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_DIR_WR)
    #define MTB_PMBUS_IMPL_CMD_PHASE_SIZE       (1U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_PHASE) && (MTB_PMBUS_IMPL_CMD_PHASE != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U))
    #define MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_CODE (0x07U)
    #define MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_CAP  (MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD)
    #define MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_SIZE (2U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG) && (MTB_PMBUS_IMPL_CMD_ZONE_CONFIG != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U))
    #define MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_CODE (0x08U)
    #define MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_CAP  (MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD)
    #define MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_SIZE (2U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE) && (MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE != 0U))
    #define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_CODE (0x05U)
    #define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_CAP  (MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK)
    #define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_SIZE (255U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ != 0U))
    #define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_CODE  (0x06U)
    #define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_CAP   (MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL | MTB_PMBUS_CMD_CAP_BLOCK)
    #define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_SIZE  (255U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE != 0U))
    #define MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_CODE   (0x09U)
    #define MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_CAP    (MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK)
    #define MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_SIZE   (255U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE != 0U)) */

#if (defined (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ != 0U))
    #define MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_CODE    (0x0AU)
    #define MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_CAP     (MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL | MTB_PMBUS_CMD_CAP_BLOCK)
    #define MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_SIZE    (255U)
#endif /* #if (defined (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ) && (MTB_PMBUS_IMPL_CMD_P2_PLUS_READ != 0U)) */
#endif /* MTB_PMBUS_TRGT_CMD_CONF_H */
