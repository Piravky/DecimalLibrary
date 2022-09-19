#include "decimal.h"

/*---------------------------------------------------------------------------
            FLOOR rounds a specified Decimal number to the closest
            integer toward negative infinity
 ---------------------------------------------------------------------------*/
int decimal_floor(decimal value, decimal *result) {
    status_code status = OK;
    if (result == NULL) {
        status = CALCULATION_ERROR;
    } else {
        decimal_initialization(result);
        truncate(value, result);
        if (get_sign(value) && !is_equal(value, *result)) {
            decimal one = {{1, 0, 0, 0}};
            decimal_sub(*result, one, result);
        }
    }
    return status;
}

/*---------------------------------------------------------------------------
            ROUND rounds a decimal value to the nearest integer
 ---------------------------------------------------------------------------*/
int decimal_round(decimal value, decimal *result) {
    status_code status = OK;
    if (result == NULL) {
        status = CALCULATION_ERROR;
    } else {
        decimal_initialization(result);
        *result = value;
        bit sign = get_sign(value);
        if (get_scale(value) > 0) {
            set_sign(result, plus);
            bit cloud = 0;
            for (bit index = get_scale(value); index > 0; index--) {
                cloud = mod_div_10(*result);
                *result = div_10(*result);
            }
            if (cloud >= 5) {
                decimal one = {{1, 0, 0, 0}};
                decimal_add(*result, one, result);
            }
            set_sign(result, sign);
        }
    }
    return status;
}

/*---------------------------------------------------------------------------
            TRUNCATE returns the integral digits of the specified
            Decimal; any fractional digits are discarded, including
            trailing zeroes
 ---------------------------------------------------------------------------*/
int truncate(decimal value, decimal *result) {
    status_code status = OK;
    if (result == NULL) {
        status = CALCULATION_ERROR;
    } else {
        *result = value;
        for (scale Scale = get_scale(value); Scale > 0; Scale--) {
            *result = div_10(*result);
        }
        set_sign(result, get_sign(value));
    }
    return status;
}

/*---------------------------------------------------------------------------
            NEGATE returns the result of multiplying the specified
            Decimal value by negative one
 ---------------------------------------------------------------------------*/
int negate(decimal number, decimal *result) {
    status_code status = OK;
    if (result == NULL) {
        status = CALCULATION_ERROR;
    } else {
        for (int index = 0; index < SIZE_DECIMAL; index++) {
            result->bits[index] = number.bits[index];
        }
        if (get_sign(number)) {
            set_sign(result, plus);
        } else {
            set_sign(result, minus);
        }
    }
    return status;
}
