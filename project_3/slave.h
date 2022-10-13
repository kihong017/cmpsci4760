/*
 * slave.h
 *
 *  Created on: Oct 13, 2022
 *      Author: kihong.park
 */

#ifndef PROJECT_3_SLAVE_H_
#define PROJECT_3_SLAVE_H_

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
#include <time.h>

#define SHMKEY 859047
#define SHMKEY2 859048
#define BUFF_SZ	sizeof ( int )
#define IDLE 0
#define WANT_IN 1
#define IN_CS 2
#define MAX_NUM_SLAVE 20

void critical_section(int processNumber);
void writeToLog(char* stage, int processNumber);


#endif /* PROJECT_3_SLAVE_H_ */
