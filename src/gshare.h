#ifndef GSHARE_H
#define GSHARE_H

#include <stdint.h>

void init_gshare();
uint8_t make_prediction_gshare(uint32_t pc);
void train_gshare(uint32_t pc, uint8_t outcome);

#endif
