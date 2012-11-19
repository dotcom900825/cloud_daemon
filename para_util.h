#include<iostream>
#include<unistd.h>
#include<iomanip>
#include<cstdlib>
#include<libconfig.h++>
using namespace std;
using namespace libconfig;

long get_core_num();

class parameters
{
	public:
		parameters(long sleep_time, long sampling_time, string events,string dump_file_path)
			{
				this->setSamplingTime(sleep_time);
				this->setSamplingTime(sampling_time);
				this->setDumpFilePath(dump_file_path);
				this->setEvents(events);
			}
		parameters()
			{
				sleep_time = 5;
				sampling_time = 5;
				dump_file_path = "./result.txt";
				gen_events = "cycles,instructions";
			}
		
	int read_from_config_file(const char *config_file_name);

	long getSleepTime();
	long getSamplingTime();
	string getDumpFilePath();
	string getEvents();

	void setSleepTime(long time);
	void setSamplingTime(long time);
	void setDumpFilePath(string path);
	void setEvents(string gen_events);

		long sleep_time;
		long sampling_time;
		string dump_file_path;
	    string gen_events;
};



