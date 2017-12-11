#include "bullet.h"
#include "OS.h"
#include "LCD.h"
#include "Aliens.h"

bullet spaceShipBullet, alienBullet;

void initSSBullet(){
	ssBulletFired = 0;
	spaceShipBullet.position[0] = -10;
	spaceShipBullet.position[1] = -10;
	spaceShipBullet.old_position[0] = -10;
	spaceShipBullet.old_position[1] = -10;
	spaceShipBullet.color = LCD_GREEN;
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
	while(1){
		if(ssBulletFired == 1 && spaceShipBullet.active!=1){
			spaceShipBullet.position[0] = ssBulletPos+7;
			spaceShipBullet.old_position[0] = ssBulletPos+7;
			spaceShipBullet.position[1] = 100;
			spaceShipBullet.active = 1;
		}
		
		if((alienBullet.CurrentTime - OS_MsTime()) > 5000 && alienBullet.active!=1){
			alienBullet.position[0] = alienArray[0].position[0]+37;
			alienBullet.old_position[0] = alienArray[0].position[0]+37;
			alienBullet.position[1] = alienArray[0].position[1]+10;
			alienBullet.CurrentTime = OS_MsTime();
			alienBullet.active = 1;
		}
		if((alienBullet.CurrentTime - OS_MsTime()) > 1000){
			if(alienBullet.active){
				alienBullet.old_position[1] = alienBullet.position[1];
				alienBullet.position[1] += 10;
				if(alienBullet.position[1] >= 115)
					initAlBullet();//alienBullet.active = 0;
			}
			
			if(spaceShipBullet.active){
				spaceShipBullet.old_position[1] = spaceShipBullet.position[1];
				spaceShipBullet.position[1] -= 10;
				if(spaceShipBullet.position[1] <= 0){
					initSSBullet();
				}
			}
		}
	}
}


/*void bulletThread() {
	if(playerFlag == 1) {
		addPlayerBullet(); // Makes new bullet, direction up
	}
	if(no alien bullet alive) {
		addAlienBullet();	// Makes new bullet, direction down
	}
	for(all bullets) {
		if(going up) {
			y++;
		}
		else {
			y--;
		}
	}
}*/