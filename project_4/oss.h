/*
 * oss.h
 *
 *  Created on: Oct 26, 2022
 *      Author: kihong.park
 */

#ifndef PROJECT_4_OSS_H_
#define PROJECT_4_OSS_H_

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

#define MAX_NUM_USER_PROC 18
#define ONE_SEC_IN_NANO 1000000000
#define PARENT_QUEUE_ADDRESS 999
#define BUFF_SZ	sizeof ( int )
#define TRUE  1
#define FALSE 0
#define maxTimeBetweenNewProcsSecs 1
#define maxTimeBetweenNewProcsNS  0
#define REAL_TIME_PROC_PERCENTAGE 5

#define TOTAL_DISPATCH_RANDOM 0
#define REAL_OR_USER_RANDOM 1
#define LOOP_ADVANCE_RANDOM 2

#define USER_PROCESS 0
#define REAL_TIME 1

typedef struct
{
	unsigned int sec;
	unsigned int nano_sec;
} system_clock;

typedef struct
{
	unsigned int total_used_cpu_time;
	unsigned int total_system_time;
	unsigned int total_last_burst_time;
	unsigned int local_sim_pid;
	unsigned int proc_priority;
} proc_ctrl_blck;

typedef struct
{
    long mesg_type;
    int time_slice;
} message;

struct Queue queue;

void help();
int  checkToCreateNewProcess(struct Queue* pid_queue, system_clock* sysclock, int last_proc_created_sec, int last_proc_created_ns);
int  generateRandomNumber(int flag);
void incrementSysClock(system_clock* sysclock, int seconds, int nano_seconds);
struct Queue* initPidQueue();
void timeout();
void interrupt();
void terminate();

#endif /* PROJECT_4_OSS_H_ */
