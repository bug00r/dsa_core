#ifndef TAW_H
#define TAW_H

#include <stddef.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>

#if debug > 0
    #include <stdio.h>
#endif 

typedef struct {            
    int start;          /* beginning calculation taw */
    int end;            /* target calculation taw */
    unsigned int ap;    /* needed ap */
} taw_result_item_t;

typedef struct {
    taw_result_item_t *complete;    /* result of complete intervall */
    unsigned int cnt_details;       /* Number of calculated details */
    taw_result_item_t *details;     /* Details of taw calculation */
} taw_result_t;

typedef enum {
    A_, A, B, C, D, E, F, G, H  /* category columns */
} taw_col_t;

/*
	This function calculates an needed AP Amount of an TAW Intervall.
    The minimum start and end taw is -3 and the maximum end taw is 31.
    If this values will go out of scope they will be adjusted.
	
	Parameter		Decription
	---------		-----------------------------------------
	column			pointer to used taw calculation column
	start       	taw calculation starts with this level
    end       	    taw calculation ends with this level
	
	returns: pointer of taw_result_t with given taw result data.
*/
taw_result_t* taw_calc(taw_col_t *column, int start, int end);

/*
	This function calculates an needed AP Amount of an TAW Intervall.
    The minimum end taw is -3 and the maximum end taw is 31.
    If this value will go out of scope it will be adjusted.

    In summary this function will do the same like:
        
        taw_result_t* taw_calc(taw_col_t *column, int start, int end);

        But with automatic adjusted value start = end - 1


	Parameter		Decription
	---------		-----------------------------------------
	column			pointer to used taw calculation column
	end       	    taw calculation ends with this level
	
	returns: pointer of taw_result_t with given taw result data.
*/
taw_result_t* taw_calc_once(taw_col_t *column, int end);

/*
    This function frees the taw calculation result and sets the
    value of given pointer to NULL.

	Parameter		Decription
	---------		-----------------------------------------
	result			pointer to have to be deleted result
	
*/
void taw_result_free(taw_result_t **result);

#endif

