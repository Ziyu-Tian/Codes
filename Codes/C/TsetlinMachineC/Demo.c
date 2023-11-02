#include "MultiClassTsetlinMachine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define NUMBER_OF_EXAMPLES 4
#define NUMBER_OF_TESTING 4

int X_train[NUMBER_OF_EXAMPLES][FEATURES];
int y_train[NUMBER_OF_EXAMPLES];

int X_test[NUMBER_OF_EXAMPLES][FEATURES];
int y_test[NUMBER_OF_EXAMPLES];

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

	fp = fopen("test.csv", "r");
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
void write_file(struct MultiClassTsetlinMachine *mc_tm)
{
	FILE *fp;
	fp = fopen("BestEpoch.txt", "w");

	if (fp == NULL)
	{
		printf("Error opening\n");
		exit(EXIT_FAILURE);
	}
	for (int k = 0; k < CLASSES; k++)
	{
		// fprintf(fp, "class %d\n", k);

		for (int i = 0; i < CLAUSES; i++)
		{
			for (int j = 0; j < FEATURES; j++)
			{
				int regular_state = tm_get_state(mc_tm->tsetlin_machines[k], i, j, 0);

				fprintf(fp, "%d ", regular_state);
			} // print regular_feature
			for (int j = 0; j < FEATURES; j++)
			{
				int negated_state = tm_get_state(mc_tm->tsetlin_machines[k], i, j, 1);

				if (j == FEATURES - 1)
				{
					fprintf(fp, "%d\n", negated_state);
				}
				else
				{
					fprintf(fp, "%d ", negated_state);
				}
			} // print negated_features
		}
		// fprintf(fp, "---------------------------------\n");
	}
}

void read_test(void)
{

	FILE *fp;
	char *line = NULL;
	size_t len = 0;

	const char *s = ",";
	char *token = NULL;
	fp = fopen("test.csv", "r");
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

void load_epoch(struct MultiClassTsetlinMachine *mc_tm)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;

	const char *s = " ";
	char *token = NULL;

	fp = fopen("BestEpoch.txt", "r");
	if (fp == NULL)
	{
		printf("Error opening\n");
		exit(EXIT_FAILURE);
	}

	for (int k = 0; k < CLASSES; k++)
	{
		for (int i = 0; i < CLAUSES; i++)
		{
			getline(&line, &len, fp);

			token = strtok(line, s);
			for (int j = 0; j < FEATURES; j++)
			{
				mc_tm->tsetlin_machines[k]->ta_state[i][j][0] = round(atoi(token)); // convert input data to double
				token = strtok(NULL, s);
				// strtok should be set to NULL after each usage
			}
			for (int j = 0; j < FEATURES; j++)
			{
				mc_tm->tsetlin_machines[k]->ta_state[i][j][1] = round(atoi(token)); // convert input data to double
				token = strtok(NULL, s);
				// strtok should be set to NULL after each usage
			}
		}
	}
}

int main(void)
{
	srand(time(NULL));

	read_file();

	struct MultiClassTsetlinMachine *mc_tsetlin_machine = CreateMultiClassTsetlinMachine();

	// float average = 0.0;
	// float total_error = 0.0;
	// float accuracy = 0.0;
	float accuracy[10] = {0};
	printf("Choose the operation mode ('1' for training and '2' for testing):\n");
	int mode_select = 0;
	scanf("%d", &mode_select);

	if (mode_select == 1) // Training-mode of Tsetlin machine
	{
		float average_accuracy = 0.0;
		//int max_epoch_index = 1;
		printf("Training Start......\n");
		printf("------------------------------\n");
		float num = 0.0;
		for (int i = 0; i < 1; i++) // i = epoch number
		{
			mc_tm_initialize(mc_tsetlin_machine);
			// clock_t start_total = clock();
			
			printf("Epoch %d: \n", i + 1);
			mc_tm_fit(mc_tsetlin_machine, X_train, y_train, NUMBER_OF_EXAMPLES, 100, 15); // 200
			/*
			clock_t end_total = clock();
			double time_used = ((double) (end_total - start_total)) / CLOCKS_PER_SEC;

			printf("EPOCH %d TIME: %f\n", i+1, time_used);
			*/
			accuracy[i] = (NUMBER_OF_TESTING - mc_tm_evaluate(mc_tsetlin_machine, X_test, y_test, NUMBER_OF_TESTING)) / NUMBER_OF_TESTING;

			/*
			if (accuracy[i] > max_accuracy)
			{
				max_accuracy = accuracy[i];
				max_epoch_index = i;
				write_file(mc_tsetlin_machine);
			}
			*/
			//printf("Epoch accuracy: %f\n", accuracy[i]);
			num += accuracy[i];
		}


		average_accuracy = num ;

		printf("Average Epoch accuracy: %f\n", average_accuracy);


		// printf("The highest accuracy epoch is %d\n", max_epoch_index + 1);
	}

	else
	{
		read_test();
		for (int i = 0; i < 1; i++) // i = epoch number
		{
			printf("Loading stored epoch......\n");

			load_epoch(mc_tsetlin_machine);

			printf("Loading Successfully!\n");
			// mc_tm_initialize(mc_tsetlin_machine);
			//  clock_t start_total = clock();
			printf("Testing Epoch: \n");
			/*
			clock_t end_total = clock();
			double time_used = ((double) (end_total - start_total)) / CLOCKS_PER_SEC;

			printf("EPOCH %d TIME: %f\n", i+1, time_used);
			*/

			accuracy[i] = (NUMBER_OF_TESTING - mc_tm_evaluate(mc_tsetlin_machine, X_test, y_test, NUMBER_OF_TESTING)) / NUMBER_OF_TESTING;

			printf("Testing Average accuracy: %f\n", accuracy[i]);
		}
	}

	// printf("The highest accuracy epoch is %d\n", max_epoch_index + 1);

	return 0;
}
