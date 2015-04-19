#ifndef BARBED_WIRE_H
#define BARBED_WIRE_H

#include "../../collisionDetection.h"

CollisionCollection barbedWireColliders;

void initBarbedWire( void );

void spawnBarbedWire( Vector2 pos );
void destroyBarbedWire( int idx );

void drawBarbedWire( void );
void debugRenderBarbedWire( void );

#endif /* inclusion guard */