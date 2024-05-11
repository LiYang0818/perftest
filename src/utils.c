#include "utils.h"
#include "get_clock.h"

unsigned int cycles_per_second(struct perftest_parameters *user_param){
	return get_cpu_mhz(user_param->cpu_freq_f) * 1000000;
}