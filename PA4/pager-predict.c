/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 *  This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

int lru_replace(Pentry q[MAXPROCESSES], int timestamps[MAXPROCESSES][MAXPROCPAGES], int tick, int proc) {
    int min_time = tick;  
    int swap_page;    
    for(int page_iter = 0; page_iter < MAXPROCPAGES; page_iter++){

        if(q[proc].pages[page_iter] && min_time > timestamps[proc][page_iter]){
            swap_page = page_iter;
            min_time = timestamps[proc][page_iter];
        }
    }
    return swap_page;
}

void swap(int *l, int *r){
    int ptr = *l;
    *l = *r;
    *r = ptr;
}

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* static vars */
    /* givens */
    static int initialized = 0;
    static int tick = 1; // artificial time
    /* created */
    static int prev_pc[MAXPROCESSES]; /* lalst used process process counter */
    static int timestamps[MAXPROCESSES][MAXPROCPAGES]; /* time stamp for pages and procs */
    /* use the page timestamps, number, and frequency~ to determine when it should be paged out. */
    /* use 3d array so each element has something else it is pointing to for another flow patern */
    static int *CTRL_FLOW_timestamps[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
    static int flow_page[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
    static int proc_transitions[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
    /* Local vars */
    /* we were given none :c */
    /* so like with the 3ed array we need to determine it for bekadys algorithm.*/
    int **PRED_timestamps;
    int *PRED_PG_NUM;
    int *PRED_FREQ;

    /* local vars to determine lru and last/current page loc */
    int proc;
    int curr_page;
    int prev_page = 0;
    int NUM_PG_FILLED;
    int forw_page;
    int i, j, k;

    /* like in the lru, initialized the static vars */
    if(!initialized){
        /* initialized the cmplx vars */
        for(i = 0; i < MAXPROCESSES; i++){
            for(j = 0; j < MAXPROCESSES; j++){
                for(k = 0; k < MAXPROCESSES; k++){
                    /* loop through elems in 3D array */
                    proc_transitions[i][j][k] = -1;
                    flow_page[i][j][k] = -1;
                    CTRL_FLOW_timestamps[i][j][k] = NULL;
                } /* end for */
            } /* end for */
        } /* end for */
        for(proc = 0; proc < MAXPROCESSES; proc++){
            for(i = 0; i < MAXPROCPAGES; i++){
                timestamps[proc][i] = 0;
            }
        }
        /* after its finished, its fully initialized */
        initialized = 1;
    }
    
    /* TODO: Implement Predictive Paging */
    /* going to udate the ctrl flow for each process */
    for(proc = 0; proc < MAXPROCESSES; proc++){
        if(q[proc].active){
            prev_page = prev_pc[proc] / PAGESIZE;
            prev_pc[proc] = q[proc].pc;
            curr_page = q[proc].pc / PAGESIZE;

            timestamps[proc][curr_page] = tick;

            if(curr_page != prev_page){
                pageout(proc, prev_page);
                /* for throughout the proc last page */
                for(i = 0; i < MAXPROCPAGES; i ++){
                    /* the magic of belady */
                    if(curr_page == flow_page[proc][prev_page][i]){
                        /* the freq goe up */
                        proc_transitions[proc][prev_page][i]++;
                        break;
                    } /* end if */
                    if(flow_page[proc][prev_page][i] == -1){
                        proc_transitions[proc][prev_page][i] = 1;
                        flow_page[proc][prev_page][i] = curr_page;
                        CTRL_FLOW_timestamps[proc][prev_page][i] = &(timestamps[proc][i]);
                        break;
                    }
                }
            }
        }
    }

    /* active process page swap */
    for(proc = 0; proc < MAXPROCESSES; proc++) {
        
        if(q[proc].active){

            curr_page = (q[proc].pc) / PAGESIZE;
            if(!q[proc].pages[curr_page]) {
                pagein(proc, curr_page);
            }
        } else {
            // Get rid of inactive pages
            for(curr_page = 0; curr_page < MAXPROCPAGES; curr_page++){
                pageout(proc, curr_page);
            }
        }
    }

    for(proc = 0; proc < MAXPROCESSES; proc++){
        if(q[proc].active){
            /* learning algorithm */
            NUM_PG_FILLED = 0;

            // What page will we be on after swap?
            forw_page = (q[proc].pc + PAGEWAIT + 1) / PAGESIZE;

            PRED_timestamps = CTRL_FLOW_timestamps[proc][forw_page];
            PRED_FREQ = proc_transitions[proc][forw_page];
            PRED_PG_NUM = flow_page[proc][forw_page];
            /* look for empty pages and if not inc the num o filled pages. eleminate seg faults & pg faults */
            for(i = 0; i < MAXPROCPAGES; i++){
                if((NUM_PG_FILLED < MAXPROCPAGES) && (PRED_PG_NUM[NUM_PG_FILLED] != -1)){
                    NUM_PG_FILLED++;
                }
            }
            /* go until the t is less than tehe cur t, swhap bc beladys */
            for(i = 0; i < MAXPROCPAGES; i++){
                for(j = 1; j < NUM_PG_FILLED; j++){
                    if((*PRED_timestamps[j]) > (*PRED_timestamps[j - 1])){
                        if((PRED_FREQ[j]) > (PRED_FREQ[j - 1])){
                            swap(PRED_PG_NUM + (j - 1), PRED_PG_NUM + j);
                            swap(*PRED_timestamps + (j - 1), *PRED_timestamps + j);
                            swap(PRED_FREQ + (j - 1), PRED_FREQ + j);
                        }
                    }
                }
            }
            /* its all sorted ptimally so we swap in*/
            for(i = 0; i < NUM_PG_FILLED; i++){
                pagein(proc, PRED_PG_NUM[i]);
            }
        }
    }


    tick++;
}
