#include "decimal.h"

/*---------------------------------------------------------------------------
            DECIMAL_INITIALIZATION initialization and filling a number
            with zeros
 ---------------------------------------------------------------------------*/
void super_decimal_initialization(super_decimal *number) {
    for (int value = 0; value < SIZE_SUPER_DECIMAL; value++) {
        number->bits[value] = 0UL;
    }
}

/*---------------------------------------------------------------------------
            FROM_DECIMAL_TO_SUPER_DECIMAL conversion from
            decimal to super decimal
 ---------------------------------------------------------------------------*/
super_decimal from_decimal_to_super_decimal(decimal value) {
    super_decimal result;
    super_decimal_initialization(&result);
    for (int number = 0; number < SIZE_DECIMAL - 1; number++) {
        result.bits[number] = value.bits[number];
    }
    result.bits[SUPER_SCALE] = value.bits[SCALE];
    return result;
}

/*---------------------------------------------------------------------------
            GET_BIT_SUPER get a bit value of super decimal by given index
 ---------------------------------------------------------------------------*/
bit get_bit_super(super_decimal value, int index) {
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
            SET_BIT_SUPER set a bit value of super decimal by given index
 ---------------------------------------------------------------------------*/
void set_bit_super(super_decimal *value, int index, bit value_bit) {
    bit mask = 1u << (index % 32);
    if (value_bit) {
        value->bits[index / 32] |= mask;
    } else {
        value->bits[index / 32] &= ~mask;
    }
}

/*---------------------------------------------------------------------------
            GET_SIGN_SUPER get a sign bit of super decimal
 ---------------------------------------------------------------------------*/
bit get_sign_super(super_decimal value) {
    return (value.bits[SUPER_SCALE] >> 31) & 1UL;
}

/*---------------------------------------------------------------------------
            SET_SIGN_SUPER set a sign bit of float number
 ---------------------------------------------------------------------------*/
void set_sign_super(super_decimal *value, bit sign) {
    scale scale = get_scale_super(*value);
    value->bits[SUPER_SCALE] = ((value->bits[SUPER_SCALE] >> 31)  ^ sign) << 31;
    set_scale_super(value, scale);
}

/*---------------------------------------------------------------------------
            GET_SCALE_SUPER get a scale of super decimal
 ---------------------------------------------------------------------------*/
scale get_scale_super(super_decimal value) {
    scale mask = 0x00ff0000;
    scale Scale = value.bits[SUPER_SCALE] & mask;
    Scale >>= 16;
    return Scale;
}

/*---------------------------------------------------------------------------
            SET_SCALE set a scale of super decimal
 ---------------------------------------------------------------------------*/
void set_scale_super(super_decimal *value, scale scale_value) {
    value->bits[SUPER_SCALE] &= 0x80000000;
    scale_value <<= 16;
    value->bits[SUPER_SCALE] |= scale_value;
}

/*---------------------------------------------------------------------------
            ADD_SUPER_DECIMAL bitwise addition of two super decimal
 ---------------------------------------------------------------------------*/
super_decimal add_super_decimal(super_decimal value_1, super_decimal value_2) {
    super_decimal result;
    super_decimal_initialization(&result);
    bit cloud = 0;
    for (int index = 0; index < 192; index++) {
        bit bit_value_1 = get_bit_super(value_1, index);
        bit bit_value_2 = get_bit_super(value_2, index);
        if (bit_value_1 & bit_value_2) {
            if (cloud) {
                set_bit_super(&result, index, 1);
            } else {
                set_bit_super(&result, index, 0);
                cloud = 1;
            }
        } else if (bit_value_1 ^ bit_value_2) {
            if (cloud) {
                set_bit_super(&result, index, 0);
            } else {
                set_bit_super(&result, index, 1);
            }
        } else {
            if (cloud) {
                set_bit_super(&result, index, 1);
                cloud = 0;
            } else {
                set_bit_super(&result, index, 0);
            }
        }
    }
    return result;
}

/*---------------------------------------------------------------------------
            CHECK_ENTRY_SUPER check if super decimal has empty bites
 ---------------------------------------------------------------------------*/
bool check_entry_super(super_decimal value) {
    bool answer = TRUE;
    for (int index = LOW_SUPER; index <= HIGH_SUPER && answer; index++) {
        if (value.bits[index]) answer = FALSE;
    }
    return answer;
}

/*---------------------------------------------------------------------------
            EQUALIZE_SCALE equalization of scales of super decimal
 ---------------------------------------------------------------------------*/
void equalize_scale(super_decimal *value_1, super_decimal *value_2) {
    scale scale_1 = get_scale_super(*value_1);
    scale scale_2 = get_scale_super(*value_2);
    int difference = scale_1 - scale_2;
    if (difference < 0) {
        difference *= -1;
        *value_1 = increase_super_decimal(*value_1, difference);
    } else if (difference > 0) {
        *value_2 = increase_super_decimal(*value_2, difference);
    }
}

/*---------------------------------------------------------------------------
            INCREASE_SUPER_DECIMAL increase a number and scale
            of super decimal
 ---------------------------------------------------------------------------*/
super_decimal increase_super_decimal(super_decimal value, scale difference) {
    super_decimal result = value;
    scale scale_value = get_scale_super(value);
    while (difference > 0) {
        result = mul_10_super(result);
        scale_value++;
        difference--;
    }
    set_scale_super(&result, scale_value);
    return result;
}

/*---------------------------------------------------------------------------
            MUL_10_SUPER multiplies super decimal by 10
 ---------------------------------------------------------------------------*/
super_decimal mul_10_super(super_decimal value) {
    super_decimal result;
    super_decimal_initialization(&result);
    result.bits[SUPER_SCALE] = value.bits[SUPER_SCALE];

    for (int index = 1; index <  3; index++) {
        value = shift_left_super(value, index);
        result = add_super_decimal(result, value);
    }
    return result;
}

/*---------------------------------------------------------------------------
            SHIFT_LEFT_SUPER bitwise shift of super decimal
            to the left by number of character
 ---------------------------------------------------------------------------*/
super_decimal shift_left_super(super_decimal value, int shift) {
    super_decimal result;
    super_decimal_initialization(&result);
    for (int index = 191; index >= shift; index--) {
        set_bit_super(&result, index, get_bit_super(value, index - shift));
    }
    return result;
}

/*---------------------------------------------------------------------------
            SHIFT_RIGHT_SUPER bitwise shift of super decimal
            to the right by number of character
 ---------------------------------------------------------------------------*/
super_decimal shift_right_super(super_decimal value, int shift) {
  super_decimal result;
  super_decimal_initialization(&result);
  for (int index = 0; index < 192 - shift; index++) {
    set_bit_super(&result, index, get_bit_super(value, index + shift));
  }
  return result;
}

/*---------------------------------------------------------------------------
            IS_EQUAL_SUPER one super decimal is equal to another
 ---------------------------------------------------------------------------*/
bool is_equal_super(super_decimal value_1, super_decimal value_2) {
    status_code status = TRUE;
    for (int index = LOW; index < SUPER_SCALE; index++) {
        if (value_1.bits[index] != value_2.bits[index]) {
            status = FALSE;
            break;
        }
    }
    return status;
}

/*---------------------------------------------------------------------------
            IS_GREATER_SUPER one super decimal is greater than the other
 ---------------------------------------------------------------------------*/
bool is_greater_super(super_decimal value_1, super_decimal value_2) {
    status_code status = FALSE;
    for (int index = HIGH_SUPER; index >= LOW; index--) {
        if (value_1.bits[index] > value_2.bits[index]) {
            status = TRUE;
            break;
        } else if (value_1.bits[index] < value_2.bits[index]) {
            break;
        }
    }
    return status;
}

/*---------------------------------------------------------------------------
            SUB_SUPER the operation of subtraction of
            two super decimal
 ---------------------------------------------------------------------------*/
super_decimal sub_super(super_decimal value_1, super_decimal value_2) {
    super_decimal result;
    super_decimal_initialization(&result);
    int cloud = 0;
    for (int index = 0; index < 192; index++) {
        bit value_1_bit = get_bit_super(value_1, index);
        bit value_2_bit = get_bit_super(value_2, index);
        if (value_1_bit & value_2_bit) {
            if (cloud) {
                set_bit_super(&result, index, 1);
            } else {
                set_bit_super(&result, index, 0);
            }
        } else if (!(value_1_bit | value_2_bit)) {
            if (cloud) {
                set_bit_super(&result, index, 1);
            } else {
                set_bit_super(&result, index, 0);
            }
        } else if (value_1_bit > value_2_bit) {
            if (cloud) {
                set_bit_super(&result, index, 0);
                cloud = 0;
            } else {
                set_bit_super(&result, index, 1);
            }
        } else {
            if (cloud) {
                set_bit_super(&result, index, 0);
            } else {
                set_bit_super(&result, index, 1);
                cloud = 1;
            }
        }
    }
    return result;
}

/*---------------------------------------------------------------------------
            MOD_DIV_10_SUPER get a remainder of super decimal
            division by 10
 ---------------------------------------------------------------------------*/
int mod_div_10_super(super_decimal value) {
    int result = 0;
    for (int index = 191; index >= 0; index--) {
        result <<= 1;
        result |= get_bit_super(value, index);
        if (result >= 10) result -= 10;
    }
    return result;
}

/*---------------------------------------------------------------------------
            DIV_10_SUPER dividing super decimal by 10
 ---------------------------------------------------------------------------*/
super_decimal div_10_super(super_decimal value) {
    super_decimal result, quotient;
    super_decimal_initialization(&result);
    super_decimal_initialization(&quotient);
    int remnant = 0;
    int pos = 0;
    int status = 0;
    for (int index = 191; index >= 0; index--) {
        if (status) pos++;
        remnant <<= 1;
        remnant |= get_bit_super(value, index);
        if (remnant >= 10) {
            set_bit_super(&quotient, pos, 1);
            remnant -= 10;
            status = 1;
        }
    }
    for (int index = pos, index_2 = 0; index >= 0; index--, index_2++) {
        int bit = get_bit_super(quotient, index);
        set_bit_super(&result, index_2, bit);
    }
    return result;
}

/*---------------------------------------------------------------------------
            MOD_DIV_2_SUPER get a remainder of super decimal
            division by 2
 ---------------------------------------------------------------------------*/
super_decimal mod_div_2_super(super_decimal value_1, super_decimal value_2) {
    int shift = 0;
    while (!get_bit_super(value_2, 191 - shift)) {
        shift++;
    }
    for (int index = 0; !get_bit_super(value_1, 191 - index); index++) {
        shift--;
    }
    if (shift >= 0) {
    value_2 = shift_left_super(value_2, shift);
    while (shift >= 0) {
        if (is_greater_super(value_1, value_2) || is_equal_super(value_1, value_2)) {
            value_1 = sub_super(value_1, value_2);
        }
        value_2 = shift_right_super(value_2, 1);
        shift--;
    }
    }
    return value_1;
}

/*---------------------------------------------------------------------------
            IS_ZERO_SUPER check if super decimal is zero
 ---------------------------------------------------------------------------*/
bool is_zero_super(super_decimal value) {
    status_code status = TRUE;
    for (int index = HIGH_SUPER; index >= LOW; index--) {
        if (value.bits[index]) {
            status = FALSE;
            break;
        }
    }
    return status;
}

/*---------------------------------------------------------------------------
            DIV_SUPER the operation of division two super decimal numbers
 ---------------------------------------------------------------------------*/
super_decimal div_super(super_decimal value_1, super_decimal value_2) {
    super_decimal result;
    super_decimal_initialization(&result);
    int shift = 0;
    while (!get_bit_super(value_2, 191 - shift)) {
        shift++;
    }
    for (int index = 0; !get_bit_super(value_1, 191 - index); index++) {
        shift--;
    }
    if (shift >= 0) {
        value_2 = shift_left_super(value_2, shift);
        while (shift >= 0) {
            set_bit_super(&result, 0, (is_greater_super(value_1, value_2) ||
             is_equal_super(value_1, value_2)));
            if (get_bit_super(result, 0)) {
                value_1 = sub_super(value_1, value_2);
            }
            value_2 = shift_right_super(value_2, 1);
            if (shift) result = shift_left_super(result, 1);
            shift--;
        }
    }
    return result;
}

/*---------------------------------------------------------------------------
            MOD_SUPER the remainder of the division of
            two super decimal numbers
 ---------------------------------------------------------------------------*/
super_decimal mod_super(super_decimal value_1, super_decimal value_2) {
    int shift = 0;
    while (!get_bit_super(value_2, 191 - shift)) {
        shift++;
    }
    for (int index = 0; !get_bit_super(value_1, 191 - index); index++) {
        shift--;
    }
    if (shift >= 0) {
        value_2 = shift_left_super(value_2, shift);
        while (shift >= 0) {
            if (is_greater_super(value_1, value_2) || is_equal_super(value_1, value_2)) {
                value_1 = sub_super(value_1, value_2);
            }
            value_2 = shift_right_super(value_2, 1);
            shift--;
        }
    }
    return value_1;
}
