// Lab3.c
// Runs on LM4F120/TM4C123
// Real Time Operating System for Lab 3

// Jonathan W. Valvano 2/20/17, valvano@mail.utexas.edu
// Modified by Sile Shu 10/4/17, ss5de@virginia.edu
// You may use, edit, run or distribute this file 
// You are free to change the syntax/organization of this file

#include <stdint.h>
#include <string.h> 
#include "OS.h"
#include "tm4c123gh6pm.h"
#include "LCD.h"
#include "FIFO.h"
#include "joystick.h"
#include "Game.h"
#include "Aliens.h"
#include "graphics.h"
#include "bullet.h"

uint16_t ssBulletPos;
uint8_t ssBulletFired;
uint16_t shieldColors[4] = {LCD_BLACK, LCD_RED, LCD_YELLOW, LCD_GREEN};
uint16_t ssPosition[2];


//constants
#define BGCOLOR     LCD_BLACK
#define AXISCOLOR   LCD_ORANGE
#define MAGCOLOR    LCD_YELLOW
#define EWMACOLOR   LCD_CYAN
#define SOUNDCOLOR  LCD_CYAN
#define LIGHTCOLOR  LCD_LIGHTGREEN
#define TOPTXTCOLOR LCD_WHITE
#define TOPNUMCOLOR LCD_ORANGE
#define CROSSSIZE            5
#define PERIOD               4000000   // DAS 20Hz sampling period in system time units
#define PSEUDOPERIOD         8000000
#define LIFETIME             1000
#define RUNLENGTH            600 // 30 seconds run length

void GameOver();


extern Sema4Type LCDFree;
extern player player1;
uint16_t origin[2]; // the original ADC value of x,y if the joystick is not touched
int16_t x = 63;  // horizontal position of the crosshair, initially 63
int16_t y = 63;  // vertical position of the crosshair, initially 63
int16_t prevx,prevy;
uint8_t select;  // joystick push
uint8_t area[2];
uint32_t PseudoCount;

unsigned long NumCreated;   // number of foreground threads created
unsigned long NumSamples;   // incremented every ADC sample, in Producer
unsigned long UpdateWork;   // incremented every update on position values
unsigned long Calculation;  // incremented every cube number calculation
unsigned long DisplayCount; // incremented every time the Display thread prints on LCD 
unsigned long ConsumerCount;// incremented every time the Consumer thread prints on LCD

//---------------------User debugging-----------------------
unsigned long DataLost;     // data sent by Producer, but not received by Consumer

#define PE0  (*((volatile unsigned long *)0x40024004))
#define PE1  (*((volatile unsigned long *)0x40024008))
#define PE2  (*((volatile unsigned long *)0x40024010))
#define PE3  (*((volatile unsigned long *)0x40024020))

void creatAlienImage(){
	int i=0;
	for(i=0;i<198;i=i+2){
		alienRed[i/2] = (alienRedInitial[198-i]<<8)+alienRedInitial[198-i+1];
		alienGreen[i/2] = (alienGreenInitial[198-i]<<8)+alienGreenInitial[198-i+1];
		alienYellow[i/2] = (alienYellowInitial[198-i]<<8)+alienYellowInitial[198-i+1];
	}
	for(i=0;i<448;i=i+2)
		spaceShip[i/2] = (spaceShipInitial[448-i]<<8)+spaceShipInitial[448-i+1];
}

void PortE_Init(void){ unsigned long volatile delay;
  SYSCTL_RCGCGPIO_R |= 0x10;       // activate port E
  while((SYSCTL_RCGCGPIO_R & 0x10) == 0){}    
  GPIO_PORTE_DIR_R |= 0x0F;    // make PE3-0 output heartbeats
  GPIO_PORTE_AFSEL_R &= ~0x0F;   // disable alt funct on PE3-0
  GPIO_PORTE_DEN_R |= 0x0F;     // enable digital I/O on PE3-0
  GPIO_PORTE_PCTL_R = ~0x0000FFFF;
  GPIO_PORTE_AMSEL_R &= ~0x0F;;      // disable analog functionality on PF
}

void Device_Init(void){
	BSP_LCD_OutputInit();
	BSP_Joystick_Init();
}
//------------------Task 1--------------------------------
// background thread executed at 20 Hz
//******** Producer *************** 
int UpdatePosition(uint16_t rawx, uint16_t rawy, jsDataType* data){
	if (rawx > origin[0]){
		x = x + ((rawx - origin[0]) >> 7);
	}
	else{
		x = x - ((origin[0] - rawx) >> 7);
	}
	if (rawy < origin[1]){
		y = y + ((origin[1] - rawy) >> 7);
	}
	else{
		y = y - ((rawy - origin[1]) >> 7);
	}
	if (x > 127){
		x = 127;}
	if (x < 0){
		x = 0;}
	if (y > 112 - CROSSSIZE){
		y = 112 - CROSSSIZE;}
	if (y < 0){
		y = 0;}
	data->x = x; data->y = y;
	return 1;
}

void Producer(void){
	uint16_t rawX,rawY; // raw adc value
	uint8_t select;
	jsDataType data;
	BSP_Joystick_Input(&rawX,&rawY,&select);
	UpdateWork += UpdatePosition(rawX,rawY,&data); // calculation work
	NumSamples++;              // number of samples
	if(JsFifo_Put(data) == 0){ // send to consumer
		DataLost++;
	}
}

//--------------end of Task 1-----------------------------

//------------------Task 2--------------------------------
// background thread executes with SW1 button
// one foreground task created with button push
// foreground treads run for 2 sec and die
// ***********ButtonWork*************
void ButtonWork(void){
	
}

//************SW1Push*************
// Called when SW1 Button pushed
// Adds another foreground task
// background threads execute once and return
void SW1Push(void){
  if(OS_MsTime() > 20 ){ // debounce
    if(OS_AddThread(&ButtonWork,128,4)){
      NumCreated++; 
    }
    OS_ClearMsTime();  // at least 20ms between touches
  }
}

//--------------end of Task 2-----------------------------

//------------------Task 3--------------------------------

//******** Consumer *************** 
// foreground thread, accepts data from producer
// Display crosshair and its positions
// inputs:  none
// outputs: none
void Consumer(void){
	while(life){
		jsDataType data;
		JsFifo_Get(&data);
		ConsumerCount++;
		if(data.x > 127-15) {
				data.x = 127-15;
		}
		player1.position[0] = data.x;
		ssPosition[0] = player1.position[0];
		ssBulletPos = data.x;
	}
  OS_Kill();  // done
}


//--------------end of Task 3-----------------------------

//------------------Task 4--------------------------------
// foreground thread that runs without waiting or sleeping
// it executes some calculation related to the position of crosshair 
//******** CubeNumCalc *************** 
// foreground thread, calculates the virtual cube number for the crosshair
// never blocks, never sleeps, never dies
// inputs:  none
// outputs: none

void CubeNumCalc(void){ 
	uint16_t CurrentX,CurrentY;
  while(life) {
		CurrentX = x; CurrentY = y;
		area[0] = CurrentX / 22;
		area[1] = CurrentY / 20;
		Calculation++;
  }
	OS_Kill();
}

//************ PeriodicUpdater *************** 
// background thread, do some pseudo works to test if you can add multiple periodic threads
// inputs:  none
// outputs: none
void PeriodicUpdater(void){
	PseudoCount++;
}

//************ Display *************** 
// foreground thread, do some pseudo works to test if you can add multiple periodic threads
// inputs:  none
// outputs: none

void Display(void){
	uint8_t i=0;
	while(life){
		OS_bWait(&LCDFree);
		for(i=0; i<NUMALIENSblock; i++){
			if(alienArray[i].active){
				if(alienArray[i].updated){
					BSP_LCD_FillRect(alienArray[i].old_position[0],alienArray[i].old_position[1],alien_w,alien_h,LCD_BLACK);
				//BSP_LCD_FillRect(alienArray[i].position[0],alienArray[i].position[1],alien_w,alien_h,alienArray[i].color);
					alienArray[i].updated = 0;
				switch(alien_type){
					case 0:
						BSP_LCD_DrawBitmap(alienArray[i].position[0],alienArray[i].position[1]+alien_h-1, alienGreen, alien_w,alien_h);
						break;
					case 1:
						BSP_LCD_DrawBitmap(alienArray[i].position[0],alienArray[i].position[1]+alien_h-1, alienYellow, alien_w,alien_h);
						break;
					case 2:
						BSP_LCD_DrawBitmap(alienArray[i].position[0],alienArray[i].position[1]+alien_h-1, alienRed, alien_w,alien_h);
						break;
				}
				}
		}
		}
		for(i = 0;i < NUMSHIELDS;i++) {
			if(shields[i]->notHit){
				BSP_LCD_FillRect(shields[i]->position[0],shields[i]->position[1],shield_w,shield_h,shieldColors[shields[i]->life]);
				if(shields[i]->life == 0)
					shields[i]->notHit = 0;
			}
 		}
		//////////////////////////////
		/*for(i = 0;i < activeBullets;i++) {
 			BSP_LCD_FillRect(bullets[i]->position[0],bullets[i]->position[1],bullet_w,bullet_h,LCD_RED);
 		}*/
		
		if(spaceShipBullet.updated){
			if(spaceShipBullet.active){
				BSP_LCD_FillRect(spaceShipBullet.position[0],spaceShipBullet.position[1],2,3,spaceShipBullet.color);
				BSP_LCD_FillRect(spaceShipBullet.old_position[0],spaceShipBullet.old_position[1],2,3,LCD_BLACK);
				spaceShipBullet.old_position[1] = spaceShipBullet.position[1];
				spaceShipBullet.updated = 0;
			}
		}
		if(alienBullet.updated){
			if(alienBullet.active){
				BSP_LCD_FillRect(alienBullet.position[0],alienBullet.position[1],2,3,alienBullet.color);
				BSP_LCD_FillRect(alienBullet.old_position[0],alienBullet.old_position[1],2,3,LCD_BLACK);
				alienBullet.old_position[1] = alienBullet.position[1];
				alienBullet.updated = 0;
			}
		}
		//////////////////////////////
		drawPlayer(spaceShip);
		BSP_LCD_Message(1,5,0,"Life: ",life);
		BSP_LCD_Message(1,5,10,"Score: ",score);
		OS_bSignal(&LCDFree);
		OS_Suspend();
	}
	GameOver();
  OS_Kill();  // done
}

//--------------end of Task 6-----------------------------

//------------------Task 7--------------------------------
// background thread executes with button2
// one foreground task created with button push
// ***********ButtonWork2*************
void Restart(void){
	uint32_t StartTime,CurrentTime,ElapsedTime;
	NumSamples = RUNLENGTH; // first kill the foreground threads
	OS_Sleep(50); // wait
	StartTime = OS_MsTime();
	ElapsedTime = 0;
	OS_bWait(&LCDFree);
	BSP_LCD_FillScreen(BGCOLOR);
	while (ElapsedTime < 500){
		CurrentTime = OS_MsTime();
		ElapsedTime = CurrentTime - StartTime;
		BSP_LCD_DrawString(5,6,"Restarting",LCD_WHITE);
	}
	BSP_LCD_FillScreen(BGCOLOR);
	OS_bSignal(&LCDFree);
	// restart
	DataLost = 0;        // lost data between producer and consumer
  NumSamples = 0;
  UpdateWork = 0;

	x = 63; y = 63;
	NumCreated += OS_AddThread(&Consumer,128,1); 
	NumCreated += OS_AddThread(&Display,128,3);
  OS_Kill();  // done, OS does not return from a Kill
} 

//************SW1Push*************
// Called when Button2 pushed
// Adds another foreground task
// background threads execute once and return
void SW2Push(void){
	//fireBullet(x,y);
	initSSBullet();
	ssBulletFired = 1;
	//OS_Kill();  // done, OS does not return from a Kill
  }

void CrossHair_Init(void){
	BSP_LCD_FillScreen(BGCOLOR);
	BSP_Joystick_Input(&origin[0],&origin[1],&select);
}

void GameOver(){
	BSP_LCD_FillRect(20,56,92,24,LCD_BLACK);
	BSP_LCD_DrawString(4,6,"GAME OVER",LCD_WHITE);
	BSP_LCD_Message(1,0,4,"Final Score: ",score);
}
int main(void){ 
	creatAlienImage();
  OS_Init();           // initialize, disable interrupts
	Device_Init();
  CrossHair_Init();
  DataLost = 0;        // lost data between producer and consumer

	initSSBullet();
	initAlBullet();
//********initialize communication channels
  JsFifo_Init();
	initGame();
//*******attach background tasks***********
  OS_AddSW1Task(&SW1Push,4);
	OS_AddSW2Task(&SW2Push,4);
  OS_AddPeriodicThread(&Producer,PERIOD,3); // 2 kHz real time sampling of PD3
	//OS_AddPeriodicThread(&PeriodicUpdater,PSEUDOPERIOD,3);
	
  NumCreated = 0 ;
// create initial foreground threads
  //NumCreated += OS_AddThread(&Interpreter,128,2); 
  NumCreated += OS_AddThread(&Consumer,128,1); 
	NumCreated += OS_AddThread(&CubeNumCalc,128,3); 
	NumCreated += OS_AddThread(&Display,128,2);
	NumCreated += OS_AddThread(&bulletThread,128,3);
	
	NumCreated +=OS_AddThread(&alienThread,128,3);
	NumCreated +=OS_AddThread(&collisionThread,128,3);
 
  OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
	return 0;            // this never executes
}
