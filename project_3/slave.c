
#include "slave.h"

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
	int currentProcess = atoi(argv[1]);
	int currentTurn;
	srand(getpid());
	key_t semkey;
	int semid;
    struct sembuf sem_wait   = {0, -1, SEM_UNDO};
    struct sembuf sem_signal = {0,  1, SEM_UNDO};

	semkey = ftok("/tmp", 'E');
	if (semkey == (key_t) -1)
	{
	    printf("Error during ftok\n");
	    perror(perrorOutput);
	    exit(EXIT_FAILURE);
	}

	semid = semget(semkey, 1, 0666 | IPC_CREAT);
	if (semid == -1)
	{
	    printf("Error during semget\n");
	    perror(perrorOutput);
	    exit(EXIT_FAILURE);
	}

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

	int i;
	for ( i = 0; i < 5; i++ )
	{
		printf("semid in slave Process: %d, Sem id: %d\n", currentProcess, semid);

		// P()
	    if (semop(semid, &sem_wait, 1) == -1)
	    {
		    printf("Error during semop to wait\n");
		    perror(perrorOutput);
		    exit(EXIT_FAILURE);
	    }
		writeToLog("enter", currentProcess);

	    int waitingTime;

	    waitingTime = (rand() % 3) + 1;
	    sleep(waitingTime);

	    critical_section(currentProcess);

	    waitingTime = (rand() % 3) + 1;
	    sleep(waitingTime);

	    // V()
	    if (semop(semid, &sem_signal, 1) == -1)
	    {
		    printf("Error during semop to signal\n");
		    perror(perrorOutput);
		    exit(EXIT_FAILURE);
	    }
	    writeToLog("exit", currentProcess);
	}

	return EXIT_SUCCESS;
}
