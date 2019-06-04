#include "dice.h"

typedef enum {
    DSA_DICE_PARSER_NEW, 
    DSA_DICE_PARSER_CHUNK_NEW, 
    DSA_DICE_PARSER_CHUNK_END,
    DSA_DICE_PARSER_CHUNK_PROC,
    DSA_DICE_PARSER_CHUNK_ERROR,
    DSA_DICE_PARSER_FOUND_FACTOR
} dice_parser_state_no_t;

typedef struct {
    char * chr;
    bool isFactor;
    bool isDice;
    bool isDigit;
    bool isEnd;
    bool is_valid_sign;
} dsa_parser_char_state_t;

typedef struct {
    dice_parser_state_no_t last_state;
    dice_parser_state_no_t state;
    bool isDice;
    dice_item_data_t item;
    dsa_parser_char_state_t l_char;
    dsa_parser_char_state_t c_char;
    char *chunk_start;
    char *chunk_end;
} dice_parser_state_t;

static void __dsa_parser_reset_char_state(dsa_parser_char_state_t *state) {
    state->isDice = false;
    state->isDigit = false;
    state->isEnd = false;
    state->isFactor = false;
    state->is_valid_sign= false;
    state->chr= NULL;
}

static void __dsa_parser_update_char_state(dsa_parser_char_state_t *state, char * chr) {
    
    unsigned char c_char = *chr;

    state->isFactor = (c_char == '-' || c_char == '+');
    state->isDice = (c_char == 'W' || c_char == 'w');
    state->isDigit = ( isdigit(c_char) != 0 );
    state->isEnd = c_char == '\0';
    state->is_valid_sign = state->isFactor || state->isDice || state->isDigit || state->isEnd;
    state->chr = chr;
}

static void __dsa_parser_update_last_char_state(dice_parser_state_t *state) {
    state->l_char.isDice = state->c_char.isDice;
    state->l_char.isFactor = state->c_char.isFactor;
    state->l_char.isDigit = state->c_char.isDigit;
    state->l_char.isEnd = state->c_char.isEnd;
    state->l_char.is_valid_sign = state->c_char.is_valid_sign;
    state->l_char.chr = state->c_char.chr;
}

static void __dsa_dice_parser_set_state(dice_parser_state_t *state, dice_parser_state_no_t new_state_no) {
    state->last_state = state->state;
    state->state = new_state_no;
}

static unsigned int __dsa_dice_constant(dice_item_data_t* data) {
    return data->factor * data->max;
}

static unsigned int __dsa_dice_random(dice_item_data_t* data) {
    return data->factor * (unsigned int)nu_random_min_max(1, data->max);
}

static void __dsa_dice_parser_begin(dice_parser_state_t *state, char *cur) {
    state->state = DSA_DICE_PARSER_NEW;
    state->chunk_start = cur;
    __dsa_parser_reset_char_state(&state->l_char);
    __dsa_parser_reset_char_state(&state->c_char);
    state->chunk_end = NULL;
    state->isDice = false;
    state->item.factor = 1;
    state->item.cnt = 0;
    state->item.max = 0;
}

static void __dsa_dice_parser_factor(dice_parser_state_t *state, dice_t *dice) {
    dsa_parser_char_state_t *c_char = &state->c_char;
    dsa_parser_char_state_t *l_char = &state->l_char;

    if ( state->state == DSA_DICE_PARSER_NEW ) {
        state->item.factor = ( *c_char->chr == '-' ? -1 : 1 );
    } else {
        //reaching factor sign indicates end of last chunk
        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_END);
    }
}

static void __dsa_dice_parser_digit(dice_parser_state_t *state, dice_t *dice) {
    dsa_parser_char_state_t *c_char = &state->c_char;
    dsa_parser_char_state_t *l_char = &state->l_char;

    if ( state->state == DSA_DICE_PARSER_NEW ) {
        state->chunk_start = c_char->chr;
    } else {
        //reaching factor sign indicates end of last chunk
        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_END);
    }
}

static void __dsa_dice_parser_chunk_update(dice_parser_state_t *state, dice_t *dice, char *cur) {

    __dsa_parser_update_char_state(&state->c_char, cur);

    dsa_parser_char_state_t *c_char = &state->c_char;
    dsa_parser_char_state_t *l_char = &state->l_char;

    if ( c_char->is_valid_sign ) {
        if (c_char->isFactor) { __dsa_dice_parser_factor(state, dice); }
        else if ( c_char->isDigit ) { __dsa_dice_parser_digit(state, dice); }
    } else {
        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_ERROR);
    }

    __dsa_parser_update_last_char_state(state);
}

static void __dsa_dice_parser_chunk_end(dice_parser_state_t *state, dice_t *dice, char *cur) {
    //here we should build result
}

static void __dsa_dice_parser_update(dice_parser_state_t *state, dice_t *dice, char *cur){
    #if debug > 0
        printf("parse char \'%c\'\n", *cur);
    #endif
    switch(state->state) {
        case DSA_DICE_PARSER_NEW:
        case DSA_DICE_PARSER_CHUNK_PROC:
        case DSA_DICE_PARSER_CHUNK_NEW: __dsa_dice_parser_chunk_update(state, dice, cur); break;
        case DSA_DICE_PARSER_CHUNK_END: __dsa_dice_parser_chunk_end(state, dice, cur); break;
        default: break;
    }
}

static void __dsa_dice_parse(dice_t *dice) {
    
    const char * dpattern = dice->raw;
    char *cur = (char *)dpattern;
    dice_parser_state_t ps;

    do {
        __dsa_dice_parser_update(&ps, dice, cur);
        cur++;
    } while (*cur != '\0' && ps.state != DSA_DICE_PARSER_CHUNK_ERROR);
}


#if 0
//######################################################################################################
//Eof private section
//######################################################################################################
#endif


dice_t* dsa_dice_new(const char *dice_pattern) {
    dice_t* newdice = malloc(sizeof(dice_t));
    newdice->raw = copy_string(dice_pattern);
    newdice->first = NULL;

    __dsa_dice_parse(newdice);

    return newdice;
}

void dsa_dice_free(dice_t **dice) {
    if (dice != NULL && *dice != NULL) {
        dice_t *todelete = *dice;

        //TODO free all items
        dice_item_t *cur_item = todelete->first;
        dice_item_t *next_item = NULL;
        while(cur_item != NULL) {
            next_item = cur_item->next;
            free(cur_item);
            cur_item = next_item;
        }

        free(todelete->raw);
        free(todelete);
        *dice = NULL;
    }
}

unsigned int dsa_dice_roll(dice_t *dice) {
    return 0;
}