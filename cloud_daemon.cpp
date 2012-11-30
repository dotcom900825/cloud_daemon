/* This is the cloud_daemon.cpp file which has the void main function for
 * cloud_daemon program, handling the major work flow of the program.
 * able to switch from daemon mode or normal program.
 *
 * The user can start cloud_daemon
 *
 */
#include"para_util.h"
#include"syst_count.h"
#include"process_helper.h"
#include"syst_smpl.h"
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
static const char *optString = "e:i:s:t:p:r:d";

/*
 * Name: excute funtion
 * Purpose: start excute sampling task with given parameters
 * Parameters: long num_procs: processor's number
 * 			   parameters *para: parameter class pointer
 * 			   process_helper *pHelper: process_helper class pointer
 * 			   syst_smpl *smsSmpl:  syst_smpl class pointer
 * Return: void
 */
void excute(long num_procs,parameters *para,process_helper *pHelper,syst_smpl *sysSmpl)
{
	int ret;
	cout<<"Current machine has "<<num_procs<<" cores"<<endl;
	cout<< para->sleep_time <<endl;
    cout<< para->sampling_time <<endl;
	cout<< para->dump_file_path <<endl;

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
 	syst_smpl *sysSmpl = new syst_smpl();

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

			printf("daemon mode");
            excute(num_procs,para,pHelper,sysSmpl);

        }
        else
        {
          printf("normal mode");
          excute(num_procs,para,pHelper,sysSmpl);
        }

	return 0;
}
                                                                                                                                                                                 
