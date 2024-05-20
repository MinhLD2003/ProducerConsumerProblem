
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

#define BUFFER_SIZE 10;
void *ProducerFunc(void *ptr);
void *ConsumerFunc(void *ptr);
int GetRandomNumber();


pthread_mutex_t mutex;
sem_t empty;
sem_t full;
int buffer[BUFFER_SIZE];
bool active = true;
int main(int argc , char ** argv) {
	// INPUT DATA
	try {
		int sleep_number = atoi(argv[1]);
		int num_of_producers = atoi(argv[2]);
		int num_of_consumers = atoi(argv[3]);
		printf("Number of Producers: %d \n,
		       Number of Consumers: %d\n",
		       num_of_producers ,
		       num_of_consumers);
	}
	catch (...) {
		printf("Exception Caught\n");
		exit(0);
	}

	// INIT semaphore (empty & full) , INIT mutex
	if (sem_init(&empty , 0 , BUFFER_SIZE) != 0) {
		fprintf(stderr, "Failed to initialize empty\n");
	}
	if (sem_init(&full, 0 , 0) != 0 ) {
		fprintf(stderr , "Failed to initialize full");
	}
	if (pthread_mutext_init(&mutex , NULL) != 0) {
		fprintf(stderr, "Failed to initialize mutex\n");
	}
	// DECLARE THREAD ARRAY
	pthread_t producer_threads[num_of_producers];
	pthread_t consumer_threads[num_of_consumers];
	// INT ARRAY USED FOR COUNTING
	int producer_arr[num_of_producers];
	int consumer_arr[num_of_consumers];
	for (int i = 0 ; i < num_of_producers ; i++) {
		producer_arr[i] = i;
	}
	for (int i = 0 ; i < num_of_consumers ; i++) {
		consumer_arr[i] = i;
	}

	for (int i = 0 ; i < num_of_producers ; i++) {
		pthread_create(&producer_threads[i] , NULL , ProducerFunc , (void*) &producer_arr[i] );
		printf("Create producer thread: %d\n" , i);
	}
	for (int i = 0 ; i < num_of_consumers ; i++) {
		pthread_create(&consumer_threads[i] , NULL , ConsumerFunc , (void*) &producer_arr[i] );
		printf("Create producer thread: %d\n" , i);
	}

	sleep(sleep_number);
	printf("\n");
	active = false;
	int  iretP, iretC;

	int index = 0;
	while (index < num_of_producers) {
		iretP = pthread_join(producer_threads[index] , NULL);
		printf("Producer thread %d returns: %d\n" , index , iretP);
	}
	index = 0;
	while (index < num_of_consumers) {
		iretC = pthread_join(consumer_threads[index] , NULL);
		printf("Consumer thread %d returns: %d\n" , index, iretC);
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
		int empty_value;
		sem_getvalue(&empty , &empty_value);
		if (active) {
			sem_wait(&empty);
		} else return NULL;

		pthread_mutext_lock(&mutex);
		int full_value;
		sem_getvalue(&full , &full_value);
		buffer[full_value] = random_number;
		if (active) printf("Producer thread %d produced value: %d\n" , thread_num , random_number );
		pthread_mutex_unlock(&mutex);

		if (active) {
			sem_post(&full);
		}
		sleep(5);
	} while (active);
	sem_post(&empty);
	return NULL;
}

void* ConsumerFunc(void *ptr) {
	int thread_num = *((int *) ptr);

	do {
		int empty_value;
		sem_getvalue(&empty , &empty_value);
		if(active) {
			sem_wait(&full);
		}

	} while (active);
	sem_post(&full);
}