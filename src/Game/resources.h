#ifndef RESOURCES_H
#define RESOURCES_H

#include "../Graphics/color.h"

// images
//  start screen
//  tutorial screens
//  game screen
int sandImg;
int launchFromImg;
int launchToImg;
int panjandrumImg;

int mineImg;
int barbedWireImg;
int pitImg;

int troopWalkImg;
int troopShootImg;
int troopCrouchImg;
int troopDeadImg;

int smokePrtImg;
int firePrtImg;
int bloodPrtImg;
int gibsPrtImg;
int muzzleFlashPrtImg;
int smallSmokePrtImg;

int drumIconImg;
int barBaseImg;

int instructionsImg;

//  game over screen
int winForeImg;
int loseForeImg;
int pressAnyImg;
int cloudImg;

// sounds
int attackersHitSnd;
int defendersHitSnd;
int drumExplosionSnd;
int mineExplosionSnd;
int rechargeCompleteSnd;
int shootSnd;
int troopKilledSnd;
int troopRunOverSnd;


// 0 is attacker side
// 1 is defender side
Color sideColors[2];

int loadAllResources( void );

#endif /* inclusion guard */