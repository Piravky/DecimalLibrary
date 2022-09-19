#include "decimal.h"

/*---------------------------------------------------------------------------
            DECIMAL_INITIALIZATION creating and filling a number
            with zeros
 ---------------------------------------------------------------------------*/
void decimal_initialization(decimal *number) {
    for (int value = 0; value < SIZE_DECIMAL; value++) {
        number->bits[value] = 0UL;
    }
}

/*---------------------------------------------------------------------------
            GET_BIT get a bit value by given index
 ---------------------------------------------------------------------------*/
bit get_bit(decimal value, int index) {
    bit mask = 1u << (index % 32);
    bit Bit = value.bits[index / 32] & mask;
    if (Bit) {
        Bit = 1;
    } else {
        Bit = 0;
    }
    return Bit;
}

/*---------------------------------------------------------------------------
            SET_BIT set a bit value by given index
 ---------------------------------------------------------------------------*/
void set_bit(decimal *value, int index, bit value_bit) {
    bit mask = 1u << (index % 32);
    if (value_bit) {
        value->bits[index / 32] |= mask;
    } else {
        value->bits[index / 32] &= ~mask;
    }
}

/*---------------------------------------------------------------------------
            GET_SCALE get a scale
 ---------------------------------------------------------------------------*/
scale get_scale(decimal value) {
    scale mask = 0x00ff0000;
    scale Scale = value.bits[SCALE] & mask;
    Scale >>= 16;
    return Scale;
}

/*---------------------------------------------------------------------------
            SET_SCALE set a scale
 ---------------------------------------------------------------------------*/
void set_scale(decimal *value, scale scale_value) {
    value->bits[SCALE] &= 0x80000000;
    scale_value <<= 16;
    value->bits[SCALE] |= scale_value;
}

/*---------------------------------------------------------------------------
            SHIFT_RIGHT bitwise shift to the right by number of character
 ---------------------------------------------------------------------------*/
decimal shift_right(decimal value, int shift) {
    decimal result;
    decimal_initialization(&result);
    for (int index = 0; index < 96 - shift; index++) {
        set_bit(&result, index, get_bit(value, shift + index));
    }
    return result;
}

/*---------------------------------------------------------------------------
            SHIFT_LEFT bitwise shift to the left by number of character
 ---------------------------------------------------------------------------*/
decimal shift_left(decimal value, int shift) {
    status_code status = OK;
    decimal result;
    decimal_initialization(&result);
    result.bits[SCALE] = value.bits[SCALE];
    for (int index = 95; index > 95 - shift; index--) {
        if (get_bit(value, index)) {
            status = CODE_INFINITY;
        }
    }
    if (status) {
        decimal_initialization(&result);
    } else {
        for (int index = 95; index >= shift; index--) {
            set_bit(&result, index, get_bit(value, index - shift));
        }
    }
    return result;
}

/*---------------------------------------------------------------------------
            GET_SIGN get a sign bit of number
 ---------------------------------------------------------------------------*/
bit get_sign(decimal value) {
    return (value.bits[SCALE] >> 31) & 1UL;
}

/*---------------------------------------------------------------------------
            SET_SIGN set a sign bit of number
 ---------------------------------------------------------------------------*/
void set_sign(decimal *value, bit sign) {
    scale scale = get_scale(*value);
    value->bits[SCALE] = sign << 31;
    set_scale(value, scale);
}

/*---------------------------------------------------------------------------
            FROM_SUPER_DECIMAL_TO_DECIMAL conversion of number
            from super decimal to decimal
 ---------------------------------------------------------------------------*/
decimal from_super_decimal_to_decimal(super_decimal value, status_code *status) {
    decimal result;
    decimal_initialization(&result);
    bit sign = get_sign_super(value);
    scale Scale = get_scale_super(value);
    bit cloud = 0;
    while (Scale > MAX_SCALE || (!check_entry_super(value) && Scale)) {
        Scale--;
        cloud = mod_div_10_super(value);
        value = div_10_super(value);
    }
    if (check_entry_super(value)) {
        if (cloud >= 5) {
            super_decimal one = {{1, 0, 0, 0, 0, 0, 0}};
            super_decimal tmp = add_super_decimal(value, one);
            if (check_entry_super(tmp) && !Scale) {
                decimal_initialization(&result);
            } else if (!check_entry_super(tmp) && Scale) {
                cloud = mod_div_10_super(value);
                value = div_10_super(value);
                Scale--;
                if (cloud >= 5) value = add_super_decimal(value, one);
                for (int i = 0; i < 3; i++) {
                    result.bits[i] = value.bits[i];
                }
                set_scale(&result, Scale);
                set_sign(&result, sign);
            } else {
                for (int i = 0; i < 3; i++) {
                    result.bits[i] = tmp.bits[i];
                }
                set_scale(&result, Scale);
                set_sign(&result, sign);
            }
        } else {
            for (int i = 0; i < 3; i++) {
                result.bits[i] = value.bits[i];
            }
            set_scale(&result, Scale);
            set_sign(&result, sign);
        }
    } else {
        if (sign) {
            *status = NEGATIVE_INFINITY;
        } else {
            *status = CODE_INFINITY;
        }
    }
    return result;
}


/*---------------------------------------------------------------------------
            DIV_10 dividing decimal by 10
 ---------------------------------------------------------------------------*/
decimal div_10(decimal value) {
    decimal result;
    decimal_initialization(&result);
    decimal quotient;
    decimal_initialization(&quotient);
    int remnant = 0;
    int pos = 0;
    int status = 0;
    for (int index = 95; index >= 0; index--) {
        if (status) pos++;
        remnant <<= 1;
        remnant |= get_bit(value, index);
        if (remnant >= 10) {
            set_bit(&quotient, pos, 1);
            remnant -= 10;
            status = 1;
        }
    }
    for (int index = pos, index_2 = 0; index >= 0; index--, index_2++) {
        set_bit(&result, index_2, get_bit(quotient, index));
    }
    return result;
}

/*---------------------------------------------------------------------------
            MOD_DIV_10 get a remainder of decimal division by 10
 ---------------------------------------------------------------------------*/
int mod_div_10(decimal value) {
    int remnant = 0;
    for (int index = 95; index >= 0; index--) {
        remnant <<= 1;
        remnant |= get_bit(value, index);
        if (remnant >= 10) remnant -= 10;
    }
    return remnant;
}

/*---------------------------------------------------------------------------
            MUL_10 multiplies decimal by 10
 ---------------------------------------------------------------------------*/
decimal mul_10(decimal value) {
    decimal result;
    decimal_initialization(&result);
    result.bits[SCALE] = value.bits[SCALE];
    for (int index = 1; index <= 2; index++) {
        value = shift_left(value, index);
        decimal_add(result, value, &result);
    }
    return result;
}

/*---------------------------------------------------------------------------
            IS_ZERO check if a number is zero
 ---------------------------------------------------------------------------*/
bool is_zero(decimal value) {
    status_code status = TRUE;
    for (int index = LOW; index < HIGH; index++) {
        if (value.bits[index]) {
            status = FALSE;
        }
    }
    return status;
}

/*---------------------------------------------------------------------------
            GET_FLOAT_SCALE get exponent value of float number
 ---------------------------------------------------------------------------*/
int get_float_scale(float source) {
    bit float_bits = *((bit *)&source);
    scale exp = (float_bits >> 23) & 0xff;
    return exp - 127;
}

/*---------------------------------------------------------------------------
            GET_BIT_FLOAT get a sign bit of float number
 ---------------------------------------------------------------------------*/
bit get_bit_float(float source, int index) {
    bit float_bits = *((bit *)&source);
    bit mask = 1u << index;
    bit result_bit = float_bits & mask;
    if (result_bit) {
        result_bit = 1;
    } else {
        result_bit = 0;
    }
    return result_bit;
}
