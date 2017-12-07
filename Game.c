#include <stdint.h>
#include "Game.h"
#include "tm4c123gh6pm.h"
#include "LCD.h"

uint16_t score,life;
shield* shields[NUMSHIELDS];
shield Shield1,Shield2,Shield3;

void initGame() {
	int i;
	score = 0;
	life = 10;
	shields[0] = &Shield1;
	shields[1] = &Shield2;
	shields[2] = &Shield3;
	for(i = 0;i < NUMSHIELDS;i++) {
		shields[i]->x = 19+i*(16+20);	// Used to define equal spacing between shields
		shields[i]->y = 85;
		shields[i]->life = 10;
		BSP_LCD_FillRect(shields[i]->x,shields[i]->y,shield_w,shield_h,LCD_GREEN);
	}
}
