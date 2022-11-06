/*
 *  Author: Daniel Park
 *  Class: CMP_SCI_4760_002
 *  Project 4
 *  10 / 26 / 2022
 *  The goal of this homework is to become familiar with semaphores in Linux.
 *
 */

#include "oss.h"
#include "queue.h"

void help()
{
	printf("The inputs for this program are: \n");
	printf("-h   : To see help message \n");
	printf("-s t : t is the maximum time in seconds before the system terminates (default 100 seconds)\n");
	printf("-l f : f is the name for the log file");
}

int checkToCreateNewProcess(struct Queue* pid_queue, system_clock* sysclock, int last_proc_created_sec, int last_proc_created_ns)
{
	printf("checkToCreateNewProcess\n");
	int current_sec = sysclock->sec;
	int current_ns  = sysclock->nano_sec;
	int create_process_at_sec = last_proc_created_sec + maxTimeBetweenNewProcsSecs;
	int create_process_at_ns  = last_proc_created_ns + maxTimeBetweenNewProcsNS;

	// Check if pids is avilable
	if (isEmpty(pid_queue))
	{
		return FALSE;
	}
	else if (current_sec == 0 && current_ns == 0)
	{
		return TRUE;
	}
	else if (current_sec > create_process_at_sec || (current_sec == create_process_at_sec && current_ns > create_process_at_ns))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	// TODO: If second is the same, then compare nano seconds
	// TODO: maxTimeBetween (Also, convert to ns)


	return TRUE;
}

int generateRandomNumber(int flag)
{
	srand(time(NULL) + getpid());
	printf("Generating flag %d\n", flag);
	switch (flag)
	{
		case TOTAL_DISPATCH_RANDOM:
			return rand() % (10000 + 1 - 100) + 100;
			break;
		case REAL_OR_USER_RANDOM:
			return rand() % 100 + 1;
			break;
		case LOOP_ADVANCE_RANDOM:
			return rand() % 1000;
			break;
		default:
			return 0;
			break;
	}
}

void incrementSysClock(system_clock* sysclock, int seconds, int nano_seconds)
{
	int ns_to_seconds = 0;

	// Handling incoming nano seconds bigger than 1 sec
	if (nano_seconds >= ONE_SEC_IN_NANO)
	{
		ns_to_seconds += nano_seconds / ONE_SEC_IN_NANO;
		nano_seconds -= ns_to_seconds * ONE_SEC_IN_NANO;
	}
	// Handling sysclock nano seconds bigger than 1 sec
	if (sysclock->nano_sec >= ONE_SEC_IN_NANO)
	{
		ns_to_seconds += sysclock->nano_sec / ONE_SEC_IN_NANO;
		sysclock->nano_sec -= ns_to_seconds * ONE_SEC_IN_NANO;
	}

	sysclock->sec += seconds + ns_to_seconds;
	sysclock->nano_sec += sysclock->nano_sec + nano_seconds;
}

struct Queue* initPidQueue()
{
	struct Queue* pids = createQueue(MAX_NUM_USER_PROC);
	int i;
	for (i = 1; i <= MAX_NUM_USER_PROC; i++)
	{
		enqueue(pids, i);
	}
	return pids;
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
	time_t current_time;
	struct tm * time_info;
	key_t oss_msgqueue_key;
	int oss_msgqueue_id;
	key_t user_msgqueue_key;
	int user_msgqueue_id;
	key_t sysclock_key;
	int sysclock_id;
	key_t process_table_key;
	int process_table_id;

	sysclock_key = ftok("/tmp", 'A');
	oss_msgqueue_key = ftok("/tmp", 'B');
	user_msgqueue_key = ftok("/tmp", 'C');
	process_table_key = ftok("/tmp", 'D');

    sysclock_id = shmget ( sysclock_key, sizeof(unsigned int), 0777 | IPC_CREAT );
    oss_msgqueue_id = msgget(oss_msgqueue_key, 0666 | IPC_CREAT);
    user_msgqueue_id = msgget(user_msgqueue_key, 0666 | IPC_CREAT);
	process_table_id = shmget (process_table_key , sizeof(proc_ctrl_blck) * MAX_NUM_USER_PROC, 0777 | IPC_CREAT );

	msgctl(oss_msgqueue_id, IPC_RMID,NULL);
	msgctl(user_msgqueue_id, IPC_RMID,NULL);
    shmctl(sysclock_id, IPC_RMID,NULL);
    shmctl(process_table_id, IPC_RMID,NULL);
    kill(0, SIGKILL);
}

int main(int argc, char** argv)
{
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
	key_t process_table_key;
	int process_table_id;
	proc_ctrl_blck *proc_ctrl_table;
	int pid_in_use[MAX_NUM_USER_PROC];
	int line_number = 0;
	struct Queue* available_pids = initPidQueue();
	struct Queue* priority_queue = createQueue(100);

	int last_proc_created_sec = 0;
	int last_proc_created_ns  = 0;

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

	int option;
	while ( (option = getopt(argc, argv, "hs:l:")) != -1 )
	{
		switch(option)
		{
			case 'h':
				help();
				return EXIT_SUCCESS;
				break;
			case 's':
				maxTimeToRunProcess = atoi(optarg);
				printf("Maximum time in seconds: %d\n", maxTimeToRunProcess);
				break;
			case 'l':
				fileName = optarg;
				printf("Logfile name is : %s\n", fileName);
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
	process_table_key = ftok("/tmp", 'D');

	if (sysclock_key == (key_t) -1 || oss_msgqueue_key == (key_t) -1 || user_msgqueue_key == (key_t) -1 || process_table_key == (key_t) -1)
	{
	    printf("Error during ftok\n");
	    perror(perrorOutput);
	    exit(EXIT_FAILURE);
	}

    sysclock_id = shmget (sysclock_key , sizeof(system_clock), 0777 | IPC_CREAT );
    oss_msgqueue_id = msgget(oss_msgqueue_key, 0666 | IPC_CREAT);
    user_msgqueue_id = msgget(user_msgqueue_key, 0666 | IPC_CREAT);
    process_table_id = shmget (process_table_key , sizeof(proc_ctrl_blck) * MAX_NUM_USER_PROC, 0777 | IPC_CREAT );
    if ( sysclock_id == -1 || oss_msgqueue_id == -1 || user_msgqueue_id == -1 || process_table_id == -1 )
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

    printf("After generated %d:%d\n", sysclock->sec, sysclock->nano_sec);

    proc_ctrl_table = shmat( process_table_id, 0, 0 );

    signal(SIGINT, interrupt);
    signal(SIGALRM, timeout);
    alarm(maxTimeToRunProcess); // specified number of seconds (default: 100).

	while(1) // while line number is less than 10000
	{
		if (checkToCreateNewProcess(available_pids, sysclock, last_proc_created_sec, last_proc_created_ns))
		{
			pid_t childPid = fork();
			int pid_to_use = dequeue(available_pids);
			if (childPid == 0)
			{
				printf("childPid\n");
				char arg1[10];
				char arg2[10];
				printf("before generateRandomNumber\n");
				int real_or_normal = generateRandomNumber(REAL_OR_USER_RANDOM) <= REAL_TIME_PROC_PERCENTAGE ? REAL_TIME : USER_PROCESS;
				printf("after generateRandomNumber\n");
				printf("real_or_normal : %d\n", real_or_normal);
				sprintf(arg1, "%d", pid_to_use);
				sprintf(arg2, "%d", real_or_normal);
				execl("./user", "./user", arg1, arg2, (char*)0);
//				execl("./user", "./user", arg1, (char*)0);
			}
			else
			{
				proc_ctrl_blck proc_ctrl_block;
				proc_ctrl_block.total_used_cpu_time = 0;
				proc_ctrl_block.total_last_burst_time = 0;
				proc_ctrl_block.total_system_time = 0;
				proc_ctrl_block.local_sim_pid = pid_to_use;
				proc_ctrl_block.proc_priority = 0;
	//			proc_ctrl_block.proc_priority = getPriority();
				proc_ctrl_table[pid_to_use] = proc_ctrl_block;
				enqueue(priority_queue, pid_to_use);
				fprintf(logfile, "OSS: Generating process with PID %d and putting it in queue %d at time %d:%d\n",
						pid_to_use, 0, sysclock->sec, sysclock->nano_sec);
				fflush(logfile);
				last_proc_created_sec = sysclock->sec;
				last_proc_created_ns = sysclock->nano_sec;
				line_number++;
			}

		}
		else
		// else if (there is a process to run)
		{
			int total_dispatch_time = generateRandomNumber(TOTAL_DISPATCH_RANDOM);
			printf("total_dispatch_time %d\n", total_dispatch_time);
			incrementSysClock(sysclock, 0, total_dispatch_time);

			message msg;
//			msg.mesg_type = next_pid;
			msg.mesg_type = 1;
			msg.time_slice = 2;

			fprintf(logfile, "OSS: Dispatching process with PID %d from queue %d at time %d:%d\n",
					1, 0, sysclock->sec, sysclock->nano_sec);
			line_number++;

			fprintf(logfile, "OSS: total time this dispatch was %d nanoseconds\n", total_dispatch_time);
			line_number++;
			fflush(logfile);

			// send to child using specific pid
			msgsnd(user_msgqueue_id, &msg, (sizeof(message)-sizeof(long)), 0);

			// receive from parent's
			msgrcv(oss_msgqueue_id, &msg, sizeof(message), PARENT_QUEUE_ADDRESS, 0);

			fprintf(logfile, "OSS: Receiving that process with PID %d ran for %d nanoseconds\n",
					1, msg.time_slice);
			line_number++;
			fflush(logfile);

			incrementSysClock(sysclock, 0, msg.time_slice);

			// TODO: Check using message if the child process is done
			// TODO: If not done, log addition info, and put it back to queue
			// TODO: If done, remove from proc table block and then enqueue id back to pid queuce


		}

		// Advance the logical clock by 1.xx seconds in each iteration of the loop where xx is the number of nanoseconds.
		incrementSysClock(sysclock, 1, generateRandomNumber(LOOP_ADVANCE_RANDOM));
		printf("increasing time after one loop\n");
		//  range [0,5] and [0,1000]
		// xx will be a random number in the interval [0,1000] to simulate some overhead activity for each iteration.
	}

	//5. Deallocate semaphore, shared memory and terminate.
	printf("ctl all\n");
	msgctl(oss_msgqueue_id, IPC_RMID,NULL);
	msgctl(user_msgqueue_id, IPC_RMID,NULL);
	shmctl(sysclock_id, IPC_RMID,NULL);
	shmctl(process_table_id, IPC_RMID,NULL);
//
	return EXIT_SUCCESS;
}
