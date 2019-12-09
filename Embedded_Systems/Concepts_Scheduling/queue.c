/**
	Zachariah Abuelhaj - ECPE 293B - Embedded Systems
	Reference material: tutorial3.pdf
*/

#include <stdio.h>
#include <stdint.h>

/* Set the size of the queue. */
#define MAXSIZE 10

/* Declare the queue object. */
typedef struct {
	int* dataPtr;
	int head;
	int tail;
	int size;
} Queue;

/* Declare procedure prototypes here. */
Queue createQueue (int dataStructure[]);
int insertItem (Queue* queuePtr, int value);
int getItem (Queue* queuePtr);
void testQueue (Queue* queuePtr);

/* Main procedure. */
int main (void) {
	/* Declare the underlying data structure. */
	int myData[MAXSIZE];

	/* Call the create queue procedure. */
	Queue queue = createQueue (myData);

	/* Test the queue. */
	testQueue (&queue);

	printf ("Exiting program! \n");
	return 0;
}

/* Without dynamic memory allocation, create array data structure. */
Queue createQueue (int dataStructure[]) {

	/* Declare the queue instance. */
	Queue myQueue = {dataStructure, 0, 0, 0};

	/* Inform user of configuration updates. */
	printf ("Queue created!\n");

	return myQueue;
}

/* Procedure to check queue and insert item. */
int insertItem (Queue* queuePtr, int value) {
	/* Local variable declarations. */
	int error = -1;

	/* If queue isn't full, else return error. */
	if (queuePtr->size < MAXSIZE) {
		/* Test size before incrementing. */
		queuePtr->size++;

		/* Put the new items at the tail and wrap if necessary. */
		queuePtr->tail = ((queuePtr->tail)+1) % MAXSIZE;
		*((queuePtr->dataPtr) + queuePtr->tail) = value;
		error = 0;
	}

	/* Print for test. */
	if (error != -1)
		printf ("Inserted: %d \n", *((queuePtr->dataPtr) + queuePtr->tail));

	return error;
}

/* Procedure to get an item from queue. */
int getItem (Queue* queuePtr) {
	/* Local variable declareations. */
	int retVal = -1;

	/* If the queue isn't empty. */
	if (queuePtr->size > 0) {
		queuePtr->size--;
		queuePtr->head = ((queuePtr->head)+1) % MAXSIZE;
		retVal = *((queuePtr->dataPtr) + queuePtr->head);
	}

	return retVal;
}

/* Test queue is going to perform corner cases. */
/* This is to prove queue functionaluty. */
void testQueue (Queue* queuePtr) {
	/* Inform user that the test procedure has been reached. */
	printf ("Test Queue Reached!\n");

	/* Local variable declarations. */
	int retValue = 0;

	/* Populate the queue. */
	insertItem (queuePtr, 1);
	insertItem (queuePtr, 2);
	insertItem (queuePtr, 3);

	/* Test the get item function. */
	retValue = getItem (queuePtr);
	printf ("Removed: %d \n", retValue);

	/* Empty the queue. */
	retValue = getItem (queuePtr);
	printf ("Removed: %d \n", retValue);
	retValue = getItem (queuePtr);
	printf ("Removed: %d \n", retValue);

	/* Test wrap-around. */
	insertItem (queuePtr, 3);
	insertItem (queuePtr, 2);

	/* Get wrapped-around value. */
	retValue = getItem (queuePtr);
	printf ("Removed: %d \n", retValue);
	retValue = getItem (queuePtr);
	printf ("Removed: %d \n", retValue);

	/* Try to get an element from an empty queue. */
	retValue = getItem (queuePtr);
	if (retValue == -1)
		printf ("Queue Empty! \n");

	/* Fill the empty queue and add another element. */
	for (int i = 1; i <= 10; i++) {
		insertItem (queuePtr, i);
	}
	if (insertItem (queuePtr, 1) == -1)
		printf("Queue Full! \n");

	return;
}








