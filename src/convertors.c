#include "decimal.h"

/*---------------------------------------------------------------------------
            FROM_INT_TO_DECIMAL conversion from int to decimal
 ---------------------------------------------------------------------------*/
int from_int_to_decimal(int src, decimal *dst) {
    status_code status = SUCCESS;
    if (dst != NULL) {
        decimal_initialization(dst);
        if (src < 0) {
            set_sign(dst, minus);
            src = -src;
        }
        dst->bits[LOW] = src;
    } else {
        status = CONVERTING_ERROR;
    }
    return status;
}

/*---------------------------------------------------------------------------
            FROM_DECIMAL_TO_INT conversion from decimal to int
 ---------------------------------------------------------------------------*/
int from_decimal_to_int(decimal src, int *dst) {
    status_code status = SUCCESS;
    if (dst == NULL) {
        status = CONVERTING_ERROR;
    } else {
        decimal result;
        truncate(src, &result);
        if (result.bits[MID] == 0 && result.bits[HIGH] == 0 && (result.bits[LOW] < (1u << 31))) {
            *dst = (int) result.bits[LOW];
            if (get_sign(src)) *dst = -(*dst);
        } else {
            status = CONVERTING_ERROR;
        }
    }
    return status;
}

/*---------------------------------------------------------------------------
            FROM_FLOAT_TO_DECIMAL conversion from float to decimal
 ---------------------------------------------------------------------------*/
int from_float_to_decimal(float src, decimal *dst) {
    status_code status = SUCCESS;
    if (dst == NULL || isnan(src)) status = CONVERTING_ERROR;
    decimal_initialization(dst);
    int exp = get_float_scale(src);
    bit sign = get_bit_float(src, 31);
    if (exp > 95) {
        status = CONVERTING_ERROR;
    } else if (exp > -95) {
        if (sign) src *= -1;
        bit float_bits = *((bit *)&src);
        int mask = 0x400000;
        double binary = pow(2, exp);
        for (int index = exp - 1; mask; mask >>= 1, index--) {
            if (float_bits & mask) binary += pow(2, index);
        }
        int scale = 0;
        while (src < 1) {
            src *= 10;
            scale++;
        }
        while (src > 10) {
            src /= 10;
            scale--;
        }
        binary *= pow(10, 8 + scale);
        if (scale > 0) {
            while (binary < 10000000) {
                binary *= 10;
            }
        }
        long int res = round(binary);
        int cloud = 0;
        while (res >= 10000000) {
            cloud = res % 10;
            res /= 10;
        }
        while (scale > 22) {
            cloud = res % 10;
            res /= 10;
            scale--;
        }
        if (cloud > 4) res++;
        while (res % 10 == 0) {
            res /= 10;
            scale--;
        }
        from_int_to_decimal((int)res, dst);
        while (scale < -6) {
            *dst = mul_10(*dst);
            scale++;
        }
        set_sign(dst, sign);
        set_scale(dst, scale + 6);
    }
    return status;
}

/*---------------------------------------------------------------------------
            FROM_DECIMAL_TO_FLOAT conversion from decimal to float
 ---------------------------------------------------------------------------*/
int from_decimal_to_float(decimal src, float *dst) {
    status_code status = SUCCESS;
    if (dst == NULL) {
        status = CONVERTING_ERROR;
    } else {
        scale exp = get_scale(src);
        double tmp = 0.0;
        for (int i = 0; i < 96; i++) {
            if (get_bit(src, i)) {
                tmp += pow(2, i);
            }
        }
        while (exp) {
            tmp /= 10.0;
            exp--;
        }
        *dst = (float) tmp;
        if (get_sign(src)) *dst = -(*dst);
    }
    return status;
}
