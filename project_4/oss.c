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

int checkToCreateNewProcess(struct Queue* priority_queue)
{
	// Check maximum process

//	if (lastPidUsed > MAX_NUM_USER_PROC)
	if (!isEmpty(priority_queue))
	{
		return FALSE;
	}

	return TRUE;
}

void incrementSysClock()
{

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
	int lastPidUsed = 1;
	int line_number = 0;
	struct Queue* priority_queue = createQueue(100);

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

    proc_ctrl_table = shmat( process_table_id, 0, 0 );

    signal(SIGINT, interrupt);
    signal(SIGALRM, timeout);
    alarm(maxTimeToRunProcess); // specified number of seconds (default: 100).

	// after it cedes control back to oss, oss would update the simulated system clock by the appropriate amount.
	// This is achieved by generating a random number within the child that will be sent back to oss through
	// either shared memory or a message queue. In the same fashion,
	// if oss does something that should take some time if it was a real operating system,
	//it should increment the clock by a small amount to indicate the time it spent.

	while(1)
	{
		// generate simPid and control block
		// put them in the table and priority queue

		if (checkToCreateNewProcess(priority_queue))
		{
			printf("checktocreatenewprocess == true\n");
			pid_t childPid = fork();
			if (childPid == 0)
			{
				char arg1[10];
				sprintf(arg1, "%d", lastPidUsed);
				execl("./user", "./user", arg1, (char*)0);
//				execl("./user", "./user", NULL);
				printf("After execl\n");
			}
			else
			{
				printf("At parent\n");
				proc_ctrl_blck proc_ctrl_block;
				proc_ctrl_block.total_used_cpu_time = 0;
				proc_ctrl_block.total_last_burst_time = 0;
				proc_ctrl_block.total_system_time = 0;
				proc_ctrl_block.local_sim_pid = lastPidUsed;
				proc_ctrl_block.proc_priority = 0;
	//			proc_ctrl_block.proc_priority = getPriority();
				proc_ctrl_table[lastPidUsed] = proc_ctrl_block;
				enqueue(priority_queue, lastPidUsed);
				//TODO: Log for generating a process
				printf("before file write\n");
				fprintf(logfile, "OSS: Generating process with PID %d and putting it in queue %d at time %d:%d\n",
						lastPidUsed, 0, sysclock->nano_sec, sysclock->nano_sec);
				fflush(logfile);
				lastPidUsed++;
				int status = 0;
//				wait(&status);
			}

		}
		else
		{


				//TODO: Log for dispatching a process with time
	//			printf("sending message\n");

				message msg;
				msg.mesg_type = 1;
				msg.time_slice = 2;

				fprintf(logfile, "OSS: Dispatching process with PID %d from queue %d at time %d:%d\n",
						lastPidUsed, 0, sysclock->nano_sec, sysclock->nano_sec);
				fflush(logfile);

				// send to child using specific pid
				msgsnd(user_msgqueue_id, &msg, (sizeof(message)-sizeof(long)), 0);

				// receive from parent's
				msgrcv(oss_msgqueue_id, &msg, sizeof(message), 100, 0);


				fprintf(logfile, "OSS: total time this dispatch was %d nanoseconds\n",
						msg.time_slice);
				fflush(logfile);
				sysclock -> sec = sysclock->sec + msg.time_slice;

				//Update clock with values coming in from the message

				//TODO: Log for total time dispatch

	//			printf("OSS Message received: %d\n", msg.time_slice);
	//			printf("SYSCLOCK current: %d\n", sysclock -> sec);
				line_number++;

		}


	}
	//it should increment the clock until it is the time when it should launch a process.
	// It should then set up that process, generate a new time when it will create a new process and then using a message queue or semaphore,
	// schedule a process to run by sending it a message. It should then wait for a message back from that process that it has finished its task.
	// If your process table is already full when you go to generate a process, just skip that generation,
	// but do determine another time in the future to try and generate a new process.


	//5. Deallocate semaphore, shared memory and terminate.
	printf("ctl all\n");
	msgctl(oss_msgqueue_id, IPC_RMID,NULL);
	msgctl(user_msgqueue_id, IPC_RMID,NULL);
	shmctl(sysclock_id, IPC_RMID,NULL);
	shmctl(process_table_id, IPC_RMID,NULL);
//
	return EXIT_SUCCESS;
}
