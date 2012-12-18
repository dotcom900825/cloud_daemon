#include"para_util.h"

  long get_core_num()
{
  long num_procs;
  num_procs = sysconf(_SC_NPROCESSORS_CONF);
  return num_procs;

}
 void parameters::setDumpFilePath(string path)
 {
	 if(path != "")
	 {
		 this->dump_file_path = path;
	 }
	 else
	 {
		 this->dump_file_path = "./";
	 }
 }

 void parameters::setTimeBetweenSampling(long time)
 {
	 this->timeBetweenSampling = time;
 }
 void parameters::setSamplingTime(long time)
 {
	 this->sampling_time = time;
 }


 void parameters::setEvents(string events)
  {
 	 this->pfm_events = events;
  }

 void parameters::setSleepTime(long time)
 {
	 this->sleep_time = time;
 }

 string parameters::getEvents()
 {
	 return this->pfm_events;
 }
 long parameters::getSamplingTime()
 {
	 return this->sampling_time;
 }

 long parameters::getSleepTime()
 {
	 return this->sleep_time;
 }

 string parameters::getDumpFilePath()
 {
	 return this->dump_file_path;
 }

 long parameters::getTimeBetweenSampling()
 {
	 return this->timeBetweenSampling;
 }

  int parameters::read_from_config_file(const char *config_file_name)
{
	    Config cfg;
	                try
	                {
	                    cfg.readFile(config_file_name);
	                }
	                catch(const FileIOException &fioex)
	                {
	                    std::cerr << "I/O error while reading file." << std::endl;
	                    return(EXIT_FAILURE);
	                }
	                catch(const ParseException &pex)
	                {
	                    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
	                    << " - " << pex.getError() << std::endl;
	                    return(EXIT_FAILURE);
	                }

	                // Get the store name.

	                    string sleep_time = cfg.lookup("sleep_time");

	                        this->setSleepTime(atol(sleep_time.c_str()));


	                    string sampling_time = cfg.lookup("sampling_time");

	                    	this->setSamplingTime(atol(sampling_time.c_str()));

	                    string dump_file_path = cfg.lookup("dump_file_path");
	                    	this->setDumpFilePath(dump_file_path);
				return 0;
}
