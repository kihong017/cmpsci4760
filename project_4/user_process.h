/*
 * user_process.h
 *
 *  Created on: Oct 27, 2022
 *      Author: kihong.park
 */

#ifndef PROJECT_4_USER_PROCESS_H_
#define PROJECT_4_USER_PROCESS_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>

#define BUFF_SZ	sizeof ( int )
#define PROB_TERMINATE 1
#define PARENT_QUEUE_ADDRESS 999
#define MAX_NUM_USER_PROC 18

void timeout();
void interrupt();
void terminate();

typedef struct
{
    long mesg_type;
    int time_slice;
} message;

typedef struct
{
	unsigned int total_used_cpu_time;
	unsigned int total_system_time;
	unsigned int total_last_burst_time;
	unsigned int local_sim_pid;
	unsigned int proc_priority;
} proc_ctrl_blck;

#endif /* PROJECT_4_USER_PROCESS_H_ */
