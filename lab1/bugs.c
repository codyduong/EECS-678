#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// In c, types are really just an indication of how large a data type is. For
// example, c will not complain if you cast an intptr_t (usually 32-bits or
// greater) type into a character array of length 4 (32-bits). For example the
// following is valid c code and may produce warnings but not actual errors:
//
// intptr_t cat = 0x00544143;
// char* cat_ptr = &cat;
// printf("%s\n", cat_ptr);
//
// On a little endian processor, this will print "CAT".
//
// In fact the "union" structure will allow two unrelated types to be read as
// each other without producing so much as a warning. There are valid use cases
// for this design pattern, however it is best to avoid them if at all possible.
//
// In c there is no implicit protection against accessing beyond an array
// boundary or structure boundary. These types of errors are known as buffer
// overflow errors.
//
// In the Storage structure, the type intptr_t ensures that the integers in this
// structure take up the same amount of space as the pointers by the definition
// of intptr_t. This means that the following structure could be read as an
// array of length 12 containing strings. In reality the only thing to stop the
// program from reading before or beyond the 12 pointers is by encountering a
// segmentation fault, but segmentation faults may not always occur on invalid
// memory accesses.
//
// TLDR: Pay very careful attention to how this structure is laid out in memory
// and what is stored in it when looking at gdb and valgrind.
struct Storage {
  intptr_t num_bugs_on_mars;
  const char* scary_bug;
  char* sentence[6]; // It might be necessary to grow this array.
  const char* colorful_bug;
  intptr_t num_bugs_on_earth;
  intptr_t num_bugs_on_venus;
  char* useless_bug;
};

// Print a **NULL terminated** array of strings to standard out (This is not
// broken but malformed arrays of non-null terminated arrays will break inside
// this)
//
// An example of a NULL terminated array of strings is:
// char** strs = { "Hello", "World!", NULL };
void echo(char** strs) {
  if (*strs != NULL) {
    printf("%s ", *strs);
    fflush(stdout);

    echo(strs + 1); // Recurse on the next element in the string array
  }
}

// Print a **NULL terminated** array of strings to standard out and then print the
// array backwards (This is broken).
void echoohce(char** strs) {
  char** iter;
  char** stop_beginning = strs - 1; // HINT: What could this possibly used for?

  // FIXME: Something is wrong in this for loop. It is printing garbage characters.
  for (iter = strs; *iter != NULL; ++iter)
    printf("%s ", iter);

  fflush(stdout);

  // The iterator is currently at the NULL pointer, so go back one character.
  --iter;

  // FIXME: Do the same thing only backwards. The array has a NULL pointer at
  // the front, right? Nope. Both sides of the condition in the for loop are
  // wrong.
  for (; *iter != NULL; --iter)
    printf("%s ", *iter);

  printf("\n");
  fflush(stdout);
}

int main(int argc, char** argv) {
  struct Storage bug_info;

  // Initialize structure
  bug_info.num_bugs_on_earth = 7400000000 * 200000000;
  bug_info.num_bugs_on_venus = 0;
  bug_info.colorful_bug = "butterfly";
  bug_info.useless_bug = "mosquito";
  bug_info.scary_bug = "~~~~~~~~ SPIDER!!! ~~~~~~~~";

  // Setup the sentence structure. Strdup uses malloc to allocate space for the
  // new string, therefore all of the calls to strdup must be freed before the
  // process exits or the pointers change.
  bug_info.sentence[0] = strdup("The");
  bug_info.sentence[1] = strdup("most");
  bug_info.sentence[2] = strdup("useless");
  bug_info.sentence[3] = strdup("bug");
  bug_info.sentence[4] = strdup("is");
  bug_info.sentence[5] = strdup("a");

  // Print the current bug population on various planets
  printf("The current bug population of Earth is about: %zu\n",
         bug_info.num_bugs_on_earth); // 1480000000000000000
  printf("The current bug population of Mars is about: %zu\n",
         bug_info.num_bugs_on_mars);  // 0
  printf("The current bug population of Venus is about: %zu\n",
         bug_info.num_bugs_on_venus); // 0

  // Initialize num_bugs_on_mars
  bug_info.num_bugs_on_mars = 0;

  // Print the following line:
  // "The total bug population of the solar system is: 1480000000000000000"
  printf("The total bug population of the solar system is: %zu\n",
         bug_info.num_bugs_on_mars +
         bug_info.num_bugs_on_venus +
         bug_info.num_bugs_on_earth);

  // Print "The most useless bug is a mosquito"
  echo(bug_info.sentence);
  printf("%s\n", bug_info.useless_bug);

  // Change the adjective to something appropriate for butterflies
  bug_info.sentence[2] = strdup("colorful");

  // Print "The most colorful bug is a butterfly"
  echo(bug_info.sentence);
  printf("%s\n", bug_info.colorful_bug);

  // Print "The most colorful bug is a a is bug colorful most The"
  echoohce(bug_info.sentence);

  // Free all duplicated strings
  free(bug_info.sentence[0]);
  free(bug_info.sentence[1]);
  free(bug_info.sentence[2]);
  free(bug_info.sentence[3]);
  free(bug_info.sentence[4]);
  free(bug_info.sentence[5]);

  // Prints "The current bug adjective is: (null)"
  printf("The current bug adjective is: %s\n", bug_info.sentence[2]);

  // Can we please forget about mosquitoes?
  // HINT: Where is the string that useless_bug is pointing to located in
  // memory and what area of memory does free() work with?
  free(bug_info.useless_bug); // Remove this line if it is problematic

  printf("Bugs didn't cause me to crash!\n\n");

  return EXIT_SUCCESS;
}
