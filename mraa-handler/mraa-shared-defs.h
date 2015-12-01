
#ifndef MRAA_SHARED_DEFS_H__

#define MRAA_SHARED_DEFS_H__

#define BUFFER_SIZE 128
#define MRAA_SERVER "/tmp/mraa-server"

struct  mraa_cmd {
	int  addr;
	int  retval;
	char buffer[BUFFER_SIZE];
	char reply[BUFFER_SIZE];
};

#endif
