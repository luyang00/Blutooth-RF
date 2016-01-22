#pragma once
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

void hoop_sequence_gen(int seed,u8 * hoop_sequence,int length);
void sync_sequence_gen(int seed,u8 * sync_sequence,int length);
#ifdef __cplusplus
}
#endif
