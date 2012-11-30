#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <time.h>
#include <linux/limits.h>
#include <sys/times.h>
#if (__GNU_LIBRARY__ >= 6)
# include <locale.h>
#endif

/* username lookups */
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

/* major/minor number */
#include <sys/sysmacros.h>
#include <vector>
#include <map>
#include <algorithm>
#include <signal.h>   /* catch signals */

#include <string.h>
#include <inttypes.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <err.h>

#include "perf_util.h"



#include "/home/thomas/Downloads/cloud_daemon/procps-3.2.8/proc/wchan.h"
#include "/home/thomas/Downloads/cloud_daemon/procps-3.2.8/proc/version.h"
#include "/home/thomas/Downloads/cloud_daemon/procps-3.2.8/proc/readproc.h"
#include "/home/thomas/Downloads/cloud_daemon/procps-3.2.8/proc/sysinfo.h"
#include "/home/thomas/Downloads/cloud_daemon/procps-3.2.8/proc/sig.h"
using namespace std;

static unsigned needs_for_threads;
static unsigned needs_for_sort;
static unsigned proc_format_needs;
static unsigned task_format_needs;
#define PROC_FILLSTAT        0x0040 
#define PROC_FILLSTATUS      0x0020
#define needs_for_format (proc_format_needs|task_format_needs)
#define needs_for_select (PROC_FILLSTAT | PROC_FILLSTATUS)
#define msleep(n) usleep(n*1000)

typedef long long int num;

typedef struct {
	const char *events;
	int samplingTime;
	int excl;
	int cpu;
	int group;
	int sleepTimeBetweenSampling;
} options_counter_per_cpu;


class process_helper
{
public:

		 int getProcess();
		 int getProcessAfterSampling();
		 int getCpuInfo();
		 void readone(num *x) { fscanf(input, "%lld ", x); }
		 void readunsigned(unsigned long long *x) { fscanf(input, "%llu ", x); }
		 void readstr(char *x) {  fscanf(input, "%s ", x);}
		 void readchar(char *x) {  fscanf(input, "%c ", x);}
		 void printone(char *name, num x) {  printf("%20s: %lld\n", name, x);}

		void setup_cpu(int cpu);
		void measure();
		options_counter_per_cpu options;
private:
	num pid;
	char tcomm[PATH_MAX];
	char state;

	num ppid;
	num pgid;
	num sid;
	num tty_nr;
	num tty_pgrp;

	num flags;
	num min_flt;
	num cmin_flt;
	num maj_flt;
	num cmaj_flt;
	num utime;
	num stimev;

	num cutime;
	num cstime;
	num priority;
	num nicev;
	num num_threads;
	num it_real_value;

	unsigned long long start_time;

	num vsize;
	num rss;
	num rsslim;
	num start_code;
	num end_code;
	num start_stack;
	num esp;
	num eip;

	num pending;
	num blocked;
	num sigign;
	num sigcatch;
	num wchan;
	num zero1;
	num zero2;
	num exit_signal;
	num cpu;
	num rt_priority;
	num policy;

		vector<int> procVectorBeforeSample;
		vector<int> procVectorAfterSample;
		vector<int> surviveVector;
		map<long long int,vector<string> > cpu_map;
		proc_t *task;
		proc_t *taskAfterSampling;
	    PROCTAB *ptp;
	    pid_t mypid[2];
	    long tickspersec;
	    FILE *input;

	    perf_event_desc_t **all_fds;
	   	int *num_fds;

	    };
