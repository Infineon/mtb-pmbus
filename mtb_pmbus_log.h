/***************************************************************************//**
* \file mtb_pmbus_log.h
* \version 1.0
*
* Provides API declarations for logging.
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

#ifndef MTB_PMBUS_LOG_H
#define MTB_PMBUS_LOG_H

#include <stdio.h>
#include <inttypes.h>
#include "mtb_pmbus_log_level.h"

#ifdef __cplusplus
extern "C"{
#endif

#if (defined (MTB_PMBUS_CUSTOM_LOG) && (MTB_PMBUS_CUSTOM_LOG != 0U))
    #ifndef MTB_PMBUS_LOG_BUF
        #define     MTB_PMBUS_LOG_BUF      (128U)
    #endif /* MTB_PMBUS_LOG_BUF */
    extern char mtb_pmbus_msg[MTB_PMBUS_LOG_BUF];
    void mtb_pmbus_log(const char* msg);
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

#if MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_ERROR
    #define MTB_PMBUS_LOG_ERR(_fmt, ...)                               \
        MTB_PMBUS_LOG_WRITE("[PMBUS_ERR] " _fmt "\n\r", ##__VA_ARGS__)
#else
    #define MTB_PMBUS_LOG_ERR(...)
#endif /* MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_ERROR */

#if MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_WARNING
    #define MTB_PMBUS_LOG_WRN(_fmt, ...)                               \
        MTB_PMBUS_LOG_WRITE("[PMBUS_WRN] " _fmt "\n\r", ##__VA_ARGS__)
#else
    #define MTB_PMBUS_LOG_WRN(...)
#endif /* MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_WARNING */

#if MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_INFO
    #define MTB_PMBUS_LOG_INF(_fmt, ...)                               \
        MTB_PMBUS_LOG_WRITE("[PMBUS_INF] " _fmt "\n\r", ##__VA_ARGS__)
#else
    #define MTB_PMBUS_LOG_INF(...)
#endif /* MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_INFO */

#if MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_DEBUG
    #define MTB_PMBUS_LOG_DBG(_fmt, ...)                               \
        MTB_PMBUS_LOG_WRITE("[PMBUS_DBG] " _fmt "\n\r", ##__VA_ARGS__)
#else
    #define MTB_PMBUS_LOG_DBG(...)
#endif /* MTB_PMBUS_LOG_LEVEL >= MTB_PMBUS_LOG_LEVEL_DEBUG */

#ifdef __cplusplus
}
#endif


#endif /* MTB_PMBUS_LOG_H */
