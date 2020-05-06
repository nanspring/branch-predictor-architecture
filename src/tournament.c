#include <stdio.h>
#include "tournament.h"
#include "predictor.h"

uint32_t ghr; // global histroy record
uint8_t *gpt; // global prediction table
uint8_t *cpt; // choice prediction table
uint32_t *lht; // local history table
uint8_t *lpt; // local prediction table

uint32_t gpt_index;
uint32_t lht_index;
uint32_t lpt_index;

uint8_t gpt_prediction;
uint8_t lpt_prediction;

void add_ghr(uint8_t outcome)
{
  uint32_t ghistory_msk;
  
  ghistory_msk = (1 << ghistoryBits) - 1;
  ghr = (ghr << 1) | outcome;
  ghr &= ghistory_msk;
}

void init_tournament(){
    uint32_t gpt_num_counters;
    uint32_t lhr_num_counters;

    gpt_num_counters = 1 << ghistoryBits;  //2^(ghistoryBits)
    lhr_num_counters = 1 << pcIndexBits;  //2^(pcIndexBits)

    ghr = 0; // ghr = 00000000
    gpt = (uint8_t *)malloc(sizeof(uint8_t) * gpt_num_counters);
    cpt = (uint8_t *)malloc(sizeof(uint8_t) * gpt_num_counters);
    lht = (uint32_t *)malloc(sizeof(uint32_t) * lhr_num_counters);
    lpt = (uint8_t *)malloc(sizeof(uint8_t) * lhr_num_counters);
    if (gpt == NULL) {
        return;
    }

    for (int i = 0; i < gpt_num_counters; i++) {
        gpt[i] = SN;
        cpt[i] = SN;
        
    }

    if (lpt == NULL) {
        return;
    }
    for (int i = 0; i < lhr_num_counters; i++) {
        lht[i] = 0;
        lpt[i] = SN;
    }

}

uint8_t mux(uint8_t ght_pred, uint8_t lpt_pred, uint8_t ght_index){
    uint8_t prediction;

    if(ght_pred != lpt_pred){
        if(cpt[ght_index] >= 2){ // if choser lean more towards ght decision
            return ght_pred;
        }else{
            return lpt_pred; // if choser lean more toward lpt decision
        }
    }
    return ght_pred; // if glocal and local predict the same, return the prediction
}

uint8_t make_prediction_tournament(uint32_t pc)
{

  uint8_t ght_counter;
  uint8_t lpt_counter;

  uint32_t ght_counter_idx_msk;
  uint32_t lht_counter_idx_msk;


  ght_counter_idx_msk = (1 << ghistoryBits) - 1;
  lht_counter_idx_msk = (1 << pcIndexBits) - 1;

  gpt_index = ghr & ght_counter_idx_msk;
  lht_index = pc & lht_counter_idx_msk;
  lpt_index = lht[lht_index];

  ght_counter = gpt[gpt_index];
  lpt_counter = lpt[lpt_index];

  if (ght_counter >= WT){
    gpt_prediction =  TAKEN;
  }  
  else{
    gpt_prediction = NOTTAKEN;
  }

  if (lpt_counter >= WT){
    lpt_prediction =  TAKEN;
  }  
  else{
    lpt_prediction = NOTTAKEN;
  }

  return mux(gpt_prediction,lpt_prediction,gpt_index); // make decision

}

void update_cpt(uint8_t outcome){
    uint8_t cpt_pred = cpt[gpt_index];
    if(gpt_prediction == outcome){
        cpt[gpt_index] = cpt_pred >= 3 ? cpt_pred : cpt_pred + 1;
    }else{
        cpt[gpt_index] = cpt_pred <= 0 ? cpt_pred : cpt_pred - 1;
    }
    
}

void update_gpt(uint8_t outcome, uint8_t gpt_counter){
    if (outcome == TAKEN)
        gpt[gpt_index] = gpt_counter >= ST ? gpt_counter : gpt_counter + 1;
    else
        gpt[gpt_index] = gpt_counter <= SN ? gpt_counter : gpt_counter - 1;
}

void update_lpt(uint8_t outcome, uint8_t lpt_counter){
    if(outcome == TAKEN){
        lpt[lpt_index] = lpt_counter >= ST ? lpt_counter : lpt_counter + 1;
    }else{
        lpt[lpt_index] = lpt_counter <= SN ? lpt_counter : lpt_counter - 1;
    }
}

void add_lhrecord_to_lht(uint8_t outcome){
    uint32_t lhistory_msk;
    uint32_t lhistory;
  
    lhistory_msk = (1 << lhistoryBits) - 1;
    lhistory = lht[lht_index];

    lht[lht_index] = (lhistory << 1) | outcome;
    lht[lht_index] &= lhistory_msk;
}

void train_tournament(uint32_t pc, uint8_t outcome){
  
  uint8_t gpt_counter;
  uint8_t lpt_counter;

  gpt_counter = gpt[gpt_index];
  lpt_counter = lpt[lpt_index];
  
  if(gpt_prediction != lpt_prediction){
      update_cpt(outcome);
  }
  update_gpt(outcome, gpt_counter);
  update_lpt(outcome, lpt_counter);
  add_lhrecord_to_lht(outcome);
  add_ghr(outcome);
  
}