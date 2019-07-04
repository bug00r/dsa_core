#ifndef DSA_DICE_H
#define DSA_DICE_H

#include <stddef.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>

#include "defs.h"

#include "string_utils.h"
#include "number_utils.h"

#if debug > 0
    #include <stdio.h>
#endif 

typedef enum {
    DSA_DICE_CONSTANT, DSA_DICE
} dice_item_type_t;

typedef struct _dice_item_data {
    dice_item_type_t type;
    int factor;                 /* -1 if negative and 1 if positive */
    unsigned int cnt;           /* minimum cnt is 1 ex.: 3w20 (cnt of 3), w6 or 1w6 (cnt of 1) */
    unsigned int max;           /* max random value of dice ex: 2w20 (max is 20), w6 (max is 6), w50 (max is 50)
                                   As constant max is used to save the value */
} dice_item_data_t;

typedef struct _dice_item_ {
    dice_item_data_t data;
    struct _dice_item_* next;
    unsigned int (*get_value)(dice_item_data_t*);
} dice_item_t;

typedef struct {
    char *raw;          /* raw string representation if exist */
    dice_item_t *first; /* first dice item, holds NULL if an Error occurs */
    dice_item_t *last; /* first dice item, holds NULL if an Error occurs */
} dice_t;

dice_t* dsa_dice_new(const char *dice_pattern);

void dsa_dice_free(dice_t **dice);

int dsa_dice_roll(dice_t *dice);

int dsa_dice_result(const char *dice_pattern);

int dsa_dice_min(dice_t *dice);
int dsa_dice_min_result(const char *dice_pattern);

int dsa_dice_max(dice_t *dice);
int dsa_dice_max_result(const char *dice_pattern);

#endif

