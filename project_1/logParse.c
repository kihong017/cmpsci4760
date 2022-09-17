/*
 *  Author: Daniel Park
 *  Class: CMP_SCI_4760_002
 *  Project 1
 *  The goal of this project is to become familiar
 *  with the environment in opsys while practicing system calls.
 *  This project also requires demonstrating proficiency in the use of
 *  perror and getopt in this submission.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void help() {
	printf("The inputs for this program are: \n");
	printf("fileName (output) Series of numbers (ex: ./mathwait tempfile.txt 32 9 10 -13) \n");
}

int main(int argc, char** argv)
{
	char* fileName = "input.dat";
	char* outputFileName = "output.dat";

	if (argc == 2) {
		int option;
		while ( (option = getopt(argc, argv, "hi:o:")) != -1 ) {
		switch(option) {
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
	}
}
