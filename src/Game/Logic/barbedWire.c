#include "barbedWire.h"

#include "../../Graphics/graphics.h"
#include "../resources.h"

typedef struct {
	Vector2 pos;
	int active;
	union Collider collision;
} BarbedWire;

#define NUM_BARBED_WIRES 1024
static BarbedWire barbedWires[NUM_BARBED_WIRES];

void initBarbedWire( void )
{
	Vector2 collSize = { 28.0f, 4.0f };
	for( int i = 0; i < NUM_BARBED_WIRES; ++i ) {
		barbedWires[i].active = 0;
		barbedWires[i].collision.type = CT_DEACTIVATED;
		barbedWires[i].collision.aabb.halfDim = collSize;
	}

	barbedWireColliders.firstCollider = &( barbedWires[0].collision );
	barbedWireColliders.count = NUM_BARBED_WIRES;
	barbedWireColliders.stride = sizeof( BarbedWire );
}

void spawnBarbedWire( Vector2 pos )
{
	int idx = -1;
	for( int i = 0; ( i < NUM_BARBED_WIRES ) && ( idx == -1 ); ++i ) {
		if( barbedWires[i].active == 0 ) {
			idx = i;
		}
	}

	if( idx < 0 ) {
		SDL_LogVerbose( SDL_LOG_CATEGORY_APPLICATION, "Too many barbed wires spawned." );
		return;
	}

	barbedWires[idx].active = 1;
	barbedWires[idx].collision.aabb.center = pos;
	barbedWires[idx].collision.type = CT_AABB;
	barbedWires[idx].pos = pos;
}

void destroyBarbedWire( int idx )
{
	barbedWires[idx].active = 0;
	barbedWires[idx].collision.type = CT_DEACTIVATED;
}

void drawBarbedWire( void )
{
	for( int i = 0; i < NUM_BARBED_WIRES; ++i ) {
		if( barbedWires[i].active == 0 ) { 
			continue;
		}

		queueRenderImage( barbedWireImg, 1, barbedWires[i].pos, barbedWires[i].pos, 70 );
	}
}

void debugRenderBarbedWire( void )
{
	collisionDebugRendering( barbedWireColliders.firstCollider, 1, barbedWireColliders.stride, barbedWireColliders.count, 1.0f, 0.0f, 0.0f );
}