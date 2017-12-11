#ifndef __Bullet_H__
#define __Bullet_H__
#include <stdint.h>
#include "tm4c123gh6pm.h"



typedef struct bulletObj{
	int16_t position[2];
	int16_t old_position[2];
	uint16_t color;
	uint8_t active;
	uint8_t hit;
	uint32_t CurrentTime;
}bullet;

extern bullet spaceShipBullet, alienBullet;
extern uint16_t ssBulletPos;
extern uint8_t ssBulletFired;

void initSSBullet();
void initAlBullet();
void bulletThread();
void collisionThread();

#endif