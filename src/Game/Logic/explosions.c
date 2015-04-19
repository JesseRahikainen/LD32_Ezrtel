#include "explosions.h"

#include <SDL_log.h>
#include "../resources.h"
#include "../../particles.h"
#include "../../Math/mathUtil.h"
#include "../../sound.h"

typedef struct {
	union Collider collision;
	int lifetime;
} Explosion;

#define NUM_EXPLOSIONS 32
static Explosion explosions[NUM_EXPLOSIONS];

void initExplosions( void )
{
	for( int i = 0; i < NUM_EXPLOSIONS; ++i ) {
		explosions[i].lifetime = 0;
		explosions[i].collision.type = CT_DEACTIVATED;
	}

	explosionColliders.firstCollider = &( explosions[0].collision );
	explosionColliders.stride = sizeof( Explosion );
	explosionColliders.count = NUM_EXPLOSIONS;
}

static int findFreeExplosion( void )
{
	int idx = -1;
	for( int i = 0; ( i < NUM_EXPLOSIONS ) && ( idx == -1 ); ++i ) {
		if( explosions[i].lifetime <= 0 ) {
			idx = i;
		}
	}

	if( idx < 0 ) {
		SDL_LogVerbose( SDL_LOG_CATEGORY_APPLICATION, "Failed to find empty explosion." );
	}

	return idx;
}

void spawnDrumExplosion( Vector2 pos )
{
	int idx = findFreeExplosion( );
	if( idx < 0 ) {
		return;
	}

	explosions[idx].collision.type = CT_CIRCLE;
	explosions[idx].collision.circle.center = pos;
	explosions[idx].collision.circle.radius = 48.0f;

	explosions[idx].lifetime = 2;

	playSound( drumExplosionSnd );

	// spawn a particle burst at it's center
	//  fire
	for( int i = 0; i < 32; ++i ) {
		Vector2 startPos;
		Vector2 velocity;

		Vector2 forward;
		forward.x = randFloat( 1.0f, 10.0f ) * sign( randFloat( -1.0f, 1.0f ) );
		forward.y = randFloat( 1.0f, 10.0f ) * sign( randFloat( -1.0f, 1.0f ) );
		vec2_Add( &pos, &forward, &startPos );

		vec2_Scale( &forward, randFloat( 5.0f, 10.0f ), &velocity );
		spawnParticle( startPos, velocity, randFloat( 0.0f, 2.0f * M_PI_F ), VEC2_ZERO, 1.0f, 0.5f, 0.15f, firePrtImg, 1, 49 );
	}

	//  smoke
	for( int i = 0; i < 32; ++i ) {
		Vector2 startPos;
		Vector2 velocity;

		Vector2 forward;
		forward.x = randFloat( 1.0f, 10.0f ) * sign( randFloat( -1.0f, 1.0f ) );
		forward.y = randFloat( 1.0f, 10.0f ) * sign( randFloat( -1.0f, 1.0f ) );
		vec2_Add( &pos, &forward, &startPos );

		vec2_Scale( &forward, randFloat( 5.0f, 15.0f ), &velocity );
		spawnParticle( startPos, velocity, randFloat( 0.0f, 2.0f * M_PI_F ), VEC2_ZERO, 2.0f, 1.0f, 0.5f, smokePrtImg, 1, 50 );
	}
}

void spawnMineExplosion( Vector2 pos )
{
	int idx = findFreeExplosion( );
	if( idx < 0 ) {
		return;
	}

	explosions[idx].collision.type = CT_CIRCLE;
	explosions[idx].collision.circle.center = pos;
	explosions[idx].collision.circle.radius = 32.0f;

	explosions[idx].lifetime = 2;

	playSound( mineExplosionSnd );

	// spawn a particle burst at it's center
	//  fire
	for( int i = 0; i < 8; ++i ) {
		Vector2 startPos;
		Vector2 velocity;

		Vector2 forward;
		forward.x = randFloat( 1.0f, 5.0f ) * sign( randFloat( -1.0f, 1.0f ) );
		forward.y = randFloat( 1.0f, 5.0f ) * sign( randFloat( -1.0f, 1.0f ) );
		vec2_Add( &pos, &forward, &startPos );

		vec2_Scale( &forward, randFloat( 1.0f, 5.0f ), &velocity );
		spawnParticle( startPos, velocity, randFloat( 0.0f, 2.0f * M_PI_F ), VEC2_ZERO, 1.0f, 0.5f, 0.15f, firePrtImg, 1, 88 );
	}

	//  smoke
	for( int i = 0; i < 8; ++i ) {
		Vector2 startPos;
		Vector2 velocity;

		Vector2 forward;
		forward.x = randFloat( 1.0f, 5.0f ) * sign( randFloat( -1.0f, 1.0f ) );
		forward.y = randFloat( 1.0f, 5.0f ) * sign( randFloat( -1.0f, 1.0f ) );
		vec2_Add( &pos, &forward, &startPos );

		vec2_Scale( &forward, randFloat( 3.0f, 10.0f ), &velocity );
		spawnParticle( startPos, velocity, randFloat( 0.0f, 2.0f * M_PI_F ), VEC2_ZERO, 2.0f, 1.0f, 0.5f, smokePrtImg, 1, 89 );
	}
}

void updateExplosions( void )
{
	for( int i = 0; i < NUM_EXPLOSIONS; ++i ) {
		if( explosions[i].lifetime < 0 ) {
			continue;
		}
		explosions[i].lifetime -= 1;
		if( explosions[i].lifetime < 0 ) {
			explosions[i].lifetime = 0;
			explosions[i].collision.type = CT_DEACTIVATED;
		}
	}
}

void debugRenderExplosions( void )
{
	collisionDebugRendering( explosionColliders.firstCollider, 1, explosionColliders.stride, explosionColliders.count, 1.0f, 1.0f, 0.0f );
}