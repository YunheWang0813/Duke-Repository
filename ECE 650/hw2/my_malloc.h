#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);
