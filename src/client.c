/*
 * Copyright (c) 2005 Topspin Communications.  All rights reserved.
 * Copyright (c) 2005 Mellanox Technologies Ltd.  All rights reserved.
 * Copyright (c) 2009 HNR Consulting.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * $Id$
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "utils.h"

extern int write_bw_client(struct C_THREAD_ARGS* args);

int main(int argc, char *argv[])
{
    const int NUM_THREADS = 2;
	pthread_mutex_t pth_mutex;
	pthread_t threads[NUM_THREADS];
	pthread_barrier_t barrier;

	
	pthread_barrier_init(&barrier, NULL, NUM_THREADS);
	struct C_THREAD_ARGS thread_arg = {&pth_mutex, &barrier, argc, argv};
    
	// Create threads
	for (long i = 0; i < NUM_THREADS; ++i) {
		#if 0
		if(i>0){
			for (int j = 1; j<argc; j++)
			if(strcmp(argv[j],"10.0.0.1") == 0 ) argv[j] = "10.0.0.2"; 
		}
		#endif
		pthread_create(&threads[i], NULL, (FuncPtr)write_bw_client, (void*)&thread_arg);
		set_cpu_affinity(threads[i], i);	
		sleep(3);
    }
    // Join threads
    for (long i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    pthread_mutex_destroy(&pth_mutex);
    pthread_barrier_destroy(&barrier);
    
    return 0;
    
}

