/*
 * user_process.c
 *
 *  Created on: Oct 27 2022
 *      Author: Daniel Park
 */
#include "user_process.h"

void timeout()
{
	printf("\nTime out, so killing all the processes, semaphores, and shm\n");
	terminate();
	return;
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
	printf("In User Process\n");
	char perrorOutput[100];
	int current_process_id = atoi(argv[1]);
	int real_or_normal = atoi(argv[2]);

	printf("current_process_id : %d\n", atoi(argv[1]));
	printf("real_or_normal : %d\n", atoi(argv[2]));
	key_t oss_msgqueue_key;
	int oss_msgqueue_id;
	key_t user_msgqueue_key;
	int user_msgqueue_id;
	message msg;
	key_t process_table_key;
	int process_table_id;
	proc_ctrl_blck *proc_ctrl_table;
	proc_ctrl_blck proc_ctrl_block;

	oss_msgqueue_key = ftok("/tmp", 'B');
	user_msgqueue_key = ftok("/tmp", 'C');
	process_table_key = ftok("/tmp", 'D');

	if (oss_msgqueue_key == (key_t) -1 || user_msgqueue_key == (key_t) -1 || process_table_key == (key_t) -1)
	{
		printf("Error during ftok\n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
	}

	printf("before msgget\n");
	oss_msgqueue_id = msgget(oss_msgqueue_key, 0666);
	user_msgqueue_id = msgget(user_msgqueue_key, 0666);
	process_table_id = shmget (process_table_key , sizeof(proc_ctrl_blck) * MAX_NUM_USER_PROC, 0777 | IPC_CREAT );

	if ( oss_msgqueue_id == -1 || user_msgqueue_id == -1 || process_table_id == -1 )
	{
		printf("Error during get\n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
	}

	proc_ctrl_table = shmat( process_table_id, 0, 0 );
	proc_ctrl_block = proc_ctrl_table[current_process_id];

	printf("proc_ctrl_block pid : %d\n", proc_ctrl_block.local_sim_pid);

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

		while (1)
		// while (!isTerminated)
		{
			printf("waiting for message\n");
			int msg_rcv = msgrcv(user_msgqueue_id, &msg, (sizeof(message)-sizeof(long)), 1, 0);
			if (msg_rcv != -1)
			{
				sleep(5);
				msg.mesg_type = PARENT_QUEUE_ADDRESS;
				msg.time_slice = 1000000002;
				//TODO: Need to send back whether the process terminated or not

				printf("User Message sent: %d\n", msg.time_slice);
				printf("Message Sent\n");
				msgsnd(oss_msgqueue_id, &msg, sizeof(message), 0);
			}
		}


		// TODO: Generate random number: to decide whether to terminate or not
		// seeding off of some function of the process’ pid (for example, time + pid)

		// TODO: Terminate: If it would terminate, it would of course use some random amount of its timeslice before terminating
		// indicate to oss that it has decided to terminate and also how much of its timeslice it used


		// TODO: Generate random number: to define how much timeslice was used

		/// TODO: Send a message to oss saying how much time they used
		//        and if they had to use i/o or had to terminate.


		// Need to send back with time



	return EXIT_SUCCESS;
}
