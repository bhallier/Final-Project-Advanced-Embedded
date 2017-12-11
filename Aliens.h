#ifndef __Aliens_H__
#define __Aliens_H__
#include <stdint.h>
#include "tm4c123gh6pm.h"

#define NUMALIENSblock 18
#define NUMALIENSthread 6
#define alien_h 10
#define alien_w 10
#define directionStep 5

typedef struct alienObj{
	uint16_t position[2];
	uint16_t old_position[2];
	uint16_t color;	// Points to array for shield health values
	uint8_t active;
	uint8_t hit;
	uint8_t hitGround;
	uint8_t score;
	uint8_t updated;
} alien;

extern uint16_t life;
extern alien alienArray[NUMALIENSblock];	// Contains the object for each alien
extern uint16_t alien_type; // can be 0,1 or 2 for three different types of aliens

void initAlien();
void alienThread();
void alienErase(alien*);

#endif