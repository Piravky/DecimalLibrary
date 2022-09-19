#include "decimal.h"

/*---------------------------------------------------------------------------
            IS_LESS one number is smaller than the other
 ---------------------------------------------------------------------------*/
int is_less(decimal value_1, decimal value_2) {
    status_code status = FALSE;
    bit sign_1 = get_sign(value_1);
    bit sign_2 = get_sign(value_2);
    if (is_zero(value_1)) {
        sign_1 = plus;
    }
    if (is_zero(value_2)) {
        sign_2 = plus;
    }
    if (sign_1 == sign_2) {
        super_decimal super_value_1 = from_decimal_to_super_decimal(value_1);
        super_decimal super_value_2 = from_decimal_to_super_decimal(value_2);
        equalize_scale(&super_value_1, &super_value_2);
        for (int index = HIGH_SUPER; index >= LOW; index--) {
            if (super_value_1.bits[index] > super_value_2.bits[index]) {
                if (sign_1) {
                    status = TRUE;
                }
                break;
            } else if (super_value_1.bits[index] < super_value_2.bits[index]) {
                if (!sign_1) {
                    status = TRUE;
                }
                break;
            }
        }
    } else if (sign_1) {
        status = TRUE;
    }
    return status;
}

/*---------------------------------------------------------------------------
            IS_LESS_OR_EQUAL one number is less than or equal
            to another
 ---------------------------------------------------------------------------*/
int is_less_or_equal(decimal value_1, decimal value_2) {
    status_code status = FALSE;
    if (is_less(value_1, value_2) || is_equal(value_1, value_2)) {
        status = TRUE;
    }
    return status;
}

/*---------------------------------------------------------------------------
            IS_GREATER one number is greater than the other
 ---------------------------------------------------------------------------*/
int is_greater(decimal value_1, decimal value_2) {
    status_code status = FALSE;
    bit sign_1 = get_sign(value_1);
    bit sign_2 = get_sign(value_2);
    if (is_zero(value_1)) {
        sign_1 = plus;
    }
    if (is_zero(value_2)) {
        sign_2 = plus;
    }
    if (sign_1 == sign_2) {
        super_decimal super_value_1 = from_decimal_to_super_decimal(value_1);
        super_decimal super_value_2 = from_decimal_to_super_decimal(value_2);
        equalize_scale(&super_value_1, &super_value_2);
        for (int index = HIGH_SUPER; index >= LOW; index--) {
            if (super_value_1.bits[index] > super_value_2.bits[index]) {
                if (!sign_1) {
                    status = TRUE;
                }
                break;
            } else if (super_value_1.bits[index] < super_value_2.bits[index]) {
                if (sign_1) {
                    status = TRUE;
                }
                break;
            }
        }
    } else if (sign_2) {
        status = TRUE;
    }
    return status;
}

/*---------------------------------------------------------------------------
            IS_GREATER_OR_EQUAL one number is greater than or equal
            to another
 ---------------------------------------------------------------------------*/
int is_greater_or_equal(decimal value_1, decimal value_2) {
    status_code status = FALSE;
    if (is_greater(value_1, value_2) || is_equal(value_1, value_2)) {
        status = TRUE;
    }
    return status;
}

/*---------------------------------------------------------------------------
            IS_EQUAL one number is equal to another
 ---------------------------------------------------------------------------*/
int is_equal(decimal value_1, decimal value_2) {
    status_code status = FALSE;
    if (is_zero(value_1) && is_zero(value_2)) {
        status = TRUE;
    } else if (get_sign(value_1) == get_sign(value_2)) {
        super_decimal value_1_super = from_decimal_to_super_decimal(value_1);
        super_decimal value_2_super = from_decimal_to_super_decimal(value_2);
        equalize_scale(&value_1_super, &value_2_super);
        status = TRUE;
        for (int index = HIGH_SUPER; index >= LOW; index--) {
            if (value_1_super.bits[index] != value_2_super.bits[index]) {
                status = FALSE;
                break;
            }
        }
    }
    return status;
}

/*---------------------------------------------------------------------------
            NOT_EQUAL one number is not equal to another
 ---------------------------------------------------------------------------*/
int is_not_equal(decimal value_1, decimal value_2) {
    status_code status = TRUE;
    if (is_equal(value_1, value_2)) {
        status = FALSE;
    }
    return status;
}
