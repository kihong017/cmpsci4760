/*
 *  Author: Daniel Park
 *  Class: CMP_SCI_4760_002
 *  Project 2
 *  9 / 26 / 2022
 *  The goal of this homework is to become familiar
 *  with concurrent processing in Linux using shared memory
 */

#include "master.h"

void help()
{
	printf("The inputs for this program are: \n");
	printf("-h  : To see help message \n");
	printf("-t ss n \n");
	printf(" ss is the maximum time in seconds (default 100 seconds)\n");
	printf(" after which the process should terminate itself if not completed\n");
	printf(" n is the number of slave processes at a time. n never exceeds 20");
}

void callSlaves(int numOfSlaves)
{
	//3. Execute the slave processes and wait for all of them to terminate.
	int i ;
	for (i = 1; i <= numOfSlaves; i++)
	{
		printf("%d\n", i);
		pid_t childPid = fork();
		if (childPid == 0)
		{
			char arg1[10];
			sprintf(arg1, "%d", i);
			execl("./slave", "./slave", arg1, (char*)0);
			exit(EXIT_SUCCESS);
		}
	}

	for (i = 1; i <= numOfSlaves; i++)
	{
        int status = 0;
        pid_t childpid = wait(&status);
        printf("Child %d is finished. \n", (int)childpid);
	}
}

void timeout()
{
	printf("Time out, so killing all the processes and shm");
	terminate();
	return;
}

void interrupt()
{
	printf("Interrupt, so killing all the processes and shm");
	terminate();
}

void terminate()
{
	time_t current_time;
	struct tm * time_info;
	char timeString[9];  // space for "HH:MM:SS\0"

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);

	FILE* logfile = fopen("logfile", "a+");
	fprintf(logfile, "Terminated at %s\n", timeString);
	fclose(logfile);

    int shmid1 = shmget ( SHMKEY, BUFF_SZ, 0777 | IPC_CREAT );
    int shmid2 = shmget ( SHMKEY2, BUFF_SZ, 0777 | IPC_CREAT );
    shmctl(shmid1, IPC_RMID,NULL);
    shmctl(shmid2, IPC_RMID,NULL);

	kill(0, SIGKILL);
}

int main(int argc, char** argv)
{
	int numOfSlaves = MAX_NUM_SLAVE;
	int maxTimeToRunProcess = 100;
	char perrorOutput[100];

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

	if (argc >= 4) {
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

	//2. Allocate shared memory and initialize it appropriately.
    int shmid1 = shmget ( SHMKEY, BUFF_SZ, 0777 | IPC_CREAT );
    int shmid2 = shmget ( SHMKEY2, BUFF_SZ, 0777 | IPC_CREAT );

    if ( shmid1 == -1 || shmid2 == -1 )
    {
		printf("master: Error in shmget \n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
    }

    int* turn = ( int * )( shmat ( shmid1, 0, 0 ) );
    int* flag = ( int * )( shmat ( shmid2, 0, 0 ) );
    *turn = 1;

    int j;
    for (int j = 1; j <= 20; j++)
    {
    	flag[j] = IDLE;
    }

    shmdt(turn);

    signal(SIGINT, interrupt);
    signal(SIGALRM, timeout);
    alarm(maxTimeToRunProcess); // specified number of seconds (default: 100).

    callSlaves(numOfSlaves);

	//5. Deallocate shared memory and terminate.
    shmctl(shmid1, IPC_RMID,NULL);
    shmctl(shmid2, IPC_RMID,NULL);

}
