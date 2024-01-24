/* valgrind_test.c
 *
 * This is a test program for use with Valgrind. It demonstrates a few of the
 * error cases Valgrind catches.
 *
 *   Author: Jamie Robinson
 *
 *   Compile with:  make valgrind-test-build
 *       Run with:  valgrind ./valgrind-test
 */

#include <stdlib.h>

void* still_reachable;
void* possibly_lost;

int main() {
  int uninitialized_variable; // This variable is never given a value.

  for (; uninitialized_variable < 100; uninitialized_variable++) {
    void** definitely_lost = (void**) malloc(sizeof(void*)); // allocate a
                                                             // pointer on the
                                                             // heap.

    *definitely_lost = (void*) malloc(7); // Give the pointer something else to
                                          // point to on the heap. This will be
                                          // indirectly lost.
  }

  // At this point, definitely_lost is out of scope and we can no longer free
  // it. The pointer pointed to by definitely_lost is indirectly lost since we
  // were only able to reach the pointer through definitely_lost.

  still_reachable = malloc(42); // This value is never freed but is pointed to
                                // in the global scope at program completion.

  possibly_lost = malloc(10);
  possibly_lost += 4; // This is similar to still reachable except there is a
                      // pointer pointing to the middle of the allocated block
                      // but nothing points to the front of the block. This is
                      // very odd behavior and usually is a memory leak (but not
                      // always).

  return 0;
}
