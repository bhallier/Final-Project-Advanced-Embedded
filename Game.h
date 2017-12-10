#ifndef __Game_H__
#define __Game_H__
#include <stdint.h>
#include "tm4c123gh6pm.h"

#define NUMSHIELDS 3
#define shield_h 8
#define shield_w 16

typedef struct shield{
	uint16_t x;
	uint16_t y;
	uint16_t life;	// Points to array for shield health values
} shield;

/*
tyepdef struct {

} player;

typedef struct {
	
} alien_row;
*/

extern uint16_t score,life;
extern shield Shield1,Shield2,Shield3;
extern shield* shields[NUMSHIELDS];	// Contains the objects for each shield

void initGame();

#endif