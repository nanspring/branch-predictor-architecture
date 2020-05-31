/*
 * A custom branch predictor based on the Perceptron branch predictor
 */

#include "custom.h"
#include "predictor.h"
#include <stdio.h>

uint64_t ghistory_records;  // maximum 64 global history records
int **perceptron_table;
int last_output;

void add_ghrecord_custom(uint8_t outcome)
{
	uint32_t ghistory_msk;
  
  ghistory_msk = (1 << ghistoryBits) - 1;
  ghistory_records = (ghistory_records << 1) | outcome;
  ghistory_records &= ghistory_msk;
}

void init_custom()
{
	ghistory_records = 0;
	perceptron_table = (int **)malloc(sizeof(int *) * numPerceptrons);
	if (perceptron_table == NULL) {
		perror("fail to alloc perceptron table");
		return;
	}

	for (int i = 0; i < numPerceptrons; i++) {
		perceptron_table[i] = (int *)malloc(sizeof(int) * (1 + ghistoryBits));
		if (perceptron_table[i] == NULL) {
			perror("fail to alloc perceptron");
			return;
		}
		for (int j = 0; j <= ghistoryBits; j++) {
			perceptron_table[i][j] = 0;
		}
	}
}

uint8_t make_prediction_custom(uint32_t pc)
{
	int *perceptron;
	int input;
	int output;

	perceptron = perceptron_table[pc % numPerceptrons];
	output = perceptron[0];

	for (int i = 0; i < ghistoryBits; i++) {
		input = (((1 << i) & ghistory_records) >> i) > 0 ? 1 : -1;
		output += perceptron[i + 1] * input;
	}

	last_output = output;

	if (output < 0)
		return NOTTAKEN;
	else
		return TAKEN;
}

static inline int sign(int output)
{
	if (output >= 0)
		return 1;
	else
		return -1;
}

void train_custom(uint32_t pc, uint8_t outcome)
{
	int *perceptron;
	int input;
	int t;

	if (outcome == TAKEN)
		t = 1;
	else
		t = -1;

	perceptron = perceptron_table[pc & numPerceptrons];
	if (sign(last_output)  != t || abs(last_output) <= threshold) {
		perceptron[0] += t;
		for (int i = 0; i < ghistoryBits; i++) {
			input = (((1 << i) & ghistory_records) >> i) > 0 ? 1 : -1;
			perceptron[i + 1] += input * t;
		}
	}

	add_ghrecord_custom(outcome);
}
