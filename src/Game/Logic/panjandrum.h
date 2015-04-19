#ifndef PANJANDRUM_H
#define PANJANDRUM_H

#include "../../Math/vector2.h"
#include "../../collisionDetection.h"

CollisionCollection panjandrumColliders;

void initPanjandrums( void );

void slowPanjanDrum( int idx, float amount );
void panjanDrumHitByExplosion( int idx, Vector2 separation );

void launchPanjandrum( Vector2 startPoint, Vector2 targetPoint );
void updatePanjandrums( float dt );

void debugRenderPanjandrums( void );

#endif /* inclusion guard */