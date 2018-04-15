#include "util.h"

#include <stdlib.h>
#include <sys/time.h>

unsigned long current_time_millis() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000L + time.tv_usec / 1000L);
}
