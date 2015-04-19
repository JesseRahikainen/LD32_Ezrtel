#ifndef EXPLOSIONS_H
#define EXPLOSIONS_H

#include "../../collisionDetection.h"

CollisionCollection explosionColliders;

void initExplosions( void );

void spawnDrumExplosion( Vector2 pos );
void spawnMineExplosion( Vector2 pos );

void updateExplosions( void );
void debugRenderExplosions( void );

#endif /* inclusion guard */