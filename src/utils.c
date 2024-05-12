#include <unistd.h>
#include "utils.h"
#include "get_clock.h"

uint64_t cycles_per_second(struct perftest_parameters *user_param){
	return get_cpu_mhz(user_param->cpu_freq_f) * 1000000;
}

// Function to set CPU affinity of a thread
int set_cpu_affinity(pthread_t thread, int cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    return pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
}