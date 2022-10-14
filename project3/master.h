/*
 * master.h
 *
 *  Created on: Sep 29, 2022
 *      Author: Daniel Park
 */

#ifndef PROJECT_2_MASTER_H_
#define PROJECT_2_MASTER_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_NUM_SLAVE 20
#define SHMKEY 859047
#define SHMKEY2 859048
#define BUFF_SZ	sizeof ( int )
#define IDLE 0
#define WANT_IN 1
#define IN_CS 2

void help();
void callSlaves(int numOfSlaves);
void timeout();
void interrupt();
void terminate();

#endif /* PROJECT_2_MASTER_H_ */
