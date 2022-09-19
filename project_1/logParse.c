/*
 *  Author: Daniel Park
 *  Class: CMP_SCI_4760_002
 *  Project 1
 *  9 / 18 / 2022
 *  The goal of this project is to become familiar
 *  with the environment in opsys while practicing system calls.
 *  This project also requires demonstrating proficiency in the use of
 *  perror and getopt in this submission.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void help();
void protocolHandler(FILE* inputFilePointer, FILE* outputFilePointer);
void childProcess(FILE* inputFilePointer, FILE* outputFilePointer);

void help()
{
	printf("The inputs for this program are: \n");
	printf("-h  : To see help message \n");
	printf("-i inputfilename  : To set the input file name  (Default: input.dat) \n");
	printf("-o outputfilename : To set the output file name (Default: output.dat) \n");
}

void protocolHandler(FILE* inputFilePointer, FILE* outputFilePointer)
{
	char line[250];
	int numberOfChild = 0;
	int parentPid     = 0;
	int childPids[numberOfChild];

	fgets(line, sizeof(line), inputFilePointer);
	numberOfChild = atoi(line);

	int i;
	for (i = 0; i < numberOfChild; i++)
	{
		pid_t childPid = fork();
		if (childPid == 0)
		{
			childProcess(inputFilePointer, outputFilePointer);
			exit(EXIT_SUCCESS);
		}
		else
		{
			wait(NULL);
			if (i == 0)
			{
				parentPid = getpid();
			}

			// This code will jump two lines, so that the two lines after
			// what was processsed will be read
            if (!feof(inputFilePointer))
            {
                fgets(line, sizeof(line), inputFilePointer);
                fgets(line, sizeof(line), inputFilePointer);
            }
			printf("Child: %d\n", childPid);
			childPids[i] = childPid;
		}
	}

	fprintf(outputFilePointer, "All children were: ");
	for (i = 0; i < numberOfChild; i++)
	{
		fprintf(outputFilePointer, "%d ", childPids[i]);
	}
	fprintf(outputFilePointer, "\nParent PID: %d \n", parentPid);

	return;
}

void childProcess(FILE* inputFilePointer, FILE* outputFilePointer)
{
	char line[250];
	int* childArray     = 0;
	char *numberToRead  = 0;
	int numberOfNumbers = 0;

	// Reading the number of integers, creating an array
	fgets(line, sizeof(line), inputFilePointer);
	numberOfNumbers = atoi(line);
	childArray = malloc(sizeof(int) * numberOfNumbers);

	// Reading the integers and put them in the array
	fgets(line, sizeof(line), inputFilePointer);
	numberToRead = strtok(line, " ");

	int i = 0;
	while (numberToRead != NULL)
	{
		childArray[i] = atoi(numberToRead);
		i++;
		if (i > numberOfNumbers)
		{
			printf("Number of integer on a line exceeds the expected number\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			numberToRead = strtok(NULL, " ");
		}
	}

	fprintf(outputFilePointer, "%d: ", getpid());
	for (i = numberOfNumbers-1; i >= 0; i--)
	{
		fprintf(outputFilePointer, "%d ", childArray[i]);
	}
	fprintf(outputFilePointer, "\n");

	free(childArray);
}

int main(int argc, char** argv)
{
	char* fileName = "input.dat";
	char* outputFileName = "output.dat";
	char perrorOutput[100];

	strcpy(perrorOutput, argv[0]);
	strcat(perrorOutput, ": Error: ");

	int option;
	while ( (option = getopt(argc, argv, "hi:o:")) != -1 )
	{
		switch(option)
		{
			case 'h':
				help();
				return EXIT_SUCCESS;
				break;
			case 'i':
				fileName = optarg;
				break;
			case 'o':
				outputFileName = optarg;
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

	FILE* inputFilePointer;
	FILE* outputFilePointer;

	inputFilePointer = fopen(fileName, "r");
	outputFilePointer = fopen(outputFileName, "a+");

	if (inputFilePointer == NULL) {
		printf("File does not exist or not readable\n");
		perror(perrorOutput);
		exit(EXIT_FAILURE);
	 }

	protocolHandler(inputFilePointer, outputFilePointer);

	fclose(inputFilePointer);
	fclose(outputFilePointer);

	return EXIT_SUCCESS;
}
