#include <sys/types.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <err.h>


#include "perf_util.h"
typedef struct {
	const char *events;
	int delay;
	int excl;
	int cpu;
	int group;
} options_counter_per_cpu;

class sys_counter_per_cpu
{
public:


	void setup_cpu(int cpu);
	void measure();
    options_counter_per_cpu options;
private:
	 perf_event_desc_t **all_fds;
	 int *num_fds;


};
