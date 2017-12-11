#include "bullet.h"
#include "OS.h"
#include "LCD.h"
#include "Aliens.h"
#include "Game.h"

bullet spaceShipBullet, alienBullet;

void initSSBullet(){
	ssBulletFired = 1;
	spaceShipBullet.position[0] = -10;
	spaceShipBullet.position[1] = -10;
	spaceShipBullet.old_position[0] = spaceShipBullet.position[0];
	spaceShipBullet.old_position[1] = spaceShipBullet.position[1];
	spaceShipBullet.color = LCD_BLUE;
	spaceShipBullet.active = 0;
	spaceShipBullet.hit = 0;
	spaceShipBullet.CurrentTime = OS_MsTime();
}

void initAlBullet(){
	alienBullet.position[0] = -10;
	alienBullet.position[1] = -10;
	alienBullet.old_position[0] = -10;
	alienBullet.old_position[1] = -10;
	alienBullet.color = LCD_RED;
	alienBullet.active = 0;
	alienBullet.hit = 0;
	alienBullet.CurrentTime = OS_MsTime();
}
/*
void ssBulletThread(){
	initSSBullet();
	while(spaceShipBullet.active){
		spaceShipBullet.old_position[1] = spaceShipBullet.position[1];
		spaceShipBullet.position[1] -= 10;
		
		//need to find whether the bullet hits something or not
		
		if(spaceShipBullet.hit){
			spaceShipBullet.active = 0;
		}
		if(spaceShipBullet.position[1] <= 0)
			spaceShipBullet.active = 0;		
	}
	OS_Kill();
}
*/
void bulletThread() {
	while(life){
		if(ssBulletFired == 1 && spaceShipBullet.active!=1){
			spaceShipBullet.position[0] = ssBulletPos+7;
			spaceShipBullet.old_position[0] = spaceShipBullet.position[0];
			spaceShipBullet.position[1] = 97;
			spaceShipBullet.active = 1;
		}
		
		if((OS_MsTime() - alienBullet.CurrentTime) > 1000 && alienBullet.active!=1){
			alienBullet.CurrentTime = OS_MsTime();
			alienBullet.position[0] = alienArray[0].position[0]+37;
			alienBullet.old_position[0] = alienArray[0].position[0]+37;
			alienBullet.position[1] = alienArray[0].position[1]+10;
			alienBullet.CurrentTime = OS_MsTime();
			alienBullet.active = 1;
		}
		if((OS_MsTime() - spaceShipBullet.CurrentTime) > 100){
			spaceShipBullet.CurrentTime = OS_MsTime();
			if(alienBullet.active){
				alienBullet.old_position[1] = alienBullet.position[1];
				alienBullet.position[1] += 10;
				if(alienBullet.position[1] >= 138){
					BSP_LCD_FillRect(alienBullet.old_position[0],alienBullet.old_position[1],2,3,LCD_BLACK);
					initAlBullet();
				}
			}
			
			if(spaceShipBullet.active){
				spaceShipBullet.old_position[1] = spaceShipBullet.position[1];
				spaceShipBullet.position[1] -= 10;
				if(spaceShipBullet.position[1] <= -10){
					//BSP_LCD_FillRect(spaceShipBullet.old_position[0],spaceShipBullet.old_position[1],2,3,LCD_BLACK);
					//initSSBullet();
					spaceShipBullet.active = 0;
					ssBulletFired = 0;
				}
			}
			/*else{
				initSSBullet();
			}*/
		}
	}
}




/////////////////////////Collision

int collision(ax1,ay1,width_a,len_a,bx1,by1,width_b,len_b) {
	int ax2,ay2,bx2,by2;
	ax2 = ax1 + len_a;
	ay2 = ay1 + width_a;
	bx2 = bx1 + len_b;
	by2 = by1 + width_b;
	if(ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1) {
		// it is collinding
		return 1;
	}
	else { // It isn't colliding
		return 0;
	}
}

void collisionThread() {
	int hit=0;
	uint8_t i;

	while(life) {
		if(spaceShipBullet.active)
			hit = collision(spaceShipBullet.position[0],spaceShipBullet.position[1],3,2,Shield1.position[0],Shield1.position[1],shield_h,shield_w);
		if(hit && (Shield1.life > 0)) {
			ssBulletFired = 0;
			Shield1.life--;
			spaceShipBullet.active = 0;
			BSP_LCD_FillRect(spaceShipBullet.position[0],spaceShipBullet.position[1],2,3,LCD_BLACK);
			BSP_LCD_FillRect(spaceShipBullet.old_position[0],spaceShipBullet.old_position[1],2,3,LCD_BLACK);
		 	hit = 0;
		 //initSSBullet();
		 //spaceShipBullet.active = 0;
		 //spaceShipBullet.hit = 1;
			//BSP_LCD_FillRect(spaceShipBullet.position[0],spaceShipBullet.position[1],2,3,LCD_BLACK);
	 
				//BSP_LCD_FillRect(spaceShipBullet.position[0],spaceShipBullet.position[1],2,3,LCD_BLACK);
				//BSP_LCD_FillRect(spaceShipBullet.old_position[0],spaceShipBullet.old_position[1],2,3,LCD_BLACK);
	   //initSSBullet();
		}
		OS_Sleep(10);
 }
	/* 
	 1) check to see if spaceship bullet has hit alien
		- if yes, declare that the specific alien is dead
	 2) check to see if alien bullet has hit spaceship
		- is yes, deduct one life point from spaceship
	 3) check to see if either alien or spaceship bullet has hit one of the shields
	  - if yes, deduct life point from shield
	*/
}
