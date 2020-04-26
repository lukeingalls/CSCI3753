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

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    static int prev_page[MAXPROCESSES];
    static int frequency[MAXPROCESSES][MAXPROCPAGES];
    static int transition[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
    /* Local vars */
    int proc_iter; //variable used for process number in intialization of timestamps
    int page_iter; //sames as proc_iter but for pages
    int proc;
    int pc;
    int page;
    int swap_page;
    float min_freq;
    int next_page;

    /* initialize static vars on first run */
    if(!initialized){
        for(proc_iter=0; proc_iter < MAXPROCESSES; proc_iter++){
            prev_page[proc_iter] = 0;
            for(page_iter=0; page_iter < MAXPROCPAGES; page_iter++){
                timestamps[proc_iter][page_iter] = 0; 
                frequency[proc_iter][page_iter] = 0;
                for (proc = 0; proc < MAXPROCPAGES; proc++) { 
                    transition[proc_iter][proc][page_iter] = 1;
                }
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

            if (prev_page[proc] != page) {
                transition[proc][prev_page[proc]][page] += 1;
                prev_page[proc] = page;

            }
            timestamps[proc][page] = tick; // Update actively used pages
            frequency[proc][page] += 1;
            /* Check Page Status and swap in if necessary*/
            if(!q[proc].pages[page]) {
                if (!pagein(proc,page)) {    
                    min_freq = ~(-1 << 31);      
                        for(page_iter = 0; page_iter < MAXPROCPAGES; page_iter++){

                            if(q[proc].pages[page_iter] && min_freq > timestamps[proc][page_iter]*transition[proc][prev_page[proc]][page_iter]){
                                swap_page = page_iter;
                                min_freq = timestamps[proc][page_iter]*transition[proc][prev_page[proc]][page_iter];
                            }
                        }
                    // evict that oldest page.
                    pageout(proc,swap_page);

                } else {
                    pagein(proc, page + 1);
                }
            }
            prev_page[proc] = page;
        } else {
            pc = q[proc].pc;
            page = pc/PAGESIZE;
            for (page_iter = 0; page_iter < MAXPROCPAGES; page_iter++) {
                if (q[proc].pages[page_iter]) {
                    if (page_iter != page && page_iter != page + 1) {
                        pageout(proc, swap_page);
                    }
                }
            }
        }
    } 
    tick++;
} 
