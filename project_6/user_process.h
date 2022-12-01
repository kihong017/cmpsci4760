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
#define PARENT_QUEUE_ADDRESS 999
#define TERMINATE_CHECK_NS 250000000
#define TERMINATE_PCT 10
#define REQUEST_PCT 50

#define NUM_OF_RESOURCES 20
#define MAX_NUM_USER_PROC 15
#define TRUE  1
#define FALSE 0

#define GET_RANDOM_PERCENTAGE 0
#define MAX_CLAIM_RANDOM 1
#define RESOURCE_ID_RANDOM 2

#define TERMINATE_FLAG 1
#define REQUEST_FLAG   2
#define RELEASE_FLAG   3

int  isTerminated();
int  isRequesting();
int  generateRandomNumber(int flag);
void interrupt();
void terminate();

typedef struct
{
	unsigned int sec;
	unsigned int nano_sec;
} system_clock;

typedef struct
{
    long mesg_type;
    int process_id;
    int resource_id;
    int action_flag;
    int mesg_granted;
    int num_of_resources;
} message;

typedef struct
{
	unsigned int request[NUM_OF_RESOURCES * MAX_NUM_USER_PROC];
	unsigned int allocated[NUM_OF_RESOURCES * MAX_NUM_USER_PROC];
	unsigned int available[NUM_OF_RESOURCES];
	unsigned int shareable[NUM_OF_RESOURCES];
} resource_desc;

#endif /* PROJECT_5_USER_PROCESS_H_ */
