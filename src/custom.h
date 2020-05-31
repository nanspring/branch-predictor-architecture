#ifndef CUSTOM_H
#define CUSTOM_H

#include <stdint.h>

void init_custom();
uint8_t make_prediction_custom(uint32_t pc);
void train_custom(uint32_t pc, uint8_t outcome);

#endif
