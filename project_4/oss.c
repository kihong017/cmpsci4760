/*
 *  Author: Daniel Park
 *  Class: CMP_SCI_4760_002
 *  Project 4
 *  10 / 26 / 2022
 *  The goal of this homework is to become familiar with semaphores in Linux.
 *
 */

#include "oss.h"

void help()
{
	printf("The inputs for this program are: \n");
	printf("-h  : To see help message \n");
	printf("-t ss n \n");
	printf(" ss is the maximum time in seconds (default 100 seconds)\n");
	printf(" after which the process should terminate itself if not completed\n");
	printf(" n is the number of slave processes at a time. n never exceeds 20");
}

void callUserProcess()
{

}

void interrupt()
{
	printf("\nInterrupt, so killing all the processes, semaphores, and shm\n");
	terminate();
}

void terminate()
{
	time_t current_time;
	struct tm * time_info;
	char timeString[9];  // space for "HH:MM:SS\0"
	key_t semkey;
	int semid;

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);

	FILE* logfile = fopen("logfile", "a+");
	fprintf(logfile, "Terminated at %s\n", timeString);
	fclose(logfile);

	semkey = ftok("/tmp", 'E');
	semid = semget(semkey, 1, 0666 | IPC_CREAT);
    semctl(semid, 0, IPC_RMID);
}

int main(int argc, char** argv)
{
	int numOfSlaves = MAX_NUM_SLAVE;
	int maxTimeToRunProcess = 100;
	char perrorOutput[100];
	key_t semkey;
	int semid;

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

	if (argc >= 4)
	{
		numOfSlaves = atoi(argv[3]);
		if (numOfSlaves > MAX_NUM_SLAVE)
		{
			printf("Number of slaves cannot be bigger than 20, setting it back to 20\n");
			numOfSlaves = MAX_NUM_SLAVE;
		}
		printf("Number of slaves : %d\n", numOfSlaves);
	}

	int option;
	while ( (option = getopt(argc, argv, "ht:")) != -1 )
	{
		switch(option)
		{
			case 'h':
				help();
				return EXIT_SUCCESS;
				break;
			case 't':
				maxTimeToRunProcess = atoi(optarg);
				printf("maximum time in seconds: %d\n", maxTimeToRunProcess);
				break;
			case '?':
				if (isprint (optopt))
				   fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
				   fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				return EXIT_FAILURE;
			default:
				help();
				return EXIT_SUCCESS;
		}
	}

    signal(SIGINT, interrupt);

    callUserProcess(numOfSlaves);

	//5. Deallocate semaphore, shared memory and terminate.
    semctl(semid, 0, IPC_RMID);
}
