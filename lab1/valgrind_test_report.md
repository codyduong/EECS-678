# `valgrind_test.c`

In order to fix `valgrind_test.c`

1. Including examples from the video:
  a. initialize `unititialized_variable` to `0` inside the `for` initializer (or elsewhere).
  b. free `still_reachable`

2. `possibly_lost` has a memory leak solved by freeing `possibly_lost` front block (ie -= 4).

3. `indirectly_lost` has a memory leak solved by freeing the `*definitely_lost` (pointer).

4. `definitely_lost` has a memory leak solved by freeing `definitely_lost`.
