#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include <stdint.h>

void init_tournament();
uint8_t make_prediction_tournament(uint32_t pc);
void train_tournament(uint32_t pc, uint8_t outcome);

#endif
