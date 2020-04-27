#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"


// Implements the LRU page selection from pager-lru
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

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    static int prev_page[MAXPROCESSES];
    static int encountered[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
    static int enc_frequency[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];

    /* Local vars */
    int proc, page, prev, next;

    int i, temp;

    /* initialize static vars on first run */
    if(!initialized) {
        for(proc=0; proc < MAXPROCESSES; proc++) {
            for(page=0; page < MAXPROCPAGES; page++) {
                timestamps[proc][page] = 0;
                for(i=0;i<MAXPROCPAGES;i++){
                    encountered[page][proc][i]=-1;
                    enc_frequency[page][proc][i] = 0;
                }
            }
        }
        initialized = 1;
    }

    // Populate the encountered matrix
    for(proc=0; proc<MAXPROCESSES; proc++) {
        if(q[proc].active) {
            // Save the previous page
            prev = prev_page[proc];

            // Update the stored previous
            prev_page[proc] = q[proc].pc/PAGESIZE;

            // Get current page
            page = q[proc].pc/PAGESIZE;
            if(prev != page) {
                // Remove pages as we leave them
                pageout(proc, prev);

                for(i=0; i<MAXPROCPAGES; i++) {
                    //Page has already been encountered
                    if (encountered[proc][prev][i] == page) {
                        enc_frequency[proc][prev][i]++;
                        break;
                    } else if((encountered[proc][prev][i] == -1)) {
                    //Page hasn't yet been encountered on this transition
                        encountered[proc][prev][i] = page;
                        enc_frequency[proc][prev][i]++;
                        break;
                    }
                }
                // Sort page hits based on frequency
                while (i > 0) {
                    if (enc_frequency[proc][prev][i] > enc_frequency[proc][prev][i - 1]) {
                        temp = enc_frequency[proc][prev][i - 1];
                        enc_frequency[proc][prev][i - 1] = enc_frequency[proc][prev][i];
                        enc_frequency[proc][prev][i] = temp;
                        temp = encountered[proc][prev][i - 1];
                        encountered[proc][prev][i - 1] = encountered[proc][prev][i];
                        encountered[proc][prev][i] = temp;
                    }
                    i--;
                }
            }
        } else {
            for (i=0; i<MAXPROCPAGES; i++) {
                // Pageout inactive processes
                if (q[proc].pages[i]) pageout(proc, i);
            }
        }      
    }

    // predictively paging in pages based on the matrix built (if it has values for us to use to make predictions)
    for(proc=0; proc<MAXPROCESSES; proc++){
        if(q[proc].active) {
            // This is the page we would be on at the end of a PAGEWAIT (time to swap pages)
            next = (q[proc].pc + PAGEWAIT + 1)/PAGESIZE;

            for(i=0;i<MAXPROCPAGES;i++) {
                if(encountered[proc][next][i]==-1) {
                    // No more pages likely to be needed
                    break;
                } else {
                    // Pagein the most pages that have occured after this one previously (prioritizing the most frequent)
                    pagein(proc, encountered[proc][next][i]);
                }
            }
        }        
    }

    // LAST CASE SCENARIO use lru to get a page in that is currently needed.
    for(proc = 0; proc < MAXPROCESSES; proc++) {
        if(q[proc].active) {
            page = q[proc].pc/PAGESIZE;  
            if(!q[proc].pages[page] && !pagein(proc, page)) { 
                pageout(proc, lru_replace(q, timestamps, tick, proc));
            }
            timestamps[proc][page] = tick;
        }
    }
    tick++;
}

