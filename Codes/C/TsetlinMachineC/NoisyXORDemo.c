#include "MultiClassTsetlinMachine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define NUMBER_OF_EXAMPLES 5
#define NUMBER_OF_TESTING 50

int X_train[NUMBER_OF_EXAMPLES][FEATURES];
int y_train[NUMBER_OF_EXAMPLES];

int X_test[NUMBER_OF_EXAMPLES][FEATURES];
int y_test[NUMBER_OF_EXAMPLES];

void read_file(void)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;

	const char *s = " ";
	char *token = NULL;

	fp = fopen("NoisyXORTrainingData.txt", "r");
	if (fp == NULL)
	{
		printf("Error opening\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUMBER_OF_EXAMPLES; i++)
	// 1 example per line, 1 feature (including nageted) per column
	{
		getline(&line, &len, fp);

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

	fp = fopen("NoisyXORTestData.txt", "r");
	if (fp == NULL)
	{
		printf("Error opening\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUMBER_OF_TESTING; i++)
	{
		getline(&line, &len, fp);

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
	srand(time(NULL));

	read_file();

	struct MultiClassTsetlinMachine *mc_tsetlin_machine = CreateMultiClassTsetlinMachine();

	// float average = 0.0;
	float total_error = 0.0;
	float accuracy = 0.0;
	for (int i = 0; i < 100; i++)
	{
		mc_tm_initialize(mc_tsetlin_machine);
		// clock_t start_total = clock();
		printf("Epoch %d\n", i + 1);
		printf("Training Session:/n");
		mc_tm_fit(mc_tsetlin_machine, X_train, y_train, NUMBER_OF_EXAMPLES, 200, 3);
		/*
		clock_t end_total = clock();
		double time_used = ((double) (end_total - start_total)) / CLOCKS_PER_SEC;

		printf("EPOCH %d TIME: %f\n", i+1, time_used);
		*/
		// average += mc_tm_evaluate(mc_tsetlin_machine, X_test, y_test, NUMBER_OF_EXAMPLES);

		// printf("Average accuracy: %f\n", average/(i+1));
		total_error += total_error;

		// mc_tm_evaluate(mc_tsetlin_machine, X_test, y_test, NUMBER_OF_EXAMPLES);
	}
	accuracy = (500 - mc_tm_evaluate(mc_tsetlin_machine, X_test, y_test, NUMBER_OF_TESTING)) / 500;
	printf("Average accuracy: %f\n", accuracy);
	return 0;
}
