#include "perftest_parameters.h"

extern uint64_t cycles_per_second(struct perftest_parameters *user_param);
extern int set_cpu_affinity(pthread_t thread, int cpu);