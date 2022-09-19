#include "decimal.h"

/*---------------------------------------------------------------------------
            ADD the operation of adding two decimal numbers
 ---------------------------------------------------------------------------*/
int decimal_add(decimal value_1, decimal value_2, decimal *result) {
    status_code status = OK;
    super_decimal super_result;
    super_decimal_initialization(&super_result);
    if (result == NULL) {
        status = CODE_NAN;
    } else {
        bit sign_value_1 = get_sign(value_1);
        bit sign_value_2 = get_sign(value_2);
        super_decimal super_value_1 = from_decimal_to_super_decimal(value_1);
        super_decimal super_value_2 = from_decimal_to_super_decimal(value_2);
        equalize_scale(&super_value_1, &super_value_2);
        if (sign_value_1 == sign_value_2) {
            super_result = add_super_decimal(super_value_1, super_value_2);
            set_sign_super(&super_result, sign_value_1);
            set_scale_super(&super_result, get_scale_super(super_value_1));
        } else if (is_equal_super(super_value_1, super_value_2) == FALSE) {
            if (is_greater_super(super_value_1, super_value_2)) {
                super_result = sub_super(super_value_1, super_value_2);
                set_sign_super(&super_result, sign_value_1);
            } else {
                super_result = sub_super(super_value_2, super_value_1);
                set_sign_super(&super_result, sign_value_1);
            }
            set_scale_super(&super_result, get_scale_super(super_value_1));
        }
        *result = from_super_decimal_to_decimal(super_result, &status);
    }
    return status;
}

/*---------------------------------------------------------------------------
            SUB the operation of subtraction two decimal numbers
 ---------------------------------------------------------------------------*/
int decimal_sub(decimal value_1, decimal value_2, decimal *result) {
    status_code status = OK;
    super_decimal super_result;
    super_decimal_initialization(&super_result);
    if (result == NULL) {
        status = CODE_NAN;
    } else {
        bit sign_value_1 = get_sign(value_1);
        bit sign_value_2 = get_sign(value_2);
        super_decimal super_value_1 = from_decimal_to_super_decimal(value_1);
        super_decimal super_value_2 = from_decimal_to_super_decimal(value_2);
        equalize_scale(&super_value_1, &super_value_2);
        if (sign_value_1 != sign_value_2) {
            super_result = add_super_decimal(super_value_1, super_value_2);
            set_sign_super(&super_result, sign_value_1);
            set_scale_super(&super_result, get_scale_super(super_value_1));
        } else if (is_equal_super(super_value_1, super_value_2) == FALSE) {
            if (is_greater_super(super_value_1, super_value_2)) {
                super_result = sub_super(super_value_1, super_value_2);
                set_sign_super(&super_result, sign_value_1);
            } else {
                super_result = sub_super(super_value_2, super_value_1);
                set_sign_super(&super_result, !sign_value_2);
            }
            set_scale_super(&super_result, get_scale_super(super_value_1));
        }
        *result = from_super_decimal_to_decimal(super_result, &status);
    }
    return status;
}


/*---------------------------------------------------------------------------
            MUL the operation of multiplication two decimal numbers
 ---------------------------------------------------------------------------*/
int decimal_mul(decimal value_1, decimal value_2, decimal *result) {
    status_code status = OK;
    decimal_initialization(result);
    super_decimal super_result;
    super_decimal_initialization(&super_result);
    if (result == NULL) {
        status = CODE_NAN;
    } else {
        bit sign = get_sign(value_1) ^ get_sign(value_2);
        super_decimal value = from_decimal_to_super_decimal(value_1);
        if (is_zero(value_1) || is_zero(value_2)) {
            if (is_zero(value_1)) {
                set_scale(result, get_scale(value_1));
            } else {
                set_scale(result, get_scale(value_2));
            }
            set_sign(result, sign);
        } else {
            scale Scale = get_scale(value_1) + get_scale(value_2);
            while (is_zero(value_2) == FALSE) {
                set_scale(&value_2, 0);
                if (get_bit(value_2, 0)) {
                    super_result = add_super_decimal(super_result, value);
                }
                value = shift_left_super(value, 1);
                value_2 = shift_right(value_2, 1);
            }
            set_sign_super(&super_result, sign);
            set_scale_super(&super_result, Scale);
            *result = from_super_decimal_to_decimal(super_result, &status);
        }
    }
    return status;
}

/*---------------------------------------------------------------------------
            DIV the operation of division two decimal numbers
 ---------------------------------------------------------------------------*/
int decimal_div(decimal value_1, decimal value_2, decimal *result) {
    status_code status = OK;
    decimal_initialization(result);
    decimal neg_value_2;
    negate(value_2, &neg_value_2);
    bit sign = get_sign(value_1) ^ get_sign(value_2);
    decimal one = {{1, 0, 0, 0}};
    if (is_zero(value_2)) {
        if (is_zero(value_1)) {
            status = CODE_NAN;
        } else {
            if (sign) {
                status = NEGATIVE_INFINITY;
            } else {
                status = CODE_INFINITY;
            }
        }
    } else if (is_zero(value_1)) {
        if (!is_zero(value_2)) {
            if (sign) {
                set_sign(result, minus);
            } else {
                set_sign(result, plus);
            }
            set_scale(result, get_scale(value_1));
        }
    } else if (is_equal(value_2, one) || is_equal(neg_value_2, one)) {
        *result = value_1;
        set_sign(result, sign);
    } else if (is_equal(value_1, value_2) || is_equal(value_1, neg_value_2)) {
        *result = one;
        set_sign(result, sign);
    } else {
        super_decimal super_value_1 = from_decimal_to_super_decimal(value_1);
        super_decimal super_value_2 = from_decimal_to_super_decimal(value_2);
        equalize_scale(&super_value_1, &super_value_2);
        super_decimal super_result = div_super(super_value_1, super_value_2);
        super_value_1 = mod_super(super_value_1, super_value_2);
        scale cloud_scale = 0;
        super_decimal ten = {{10, 0, 0, 0, 0, 0, 0}};
        while (is_greater_super(super_result, ten) || is_equal_super(super_result, ten)) {
            ten = mul_10_super(ten);
            cloud_scale++;
        }
        scale super_result_scale = 0;
        while (super_result_scale + cloud_scale < 29 && !is_zero_super(super_value_1)) {
            super_value_1 = mul_10_super(super_value_1);
            super_result = mul_10_super(super_result);
            super_result = add_super_decimal(super_result, div_super(super_value_1, super_value_2));
            super_value_1 = mod_super(super_value_1, super_value_2);
            super_result_scale++;
        }
        set_sign_super(&super_result, sign);
        set_scale_super(&super_result, super_result_scale);
        *result = from_super_decimal_to_decimal(super_result, &status);
        if (!mod_div_10(*result) && super_result_scale) {
            super_result_scale = get_scale(*result);
            while (!mod_div_10(*result)) {
                *result = div_10(*result);
                super_result_scale--;
            }
            set_scale(result, super_result_scale);
        }
    }
    return status;
}

/*---------------------------------------------------------------------------
            MOD the remainder of the division of two decimal numbers
 ---------------------------------------------------------------------------*/
int decimal_mod(decimal value_1, decimal value_2, decimal *result) {
    status_code status = OK;
    bit sign = get_sign(value_1);
    if (result == NULL) {
        status = CODE_NAN;
    } else {
        scale result_scale;
        decimal_initialization(result);
        if (get_scale(value_1) >= get_scale(value_2)) {
            result_scale = get_scale(value_1);
        } else {
            result_scale =  get_scale(value_2);
        }
        decimal neg_val;
        negate(value_2, &neg_val);
        if (is_equal(value_1, value_2) || is_equal(value_1, neg_val)) {
            set_sign(result, sign);
        } else if (!is_zero(value_2)) {
            set_sign(&value_1, plus);
            set_sign(&value_2, plus);
            if (is_greater(value_1, value_2)) {
                super_decimal super_value_1 = from_decimal_to_super_decimal(value_1);
                super_decimal super_value_2 = from_decimal_to_super_decimal(value_2);
                equalize_scale(&super_value_1, &super_value_2);
                super_value_1 = mod_div_2_super(super_value_1, super_value_2);
                *result = from_super_decimal_to_decimal(super_value_1, &status);
                set_scale(result, result_scale);
            } else {
                *result = value_1;
            }
            set_sign(result, sign);
        } else {
            status = CODE_NAN;
        }
    }
    return status;
}
