
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h>

#define SHMKEY 859047
#define SHMKEY2 859048
#define MESSAGE "copied from"
#define BUFF_SZ	sizeof ( int )
#define IDLE 0
#define WANT_IN 1
#define IN_CS 2
#define MAX_NUM_SLAVE 20

void critical_section(int processNumber);
void writeToLog(char* stage, int processNumber);



#define SIGNATURE "Daniel"
void critical_section(int processNumber)
{
	// open cstest
	FILE* pfile = fopen("cstest", "a+");

	time_t current_time;
	struct tm * time_info;
	char timeString[9];  // space for "HH:MM:SS\0"

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
	fprintf(pfile, "%s File modified by process number %d I %s %s \n", timeString, processNumber, MESSAGE, SIGNATURE);

	fclose(pfile);
}

void writeToLog(char* stage, int processNumber)
{
	time_t current_time;
	struct tm * time_info;
	char timeString[9];  // space for "HH:MM:SS\0"

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);

	char logfileName[50];

	char processNumberChar[10];
	sprintf(processNumberChar, "%d", processNumber);

	strcpy(logfileName, "logfile.");
	strcat(logfileName, processNumberChar);

	FILE* logfile = fopen(logfileName, "a+");
	fprintf(logfile, "%s %s critical section I %s %s\n", timeString, stage, MESSAGE, SIGNATURE);
	fclose(logfile);
}

int main(int argc, char** argv)
{
	char perrorOutput[100];
	srand(getpid());
	int currentProcess = atoi(argv[1]);
	int currentTurn;

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

    int shmid1 = shmget ( SHMKEY, BUFF_SZ, 0777 | IPC_CREAT );
    int shmid2 = shmget ( SHMKEY2, BUFF_SZ, 0777 | IPC_CREAT );

    int* turn = ( int * )( shmat ( shmid1, 0, 0 ) );
    int* flag = ( int * )( shmat ( shmid2, 0, 0 ) );

	int i;
	for ( i = 0; i < 5; i++ )
	{
		//	execute code to enter critical section;
	    if ( shmid1 == -1 || shmid2 == -1 )
	    {
			printf("slave: Error in shmget \n");
			perror(perrorOutput);
			exit(EXIT_FAILURE);
	    }

	    do
	    {
			flag[currentProcess] = WANT_IN;
			currentTurn = *turn;
			while ( currentTurn != currentProcess )
			{
				currentTurn = ( flag[currentTurn] != IDLE ) ? *turn : ( currentTurn + 1 ) % MAX_NUM_SLAVE;
			}

			// Declare intention to enter critical section
			flag[currentProcess] = IN_CS;

			// Check that no one else is in critical section
			int j;
			for ( j = 1; j <= MAX_NUM_SLAVE; j++ )
			{
				if ( ( j != i ) && ( flag[j] == IN_CS ) )
				{
					break;
				}
			}

	    } while ( *turn != currentProcess && flag[*turn] != IDLE );

	    *turn = currentProcess;
		writeToLog("enter", currentProcess);

	    int waitingTime;

	    printf("Processing child %d\n", currentProcess);
	    waitingTime = (rand() % 3) + 1;
	    sleep(waitingTime);

	    critical_section(currentProcess);

	    waitingTime = (rand() % 3) + 1;
	    sleep(waitingTime);

	    //	execute code to exit critical section;
	    currentTurn = (*turn + 1) % MAX_NUM_SLAVE;
        while (flag[currentTurn] == IDLE)
        {
        	currentTurn = (currentTurn + 1) % MAX_NUM_SLAVE;
        }
        // Assign turn to next waiting process; change own flag to idle
        *turn = currentTurn;
        flag[currentProcess] = IDLE;

	    writeToLog("exit", currentProcess);
	}

    shmdt(turn);
    shmdt(flag);
	return EXIT_SUCCESS;
}
