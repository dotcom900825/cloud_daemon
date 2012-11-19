cloud_daemon: cloud_daemon.o para_util.o process_helper.o sys_counter_per_cpu.o perf_util.o
	   
	cc -o cloud_daemon -g -Wall -Werror -Wextra -Wno-unused-parameter -l boost_thread -lconfig++ -DCONFIG_PFMLIB_DEBUG -DCONFIG_PFMLIB_OS_LINUX -I. -D_GNU_SOURCE -pthread cloud_daemon.o   para_util.o process_helper.o sys_counter_per_cpu.o perf_util.o  /home/thomas/Downloads/cloud_daemon/libpfm-4.3.0/perf_examples/../lib/libpfm.a  /usr/local/lib/libconfig++.so /lib/libproc-3.2.8.so 

cloud_daemon.o: cloud_daemon.cpp para_util.h syst_count.h process_helper.h	
	cc  -g -Wall -Wextra -Wno-unused-parameter -I.   -l boost_thread -lconfig++ -DCONFIG_PFMLIB_DEBUG -DCONFIG_PFMLIB_OS_LINUX  -D_GNU_SOURCE -pthread -c cloud_daemon.cpp
 
para_util.o: para_util.cpp para_util.h
	cc  -g -Wall -Wextra -Wno-unused-parameter -I.   -l boost_thread -lconfig++ -DCONFIG_PFMLIB_DEBUG -DCONFIG_PFMLIB_OS_LINUX  -D_GNU_SOURCE -pthread -c para_util.cpp

process_helper.o: process_helper.cpp process_helper.h
	cc  -g -Wall -Wextra -Wno-unused-parameter -I. -l boost_thread -lconfig++ -DCONFIG_PFMLIB_DEBUG -DCONFIG_PFMLIB_OS_LINUX  -D_GNU_SOURCE -pthread -c process_helper.cpp

sys_counter_per_cpu.o: sys_counter_per_cpu.cpp sys_counter_per_cpu.h
	cc  -g -Wall -Wextra -Wno-unused-parameter -I. -l boost_thread -lconfig++ -DCONFIG_PFMLIB_DEBUG -DCONFIG_PFMLIB_OS_LINUX  -D_GNU_SOURCE -pthread -c sys_counter_per_cpu.cpp 

perf_util.o: perf_util.c perf_util.h
	cc  -g -Wall -Wextra -Wno-unused-parameter -I.   -l boost_thread -lconfig++ -DCONFIG_PFMLIB_DEBUG -DCONFIG_PFMLIB_OS_LINUX  -D_GNU_SOURCE -pthread -c  perf_util.c

clean :
	rm cloud_daemon cloud_daemon.o para_util.o process_helper.o sys_counter_per_cpu.o perf_util.o

