/*
 * user_process.c
 *
 *  Created on: Nov 9 2022
 *      Author: Daniel Park
 */
#include "user_process.h"

int isTerminated()
{
	return generateRandomNumber(GET_RANDOM_PERCENTAGE) <= TERMINATE_PCT;
}

int isRequesting()
{
	return generateRandomNumber(GET_RANDOM_PERCENTAGE) <= REQUEST_PCT;
}

int generateRandomNumber(int flag)
{
	switch (flag)
	{
		case GET_RANDOM_PERCENTAGE:
			return rand() % 100 + 1;
			break;
		case MAX_CLAIM_RANDOM:
			return rand() % NUM_OF_RESOURCES + 1;
			break;
		case RESOURCE_ID_RANDOM:
			return rand() % NUM_OF_RESOURCES;
			break;
		default:
			return 0;
			break;
	}
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
	printf("Inside the user process\n");
	srand(time(NULL) + getpid());
	char perrorOutput[100];
	int current_process_id = atoi(argv[1]);

	printf("current_process_id : %d\n", atoi(argv[1]));
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
	message msg;
	int max_claims = generateRandomNumber(MAX_CLAIM_RANDOM);
	int current_resource = 0;
	int is_granted;
	int is_terminated = FALSE;
	int allocated_per_resource[NUM_OF_RESOURCES];

	int i;
	for (i = 0; i < NUM_OF_RESOURCES; i++)
	{
		allocated_per_resource[i] = 0;
	}

	sysclock_key = ftok("/tmp", 'A');
	oss_msgqueue_key  = ftok("/tmp", 'B');
	user_msgqueue_key = ftok("/tmp", 'C');
	system_data_structure_key = ftok("/tmp", 'D');

	if (sysclock_key == (key_t) -1 || oss_msgqueue_key == (key_t) -1 || user_msgqueue_key == (key_t) -1 || system_data_structure_key == (key_t) -1)
	{
		printf("Error during ftok\n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
	}

	sysclock_id = shmget ( sysclock_key, sizeof(unsigned int), 0777 | IPC_CREAT );
	oss_msgqueue_id  = msgget(oss_msgqueue_key, 0666);
	user_msgqueue_id = msgget(user_msgqueue_key, 0666);
	system_data_structure_id = shmget (system_data_structure_key , sizeof(resource_desc), 0777 | IPC_CREAT );

	if ( sysclock_id == -1 ||  oss_msgqueue_id == -1 || user_msgqueue_id == -1 || system_data_structure_id == -1 )
	{
		printf("Error during get\n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
	}

    sysclock = shmat( sysclock_id, 0, 0 );
    if ( sysclock == (void *)-1  )
    {
		printf("master: Error in shmat sysclock \n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
    }

	system_data_structure = shmat( system_data_structure_id, 0, 0 );
    if ( system_data_structure == (void *)-1  )
    {
		printf("master: Error in shmat system_data_structure \n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
    }

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

	while (1)
	{
		msg.mesg_type = PARENT_QUEUE_ADDRESS;
		msg.process_id = current_process_id;

		// At random times (between 0 and 250ms), the process checks if it should terminate.
		// Make sure to do this only after a process has run for at least 1 second.
		if ((sysclock->sec >= 1 && (sysclock->nano_sec / TERMINATE_CHECK_NS < 1)) && isTerminated())
		{
			printf("Process P%d is Terminated\n", current_process_id);
			msg.action_flag = TERMINATE_FLAG;
			is_terminated = TRUE;
			msgsnd(oss_msgqueue_id, &msg, sizeof(message), 0);
			return EXIT_SUCCESS;
		}

		int resource_id = generateRandomNumber(RESOURCE_ID_RANDOM);
		is_granted = FALSE;

		if (!is_terminated && (current_resource == 0 || (isRequesting() && max_claims > current_resource)))
		{
			// Make the process request some resources.
			// It will do so by putting a request in the shared memory.
			// The request should never exceed the maximum claims minus whatever the process already has.
			msg.num_of_resources = 1; // [0, B]
			msg.action_flag = REQUEST_FLAG;
			msg.resource_id = resource_id;

			msgsnd(oss_msgqueue_id, &msg, sizeof(message), 0);
			int msg_rcv = msgrcv(user_msgqueue_id, &msg, (sizeof(message)-sizeof(long)), current_process_id, 0);
			if (msg_rcv != -1)
			{
				is_granted = msg.mesg_granted;
			}

			if (is_granted)
			{
				allocated_per_resource[resource_id] += msg.num_of_resources;
				current_resource += msg.num_of_resources;
			}

		}
		else if (!is_terminated && (allocated_per_resource[resource_id] > 0))
		{
			// The process may decide to give up resources instead of asking for them.
			// parameter giving a bound B for when a process should request (or release) a resource.
			msg.num_of_resources = allocated_per_resource[resource_id];
			msg.action_flag = RELEASE_FLAG;
			msg.resource_id = resource_id;

			msgsnd(oss_msgqueue_id, &msg, sizeof(message), 0);

			int msg_rcv = msgrcv(user_msgqueue_id, &msg, (sizeof(message)-sizeof(long)), current_process_id, 0);
			if (msg_rcv != -1)
			{
				is_granted = msg.mesg_granted;
			}

			if (is_granted)
			{
				allocated_per_resource[resource_id] -= msg.num_of_resources;
				current_resource -= msg.num_of_resources;
			}

		}
	}

	return EXIT_SUCCESS;
}
