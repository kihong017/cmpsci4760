/*
 * oss.h
 *
 *  Created on: Nov 09, 2022
 *      Author: kihong.park
 */

#ifndef PROJECT_5_OSS_H_
#define PROJECT_5_OSS_H_

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <time.h>

#define NUM_OF_RESOURCES 20
#define MAX_NUM_USER_PROC 18
#define ONE_SEC_IN_NANO 1000000000

#define PARENT_QUEUE_ADDRESS 999
#define BUFF_SZ	sizeof ( int )
#define TRUE  1
#define FALSE 0
#define PID_TAKEN 1
#define PID_AVAILBLE 0
#define REAL_TIME_PROC_PERCENTAGE 5

#define TOTAL_DISPATCH_RANDOM 0
#define REAL_OR_USER_RANDOM 1
#define LOOP_ADVANCE_RANDOM 2
#define PROC_CREATE_SEC_RANDOM 3
#define PROC_CREATE_NS_RANDOM 4
#define EVENT_HAPPEN_SEC_RANDOM 5
#define EVENT_HAPPEN_NS_RANDOM 6

#define REAL_TIME 0
#define USER_PROCESS 1

typedef struct
{
	unsigned int sec;
	unsigned int nano_sec;
} system_clock;

typedef struct
{
	unsigned int request[NUM_OF_RESOURCES];
	unsigned int allocation[NUM_OF_RESOURCES];
	unsigned int release[NUM_OF_RESOURCES];
} resource_desc;

typedef struct
{
    long mesg_type;
    int time_slice;
    int is_interrupted;
} message;

struct Queue queue;

void help();
int  checkToCreateNewProcess(int* available_pids, system_clock* sysclock, int last_proc_created_sec, int last_proc_created_ns);
int  isAfter(int current_sec, int current_ns, int compare_sec, int compare_ns);
int  generateRandomNumber(int flag);
void incrementSysClock(system_clock* sysclock, int seconds, int nano_seconds);
int* initPidVector();
int  req_lt_avail ( const int* req, const int* avail, const int pnum, const int num_res );
int  deadlock ( const int* available, const int m, const int n, const int* request, const int* allocated );
void timeout();
void interrupt();
void terminate();

#endif /* PROJECT_5_OSS_H_ */
