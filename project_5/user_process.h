/*
 * user_process.h
 *
 *  Created on: Nov 9, 2022
 *      Author: kihong.park
 */

#ifndef PROJECT_5_USER_PROCESS_H_
#define PROJECT_5_USER_PROCESS_H_

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
#define TERMINATE_PROBABILITY 1
#define PARENT_QUEUE_ADDRESS 999

#define NUM_OF_RESOURCES 20
#define MAX_NUM_USER_PROC 18
#define TRUE  1
#define FALSE 0

#define CPU_BOUND_INTERRUPT_PCT 5
#define IO_BOUND_INTERRUPT_PCT 60

#define GET_RANDOM_PERCENTAGE 0
#define TIME_SLICE_USAGE_RANDOM 1

#define CPU_BOUND 0
#define IO_BOUND  1

int  isTerminated();
int  generateRandomNumber(int flag, int time_slice);
void timeout();
void interrupt();
void terminate();

typedef struct
{
    long mesg_type;
    int time_slice;
    int is_interrupted;
} message;

typedef struct
{
	unsigned int request[NUM_OF_RESOURCES];
	unsigned int allocation[NUM_OF_RESOURCES];
	unsigned int release[NUM_OF_RESOURCES];
} resource_desc;

#endif /* PROJECT_5_USER_PROCESS_H_ */
