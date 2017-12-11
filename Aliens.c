#include "Aliens.h"
#include "OS.h"
#include "LCD.h"

alien alienArray[NUMALIENSblock];
uint8_t activeAlien=0; // if this value is zero, that means whole block is destroyed and a new one should respawn
uint16_t alien_type = 0; // can be 0,1 or 2 for three different types of aliens
uint16_t alienColors[3] = {LCD_YELLOW, LCD_BLUE, LCD_RED};
uint16_t alienScores[3] = {10, 20, 30};

uint16_t initPosY[3] = {30,15,0};
uint16_t initPosX[NUMALIENSthread] = {5,20,35,50,65,80};
uint16_t alienLayer = 0;

uint8_t wallPosX[2] = {5,109};
uint16_t moveLayer = 0;
uint8_t hitRightWall = 0;
uint8_t hitLeftWall = 0;
int16_t direction = directionStep;  // +10 or -10
int16_t goDown = 0;
uint32_t currentTime = 0;

extern void GameOver();

void initAlien(){
	uint8_t i, alienIndex;
	alienLayer = 0;
	while(alienLayer<3){
		for(i=0; i<NUMALIENSthread; i++){
			alienIndex = alienLayer*6 + i;
			alienArray[alienIndex].position[0] = initPosX[i];
			alienArray[alienIndex].position[1] = initPosY[alienLayer];
			alienArray[alienIndex].old_position[0] = initPosX[i];
			alienArray[alienIndex].old_position[1] = initPosY[alienLayer];
			
			alienArray[alienIndex].color = alienColors[alien_type];
			alienArray[alienIndex].score = alienScores[alien_type];
			alienArray[alienIndex].hit = 0;
			alienArray[alienIndex].hitGround = 0;
			alienArray[alienIndex].active = 1;
			alienArray[alienIndex].updated = 1;
			activeAlien++;
		}
		alienLayer++;
	}
	currentTime = OS_MsTime();
}

extern Sema4Type LCDFree;
void alienErase(alien* thisCube) {
	uint16_t x,y;
	OS_bWait(&LCDFree);
	x = thisCube->position[0];
	y = thisCube->position[1];
	BSP_LCD_FillRect(x,y,alien_w,alien_h,LCD_BLACK);
	OS_bSignal(&LCDFree);
}

void alienMovement(){
	uint8_t i, alienIndex;
	if(goDown){
		for(i=0; i<NUMALIENSblock; i++){
			alienArray[i].old_position[0] = alienArray[i].position[0];
			alienArray[i].old_position[1] = alienArray[i].position[1];		
			alienArray[i].position[1] += directionStep;
			alienArray[i].updated = 1;
			if(alienArray[i].position[1] == 80 && alienArray[i].active){
				life = 0;
			}
				
		}
		goDown = 0;
		return;
	}
	for(i=0; i<NUMALIENSthread; i++){
		alienIndex = moveLayer*6 + i;
		alienArray[alienIndex].old_position[0] = alienArray[alienIndex].position[0];
		alienArray[alienIndex].old_position[1] = alienArray[alienIndex].position[1];		
		alienArray[alienIndex].position[0] += direction;
		alienArray[alienIndex].updated = 1;
	}
	if(alienArray[moveLayer*6].position[0] <= wallPosX[0])
		hitLeftWall++;
	if(alienArray[moveLayer*6+5].position[0] >= wallPosX[1])
		hitRightWall++;
	
	if(hitLeftWall==3){
		goDown = 1;
		direction = directionStep;
		hitLeftWall = 0;
	}
	
	if(hitRightWall==3){
		goDown = 1;
		direction = -1*directionStep;
		hitRightWall = 0;
	}

	moveLayer++;
	if(moveLayer==3)
		moveLayer=0;
}

void alienThread(){
	uint8_t i;
	initAlien();
	while(life){
		if((OS_MsTime()-currentTime) > 250){
			currentTime = OS_MsTime();
			alienMovement();
		}
		OS_Suspend();
	}
	//alien_type++;
	//if(alien_type>2)
	//	alien_type = 0;
	OS_Kill();
}
