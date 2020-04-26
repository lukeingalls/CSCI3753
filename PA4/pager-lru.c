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
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proc_iter; //variable used for process number in intialization of timestamps
    int page_iter; //sames as proc_iter but for pages
    int proc;
    int pc;
    int page;
    int swap_page;
    int min_time;


    /* initialize static vars on first run */
    if(!initialized){
    	for(proc_iter=0; proc_iter < MAXPROCESSES; proc_iter++){

    	    for(page_iter=0; page_iter < MAXPROCPAGES; page_iter++){
    		  timestamps[proc_iter][page_iter] = 0; 
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
