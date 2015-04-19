#include "mines.h"

#include "explosions.h"
#include "../../Graphics/graphics.h"
#include "../resources.h"

typedef struct {
	int isActive;
	Vector2 position;
	union Collider collider;
	float blinkTime;
} Mine;

#define NUM_MINES 1024
static Mine mines[NUM_MINES];

#define BLINK_ON_DURATION 0.5f
#define BLINK_OFF_DURATION 1.0f

void initMines( void )
{
	for( int i = 0; i < NUM_MINES; ++i ) {
		mines[i].isActive = 0;
		mines[i].collider.type = CT_DEACTIVATED;
		mines[i].collider.circle.radius = getMineRadius( );
	}

	mineColliders.firstCollider = &( mines[0].collider );
	mineColliders.stride = sizeof( Mine );
	mineColliders.count = NUM_MINES;
}

float getMineRadius( void )
{
	return 20.0f;
}

void spawnMine( Vector2 pos )
{
	int idx = -1;
	for( int i = 0; ( i < NUM_MINES ) && ( idx == -1 ); ++i ) {
		if( mines[i].isActive == 0 ) {
			idx = i;
		}
	}

	if( idx < 0 ) {
		SDL_LogVerbose( SDL_LOG_CATEGORY_APPLICATION, "Too many mines spawned." );
		return;
	}

	mines[idx].isActive = 1;
	mines[idx].position = pos;
	mines[idx].collider.type = CT_CIRCLE;
	mines[idx].collider.circle.center = pos;
	mines[idx].blinkTime = BLINK_ON_DURATION;
}

void explodeMine( int idx )
{
	spawnMineExplosion( mines[idx].position );
	mines[idx].isActive = 0;
	mines[idx].collider.type = CT_DEACTIVATED;
}

void updateMines( float dt )
{
	// just update the blinking
	for( int i = 0; i < NUM_MINES; ++i ) {
		if( !mines[i].isActive ) {
			continue;
		}

		mines[i].blinkTime -= dt;
		if( mines[i].blinkTime <= -BLINK_OFF_DURATION ) {
			mines[i].blinkTime = BLINK_ON_DURATION;
		}
	}
}

void drawMines( void )
{
	for( int i = 0; i < NUM_MINES; ++i ) {
		if( !mines[i].isActive ) {
			continue;
		}

		if( mines[i].blinkTime >= 0.0f ) {
			queueRenderImage( mineImg, 1, mines[i].position, mines[i].position, 90 );
		}
	}
}

void debugRenderMines( void )
{
	collisionDebugRendering( mineColliders.firstCollider, 1, mineColliders.stride, mineColliders.count, 1.0f, 0.0f, 0.0f );
}