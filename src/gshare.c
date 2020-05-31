#include <stdio.h>
#include "gshare.h"
#include "predictor.h"

uint32_t ghistory_records;
uint8_t *counter_table;

void add_ghrecord_gshare(uint8_t outcome)
{
  uint32_t ghistory_msk;
  
  ghistory_msk = (1 << ghistoryBits) - 1;
  ghistory_records = (ghistory_records << 1) | outcome;
  ghistory_records &= ghistory_msk;
}

void init_gshare()
{
  uint32_t num_counters;

  // data width of num of counters is the same as ghistory records
  num_counters = 1 << ghistoryBits;
  ghistory_records = 0;
  counter_table = (uint8_t *)malloc(sizeof(uint8_t) * num_counters);
  if (counter_table == NULL) {
    perror("fail to alloc counter table");
    return;
  }

  for (int i = 0; i < num_counters; i++) {
    counter_table[i] = SN;
  }
}

uint8_t make_prediction_gshare(uint32_t pc)
{
  uint32_t index;
  uint8_t counter;
  uint32_t counter_idx_msk;

  counter_idx_msk = (1 << ghistoryBits) - 1;
  index = (pc ^ ghistory_records) & counter_idx_msk;
  counter = counter_table[index];

  if (counter >= WT)
    return TAKEN;
  else
    return NOTTAKEN;
}

void train_gshare(uint32_t pc, uint8_t outcome)
{
  uint32_t index;
  uint8_t counter;
  uint32_t counter_idx_msk;

  counter_idx_msk = (1 << ghistoryBits) - 1;

  index = (pc ^ ghistory_records) & counter_idx_msk;
  counter = counter_table[index];
  
  if (outcome == TAKEN)
    counter_table[index] = counter >= ST ? counter : counter + 1;
  else
    counter_table[index] = counter <= SN ? counter : counter - 1;
  
  add_ghrecord_gshare(outcome);
}
