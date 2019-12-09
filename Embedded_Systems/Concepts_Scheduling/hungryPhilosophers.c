#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t chopstick[5];

/* Philosopher Process. */

void *philosopher (void *pNum) {		// Variable i is the P number.
	/* Cast ppNum to int. */
	int i = (int) pNum;

	while (true) {
		/* Wait for those glorious bamboo sticks. */
		sem_wait(&chopstick[i]);
		sem_wait(&chopstick[(i+1)%5]);

		/* Eat for a while. */
		printf("Critical Section, P %d gets to eat.\n", i);

		/* Signal that another P can have your chopstick. */
		sem_post(&chopstick[i]);
		sem_post(&chopstick[(i+1)%5]);

		/* Think for a while. */
		printf("P %d is now thinking; are we human or are we dancer?\n", i);
	}
}

/* Main function. */

int main (void) {
	/* Variable Declaration. */
	int i = 0;

	/* Initalize the semaphore. */
	for (i = 0; i < 5; i++)
		sem_init(&chopstick[i], 0, 1);

	/* Create a thread. */
	pthread_t philosopherProcess[5];

	/* Start the threads. */
	printf("Creating the philosophers.\n");
	for (i = 1; i < 6; i++)
		pthread_create(&philosopherProcess[i], NULL, philosopher, (void *)i);

	/* Let the Ps live for eternity. */
	while (true);

	return 0;
}

/* Compile with the following to surpress unnecessary warnings. */
/* Might get warning for sem_init instead of sem_open (Programmed on Mac OS X). */
/* Warning for upcasting int i on line 50. */
/* gcc -o exec hungryPhilosophers.c -w -lpthread */