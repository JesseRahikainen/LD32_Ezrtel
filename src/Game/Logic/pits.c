#include "pits.h"

#include <string.h>
#include "../../Graphics/graphics.h"
#include "../resources.h"

typedef struct {
	union Collider collision;
	int inhabited;
} Pit;

#define NUM_PITS 1024
static Pit pits[NUM_PITS];

void initPits( void )
{
	memset( pits, 0, sizeof( pits ) );

	pitColliders.firstCollider = &( pits[0].collision );
	pitColliders.count = NUM_PITS;
	pitColliders.stride = sizeof( Pit );
}

void spawnPit( Vector2 pos )
{
	int idx = -1;
	for( int i = 0; ( i < NUM_PITS ) && ( idx == -1 ); ++i ) {
		if( pits[i].collision.type == CT_DEACTIVATED ) {
			idx = i;
		}
	}

	if( idx < 0 ) {
		SDL_LogVerbose( SDL_LOG_CATEGORY_APPLICATION, "Too many barbed pits spawned." );
		return;
	}

	pits[idx].collision.type = CT_AABB;
	pits[idx].collision.aabb.center = pos;
	pits[idx].collision.aabb.halfDim.x = 10.0f;
	pits[idx].collision.aabb.halfDim.y = 10.0f;
	pits[idx].inhabited = 0;
}


int pitInhabited( int idx )
{
	return pits[idx].inhabited;
}

void pitEntered( int idx )
{
	pits[idx].inhabited = 1;
}

void pitLeft( int idx )
{
	pits[idx].inhabited = 0;
}

Vector2 pitPos( int idx )
{
	return pits[idx].collision.aabb.center;
}

void drawPits( void )
{
	for( int i = 0; i < NUM_PITS; ++i ) {
		if( pits[i].collision.type == CT_DEACTIVATED ) {
			continue;
		}

		queueRenderImage( pitImg, 1, pits[i].collision.aabb.center, pits[i].collision.aabb.center, 99 );
	}
}

void debugRenderPits( void )
{
	collisionDebugRendering( pitColliders.firstCollider, 1, pitColliders.stride, pitColliders.count, 0.0f, 0.0f, 1.0f );
}