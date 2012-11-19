#include"sys_counter_per_cpu.h"
#include <inttypes.h>
#define __STDC_FORMAT_MACROS
void sys_counter_per_cpu::measure()
{
	perf_event_desc_t *fds;
		long lret;
		int c, cmin, cmax, ncpus;
		int i, ret, l;

		printf("<press CTRL-C to quit before %ds time limit>\n", this->options.delay);

		cmin = 0;

		lret = sysconf(_SC_NPROCESSORS_ONLN);
		if (lret < 0)
			err(1, "cannot get number of online processors");

		cmax = (int)lret;

		ncpus = cmax;
		if (options.cpu != -1) {
			cmin = options.cpu;
			cmax = cmin + 1;
		}
		all_fds = (perf_event_desc_t**)calloc(ncpus, sizeof(perf_event_desc_t));
		num_fds = (int *)calloc(ncpus, sizeof(int));

		if (!all_fds || !num_fds)
			err(1, "cannot allocate memory for internal structures");
		for(c=cmin ; c < cmax; c++)
			setup_cpu(c);

		/*
		 * FIX this for hotplug CPU
		 */
		for(c=cmin ; c < cmax; c++) {
			fds = all_fds[c];
			//if (options.group)  //we disable group selection now
			//	ret = ioctl(fds[0].fd, PERF_EVENT_IOC_ENABLE, 0);
			//else
				for(i=0; i < num_fds[c]; i++) {
				ret = ioctl(fds[i].fd, PERF_EVENT_IOC_ENABLE, 0);
				if (ret)
					err(1, "cannot enable event %s\n", fds[i].name);
			}
		}

		for(l=0; l < options.delay; l++) {

			sleep(1); //need to change to msleep or usleep

			puts("------------------------");
			for(c = cmin; c < cmax; c++) {
				fds = all_fds[c];
				for(i=0; i < num_fds[c]; i++) {
					uint64_t val, delta;
					double ratio;

					ret = read(fds[i].fd, fds[i].values, sizeof(fds[i].values));
					if (ret != sizeof(fds[i].values)) {
						if (ret == -1)
							err(1, "cannot read event %d:%d", i, ret);
						else
							warnx("could not read event%d", i);
					}

					/*
					 * scaling because we may be sharing the PMU and
					 * thus may be multiplexed
					 */
					val = perf_scale(fds[i].values);
					ratio = perf_scale_ratio(fds[i].values);
					delta = perf_scale_delta(fds[i].values, fds[i].prev_values);

					printf("CPU%d val=%-20" "llu" " %-20" "llu" " raw=%" "llu" " ena=%" "llu" " run=%" "llu" " ratio=%.2f %s\n",
						c,
						val,
						delta,
						fds[i].values[0],
						fds[i].values[1], fds[i].values[2], ratio,
						fds[i].name);
					fds[i].prev_values[0] = fds[i].values[0];
					fds[i].prev_values[1] = fds[i].values[1];
					fds[i].prev_values[2] = fds[i].values[2];
				}
			}
		}
		for(c = cmin; c < cmax; c++) {
			fds = all_fds[c];
			for(i=0; i < num_fds[c]; i++)
				close(fds[i].fd);
			perf_free_fds(fds, num_fds[c]);
		}
}

void sys_counter_per_cpu::setup_cpu(int cpu)
{
	perf_event_desc_t *fds;
		int i, ret;

		ret = perf_setup_list_events(options.events, &all_fds[cpu], &num_fds[cpu]);
		if (ret || (num_fds == 0))
			errx(1, "cannot setup events\n");
		fds = all_fds[cpu]; /* temp */

		fds[0].fd = -1;
		for(i=0; i < num_fds[cpu]; i++) {
			fds[i].hw.disabled = options.group ? !i : 1;

			if (options.excl && ((options.group && !i) || (!options.group)))
				fds[i].hw.exclusive = 1;

			fds[i].hw.disabled = options.group ? !i : 1;

			/* request timing information necessary for scaling counts */
			fds[i].hw.read_format = PERF_FORMAT_SCALE;
			fds[i].fd = perf_event_open(&fds[i].hw, -1, cpu, (options.group ? fds[0].fd : -1), 0);
			if (fds[i].fd == -1)
				err(1, "cannot attach event to CPU%d %s", cpu, fds[i].name);
		}
}
