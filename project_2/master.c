/*
 *  Author: Daniel Park
 *  Class: CMP_SCI_4760_002
 *  Project 2
 *  9 / 26 / 2022
 *  The goal of this homework is to become familiar
 *  with concurrent processing in Linux using shared memory
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define MAX_NUM_SLAVE 20
#define SHMKEY 859047
#define BUFF_SZ	sizeof ( int )

void help();

void help()
{
	printf("The inputs for this program are: \n");
	printf("-h  : To see help message \n");
	printf("-t ss n \n");
	printf(" ss is the maximum time in seconds (default 100 seconds)\n");
	printf(" after which the process should terminate itself if not completed\n");
	printf(" n is the number of slave processes at a time. n never exceeds 20");
}

int main(int argc, char** argv)
{
	//Write master that runs up to n slave processes at a time. Make sure that n never exceeds 20
	//
	//Start master by
	//typing the following command:
	//
	//master -t ss n
	//
	//where ss is the maximum time in seconds (default 100 seconds) after which the process should terminate itself if not
	//completed.
	//
	//Implement master as follows:
	//
	//1. Check for the command line argument and output a usage message if the argument is not appropriate.
	//   If n is more than 20, issue a warning and limit n to 20.
	//   It will be a good idea to #define the maximum value of n or keep it as a configurable.
	int numOfSlaves = MAX_NUM_SLAVE;
	int maxTimeToRunProcess = 100;
	char perrorOutput[100];

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

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
				//TODO: Find a way to deal with multiple input
//				numOfSlaves = atoi(optarg);
//				printf("maximum time in seconds: %d\n", atoi(optarg));
//				printf("Number of slaves: %c\n", atoi(argv[1]));
//				if (numOfSlaves > MAX_NUM_SLAVE) printf("Number of slaves cannot be bigger than 20, setting it to 20");
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
    int shmid = shmget ( SHMKEY, BUFF_SZ, 0777 | IPC_CREAT );
    if ( shmid == -1 )
    {
		printf("master: Error in shmget \n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
    }

    char * paddr = ( char * )( shmat ( shmid, 0, 0 ) );
    int * pint = ( int * )( paddr );

	//3. Execute the slave processes and wait for all of them to terminate.
	sleep ( 2 );

	//4. Start a timer for specified number of seconds (default: 100). If all children have not terminated by then, terminate the children.


	//5. Deallocate shared memory and terminate.
    shmdt(pint);
    shmctl(shmid,IPC_RMID,NULL);

}
