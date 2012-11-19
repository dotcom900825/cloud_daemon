#include"process_helper.h"
#include<iostream>

void process_helper::measure()
{
				//getProcess();
				//sleep(1);
				//getProcessAfterSampling();
				//getCpuInfo();
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

			 //need to change to msleep or usleep

			getProcess();
			sleep(1);
			getProcessAfterSampling();
			getCpuInfo();
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

void process_helper::setup_cpu(int cpu)
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


int process_helper::getProcess()
	    {

			ptp = openproc(needs_for_format | needs_for_sort | needs_for_select | needs_for_threads);
		    	if(!ptp)
		    {
		    	fprintf(stderr,"Error: can not access /proc. \n");
		    }
		    	while((task = readproc(ptp,NULL)))
		    {
		    		procVectorBeforeSample.push_back(task->XXXID);
		    }
		    free(task);
		    closeproc(ptp);
		    return 0;
	    }
int process_helper::getProcessAfterSampling()
{

    ptp = openproc(needs_for_format | needs_for_sort | needs_for_select | needs_for_threads);
    	if(!ptp)
    {
    	fprintf(stderr,"Error: can not access /proc. \n");
    }
    	while((taskAfterSampling = readproc(ptp,NULL)))
    		    {
    		    		procVectorAfterSample.push_back(taskAfterSampling->XXXID);
    		    }

    	free(taskAfterSampling);
    	closeproc(ptp);

    	std::sort(procVectorBeforeSample.begin(),procVectorBeforeSample.end());
    	std::sort(procVectorAfterSample.begin(),procVectorAfterSample.end());
    	std::set_intersection(procVectorBeforeSample.begin(),procVectorBeforeSample.end(),
    						  procVectorAfterSample.begin(),procVectorAfterSample.end(),
    						  std::back_inserter(surviveVector));

    	printf("Done\n");
    	return 0;
}
int process_helper::getCpuInfo()
{
	char pid_num[10] ;
	tickspersec = sysconf(_SC_CLK_TCK);
	for(std::vector<int>::iterator iter = surviveVector.begin(); iter!=surviveVector.end(); iter++)
	{
		sprintf(pid_num,"%d",*iter);
		string url = "/proc/";
		url.append(pid_num);
		url.append("/stat");
		const char *path = url.c_str();
		//printf("%d\n",*iter);
	    input = fopen(path, "r");
	    if(!input) {
	      perror("open");
	      return 0;
	    }
	 readone(&pid);
	  readstr(tcomm);
	  readchar(&state);
	  readone(&ppid);
	  readone(&pgid);
	  readone(&sid);
	  readone(&tty_nr);
	  readone(&tty_pgrp);
	  readone(&flags);
	  readone(&min_flt);
	  readone(&cmin_flt);
	  readone(&maj_flt);
	  readone(&cmaj_flt);
	  readone(&utime);
	  readone(&stimev);
	  readone(&cutime);
	  readone(&cstime);
	  readone(&priority);
	  readone(&nicev);
	  readone(&num_threads);
	  readone(&it_real_value);
	  readunsigned(&start_time);
	  readone(&vsize);
	  readone(&rss);
	  readone(&rsslim);
	  readone(&start_code);
	  readone(&end_code);
	  readone(&start_stack);
	  readone(&esp);
	  readone(&eip);
	  readone(&pending);
	  readone(&blocked);
	  readone(&sigign);
	  readone(&sigcatch);
	  readone(&wchan);
	  readone(&zero1);
	  readone(&zero2);
	  readone(&exit_signal);
	  readone(&cpu);
	  readone(&rt_priority);
	  readone(&policy);
	  //printone("cpu", cpu);
	  map<long long int,vector<string> >::iterator it = cpu_map.find(cpu);
	  if(it == cpu_map.end())
	  {
		  char pid[20];
		  sprintf(pid, "%d", *iter);
		  vector<string> process_list;
		  process_list.push_back(pid);
		  cpu_map.insert(map<long long int, vector<string> >::value_type(cpu, process_list));

	  }
	  else
	  {
		  char pid[20];
		  sprintf(pid, ",%d", *iter);
		  it->second.at(0) += pid;
	  }
	  fclose(input);
	}
	cout<< cpu_map.find(1)->second.at(0)<<endl;
	procVectorBeforeSample.clear();
	procVectorAfterSample.clear();
	surviveVector.clear();
	map<long long int,vector<string> >::iterator it;
	for(it = cpu_map.begin(); it != cpu_map.end(); it++)
	{
		it->second.at(0).clear();
	}
	//cout<<it->second.at(0).size()<<endl;
	return 0;
}

