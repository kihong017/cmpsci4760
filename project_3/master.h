/*
 * master.h
 *
 *  Created on: Oct 12, 2022
 *      Author: kihong.park
 */

#ifndef PROJECT_3_MASTER_H_
#define PROJECT_3_MASTER_H_


#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_NUM_SLAVE 20
#define BUFF_SZ	sizeof ( int )

void help();
void callSlaves(int numOfSlaves);
void timeout();
void interrupt();
void terminate();

#endif /* PROJECT_3_MASTER_H_ */
