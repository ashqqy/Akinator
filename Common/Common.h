#ifndef COMMON
#define COMMON

#include <stdio.h>

//----------------------------------------------------------------

const int POISON = 0;

void* MyCalloc (size_t n_elems, size_t size_elems, const void* poison);
void* MyRecalloc (void* memory, size_t n_elements, size_t size_elements, size_t previous_n_elements, const void* poison);
size_t FileSizeFinder (FILE* file_input);
void CleanBufer ();
char* ScanfAndAllocateBuffer ();

//----------------------------------------------------------------

#endif // COMMON