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
#define BUFF_SZ	sizeof ( int )
#define TRUE  1
#define FALSE 0

void help();
struct Queue* priority_queue;
int  checkToCreateNewProcess(struct Queue* priority_queue);
void incrementSysClock();
void timeout();
void interrupt();
void terminate();

typedef union
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

#endif /* PROJECT_4_OSS_H_ */
