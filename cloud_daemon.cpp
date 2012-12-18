/* This is the cloud_daemon.cpp file which has the void main function for
 * cloud_daemon program, handling the major work flow of the program.
 * able to switch from daemon mode or normal program.
 *
 * The user can start cloud_daemon
 *
 */
#include"para_util.h"
#include"process_helper.h"
#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>
#include <boost/timer.hpp>
using namespace boost;
/*
 * we have several optional parameters for user to use
 */

//Parameter Option string: s == sleep time,  t == sampling time
//p == log output path, r == read configuration from configuration file
static const char *optString = "e:i:s:t:p:r:dh";


static void
usage(void)
{
	printf("usage: cloud_daemon [-d] [-h] [-r] [-d] [-s sleep_time] [-t sampling_time] [-p dump_file_path] [-i sleep_Time_In_Between_Sampling] [-e event1,event2,...]\n"
		"-h\t\tget help\n"
		"-d\t\trun as daemon\n"
		"-r\t\tread config file\n"
		"-e ev,ev\tgroup of events to measure (multiple -e switches are allowed)\n"
		);
}
/*
 * Name: excute funtion
 * Purpose: start excute sampling task with given parameters
 * Parameters: long num_procs: processor's number
 * 			   parameters *para: parameter class pointer
 * 			   process_helper *pHelper: process_helper class pointer
 * 			   syst_smpl *smsSmpl:  syst_smpl class pointer
 * Return: void
 */
void excute(long num_procs,parameters *para,process_helper *pHelper)
{
	int ret;
	cout<<"Current machine has "<<num_procs<<" cores"<<endl;
	cout<<"Sleep "<<para->sleep_time<<" before start sampling."<<endl;
    cout<<"Do "<<para->sampling_time<<" samples"<<endl;
	cout<<"The log file path is "<<para->dump_file_path<<endl;

	pHelper->options.sleepTimeBetweenSampling = para->timeBetweenSampling;
	pHelper->options.cpu = -1;
	pHelper->options.samplingTime = para->sampling_time;
	pHelper->options.events = para->pfm_events.c_str(); //we now just hard coded event here

	timer t1;
	sleep(para->sleep_time); //if run as a daemon we want program sleep given time
	ret  = pfm_initialize();
	if (ret != PFM_SUCCESS)
	errx(1, "libpfm initialization failed: %s\n", pfm_strerror(ret));
	pHelper->measure();
	cout << t1.elapsed() << endl;
	/* free libpfm resources cleanly */
	pfm_terminate();

}

int daemon_init(void)
{
    pid_t pid;
  if((pid = fork()) < 0)
    return(-1);
  else if(pid != 0)
    exit(0); /* parent exit */
/* child continues */
  setsid(); /* become session leader */
 // chdir("/"); /* change working directory */
  umask(0); /* clear file mode creation mask */
  close(0); /* close stdin */
  //close(1); /* close stdout */
  close(2); /* close stderr */
  return(0);
}

void sig_term(int signo)
{
    if(signo == SIGTERM)
/* catched signal sent by kill(1) command */
  {
    syslog(LOG_INFO, "program terminated.");
  closelog();
  exit(0);
  }
}

int main(int argc, char *argv[])
{
	parameters *para = new parameters();
	//sys_counter_per_cpu *sysCount = new sys_counter_per_cpu();
 	//syst_count *sysCount = new syst_count();
 	process_helper *pHelper = new process_helper();
 	//syst_smpl *sysSmpl = new syst_smpl();

	int opt = 0;

	
	while((opt = getopt(argc, argv, optString))!= -1)
	{
		switch(opt)
		{
			case 's' :
					  {													 
						para->sleep_time = atol(optarg);
				        break;
					  }
			case 't' :
					{
				     	para->sampling_time = atol(optarg);
				     break;
					}
			case 'p':
				     {
					para->dump_file_path = (string)optarg;
					break;
				     }
			case 'r' :
					{
				     para->read_from_config_file("config.cfg");
				     break;
					}
			case 'i' :
					{
					  para->timeBetweenSampling = atol(optarg);
					  break;
					}
			case 'd' :
					{
						para->daemonFlag = true;
						break;
					}
			case 'e':
					{
						para->pfm_events = (string)optarg;
						break;
					}
			case 'h':
					{
						usage();
						return(0);
						break;
					}
		}
	}
        long num_procs;
        num_procs = get_core_num();
        if(para->daemonFlag == true)
        {
        if(daemon_init() == -1)
          {
            printf("can't fork self/n");
            exit(0);
          }
          //openlog("daemontest", LOG_PID, LOG_USER);
          //syslog(LOG_INFO, "daemon started.");
			char name[30] = "Cloud Daemon Started";
			FILE* log = fopen(para->dump_file_path.c_str(), "w");
			/* daemon works...needs to write to log */
			fprintf(log, "%s\n", (char*)name);
			/* ...all done, close the file */
			fclose(log);

			signal(SIGTERM, sig_term); //arrange to catch the signal

			printf("You are running under daemon mode\n");
            excute(num_procs,para,pHelper);

        }
        else
        {
          printf("You are running under normal mode\n");
          excute(num_procs,para,pHelper);
        }

	return 0;
}
                                                                                                                                                                                 
