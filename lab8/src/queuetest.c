/** @file queuetest.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "libpriqueue/libpriqueue.h"

int compare1(const void * a, const void * b)
{
 	return ( *(int*)a - *(int*)b );
}

int compare2(const void * a, const void * b)
{
	return ( *(int*)b - *(int*)a );
}

int main()
{
	priqueue_t q, q2;

	priqueue_init(&q, compare1);
	priqueue_init(&q2, compare2);

	/* Pupulate some data... */
	int *values = malloc(100 * sizeof(int));

	int i;
	for (i = 0; i < 100; i++)
		values[i] = i;

	/* Add 5 values, 3 unique. */
	priqueue_offer(&q, &values[12]);
	priqueue_offer(&q, &values[13]);
	priqueue_offer(&q, &values[14]);
	priqueue_offer(&q, &values[12]);
	priqueue_offer(&q, &values[12]);
	printf("Total elements: %d (expected 5).\n", priqueue_size(&q));

	int val = *((int *)priqueue_poll(&q));
	printf("Top element: %d (expected 12).\n", val);
	printf("Total elements: %d (expected 4).\n", priqueue_size(&q));

	int vals_removed = priqueue_remove(&q, &values[12]);
	printf("Elements removed: %d (expected 2).\n", vals_removed);
	printf("Total elements: %d (expected 2).\n", priqueue_size(&q));

	priqueue_offer(&q, &values[10]);
	priqueue_offer(&q, &values[30]);
	priqueue_offer(&q, &values[20]);

	priqueue_offer(&q2, &values[10]);
	priqueue_offer(&q2, &values[30]);
	priqueue_offer(&q2, &values[20]);


	printf("Elements in order queue (expected 10 13 14 20 30): ");
	for (i = 0; i < priqueue_size(&q); i++)
		printf("%d ", *((int *)priqueue_at(&q, i)) );
	printf("\n");

	printf("Elements in reverse order queue (expected 30 20 10): ");
	for (i = 0; i < priqueue_size(&q2); i++)
		printf("%d ", *((int *)priqueue_at(&q2, i)) );
	printf("\n");

	priqueue_destroy(&q2);
	priqueue_destroy(&q);

	free(values);

	return 0;
}
