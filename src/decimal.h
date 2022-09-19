#ifndef SRC_DECIMAL_H_
#define SRC_DECIMAL_H_

#include <stdio.h>
#include <math.h>
#include <string.h>

#define SIZE_DECIMAL 4
#define SIZE_SUPER_DECIMAL 7

#define LOW 0
#define MID 1
#define HIGH 2
#define LOW_SUPER 3
#define MID_SUPER 4
#define HIGH_SUPER 5
#define SCALE 3
#define SUPER_SCALE 6
#define MAX_SCALE 28

//  data type for use bit and scale
typedef unsigned int bit;
typedef unsigned int scale;
typedef int bool;

typedef enum {
    OK = 0,
    CODE_INFINITY = 1,
    NEGATIVE_INFINITY = 2,
    CODE_NAN = 3,
    CONVERTING_ERROR = 1,
    CALCULATION_ERROR = 1,
    SUCCESS = 0,
    TRUE = 1,
    FALSE = 0,
} status_code;

typedef enum {
    plus = 0,
    minus = 1
} sign;

typedef struct {
    unsigned int bits[SIZE_DECIMAL];
} decimal;

typedef struct {
    unsigned int bits[SIZE_SUPER_DECIMAL];
} super_decimal;

//  Arithmetic operators
int decimal_add(decimal value_1, decimal value_2, decimal *result);
int decimal_sub(decimal value_1, decimal value_2, decimal *result);
int decimal_mul(decimal value_1, decimal value_2, decimal *result);
int decimal_div(decimal value_1, decimal value_2, decimal *result);
int decimal_mod(decimal value_1, decimal value_2, decimal *result);
//  0 - OK, 1 - INF, 2 - NEG_INF, 3 - DIV BY ZERO

//  Comparison operators
int is_less(decimal value_1, decimal value_2);
int is_less_or_equal(decimal value_1, decimal value_2);
int is_greater(decimal value_1, decimal value_2);
int is_greater_or_equal(decimal value_1, decimal value_2);
int is_equal(decimal value_1, decimal value_2);
int is_not_equal(decimal value_1, decimal value_2);
//  1 - TRUE, 0 - FALSE

//  Convertors
int from_int_to_decimal(int src, decimal *dst);  //  From int
int from_float_to_decimal(float src, decimal *dst);  //  From float
int from_decimal_to_int(decimal src, int *dst);  //  To int
int from_decimal_to_float(decimal src, float *dst);  //  To float
//  0 - SUCCESS, 1 - CONVERTING ERROR

//  Another functions
int decimal_floor(decimal value, decimal *result);
int decimal_round(decimal value, decimal *result);
int truncate(decimal value, decimal *result);
int negate(decimal number, decimal *result);
//  0 - OK, 1 - CALCULATION ERROR

// Common functions

//  decimal function
void decimal_initialization(decimal *number);
decimal from_super_decimal_to_decimal(super_decimal value, status_code *status);
void set_sign(decimal *value, bit sign);
bit get_sign(decimal value);
decimal shift_left(decimal value, int shift);
decimal shift_right(decimal value, int shift);
void set_scale(decimal *value, scale scale_value);
scale get_scale(decimal value);
void set_bit(decimal *value, int index, bit value_bit);
bit get_bit(decimal value, int index);
decimal div_10(decimal value);
int mod_div_10(decimal value);
bool is_zero(decimal value);
int get_float_scale(float src);
bit get_bit_float(float source, int index);
decimal mul_10(decimal value);

//  super_decimal function
void super_decimal_initialization(super_decimal *number);
super_decimal from_decimal_to_super_decimal(decimal value);
super_decimal add_super_decimal(super_decimal value_1, super_decimal value_2);
void set_scale_super(super_decimal *value, scale scale_value);
scale get_scale_super(super_decimal value);
void set_sign_super(super_decimal *value, bit sign);
bit get_sign_super(super_decimal value);
void set_bit_super(super_decimal *value, int index, bit value_bit);
bit get_bit_super(super_decimal value, int index);
bool check_entry_super(super_decimal value);
void equalize_scale(super_decimal *value_1, super_decimal *value_2);
super_decimal increase_super_decimal(super_decimal value, scale difference);
super_decimal mul_10_super(super_decimal result);
super_decimal shift_left_super(super_decimal value, int shift);
super_decimal shift_right_super(super_decimal value, int shift);
bool is_equal_super(super_decimal value_1, super_decimal value_2);
bool is_greater_super(super_decimal value_1, super_decimal value_2);
super_decimal sub_super(super_decimal value_1, super_decimal value_2);
int mod_div_10_super(super_decimal value);
super_decimal div_10_super(super_decimal value);
super_decimal mod_div_2_super(super_decimal value_1, super_decimal value_2);
bool is_zero_super(super_decimal value);
super_decimal div_super(super_decimal value_1, super_decimal value_2);
super_decimal mod_super(super_decimal value_1, super_decimal value_2);

#endif  //  SRC_DECIMAL_H_
