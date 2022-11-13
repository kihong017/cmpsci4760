/*
 *  Author: Daniel Park
 *  Class: CMP_SCI_4760_002
 *  Project 5.
 *  11 / 09 / 2022
 *  The goal of this homework is to become familiar with semaphores in Linux.
 *
 */

#include "oss.h"
#include "queue.h"

void help()
{
	printf("There is no required inputs for this program. \n");
	printf("-h : To see help message \n");
}

int checkToCreateNewProcess(int *available_pids, system_clock* sysclock, int last_proc_created_sec, int last_proc_created_ns)
{
	int ns_to_seconds;

	int current_sec = sysclock->sec;
	int current_ns  = sysclock->nano_sec;
	// A new process should be generated every 1 second, on an average.
	int create_process_at_sec = last_proc_created_sec + generateRandomNumber(PROC_CREATE_SEC_RANDOM);
	int create_process_at_ns  = last_proc_created_ns + generateRandomNumber(PROC_CREATE_NS_RANDOM);

	if (create_process_at_ns >= ONE_SEC_IN_NANO)
	{
		ns_to_seconds = 0;
		ns_to_seconds += create_process_at_ns / ONE_SEC_IN_NANO;
		create_process_at_ns -= ns_to_seconds * ONE_SEC_IN_NANO;
		create_process_at_sec += ns_to_seconds;
	}

	// Check the time
	if (!(current_sec == 0 && current_ns == 0) && !isAfter(current_sec, current_ns, create_process_at_sec, create_process_at_ns) )
	{
		return FALSE;
	}

	// Check the pid vector and return the first available
	int i;
	for (i = 0; i < MAX_NUM_USER_PROC; i++)
	{
		if (available_pids[i] == PID_AVAILBLE)
		{
			available_pids[i] = PID_TAKEN;
			return i + 1;
		}
	}

	return FALSE;
}

int isAfter(int current_sec, int current_ns, int compare_sec, int compare_ns)
{
	return current_sec > compare_sec || (current_sec == compare_sec && current_ns > compare_ns);
}

int generateRandomNumber(int flag)
{
	switch (flag)
	{
		case LOOP_ADVANCE_RANDOM:
			return rand() % 1000;
			break;
		case PROC_CREATE_NS_RANDOM:
			return rand() % (500000000 + 1 - 1000000) + 1000000; // generate between 1000000 to 500000000
			break;
		default:
			return 0;
			break;
	}
}

void incrementSysClock(system_clock* sysclock, int seconds, int nano_seconds)
{
	int ns_to_seconds = 0;

	// Handling incoming nano seconds bigger than 1 sec in nano seconds
	if (nano_seconds >= ONE_SEC_IN_NANO)
	{
		ns_to_seconds += nano_seconds / ONE_SEC_IN_NANO;
		nano_seconds  -= ns_to_seconds * ONE_SEC_IN_NANO;
	}
	// Handling sysclock nano seconds bigger than 1 sec in nano seconds
	sysclock->sec += seconds + ns_to_seconds;
	sysclock->nano_sec += sysclock->nano_sec + nano_seconds;

	// recalculate current time if nano second is bigger than one sec
	ns_to_seconds = 0;
	if (sysclock->nano_sec >= ONE_SEC_IN_NANO)
	{
		ns_to_seconds += sysclock->nano_sec / ONE_SEC_IN_NANO;
		sysclock->sec += ns_to_seconds;
		sysclock->nano_sec -= (ns_to_seconds * ONE_SEC_IN_NANO);
	}
}

int* initPidVector()
{
	int *pids = (int *)malloc(sizeof(int) * MAX_NUM_USER_PROC);

	int i;
	for (i = 0; i < MAX_NUM_USER_PROC; i++)
	{
		pids[i] = PID_AVAILBLE;
	}
	return pids;
}

// Check if the request for process pnum is less than or equal to available
// vector

// Pass as one dimensional array
// request is less than available
int req_lt_avail ( const int * req, const int * avail, const int pnum, const int num_res ) // request less than available
{
	int i = 0 ;
    for ( ; i < num_res; i++ )
        if ( req[pnum * num_res + i] > avail[i] )
            break;
    return ( i == num_res ); // number of resources
}

// availble = vector
// request   = 1 dimensional metrix
// allocated = 1 dimensional metrix
int deadlock ( const int * available, const int m, const int n, const int * request, const int * allocated )
{
    int  work[m];       // m resources
    int finish[n];      // n processes

    for ( int i = 0 ; i < m; work[i] = available[i++] );
    for ( int i = 0 ; i < n; finish[i++] = FALSE );

	int p = 0;
	for ( ; p < n ; p++ ) // For each process
	{
		if ( finish[p] ) continue;
		if ( req_lt_avail ( request, work, p, m ) )
		{
			finish[p] = TRUE;
			for ( int i = 0 ; i < m; i++ )
				work[i] += allocated[p*m+i]; // Update work
			p = -1;
		}
	}

    for ( p = 0; p < n; p++ )
        if ( ! finish[p] )
					break;
    return ( p != n ); // If someone did not finish, p != n which is Deadlock
}

void timeout()
{
	printf("\nTime out, so killing all the processes, message queues, and shm\n");
	terminate();
	return;
}

void interrupt()
{
	printf("\nInterrupt, so killing all the processes, message queues, and shm\n");
	terminate();
}

void terminate()
{
	key_t oss_msgqueue_key;
	int oss_msgqueue_id;
	key_t user_msgqueue_key;
	int user_msgqueue_id;
	key_t sysclock_key;
	int sysclock_id;
	key_t system_data_structure_key;
	int system_data_structure_id;

	sysclock_key = ftok("/tmp", 'A');
	oss_msgqueue_key = ftok("/tmp", 'B');
	user_msgqueue_key = ftok("/tmp", 'C');
	system_data_structure_key = ftok("/tmp", 'D');

    sysclock_id = shmget ( sysclock_key, sizeof(unsigned int), 0777 | IPC_CREAT );
    oss_msgqueue_id = msgget(oss_msgqueue_key, 0666 | IPC_CREAT);
    user_msgqueue_id = msgget(user_msgqueue_key, 0666 | IPC_CREAT);
    system_data_structure_id = shmget (system_data_structure_key , sizeof(resource_desc) * NUM_OF_RESOURCES, 0777 | IPC_CREAT );

	msgctl(oss_msgqueue_id, IPC_RMID,NULL);
	msgctl(user_msgqueue_id, IPC_RMID,NULL);
    shmctl(sysclock_id, IPC_RMID,NULL);
    shmctl(system_data_structure_id, IPC_RMID,NULL);
    kill(0, SIGKILL);
}

int main(int argc, char** argv)
{
	srand(time(NULL));
	int maxTimeToRunProcess = 100;
	char perrorOutput[100];
	char* fileName = "logfile";
	key_t oss_msgqueue_key;
	int oss_msgqueue_id;
	key_t user_msgqueue_key;
	int user_msgqueue_id;
	key_t sysclock_key;
	int sysclock_id;
	system_clock* sysclock;
	key_t system_data_structure_key;
	int system_data_structure_id;
	resource_desc *system_data_structure;

	int line_number = 0;
	int* available_pids = initPidVector();

	int last_proc_created_sec = 0;
	int last_proc_created_ns  = 0;

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

	int option;
	while ( (option = getopt(argc, argv, "h")) != -1 )
	{
		switch(option)
		{
			case 'h':
				help();
				return EXIT_SUCCESS;
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

	FILE* logfile = fopen(fileName, "w");

	sysclock_key = ftok("/tmp", 'A');
	oss_msgqueue_key = ftok("/tmp", 'B');
	user_msgqueue_key = ftok("/tmp", 'C');
	system_data_structure_key = ftok("/tmp", 'D');

	if (sysclock_key == (key_t) -1 || oss_msgqueue_key == (key_t) -1 || user_msgqueue_key == (key_t) -1 || system_data_structure_key == (key_t) -1)
	{
	    printf("Error during ftok\n");
	    perror(perrorOutput);
	    exit(EXIT_FAILURE);
	}

    sysclock_id = shmget (sysclock_key , sizeof(system_clock), 0777 | IPC_CREAT );
    oss_msgqueue_id = msgget(oss_msgqueue_key, 0666 | IPC_CREAT);
    user_msgqueue_id = msgget(user_msgqueue_key, 0666 | IPC_CREAT);
    system_data_structure_id = shmget (system_data_structure_key , sizeof(resource_desc) * NUM_OF_RESOURCES, 0777 | IPC_CREAT );
    if ( sysclock_id == -1 || oss_msgqueue_id == -1 || user_msgqueue_id == -1 || system_data_structure_id == -1 )
    {
		printf("master: Error in shmget \n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
    }

    // Initialize system clock
    //TODO; the logical clock resides in shared memory and is accessed as a critical resource using a semaphore.
    sysclock = shmat( sysclock_id, 0, 0 );
    if ( sysclock == (void *)-1  )
    {
		printf("master: Error in shmat sysclock \n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
    }

    sysclock -> sec = 0;
    sysclock -> nano_sec = 0;

    if ( sysclock == (void *)-1  )
    {
		printf("master: Error in shmat sysclock \n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
    }

	// including resource descriptors for each resource. All the resources are static but some of them may be shared.
	// The resource descriptor is a fixed size structure and contains information on managing the resources within oss.
	// Make sure that you allocate space to keep track of activities that affect the resources, such as request, allocation, and release.
	// The resource descriptors will reside in shared memory and will be accessible to the children.
	// Create descriptors for 20 resources, out of which about 20% should be shareable resources
	// *about implies that it should be 20 ± 5% and you should generate that number with a random number generator.

    system_data_structure = shmat( system_data_structure_id, 0, 0 );

    signal(SIGINT, interrupt);
    signal(SIGALRM, timeout);
    alarm(maxTimeToRunProcess); // specified number of seconds (default: 100).

	while (line_number < 100000) // while line number is less than 100000
	{
		int pid_to_use = checkToCreateNewProcess(available_pids, sysclock, last_proc_created_sec, last_proc_created_ns);
		if (pid_to_use > 0)
		{
			pid_t childPid = fork();
			if (childPid == 0)
			{
				char arg1[10];
				sprintf(arg1, "%d", pid_to_use);
				execl("./user_proc", "./user_proc", arg1, (char*)0);
			}
			else
			{
				fprintf(logfile, "OSS: Generating process with PID %d and putting it in queue at time %d:%d\n",
						pid_to_use, sysclock->sec, sysclock->nano_sec);
				fflush(logfile);
				// Keeps the record of when the lastest process was created
				last_proc_created_sec = sysclock->sec;
				last_proc_created_ns  = sysclock->nano_sec;
				line_number++;
			}

		}
		else
		{
			// oss also makes a decision based on the received requests whether the resources should be allocated to processes or not.
			// It does so by running the deadlock detection algorithm with the current request from a process and grants the resources
			// if there is no deadlock, updating all the data structures.
			// If a process releases resources, it updates that as well, and may give resources to some waiting processes.
			// If it cannot allocate resources, the process goes in a queue waiting for the resource requested and goes to sleep.
			// It gets awakened when the resources become available, that is whenever the resources are released by a process.

			int pid_to_process = 0;
			int queue_id = 0;
			int time_slice = 0;

			fprintf(logfile, "OSS: Dispatching process with PID %d from queue %d at time %d:%d\n",
					pid_to_process, queue_id, sysclock->sec, sysclock->nano_sec);
			line_number++;

			fflush(logfile);

			// Sending a message to the process using specific pid and how much of a time slice it has to run
			message msg;
			msg.mesg_type = pid_to_process;
			msg.time_slice = time_slice;

			msgsnd(user_msgqueue_id, &msg, (sizeof(message)-sizeof(long)), 0);

			// receive from parent's
			msgrcv(oss_msgqueue_id, &msg, sizeof(message), PARENT_QUEUE_ADDRESS, 0);

			printf("OSS: Receiving that process with PID %d ran for %d nanoseconds\n",
					pid_to_process, msg.time_slice);
			fprintf(logfile, "OSS: Receiving that process with PID %d ran for %d nanoseconds\n",
					pid_to_process, msg.time_slice);
			line_number++;
			fflush(logfile);

			incrementSysClock(sysclock, 0, msg.time_slice);
		}

		// Advance the logical clock by 1.xx seconds in each iteration of the loop where xx is the number of nanoseconds.
		incrementSysClock(sysclock, 1, generateRandomNumber(LOOP_ADVANCE_RANDOM));
	}

	// Deallocate semaphore, shared memory and terminate.
	terminate();

	return EXIT_SUCCESS;
}
