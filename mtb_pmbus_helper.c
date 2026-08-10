/***************************************************************************//**
* \file mtb_pmbus_helper.c
* \version 1.0
*
* Provides helper function implementations for the PMBus Middleware.
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

#include "mtb_pmbus.h"

/* Constants for the IEEE 754 floating point and LIN11/LIN16 formats */
#define MTB_PMBUS_FLOAT_EXP_SHIFT             (23U)
#define MTB_PMBUS_IEEE754_IMPLICIT_MAGNITUDE  (127)
#define MTB_PMBUS_LIN_EXP_SIGN_BIT            (0x10U)
#define MTB_PMBUS_LIN_EXP_SIGN_EXT            (0xE0U)

#define MTB_PMBUS_LIN11_EXP_SHIFT             (11U)
#define MTB_PMBUS_LIN11_EXP_MASK              (0x1FU)
#define MTB_PMBUS_LIN11_MANT_MASK             (0x7FFU)
#define MTB_PMBUS_LIN11_MANT_SIGN_BIT         (0x400U)
#define MTB_PMBUS_LIN11_MANT_SIGN_EXT         (0xF800U)
#define MTB_PMBUS_LIN11_FLOAT_10MSB_MAN_SHIFT (14U)
#define MTB_PMBUS_LIN11_FLOAT_10MSB_MAN_MASK  (0x03FFU)
#define MTB_PMBUS_LIN11_FLOAT_10MSB_MAN_LEAD  (0x0200U)
#define MTB_PMBUS_LIN11_EXP_SCALING_FACTOR    (9)
#define MTB_PMBUS_LIN11_EXP_MIN               (-16)

#define MTB_PMBUS_LIN16_FLOAT_16MSB_MAN_SHIFT (8U)
#define MTB_PMBUS_LIN16_FLOAT_16MSB_MAN_LEAD  (0x8000U)
#define MTB_PMBUS_LIN16_EXP_SCALING_FACTOR    (15)
#define MTB_PMBUS_LIN16_MAN_MAX_VAL           (0xFFFFU)


float32_t mtb_pmbus_lin11_to_float(uint16_t linear_11)
{
    uint8_t exp_raw;
    uint8_t sign_ext_exp;
    int8_t exponent;
    uint16_t mant_raw;
    uint16_t sign_ext_mant;
    int16_t mantissa;
    float32_t value;
    uint32_t shift;
    uint32_t pow2;

    /* Extract and sign-extend exponent */
    exp_raw = (uint8_t)((linear_11 >> MTB_PMBUS_LIN11_EXP_SHIFT) & MTB_PMBUS_LIN11_EXP_MASK);
    if ((exp_raw & (uint8_t)MTB_PMBUS_LIN_EXP_SIGN_BIT) != 0U)
    {
        sign_ext_exp = exp_raw | (uint8_t)MTB_PMBUS_LIN_EXP_SIGN_EXT;
        exponent = (int8_t)sign_ext_exp;
    }
    else
    {
        exponent = (int8_t)exp_raw;
    }

    /* Extract and sign-extend mantissa */
    mant_raw = (uint16_t)(linear_11 & MTB_PMBUS_LIN11_MANT_MASK);
    if ((mant_raw & (uint16_t)MTB_PMBUS_LIN11_MANT_SIGN_BIT) != 0U)
    {
        sign_ext_mant = mant_raw | (uint16_t)MTB_PMBUS_LIN11_MANT_SIGN_EXT;
        mantissa = (int16_t)sign_ext_mant;
    }
    else
    {
        mantissa = (int16_t)mant_raw;
    }

    /* Calculate the value */
    value = (float32_t)mantissa;
    if (exponent >= 0)
    {
        shift = (uint32_t)exponent;
        pow2 = (1UL << shift);
        value *= (float32_t)pow2;
    }
    else
    {
        shift = (uint32_t)(-exponent);
        pow2 = (1UL << shift);
        value /= (float32_t)pow2;
    }
    return value;
}


uint16_t mtb_pmbus_float_to_lin11(float32_t float_var)
{
    uint16_t mantissa;
    uint8_t raw_exp;
    int16_t exponent;
    uint16_t ret_val;
    uint32_t var_32;

    /* var_32 = floating point number loaded as raw uint32_t */
    (void)memcpy((void *)&var_32, (const void *)&float_var, sizeof(float32_t));

    if (0UL == var_32)
    {
        ret_val = 0U;
    }
    else
    {
        /* Get top 10 bits of mantissa and restore suppressed leading "1" to make 11 bits. */
        mantissa =
            (uint16_t)(((var_32 >>
                         MTB_PMBUS_LIN11_FLOAT_10MSB_MAN_SHIFT) & MTB_PMBUS_LIN11_FLOAT_10MSB_MAN_MASK) |
                       MTB_PMBUS_LIN11_FLOAT_10MSB_MAN_LEAD);
        raw_exp = (uint8_t)(var_32 >> MTB_PMBUS_FLOAT_EXP_SHIFT);

        /* Convert to "true" signed exponent by subtracting 127 */
        exponent = (int16_t)raw_exp;
        exponent -= MTB_PMBUS_IEEE754_IMPLICIT_MAGNITUDE;

        /* Linear 11 assumes decimal to far right, so need to decrease exponent by 2^9 */
        exponent -= MTB_PMBUS_LIN11_EXP_SCALING_FACTOR;

        /* If exponent is < -16, increase it to avoid exceeding 5-bit signed minimum by
           right-shifting mantissa */
        while (exponent < MTB_PMBUS_LIN11_EXP_MIN)
        {
            ++exponent;
            mantissa >>= 1U;
        }

        if (float_var < 0.0f)
        {
            mantissa = (uint16_t)(((mantissa ^ MTB_PMBUS_LIN11_MANT_MASK) + 1U) & MTB_PMBUS_LIN11_MANT_MASK);
        }

        /* Assembly linear11 from exponent and mantissa */
        raw_exp = (uint8_t)exponent & MTB_PMBUS_LIN11_EXP_MASK;
        ret_val  = (uint16_t)raw_exp << MTB_PMBUS_LIN11_EXP_SHIFT;
        ret_val |= (mantissa & MTB_PMBUS_LIN11_MANT_MASK);
    }

    return ret_val;
}


float32_t mtb_pmbus_lin16_to_float(uint16_t linear_16, int8_t int_exp)
{
    int8_t exponent_tmp;
    uint8_t int_exp_u8;
    uint8_t ext;
    float32_t value;
    uint32_t shift;
    uint32_t pow2;

    /* Sign-extend exponent */
    int_exp_u8 = (uint8_t)int_exp;
    if ((int_exp_u8 & (uint8_t)MTB_PMBUS_LIN_EXP_SIGN_BIT) != 0U)
    {
        ext = int_exp_u8 | (uint8_t)MTB_PMBUS_LIN_EXP_SIGN_EXT;
        exponent_tmp = (int8_t)ext;
    }
    else
    {
        exponent_tmp = int_exp;
    }

    /* Calculate the value */
    value = (float32_t)linear_16;
    if (exponent_tmp >= 0)
    {
        shift = (uint32_t)exponent_tmp;
        pow2 = (1UL << shift);
        value *= (float32_t)pow2;
    }
    else
    {
        shift = (uint32_t)(-exponent_tmp);
        pow2 = (1UL << shift);
        value /= (float32_t)pow2;
    }
    return value;
}


uint16_t mtb_pmbus_float_to_lin16(float32_t float_var, int8_t out_exp)
{
    uint16_t mantissa;
    uint8_t raw_exp;
    int16_t exponent;
    uint8_t round_off;
    uint32_t var_32;
    int8_t out_exp_tmp = out_exp;

    /* var_32 = floating point number loaded as raw uint32_t */
    (void)memcpy((void *)&var_32, (const void *)&float_var, sizeof(float32_t));

    if (0UL == var_32)
    {
        mantissa = 0U;
    }
    else
    {
        /* Get top 15 bits of mantissa and restore suppressed leading "1" to make 16 bits. */
        mantissa = (uint16_t)((var_32 >> MTB_PMBUS_LIN16_FLOAT_16MSB_MAN_SHIFT) | MTB_PMBUS_LIN16_FLOAT_16MSB_MAN_LEAD);
        raw_exp = (uint8_t)(var_32 >> MTB_PMBUS_FLOAT_EXP_SHIFT);

        /* Convert to "true" signed exponent by subtracting 127 */
        exponent = (int16_t)raw_exp;
        exponent -= MTB_PMBUS_IEEE754_IMPLICIT_MAGNITUDE;

        /* Linear 16 assumes decimal to far right, so need to decrease exponent by 2^15 */
        exponent -= MTB_PMBUS_LIN16_EXP_SCALING_FACTOR;

        round_off = 0U;

        /* Extend exponent sign to 8 bits */
        raw_exp = (uint8_t)out_exp_tmp;
        if ((raw_exp & MTB_PMBUS_LIN_EXP_SIGN_BIT) != 0U)
        {
            raw_exp |= MTB_PMBUS_LIN_EXP_SIGN_EXT;
        }
        out_exp_tmp = (int8_t)raw_exp;

        /* Adjust mantissa to render caller-specified exponent */
        while (exponent > out_exp_tmp)
        {
            --exponent;
            mantissa <<= 1U;
        }
        while (exponent < out_exp_tmp)
        {
            ++exponent;
            round_off = (uint8_t)mantissa & 0x01U;
            mantissa >>= 1U;
        }
        if (mantissa != MTB_PMBUS_LIN16_MAN_MAX_VAL)
        {
            mantissa += round_off;
        }
    }
    return mantissa;
}
