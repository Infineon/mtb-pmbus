/***************************************************************************//**
* \file mtb_pmbus_log.h
* \version 1.0
*
* Provides API declarations for logging.
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

#ifndef MTB_PMBUS_LOG_H
#define MTB_PMBUS_LOG_H

#include <stdio.h>
#include <inttypes.h>
#include "mtb_pmbus_log_level.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (defined (MTB_PMBUS_CUSTOM_LOG) && (MTB_PMBUS_CUSTOM_LOG != 0U))
    #ifndef MTB_PMBUS_LOG_BUF
        #define     MTB_PMBUS_LOG_BUF      (128U)
    #endif /* MTB_PMBUS_LOG_BUF */
extern char mtb_pmbus_msg[MTB_PMBUS_LOG_BUF];
void mtb_pmbus_log(const char *msg);
#endif /* (defined (MTB_PMBUS_CUSTOM_LOG) && (MTB_PMBUS_CUSTOM_LOG != 0U)) */

#if (defined (MTB_PMBUS_CUSTOM_LOG) && (MTB_PMBUS_CUSTOM_LOG != 0U))
    #define MTB_PMBUS_LOG_WRITE(_fmt, ...)                 \
        do                                              \
        {                                               \
            sprintf(mtb_pmbus_msg, _fmt, ##__VA_ARGS__);   \
            mtb_pmbus_log(mtb_pmbus_msg);                     \
        } while (0 != 0)
#else
    #define MTB_PMBUS_LOG_WRITE(_fmt, ...)    \
        do                                 \
        {                                  \
            (void) printf(_fmt, ##__VA_ARGS__);   \
        } while (0 != 0)
#endif /* (defined (MTB_PMBUS_CUSTOM_LOG) && (MTB_PMBUS_CUSTOM_LOG != 0U)) */

/**
 * \addtogroup group_pmbus_macro_log
 * \{
 */

/** Logs an error message.
 *  Active when \ref MTB_PMBUS_LOG_LEVEL >= \ref MTB_PMBUS_LOG_LEVEL_ERROR.
 *  @param _fmt  printf-style format string
 *  @param ...   Optional arguments for the format string
 */
#if MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_ERROR
    #define MTB_PMBUS_LOG_ERR(_fmt, ...)                               \
        MTB_PMBUS_LOG_WRITE("[PMBUS_ERR] " _fmt "\n\r", ##__VA_ARGS__)
#else
    #define MTB_PMBUS_LOG_ERR(...)
#endif /* MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_ERROR */

/** Logs a warning message.
 *  Active when \ref MTB_PMBUS_LOG_LEVEL >= \ref MTB_PMBUS_LOG_LEVEL_WARNING.
 *  @param _fmt  printf-style format string
 *  @param ...   Optional arguments for the format string
 */
#if MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_WARNING
    #define MTB_PMBUS_LOG_WRN(_fmt, ...)                               \
        MTB_PMBUS_LOG_WRITE("[PMBUS_WRN] " _fmt "\n\r", ##__VA_ARGS__)
#else
    #define MTB_PMBUS_LOG_WRN(...)
#endif /* MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_WARNING */

/** Logs an informational message.
 *  Active when \ref MTB_PMBUS_LOG_LEVEL >= \ref MTB_PMBUS_LOG_LEVEL_INFO.
 *  @param _fmt  printf-style format string
 *  @param ...   Optional arguments for the format string
 */
#if MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_INFO
    #define MTB_PMBUS_LOG_INF(_fmt, ...)                               \
        MTB_PMBUS_LOG_WRITE("[PMBUS_INF] " _fmt "\n\r", ##__VA_ARGS__)
#else
    #define MTB_PMBUS_LOG_INF(...)
#endif /* MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_INFO */

/** Logs a debug message.
 *  Active when \ref MTB_PMBUS_LOG_LEVEL >= \ref MTB_PMBUS_LOG_LEVEL_DEBUG.
 *  @param _fmt  printf-style format string
 *  @param ...   Optional arguments for the format string
 */
#if MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_DEBUG
    #define MTB_PMBUS_LOG_DBG(_fmt, ...)                               \
        MTB_PMBUS_LOG_WRITE("[PMBUS_DBG] " _fmt "\n\r", ##__VA_ARGS__)
#else
    #define MTB_PMBUS_LOG_DBG(...)
#endif /* MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_DEBUG */

/** \} group_pmbus_macro_log */

#ifdef __cplusplus
}
#endif
#endif /* MTB_PMBUS_LOG_H */
