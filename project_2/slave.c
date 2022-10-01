
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
#define BUFF_SZ	sizeof ( int )

void critical_section(FILE* pfile, int processNumber);
void writeToLog(char* stage, int processNumber);

void critical_section(FILE* pfile, int processNumber)
{
	// open cstest
	time_t current_time;
	struct tm * time_info;
	char timeString[9];  // space for "HH:MM:SS\0"

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
	fprintf(pfile, "%s File modified by process number %d\n", timeString, processNumber);

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
	fprintf(logfile, "%s %s critical section\n", timeString, stage);
	fclose(logfile);
}

int main(int argc, char** argv)
{
	char perrorOutput[100];
	srand(getpid());

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

	int i;
	for ( i = 0; i < 5; i++ )
	{
		//	execute code to enter critical section;
	    int shmid = shmget ( SHMKEY, BUFF_SZ, 0777 );
	    if ( shmid == -1 )
	    {
			printf("slave: Error in shmget \n");
			perror(perrorOutput);
			exit(EXIT_FAILURE);
	    }
		FILE * pfile = ( FILE * )( shmat ( shmid, 0, 0 ) );
		writeToLog("enter", atoi(argv[1]));

		pfile = fopen("cstest", "a+");

	    int waitingTime;

	    printf("Processing child %d\n", atoi(argv[1]));
	    waitingTime = (rand() % 3) + 1;
	    sleep(waitingTime);

	    critical_section(pfile, atoi(argv[1]));

	    waitingTime = (rand() % 3) + 1;
	    sleep(waitingTime);

	    //	execute code to exit critical section;
	    shmdt(pfile);
	    writeToLog("exit", atoi(argv[1]));
	}
	return EXIT_SUCCESS;
}
