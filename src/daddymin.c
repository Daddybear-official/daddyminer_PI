/*
    Minimum code required to solve a DUCO-S1 job, with no error handling or multithreading
    and intended for educational/diagnostic purposes only.
    For extended use, please run the full version!
*/
#include <stdio.h>
#include <string.h>
#include <time.h>

// Socket defines
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INIT_WINSOCK() // No-op
// Sleep defines
#define SLEEP(seconds) sleep(seconds)
// Timing defines
#define TIMESTAMP_T struct timespec
#define GET_TIME(t_ptr) clock_gettime(CLOCK_MONOTONIC, t_ptr)
#define DIFF_TIME_MS(t1_ptr, t0_ptr) \
        ((t1_ptr)->tv_sec-(t0_ptr)->tv_sec)*1000+((t1_ptr)->tv_nsec-(t0_ptr)->tv_nsec)/1000000

#include "mine_DUCO_S1.h"
#define USERNAME "daddybear"
#define identifier "RPI4_C"

int main(){
    INIT_WINSOCK();
    puts("Initializing DaddyMin v1.0.0...");
    int len, local_hashrate;
    char buf[128], job_request[256];
    int accepted = 0, rejected = 0;
    int job_request_len = sprintf(job_request, "JOB,%s,LOW", USERNAME);
    TIMESTAMP_T t1, t0;
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("51.15.127.80");
    server.sin_family = AF_INET;
    server.sin_port = htons(2811);
    unsigned int soc = socket(PF_INET, SOCK_STREAM, 0);

    len = connect(soc, (struct sockaddr *)&server, sizeof(server));

    len = recv(soc, buf, 3, 0);
    buf[len] = 0;

    GET_TIME(&t0);
	while(1){

		send(soc, job_request, job_request_len, 0);
        	len = recv(soc, buf, 128, 0);
	        buf[len] = 0;
		int diff = atoi((const char*) &buf[82]);
	        //long nonce = mine_DUCO_S1_extend_cache(
		long nonce = mine_DUCO_S1(
       			(const unsigned char*) &buf[0],
			(const unsigned char*) &buf[41],
			diff
        	);

		// Assignment to *local_hashrate is generally atomic, no mutex needed
		GET_TIME(&t1);

		int tdelta_ms = DIFF_TIME_MS(&t1, &t0);
		local_hashrate = nonce/tdelta_ms*1000;
		t0 = t1;

		len = sprintf(buf, "%ld,%d,DaddyBearMiner v1.0.0,%s\n", nonce, local_hashrate, identifier);
	        send(soc, buf, len, 0);

	        len = recv(soc, buf, 128, 0); // May take up to 10 seconds as of server v2.2!
        	buf[len] = 0;

		if(!strcmp(buf, "GOOD\n") || !strcmp(buf, "BLOCK\n")) accepted++;
		else rejected++;

		float megahash = (float)local_hashrate/1000000;
		time_t ts_epoch = time(NULL);
        	struct tm *ts_clock = localtime(&ts_epoch);
		printf("%02d:%02d:%02d", ts_clock->tm_hour, ts_clock->tm_min, ts_clock->tm_sec);
		printf("  Hashrate: %.2f MH/s,", megahash);
		printf(" Accepted %d/%d", accepted, rejected);
		printf(" Accepted share %ld Difficulty %d\n", nonce, diff);
    		}

    return 0;
}
