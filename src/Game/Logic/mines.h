#ifndef MINES_H
#define MINES_H

#include "../../Math/vector2.h"
#include "../../collisionDetection.h"

CollisionCollection mineColliders;

void initMines( void );

float getMineRadius( void );
void spawnMine( Vector2 pos );
void explodeMine( int idx );

void updateMines( float dt );
void drawMines( void );
void debugRenderMines( void );

#endif /* inclusion guard */