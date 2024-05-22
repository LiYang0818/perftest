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

// Function to create a new node
struct Host* createHost(const char* ip) {
    struct Host* newHost = (struct Host*)malloc(sizeof(struct Host));
    if (newHost == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }
    newHost->ip = strdup(ip); // Allocate memory for the IP address string and copy the IP
    newHost->next = NULL;
    return newHost;
}

// Function to initialize an empty list
void initHostList(struct HostList* list) {
    list->head = NULL;
    list->current = NULL;
}

// Function to insert a new node at the beginning of the list
void insertAtBeginning(struct HostList* list, const char* ip) {
    struct Host* newHost = createHost(ip);
    newHost->next = list->head;
    list->head = newHost;
}

// Function to print the elements of the list
void printHostList(struct HostList list) {
    struct Host* current = list.head;
    fprintf(stdout,"List: ");
    while (current != NULL) {
        fprintf(stdout, "%s ", current->ip);
        current = current->next;
    }
    fprintf(stdout,"\n");
}

// Function to free the memory allocated for the list
void freeHostList(struct HostList* list) {
    struct Host* current = list->head;
    while (current != NULL) {
        struct Host* temp = current;
        current = current->next;
        free(temp->ip); // Free the memory allocated for the IP address string
        free(temp);
    }
}

// Function to retrieve the next node from the list and wrap around to the beginning if the end is reached
struct Host* getNext(struct HostList* list) {
    if (list->current == NULL) {
        list->current = list->head; // Start from the beginning if the current node is NULL
    } else {
        list->current = list->current->next; // Move to the next node
        if (list->current == NULL) {
            list->current = list->head; // Wrap around to the beginning if the end is reached
        }
    }
    return list->current;
}