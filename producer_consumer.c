#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

#define BUFFER_SIZE 10

void *ProducerFunc(void *ptr);
void *ConsumerFunc(void *ptr);
int GetRandomNumber();

pthread_mutex_t mutex;
sem_t empty; // Number of empty slots in the buffer
sem_t full;  // Number of full slots in the buffer
int buffer[BUFFER_SIZE + 1];
bool active = true;

int main(int argc, char **argv) {
	// INPUT DATA
	int sleep_number;
	int num_of_producers;
	int num_of_consumers;

	if (argc != 4) {
		printf("Usage: %s <sleep_time> <num_producers> <num_consumers>\n", argv[0]);
		exit(0);
	}

	sleep_number = atoi(argv[1]);
	num_of_producers = atoi(argv[2]);
	num_of_consumers = atoi(argv[3]);
	printf("Number of Producers: %d\nNumber of Consumers: %d\n", num_of_producers, num_of_consumers);

	// INIT semaphore (empty & full), INIT mutex
	if (sem_init(&empty, 0, BUFFER_SIZE) != 0) {
		fprintf(stderr, "Failed to initialize empty\n");
	}
	if (sem_init(&full, 0, 0) != 0) {
		fprintf(stderr, "Failed to initialize full\n");
	}
	if (pthread_mutex_init(&mutex, NULL) != 0) {
		fprintf(stderr, "Failed to initialize mutex\n");
	}

	// DECLARE THREAD ARRAY
	pthread_t producer_threads[num_of_producers];
	pthread_t consumer_threads[num_of_consumers];
	// INT ARRAY USED FOR COUNTING
	int producer_arr[num_of_producers];
	int consumer_arr[num_of_consumers];
	for (int i = 0; i < num_of_producers; i++) {
		producer_arr[i] = i;
	}
	for (int i = 0; i < num_of_consumers; i++) {
		consumer_arr[i] = i;
	}

	for (int i = 0; i < num_of_producers; i++) {
		pthread_create(&producer_threads[i], NULL, ProducerFunc, (void*) &producer_arr[i]);
		printf("Create producer thread: %d\n", i);
	}
	for (int i = 0; i < num_of_consumers; i++) {
		pthread_create(&consumer_threads[i], NULL, ConsumerFunc, (void*) &consumer_arr[i]);
		printf("Create consumer thread: %d\n", i);
	}

	sleep(sleep_number);
	printf("\n");
	active = false;

	int iretP, iretC;
	int index = 0;
	while (index < num_of_producers) {
		iretP = pthread_join(producer_threads[index], NULL);
		printf("Producer thread %d returns: %d\n", index, iretP);
		index++;
	}
	index = 0;
	while (index < num_of_consumers) {
		iretC = pthread_join(consumer_threads[index], NULL);
		printf("Consumer thread %d returns: %d\n", index, iretC);
		index++;
	}
	pthread_mutex_destroy(&mutex);
	sem_destroy(&empty);
	sem_destroy(&full);
	exit(0);
}

void* ProducerFunc(void *ptr) {
	int thread_num = *((int *)ptr);

	do {
		int random_number = GetRandomNumber();

		if (active) {
			sem_wait(&empty);
		} else return NULL;

		pthread_mutex_lock(&mutex);
		int full_value;
		sem_getvalue(&full, &full_value);
		buffer[full_value] = random_number;
		if (active) printf("Producer thread %d produced value: %d\n", thread_num, random_number);
		pthread_mutex_unlock(&mutex);
		// Increase full semaphore to notice buffer is being pushed
		if (active) {
			sem_post(&full);
		}
		sleep(5);
	} while (active);
	sem_post(&empty);
	return NULL;
}

void* ConsumerFunc(void *ptr) {
	int thread_num = *((int *)ptr);
	int consumed_num;
	do {
		// Check if full != 0, if it is block
		if (active)
			sem_wait(&full);
		else return NULL;

		pthread_mutex_lock(&mutex);
		int full_value;
		sem_getvalue(&full, &full_value);
		consumed_num = buffer[full_value]; // Consume the last produced item
		buffer[full_value] = 0;
		if (active) printf("Consumer %d consumed %d\n", thread_num, consumed_num);
		pthread_mutex_unlock(&mutex);
		// increase empty to notice buffer has space
		if (active) sem_post(&empty);
		sleep(5);
	} while (active);
	// mechanism to avoid deadlock
	sem_post(&full);
	return NULL;
}

int GetRandomNumber() {
	return rand() % 20 + 1; // Generate a random number between 1 and 100
}
