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

#define BUFF_SZ	sizeof ( int )
#define MAX_NUM_SLAVE 20

void critical_section(int processNumber);
void writeToLog(char* stage, int processNumber);

#endif /* PROJECT_3_SLAVE_H_ */
