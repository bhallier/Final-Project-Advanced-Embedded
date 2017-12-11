#ifndef __Game_H__
#define __Game_H__
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "LCD.h"

#define NUMSHIELDS 3
#define shield_h 8
#define shield_w 16
#define bullet_h 3
#define bullet_w 1

typedef struct shield{
	uint16_t position[2];
	uint16_t life;	// Points to array for shield health values
	uint8_t notHit;
} shield;

/*typedef struct bullet{
	uint16_t position[2];
	uint16_t old_position[2];
	uint8_t hit;
} bullet;*/

typedef struct player{
	uint16_t position[2];
	uint16_t old_position[2];
	uint8_t hit;
} player;

extern uint16_t score,life, activeBullets;
extern shield Shield1,Shield2,Shield3;
extern shield* shields[NUMSHIELDS];	// Contains the objects for each shield
//extern bullet* bullets[10];	// Contains the objects for each shield

void initGame();
void fireBullet(int16_t x, int16_t y);
void drawPlayer(uint16_t*);

#endif