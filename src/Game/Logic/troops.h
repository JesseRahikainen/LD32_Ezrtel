#ifndef TROOPS_H
#define TROOPS_H

#include "../../Math/vector2.h"
#include "../../collisionDetection.h"

CollisionCollection troopColliders;

void initTroops( void );

int getTroopSide( int idx );
int troopImmuneToPanjaDrum( int idx );

void enterPit( int idx, int pitIdx );

void destroyTroop( int idx );
void killTroop( int idx, Vector2 direction );
void gibTroop( int idx, Vector2 direction );

void spawnTroop( Vector2 position, int side );

void processTroops( void );
void updateTroops( float dt );

void debugRenderTroops( void );

#endif /* inclusion guard */