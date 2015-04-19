#ifndef PITS_H
#define PITS_H

#include "../../collisionDetection.h"

CollisionCollection pitColliders;

void initPits( void );

void spawnPit( Vector2 pos );

int pitInhabited( int idx );
void pitEntered( int idx );
void pitLeft( int idx );
Vector2 pitPos( int idx );

void drawPits( void );

void debugRenderPits( void );

#endif /* inclusion guard */