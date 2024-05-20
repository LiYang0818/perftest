
#ifndef UTILS_H
#define UTILS_H

#include "perftest_parameters.h"
#include <bits/pthreadtypes.h>

struct C_THREAD_ARGS {
	pthread_mutex_t* mutex;
	pthread_barrier_t* barrier;
	int argc;
	char **argv;
};

struct S_THREAD_ARGS {
	struct perftest_comm* user_comm;
	struct perftest_parameters* user_param;
	struct pingpong_context* ctx;
};

typedef void* (*FuncPtr)(void*);
uint64_t cycles_per_second(struct perftest_parameters *user_param);
int set_cpu_affinity(pthread_t thread, int cpu);

#endif
