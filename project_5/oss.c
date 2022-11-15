/*
 *  Author: Daniel Park
 *  Class: CMP_SCI_4760_002
 *  Project 5.
 *  11 / 09 / 2022
 *  The goal of this homework is to learn about resource management inside an operating system.
 *
 */

#include "oss.h"
#include "queue.h"

static int is_timed_out = FALSE;

void help()
{
	printf("There is no required inputs for this program. \n");
	printf("-h : To see help message \n");
	printf("-v : To set the verbose True \n");
}

int checkToCreateNewProcess(int *available_pids, int total_process_generated, system_clock* sysclock, int last_proc_created_sec, int last_proc_created_ns)
{
	if (is_timed_out || total_process_generated > 40)
	{
		return FALSE;
	}

	int ns_to_seconds;

	int current_sec = sysclock->sec;
	int current_ns  = sysclock->nano_sec;
	//  fork a user process at random times (between 1 and 500 milliseconds of your logical clock)
	int create_process_at_sec = last_proc_created_sec;
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

	return TRUE;
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
			return rand() % 250000000 + 1;
			break;
		case PROC_CREATE_NS_RANDOM:
			return rand() % (500000000 + 1 - 1000000) + 1000000; // generate between 1000000 to 500000000
			break;
		case INIT_RESOURCE_NUMBER_RANDOM:
			return rand() % (10 + 1 - 1) + 1; // generate between 1 to 10
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

void initSystemDS(resource_desc* system_data_structure)
{
	int i, j;
	for (i = 0; i < NUM_OF_RESOURCES; i++)
	{
		system_data_structure->available[i] = generateRandomNumber(INIT_RESOURCE_NUMBER_RANDOM);
		system_data_structure->shareable[i] = 0;
	}

	for (j = 0; j < NUM_OF_RESOURCES * MAX_NUM_USER_PROC; j++)
	{
		system_data_structure->allocated[j] = 0;
		system_data_structure->request[j]   = 0;
	}

}

// Check if the request for process pnum is less than or equal to available
// vector

// Pass as one dimensional array
// request is less than available
int req_lt_avail ( const int * req, const int * avail, const int pnum, const int num_res ) // request less than available
{
	int i = 0 ;
    for ( ; i < num_res; i++ )
    {
        if ( req[pnum * num_res + i] > avail[i] )
        {
        	break;
        }
    }
    return ( i == num_res ); // number of resources
}

// availble = vector
// request   = 1 dimensional matrix
// allocated = 1 dimensional matrix
int deadlock ( const int * available, const int m, const int n, const int * request, const int * allocated )
{
    int  work[m];       // m resources
    int finish[n];      // n processes

    int i;
    for ( i = 0 ; i < m; i++ )
    {
    	work[i] = available[i];
    }

    for ( i = 0 ; i < n; i++ )
    {
    	finish[i++] = FALSE;
    }

	int p = 0;
	for ( ; p < n ; p++ ) // For each process
	{
		if ( finish[p] ) continue;
		if ( req_lt_avail ( request, work, p, m ) )
		{
			finish[p] = TRUE;
			for ( i = 0 ; i < m; i++ )
				work[i] += allocated[p * m + i]; // Update work
			p = -1;
		}
	}

    for ( p = 0; p < n; p++ )
    {
        if ( ! finish[p] )
        {
			break;
        }
    }
    return ( p != n ); // If someone did not finish, p != n which is Deadlock
}

void timeout()
{
	printf("\nTime out, not creating process anymore\n");
	is_timed_out = TRUE;
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
    system_data_structure_id = shmget (system_data_structure_key , sizeof(resource_desc), 0777 | IPC_CREAT );

	msgctl(oss_msgqueue_id, IPC_RMID,NULL);
	msgctl(user_msgqueue_id, IPC_RMID,NULL);
    shmctl(sysclock_id, IPC_RMID,NULL);
    shmctl(system_data_structure_id, IPC_RMID,NULL);
    kill(0, SIGKILL);
}

int main(int argc, char** argv)
{
	srand(time(NULL));
	int maxTimeToRunProcess = 5;
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
	resource_desc* system_data_structure;
	int verbose_on = FALSE;

	int line_number = 0;
	int total_process_generated = 0;
	int total_granted = 0;
	int no_action_cnt = 0;

	// Bit vector to keep the process ids
	int* available_pids = initPidVector();
	pid_t* actual_pids = (int *)malloc(sizeof(pid_t) * MAX_NUM_USER_PROC);

	int last_proc_created_sec = 0;
	int last_proc_created_ns  = 0;

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

	int option;
	while ( (option = getopt(argc, argv, "hv")) != -1 )
	{
		switch(option)
		{
			case 'h':
				help();
				return EXIT_SUCCESS;
				break;
			case 'v':
				verbose_on = TRUE;
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
    system_data_structure_id = shmget (system_data_structure_key , sizeof(resource_desc), 0777 | IPC_CREAT );
    if ( sysclock_id == -1 || oss_msgqueue_id == -1 || user_msgqueue_id == -1 || system_data_structure_id == -1 )
    {
		printf("master: Error in shmget \n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
    }

    // Initialize system clock
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

    system_data_structure = shmat( system_data_structure_id, 0, 0 );
    initSystemDS(system_data_structure);

    signal(SIGINT, interrupt);
    signal(SIGALRM, timeout);
    alarm(maxTimeToRunProcess); // Should stop generating process after 5 seconds

	while (line_number < 100000) // while line number is less than 100000
	{
		int pid_to_use = 999;
		if (checkToCreateNewProcess(available_pids, total_process_generated, sysclock, last_proc_created_sec, last_proc_created_ns))
		{
			// Check the pid vector and return the first available
			int i;
			for (i = 0; i < MAX_NUM_USER_PROC; i++)
			{
				if (available_pids[i] == PID_AVAILBLE)
				{
					printf("create a new process pid: %d\n", i);
					available_pids[i] = PID_TAKEN;
					pid_to_use = i;
					break;
				}
			}
		}

		if (total_process_generated <= 40 && pid_to_use != 999)
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
				// Keeps the record of when the lastest process was created
				last_proc_created_sec = sysclock->sec;
				last_proc_created_ns  = sysclock->nano_sec;
				actual_pids[pid_to_use] = childPid;

				total_process_generated++;
			}
		}
		else
		{
			message msg;
			// receive from user_process
			int msg_rcv = msgrcv(oss_msgqueue_id, &msg, sizeof(message), PARENT_QUEUE_ADDRESS, IPC_NOWAIT);

			if (msg_rcv != -1)
			{
				switch (msg.action_flag)
				{
					case TERMINATE_FLAG:
						if (verbose_on)
						{
							fprintf(logfile, "Process P%d is terminated\n", msg.process_id);
							line_number++;
						}

						int i;
						for ( i = 0; i < NUM_OF_RESOURCES; i++ )
						{
							// Make all the request of the process to 0
							system_data_structure->request[msg.process_id * NUM_OF_RESOURCES + i] = 0;
							// Add resources allocated to processes back to available
							system_data_structure->available[i] += system_data_structure->allocated[msg.process_id * NUM_OF_RESOURCES + i];
							// Make all the allocated of the process to 0
							system_data_structure->allocated[msg.process_id * NUM_OF_RESOURCES + i] = 0;
						}

						// Make the pid available again
						available_pids[msg.process_id] = PID_AVAILBLE;

						printf("Killing %d\n", msg.process_id);
						kill(actual_pids[msg.process_id], SIGKILL);
						waitpid(actual_pids[msg.process_id], NULL, 0);

						break;
					case REQUEST_FLAG:
						// Request
						// oss also makes a decision based on the received requests
						// whether the resources should be allocated to processes or not.
						incrementSysClock(sysclock, 1, generateRandomNumber(LOOP_ADVANCE_RANDOM));

						fprintf(logfile, "Master has detetced P%d requesting R%d at time %d:%d\n"
								, msg.process_id, msg.resource_id, sysclock->sec, sysclock->nano_sec);
						line_number++;
						fflush(logfile);

						// Add to request
						system_data_structure->request[msg.process_id * NUM_OF_RESOURCES + msg.resource_id] = msg.num_of_resources;

						// Run deadlock algorithm
						if (verbose_on)
						{
							fprintf(logfile, "Master running deadlock detection at time %d:%d\n"
									, sysclock->sec, sysclock->nano_sec);
							line_number++;
							fflush(logfile);
						}
						int isDeadlock = deadlock(system_data_structure->available,
												  NUM_OF_RESOURCES, MAX_NUM_USER_PROC,
												  system_data_structure->request,
												  system_data_structure->allocated);

						// if granted
						if (!isDeadlock)
						{
							if (verbose_on)
							{
								fprintf(logfile, "   Safe state after granting request\n");
								line_number++;
								fflush(logfile);
							}

							//  Deduct from available
							system_data_structure->available[msg.resource_id] -= msg.num_of_resources;
							// Add to allocate to process and resource
							system_data_structure->allocated[msg.process_id * NUM_OF_RESOURCES + msg.resource_id] += msg.num_of_resources;
							// Send message that request is granted
							msg.mesg_type = msg.process_id;
							msg.mesg_granted = TRUE;

							fprintf(logfile, "   Master granting P%d request R%d at time %d:%d\n"
									, msg.process_id, msg.resource_id, sysclock->sec, sysclock->nano_sec);
							line_number++;
							fflush(logfile);

							total_granted++;

							if (total_granted != 0 && total_granted % 20 == 0)
							{
								int i, j;
								fprintf(logfile, "	");
								for (i = 0; i < NUM_OF_RESOURCES; i++)
								{
									fprintf(logfile, "R%d	", i);
								}
								fprintf(logfile, "\n");
								line_number++;
								for (i = 0; i < MAX_NUM_USER_PROC; i++)
								{
									fprintf(logfile, "P%d	", i);
									for (j = 0; j < NUM_OF_RESOURCES; j++)
									{
										fprintf(logfile, "%d	", system_data_structure->allocated[i * NUM_OF_RESOURCES + j]);
									}
									fprintf(logfile, "\n");
									line_number++;
								}
							}

							msgsnd(user_msgqueue_id, &msg, (sizeof(message)-sizeof(long)), 0);
						}
						else
						{
							// If it cannot allocate resources,
							// Send deny
							if (verbose_on)
							{
								fprintf(logfile, "   Master has detected deadlock\n");
								line_number++;
								fflush(logfile);
								fprintf(logfile, "   Unsafe state after granting request; request not granted\n");
								line_number++;
								fflush(logfile);
								fprintf(logfile, "   P%d added to wait queue, waiting for R%d\n"
										, msg.process_id, msg.resource_id);
								line_number++;
								fflush(logfile);
							}

							msg.mesg_type = msg.process_id;
							msg.mesg_granted = FALSE;
							msgsnd(user_msgqueue_id, &msg, (sizeof(message)-sizeof(long)), 0);
						}

						break;
					case RELEASE_FLAG:
						incrementSysClock(sysclock, 1, generateRandomNumber(LOOP_ADVANCE_RANDOM));

						msg.mesg_type = msg.process_id;

						if (system_data_structure->allocated[msg.process_id * NUM_OF_RESOURCES + msg.resource_id] < msg.num_of_resources)
						{
							msg.mesg_granted = FALSE;
							msgsnd(user_msgqueue_id, &msg, (sizeof(message)-sizeof(long)), 0);
						}
						else
						{
							system_data_structure->available[msg.resource_id] += msg.num_of_resources;
							system_data_structure->allocated[msg.process_id * NUM_OF_RESOURCES + msg.resource_id] -= msg.num_of_resources;

							if (verbose_on)
							{
								fprintf(logfile, "   Resources released R%d: %d from P%d\n"
										, msg.resource_id, msg.num_of_resources, msg.process_id);
								line_number++;
								fflush(logfile);
							}

							msg.mesg_granted = TRUE;
							msgsnd(user_msgqueue_id, &msg, (sizeof(message)-sizeof(long)), 0);
						}
						break;
					default:
						break;
				}

			}
			else
			{
				no_action_cnt++;
				if (no_action_cnt >= 10000000)
				{
					sleep(1);
					terminate();
				}
			}

			incrementSysClock(sysclock, 0, 1);
		}

		// Advance the logical clock by 1.xx seconds in each iteration of the loop where xx is the number of nanoseconds.
		incrementSysClock(sysclock, 1, generateRandomNumber(LOOP_ADVANCE_RANDOM));
	}

	// Deallocate semaphore, shared memory and terminate.
	terminate();

	return EXIT_SUCCESS;
}
