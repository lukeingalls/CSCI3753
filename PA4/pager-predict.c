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


double archs[][MAXPROCPAGES][MAXPROCPAGES] = {
    {
        {0, 13.0/1151.0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  13.0/1151.0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  13.0/1151.0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  13.0/1151.0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  13.0/1151.0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  13.0/1151.0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  13.0/1151.0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  13.0/1151.0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  13.0/1151.0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  207.0/1151.0,   0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  207.0/1151.0,   0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  207.0/1151.0,   0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  207.0/1151.0,   0,  0,  0,  0,  0,  0},
        {12.0/1151.0,   0,  0,  0,  0,  0,  0,  0,  0,  194.0/1151.0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
    },
    {
        {0, 51.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  51.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  51.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {20.0/558.0,    0,  0,  0,  31.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  31.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  31/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  31.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  31.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {30.0/558.0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
    },
    {
        {0, 45.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  45.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  45.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0,    0,  0,  0,  45.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  45.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  45.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  45.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  45.0/558.0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {44.0/558.0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
    }
};

// int transition[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];


void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    // static int transition[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
    static int prev_page[MAXPROCESSES];
    static int arch_track[MAXPROCESSES][3];
    /* Local vars */
    int proc_iter; //variable used for process number in intialization of timestamps
    int page_iter; //sames as proc_iter but for pages
    int proc;
    int pc;
    int page;
    int swap_page;
    int min_time;
    int iterator;
    int accum_iter;
    float accum[MAXPROCPAGES];
    int max_accum;

    /* initialize static vars on first run */
    if(!initialized){
        for(proc_iter=0; proc_iter < MAXPROCESSES; proc_iter++){
            prev_page[proc_iter] = 0;
            for(page_iter=0; page_iter < MAXPROCPAGES; page_iter++){
              timestamps[proc_iter][page_iter] = 0; 
              for (proc = 0; proc < MAXPROCPAGES; proc++)
              transition[proc_iter][proc][page_iter] = 0;
            }
            for (iterator = 0; iterator < 3; iterator++) {
                arch_track[proc_iter][iterator] = 1;
            }
        }
        initialized = 1;
    }
    
    /* TODO: Implement LRU Paging */
    for(proc=0; proc<MAXPROCESSES; proc++) { 
        // Identify Active Processes
        if(q[proc].active) {
            // Page data
            pc = q[proc].pc;
            page = pc/PAGESIZE;

            timestamps[proc][page] = tick; // Update actively used pages
            if (page - prev_page[proc] != 1 && page - prev_page[proc] != 0) {
                // printf("JUMP! %d, PROCESS %d\n", page - prev_page[proc], proc);
            }
            if (prev_page[proc] != page) {
                for (iterator = 0; iterator < 3; iterator++) {
                    if (arch_track[proc][iterator]) {
                        if (archs[iterator][prev_page[proc]][page] == (float)0) {
                            arch_track[proc][iterator] = 0;
                            printf("Proc %d eliminates Arch %d on criteria %d to %d\n", proc, iterator + 1, prev_page[proc], page);
                        }
                        for (accum_iter = 0; accum_iter < q[proc].npages; accum_iter++) {

                            if (accum_iter == 0 && archs[iterator][proc][accum_iter] == 0) {
                                accum[accum_iter] = 0;
                            } else {
                                accum[accum_iter] = accum[accum_iter] + archs[iterator][proc][accum_iter];
                            }
                        }
                    }
                }
                max_accum = 0;
                for (accum_iter = 1; accum_iter < q[proc].npages; accum_iter++) {
                    if (accum[accum_iter] > accum[max_accum]) max_accum = accum_iter;
                }
                transition[proc][prev_page[proc]][page] += 1;
                if(!q[proc].pages[max_accum]) {
                    if (!pagein(proc,max_accum)) {    
                        min_time = 0;      
                            for(page_iter = max_accum - 2; page_iter >= 0; page_iter--){
                                if(q[proc].pages[page_iter]){
                                    pageout(proc,page_iter);
                                    min_time = 1;
                                    break;
                                }
                            }
                            for(page_iter = MAXPROCPAGES - 1; page_iter > max_accum && !min_time; page_iter--){
                                if(q[proc].pages[page_iter]){
                                    pageout(proc,page_iter);
                                    break;
                                }
                            }
                    }
                }
            }
            prev_page[proc] = page;
            /* Check Page Status and swap in if necessary*/
            if(!q[proc].pages[page]) {
                if (!pagein(proc,page)) {    
                    min_time = tick;      
                        for(page_iter = 0; page_iter < MAXPROCPAGES; page_iter++){

                            if(q[proc].pages[page_iter] && min_time > timestamps[proc][page_iter]){
                                swap_page = page_iter;
                                min_time = timestamps[proc][page_iter];
                            }
                        }
                    // evict that oldest page.
                    pageout(proc,swap_page);
                }
            }
        }
    } 
    tick++;
} 
