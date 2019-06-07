#include "dice.h"

typedef enum {
    DSA_DICE_PARSER_NEW, DSA_DICE_PARSER_END, DSA_DICE_PARSER_CHUNK_NEW, 
    DSA_DICE_PARSER_CHUNK_END, DSA_DICE_PARSER_CHUNK_PROC, DSA_DICE_PARSER_CHUNK_ERROR
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
    char *start;
    char *end;
    bool isDice;            /* its a dice chunk otherwise a constant chunk */
    dice_item_data_t data;  /* extracted chunk data */
} dsa_parser_chunk_t;

typedef struct {
    dice_parser_state_no_t last_state;  /* last parser state */
    dice_parser_state_no_t state;       /* current parser state */
    dsa_parser_char_state_t l_char;     /* last character state */
    dsa_parser_char_state_t c_char;     /* current character state */
    dsa_parser_chunk_t chunk;           /* current chunk data */
} dice_parser_state_t;

static unsigned int __dsa_dice_str_to_num(dice_parser_state_t *state) {
    unsigned int digit_len = state->chunk.end - state->chunk.start; //+1 for null termination
    char buffer[256]; //so dice supports only numbers with 256 digits, it should be enough

    memcpy(&buffer[0], state->chunk.start, digit_len);
    buffer[digit_len] = '\0';

    unsigned int result = atoll(&buffer[0]);

    return result;
}

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
    state->isEnd = (c_char == '\0');
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

static void __dsa_parser_reset_chunk(dsa_parser_chunk_t *chunk) {
    chunk->start = NULL;
    chunk->end = NULL;
    chunk->isDice = false;
    chunk->data.factor = 1;
    chunk->data.cnt = 0;
    chunk->data.max = 0;
}

static void __dsa_dice_parser_set_state(dice_parser_state_t *state, dice_parser_state_no_t new_state_no) {
    state->last_state = state->state;
    state->state = new_state_no;
}

static unsigned int __dsa_dice_constant(dice_item_data_t* data) {
    return data->factor * data->max;
}

static unsigned int __dsa_dice_random(dice_item_data_t* data) {
    unsigned int result = 0;

    const unsigned int maxCnt = data->cnt;
    const unsigned int maxValue = data->max;

    for ( unsigned int cnt = 0; cnt < maxCnt; ++cnt) {
        result += (unsigned int)nu_random_min_max(1, maxValue+1);
    }

    return data->factor * result;
}

static void __dsa_dice_parser_begin(dice_parser_state_t *state, char *cur) {
    state->state = DSA_DICE_PARSER_NEW;
    __dsa_parser_reset_char_state(&state->l_char);
    __dsa_parser_reset_char_state(&state->c_char);
    __dsa_parser_reset_chunk(&state->chunk);
    state->chunk.start = cur;
}

static void __dsa_dice_parser_factor(dice_parser_state_t *state, dice_t *dice) {
    dsa_parser_char_state_t *c_char = &state->c_char;

    #if debug > 1
        printf("factor: ");
    #endif

    if ( state->state == DSA_DICE_PARSER_NEW ) {
        state->chunk.data.factor = ( *c_char->chr == '-' ? -1 : 1 );

        #if debug > 1
            printf(" after new parsing start => %i\n", state->chunk.data.factor);
        #endif

        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_PROC);
        
    } else {
        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_END);
        state->chunk.end = c_char->chr;
        state->chunk.data.max = __dsa_dice_str_to_num(state);
        #if debug > 1
            printf(" chunk ends with max: %i \n", state->chunk.data.max);
        #endif
    }
}

static void __dsa_dice_parser_digit(dice_parser_state_t *state, dice_t *dice) {
    dsa_parser_char_state_t *c_char = &state->c_char;
    dsa_parser_char_state_t *l_char = &state->l_char;
    
    #if debug > 1
        printf("digit:");
    #endif

    if ( state->state == DSA_DICE_PARSER_NEW ) {
        //Digit at start
        state->chunk.start = c_char->chr;
        state->chunk.end = c_char->chr;

        if ( c_char->isDigit ) {
            state->chunk.data.factor = 1;
        }

        #if debug > 1
            printf(" after parsing start new\n");
        #endif

    } else if (state->last_state == DSA_DICE_PARSER_CHUNK_NEW) {
        state->chunk.start = c_char->chr;
        state->chunk.end = c_char->chr;

        if ( c_char->isDigit ) {
            state->chunk.data.factor = 1;
        }

        #if debug > 1
            printf(" after new chunk\n");
        #endif

    } else if ( l_char->isDigit ) { 
        //continue examine digit
        state->chunk.end = c_char->chr;

        #if debug > 1
            printf(" continue fetch digit\n");
        #endif

    } else if ( l_char->isFactor ) {

        state->chunk.data.factor = ( *l_char->chr == '-' ? -1 : 1 );
        state->chunk.start = c_char->chr;
        state->chunk.end = c_char->chr;

        #if debug > 1
            printf(" on new chunk\n");
        #endif

    } else if (l_char->isDice) {
        state->chunk.start = c_char->chr;
        state->chunk.end = c_char->chr;
        
        #if debug > 1
            printf(" max value from dice\n");
        #endif
    }

    __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_PROC);

}

static void __dsa_dice_parser_dice(dice_parser_state_t *state, dice_t *dice) {

    dsa_parser_char_state_t *c_char = &state->c_char;
    dsa_parser_char_state_t *l_char = &state->l_char;

    #if debug > 1
        printf("dice:");
    #endif
    if (state->state == DSA_DICE_PARSER_NEW ) {
        state->chunk.data.cnt = 1;
        state->chunk.data.factor = 1;

        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_PROC);

        #if debug > 1
            printf(" at pattern start with cnt %i\n", state->chunk.data.cnt);
        #endif

    } else if (l_char->isDigit) {
        state->chunk.end = c_char->chr;
        state->chunk.data.cnt = __dsa_dice_str_to_num(state);
        
        #if debug > 1
            printf(" cnt is %i\n", state->chunk.data.cnt);
        #endif

        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_PROC);
    } else if (l_char->isFactor) {
        //dice without counter => set counter to 1
        state->chunk.data.cnt = 1;
        
        #if debug > 1
            printf(" cnt is %i\n", state->chunk.data.cnt);
        #endif
        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_PROC);
    } else {
        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_ERROR);

        #if debug > 1
            printf(" error oO\n");
        #endif

    }

    //current chunk is a dice chunk
    state->chunk.isDice = true;

}

static void __dsa_dice_parser_end(dice_parser_state_t *state, dice_t *dice) {

    dsa_parser_char_state_t *c_char = &state->c_char;
    dsa_parser_char_state_t *l_char = &state->l_char;

    #if debug > 1
        printf("end:");
    #endif

    if (l_char->isDigit) {

        state->chunk.end = c_char->chr;
        state->chunk.data.max = __dsa_dice_str_to_num(state);

        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_END);

        #if debug > 1
            printf(" with digit %i\n", state->chunk.data.max);
        #endif
        
    } else {

        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_ERROR);
    
        #if debug > 1
            printf(" with error oO \n");
        #endif
    }
}


static void __dsa_dice_parser_chunk_update(dice_parser_state_t *state, dice_t *dice, char *cur) {

    __dsa_parser_update_char_state(&state->c_char, cur);

    dsa_parser_char_state_t *c_char = &state->c_char;

    if ( c_char->is_valid_sign ) {
        if (c_char->isFactor) { __dsa_dice_parser_factor(state, dice); }
        else if ( c_char->isDigit ) { __dsa_dice_parser_digit(state, dice); }
        else if ( c_char->isDice ) { __dsa_dice_parser_dice(state, dice); }
        else if ( c_char->isEnd ) { __dsa_dice_parser_end(state, dice); }
    } else {
        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_ERROR);
    }

    __dsa_parser_update_last_char_state(state);

}

static void __dsa_dice_parser_chunk_end(dice_parser_state_t *state, dice_t *dice, char *cur) {

    //here we should build result
    dice_item_t *new_item = malloc(sizeof(dice_item_t));
    new_item->next = NULL;
    new_item->data.cnt = state->chunk.data.cnt;
    new_item->data.factor = state->chunk.data.factor;
    new_item->data.max = state->chunk.data.max;
    
    #if debug > 1
        printf("new item: factor %i cnt %i max %i \n",new_item->data.factor, new_item->data.cnt, new_item->data.max);
    #endif
    
    if ( state->chunk.isDice ) {
        new_item->get_value = __dsa_dice_random;
    } else {
        new_item->get_value = __dsa_dice_constant;
    }

    if ( dice->first == NULL ) {
        dice->first = new_item;
    } else {
        dice->last->next = new_item;
    }

    dice->last = new_item;

    #if debug > 1
        printf("new %p first %p last %p", new_item, dice->first, dice->last);
    #endif

    //reset chunkstate
    __dsa_parser_reset_chunk(&state->chunk);

    if(!state->c_char.isEnd) {
        __dsa_dice_parser_set_state(state, DSA_DICE_PARSER_CHUNK_NEW);

        #if debug > 1
            printf("start new chunk\n");
        #endif

    }

}

static void __dsa_dice_parser_update(dice_parser_state_t *state, dice_t *dice, char *cur){

    unsigned int no = state->state;

    switch(no) {
        case DSA_DICE_PARSER_NEW: __dsa_dice_parser_begin(state, cur);
        case DSA_DICE_PARSER_CHUNK_PROC:
        case DSA_DICE_PARSER_CHUNK_NEW: __dsa_dice_parser_chunk_update(state, dice, cur); break;
        default: break;
    }

    if ( state->state == DSA_DICE_PARSER_CHUNK_END ||
         state->state == DSA_DICE_PARSER_END ) {
        __dsa_dice_parser_chunk_end(state, dice, cur);
    }
}

static void __dsa_dice_parse(dice_t *dice) {
    
    const char * dpattern = dice->raw;
    char *cur = (char *)dpattern;
    dice_parser_state_t ps;

    #if debug > 1
        printf("## Pattern: %s ##\n", dpattern);
    #endif

    do {
        __dsa_dice_parser_update(&ps, dice, cur);
        cur++;
    } while (ps.state != DSA_DICE_PARSER_CHUNK_ERROR && ps.state != DSA_DICE_PARSER_END);
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
    newdice->last = NULL;
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

int dsa_dice_roll(dice_t *dice) {
    int result = 0;
    
    dice_item_t *cur_item = dice->first;
    dice_item_t *next_item = NULL;
    while(cur_item != NULL) {
        next_item = cur_item->next;
        int current =  cur_item->get_value(&cur_item->data);

        #if debug > 1
            printf("single Dice: %i\n", current);
        #endif

        result += current;

        cur_item = next_item;
    }

    return result;
}

int dsa_dice_result(const char *dice_pattern) {
    dice_t* dice = dsa_dice_new(dice_pattern);

    int dice_result = dsa_dice_roll(dice);

    dsa_dice_free(&dice);

    return dice_result;
}