#include "MultiClassTsetlinMachine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define NUMBER_OF_EXAMPLES 60000
#define NUMBER_OF_TESTING 10000
#define EPOCH_NUMBER 10
#define batch 12

int X_train[NUMBER_OF_EXAMPLES][FEATURES];
int y_train[NUMBER_OF_EXAMPLES];

int X_test[NUMBER_OF_EXAMPLES][FEATURES];
int y_test[NUMBER_OF_EXAMPLES];
int threshold = 5;
int sensitivity = 5;
struct Pair // (s,T) pairs
{
	int s;
	int t;
};

void read_file(void)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;

	const char *s = ",";
	char *token = NULL;

	fp = fopen("train.csv", "r");
	if (fp == NULL)
	{
		printf("Error opening\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUMBER_OF_EXAMPLES; i++)
	// 1 example per line, 1 feature (including nageted) per column
	{
		(void)getline(&line, &len, fp);

		// size_t getline(char **lineptr, size_t *n, FILE *stream);
		// **lineptr is a pointer point to a pointer to store the input text,
		// *n is the pointer points to a integer to store the text line-number
		// *stream is the pointer points to the file being processed

		token = strtok(line, s);
		for (int j = 0; j < FEATURES; j++)
		{
			X_train[i][j] = round(atoi(token)); // convert input data to double
			token = strtok(NULL, s);
			// strtok should be set to NULL after each usage
		}
		y_train[i] = round(atoi(token));
	}

	fp = fopen("test.csv", "r");
	if (fp == NULL)
	{
		printf("Error opening\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUMBER_OF_TESTING; i++)
	{
		(void)getline(&line, &len, fp);

		token = strtok(line, s);
		for (int j = 0; j < FEATURES; j++)
		{
			X_test[i][j] = round(atoi(token));
			token = strtok(NULL, s);
		}
		y_test[i] = round(atoi(token));
	}
}

int main(void)
{
	printf("Training Start......\n");
	printf("------------------------------\n");

	FILE *fp = fopen("output_data.csv", "w+");//if need another file, change the file name
    	if (fp == NULL)
       	{
        	printf("Error opening\n");
        	exit(EXIT_FAILURE);
    	}

    	fprintf(fp, "T,s,Accuracy\n");
    	fclose(fp);
        
	printf("Title Written Successfully!\n");

        struct Pair pairs[100];

	for(int i = 0; i<100; i++)
	{
		if ((i+1)%10==0)
        	{
            		pairs[i].s = sensitivity;
            		sensitivity+=5;
           		pairs[i].t = threshold; 
            		threshold=5;
       		 }
       		 else
       		 {
           		 pairs[i].s = sensitivity;
           
           		 pairs[i].t = threshold; 
            		 threshold+=5;
       		 } 
	}

	printf("(T,s) pairs setting finished!\n");
		
	
	for (int k = 0; k<100; k++ ) // loop of change hyper-paramter pair (s,T)
	{
		sensitivity = pairs[k].s;
		threshold = pairs[k].t;
		srand(time(NULL));

		read_file();

		printf("File-reading finished\n");

		struct MultiClassTsetlinMachine *mc_tsetlin_machine = CreateMultiClassTsetlinMachine();

		float accuracy[EPOCH_NUMBER] = {0};
		float max_accuracy = 0.0;
		float sum = 0.0;
			
		for (int i = 0; i < EPOCH_NUMBER; i++) // i = epoch number
		{
			mc_tm_initialize(mc_tsetlin_machine);	
			mc_tm_fit(mc_tsetlin_machine, X_train, y_train, NUMBER_OF_EXAMPLES, batch, sensitivity); // 200
		
			accuracy[i] = (NUMBER_OF_TESTING - mc_tm_evaluate(mc_tsetlin_machine, X_test, y_test, NUMBER_OF_TESTING)) / NUMBER_OF_TESTING;
			
			sum += accuracy[i];

			printf("Epoch %d finished\n",i);

		}

		max_accuracy = sum / EPOCH_NUMBER;

		printf("T:%d s:%d Max accuracy: %f\n", threshold, sensitivity, max_accuracy);

		FILE *fp = fopen("output_data.csv", "a+");//if need another file, change the file name
    		if (fp == NULL)
	       	{
        		printf("Error opening\n");
        		exit(EXIT_FAILURE);
    		}

    		fprintf(fp, "%d,%d,%f\n", threshold, sensitivity, max_accuracy);

    		fclose(fp);
	

	}

	return 0;
}
