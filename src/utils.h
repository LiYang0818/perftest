
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


// Define the structure for an IP host
struct Host {
    char* ip;  // Pointer to dynamically allocated memory for the IP address string
    struct Host* next;
};

// Define the structure for the host list, the list can only contains either IP4 or IP6 no mix
struct HostList {
    uint32_t version;
    struct Host* head;
	struct Host* current;
};

typedef void* (*FuncPtr)(void*);
uint64_t cycles_per_second(struct perftest_parameters *user_param);
int set_cpu_affinity(pthread_t thread, int cpu);


// Function to create a new node
struct Host* createHost(const char* ip);

// Function to initialize an empty list
void initHostList(struct HostList* list);

// Function to insert a new node at the beginning of the list
void insertAtBeginning(struct HostList* list, const char* ip);

// Function to print the elements of the list
void printHostList(struct HostList list);

// Function to free the memory allocated for the list
void freeHostList(struct HostList* list);

// Function to retrieve the next node from the list and wrap around to the beginning if the end is reached
struct Host* getNext(struct HostList* list);

#endif
