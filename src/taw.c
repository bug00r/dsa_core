#include "taw.h"

static const int tawvalues[9][35] = {
    {5,5,5,5,1,1,1,2,4,5,6,8,9,11,12,14,15,17,19,20,22,24,25,27,29,31,32,34,36,38,40,42,43,45,48},
    {5,5,5,5,1,2,3,4,6,7,8,10,11,13,14,16,17,19,21,22,24,26,27,29,31,33,34,36,38,40,42,44,45,47,50},
    {10,10,10,10,2,4,6,8,11,14,17,19,22,25,28,32,35,38,41,45,48,51,55,58,62,65,69,73,76,80,84,87,91,95,100},
    {15,15,15,15,2,6,9,13,17,21,25,29,34,38,43,47,51,55,60,65,70,75,80,85,95,100,105,110,115,120,125,130,135,140,150},
    {20,20,20,20,3,7,12,17,22,27,33,39,45,50,55,65,70,75,85,90,95,105,110,115,125,130,140,145,150,160,165,170,180,190,200},
    {25,25,25,25,4,9,15,21,28,34,41,48,55,65,70,80,85,95,105,110,120,130,135,145,155,165,170,180,190,200,210,220,230,240,250},
    {40,40,40,40,6,14,22,32,41,50,60,75,85,95,105,120,130,140,155,165,180,195,210,220,230,250,260,270,290,300,310,330,340,350,375},
    {50,50,50,50,8,18,30,42,55,70,85,95,10,125,140,160,175,190,210,220,240,260,270,290,310,330,340,360,380,400,420,440,460,480,500},
    {100,100,100,100,16,35,60,85,110,140,165,195,220,250,280,320,350,380,410,450,480,510,550,580,620,650,690,720,760,800,830,870,910,950,1000}
};

static const unsigned int coloffset = 3;
static const int lowest_taw_limit = -3;
static const int highest_taw_limit = 31;

static taw_result_t* taw_result_new() {
    taw_result_t *result = malloc(sizeof(taw_result_t));
    result->complete = NULL;
    result->details = NULL;
    result->cnt_details = 0;
    return result;
}

static void compute_result(taw_result_t *result, int *values, unsigned int *value_counter, int *start, int *end) {

    if ( *value_counter > 0 ) {

        result->details = malloc(*value_counter * sizeof(taw_result_item_t));

        unsigned int ap_sum = 0;
        unsigned int detail_counter = 0;

        for ( int *value = values; *value != INT_MAX ; value+=3, ++detail_counter) {
            taw_result_item_t *cur_item = &result->details[detail_counter];

            cur_item->start = *(value);
            cur_item->end = *(value+1);
            unsigned int cur_ap_value = *(value+2);

            cur_item->ap = cur_ap_value;

            ap_sum += cur_ap_value; 
        }

        result->complete = malloc(sizeof(taw_result_item_t));
        result->complete->start = *start;
        result->complete->end = *end;

        result->complete->ap = ap_sum;
        result->cnt_details = *value_counter;

    }
}

taw_result_t* taw_calc(taw_col_t *column, int _start, int _end) {
    
    int start = ( _start < lowest_taw_limit ? lowest_taw_limit : ( _start > highest_taw_limit ? highest_taw_limit : _start ));
    int end = ( _end > highest_taw_limit ? highest_taw_limit : ( _end < lowest_taw_limit ? lowest_taw_limit : _end ));

    int range = end - start;
    int add = ( range < 0 ? -1 : 1 );

    taw_result_t *result = taw_result_new();
    
    int values[115];
    for ( int i = 0; i < 115; ++i ) values[i] = INT_MAX;
    //memset(values, INT_MAX, 115 * sizeof(int));

    unsigned int value_counter = 0;

    #if debug > 0
        for(int i = 0; i < 115; i+=3) {
            printf(">> %i -> %i = %i\n", *values,*(values+1),*(values+1));
        }
    #endif

    /* calculation starts at one column right */
    int *values_ptr = values;
    for (int cur = (start + 1); cur <= end; cur += add) {
        *(values_ptr++) = cur - 1;
        *(values_ptr++) = cur;
        *(values_ptr++) = tawvalues[*column][cur + coloffset];
        ++value_counter;
    }

    #if debug > 0
        for(int i = 0; i < 102; i+=3) {
            printf("<< %i -> %i = %i\n", values[i],values[i+1],values[i+2]);
        }
    #endif

    compute_result(result, values, &value_counter, &start, &end);

    return result;
}

taw_result_t* taw_calc_once(taw_col_t *column, int end) {
    return taw_calc(column, end - 1, end);
}

void taw_result_free(taw_result_t **result) {
    if ( result != NULL && *result != NULL ) {
        taw_result_t *to_delete = *result;
        free(to_delete->complete);
        free(to_delete->details);
        free(to_delete);
        *result = NULL;
    } 
}

