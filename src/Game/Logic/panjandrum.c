#include "panjandrum.h"

#include <math.h>

#include "../../Graphics/graphics.h"
#include "../../Graphics/sprites.h"
#include "../resources.h"
#include "../../collisionDetection.h"
#include "../../particles.h"
#include "explosions.h"
#include "../../Math/mathUtil.h"

typedef struct {
	Vector2 position;
	Vector2 forward;
	Vector2 right;
	float rotation;
	float speed;
	float emitter;
	int sprite;
} Panjandrum;

#define EMIT_TIME 0.025f

#define NUM_DRUMS 16
static Panjandrum drums[NUM_DRUMS];
static union Collider colliders[NUM_DRUMS * 2];

#define DRUM_DECELERATION -200.0f

void initPanjandrums( void )
{
	for( int i = 0; i < NUM_DRUMS; ++i ) {
		drums[i].sprite = -1;

		for( int c = 0; c < 2; ++c ) {
			colliders[i*2+c].type = CT_DEACTIVATED;
			colliders[i*2+c].circle.radius = 20.0f;
		}
	}

	panjandrumColliders.firstCollider = colliders;
	panjandrumColliders.stride = sizeof( union Collider );
	panjandrumColliders.count = sizeof( colliders ) / sizeof( union Collider );
}

void slowPanjanDrum( int idx, float amount )
{
	drums[idx].speed -= amount;
	if( drums[idx].speed < 0.0f ) {
		drums[idx].speed = 0.0f;
	}
}

void panjanDrumHitByExplosion( int idx, Vector2 separation )
{
	slowPanjanDrum( idx, -vec2_DotProduct( &separation, &( drums[idx].forward ) ) * 0.5f );
}

static void setColliderPositions( int idx )
{
	vec2_AddScaled( &( drums[idx].position ), &( drums[idx].forward ), colliders[idx*2].circle.radius,
		&(  colliders[idx*2].circle.center ) );
	vec2_AddScaled( &( drums[idx].position ), &( drums[idx].forward ), -colliders[(idx*2)+1].circle.radius,
		&( colliders[(idx*2)+1].circle.center ) );
}

void launchPanjandrum( Vector2 startPoint, Vector2 targetPoint )
{
	int idx = -1;
	for( int i = 0; ( i < NUM_DRUMS ) && ( idx == -1 ); ++i ) {
		if( drums[i].sprite == -1 ) {
			idx = i;
		}
	}

	if( idx < 0 ) {
		SDL_Log( "Too many drums" );
		return;
	}

	// calculate the initial position and speed
	Vector2 direction;
	Vector2 initialPosition; // want it slightly off screen
	float rotation;
	float speed;
	float dist;

	vec2_Subtract( &targetPoint, &startPoint, &direction );
	vec2_Normalize( &direction );
	rotation = atan2f( direction.y, direction.x );

	vec2_AddScaled( &startPoint, &direction, -32.0f, &initialPosition );

	dist = vec2_Dist( &initialPosition, &targetPoint );
	speed = sqrtf( -( 2 * DRUM_DECELERATION * dist ) );

	drums[idx].forward = direction;
	drums[idx].right.x = -direction.y;
	drums[idx].right.y = direction.x;
	drums[idx].position = initialPosition;
	drums[idx].rotation = rotation;
	drums[idx].speed = speed;
	drums[idx].sprite = createSprite( panjandrumImg, 1, initialPosition, VEC2_ONE, rotation, COL_WHITE, 55 );

	setColliderPositions( idx );
	colliders[2*idx].type = CT_CIRCLE;
	colliders[(2*idx)+1].type = CT_CIRCLE;
}

static void destroyDrum( int idx )
{
	destroySprite( drums[idx].sprite );
	drums[idx].sprite = -1;
	colliders[2*idx].type = CT_DEACTIVATED;
	colliders[(2*idx)+1].type = CT_DEACTIVATED;
}

void explode( int idx )
{
	spawnDrumExplosion( drums[idx].position );
	destroyDrum( idx );
}

void updatePanjandrums( float dt )
{
	for( int i = 0; i < NUM_DRUMS; ++i ) {
		if( drums[i].sprite < 0 ) {
			continue;
		}
		
		Vector2 newPos;
		float displacement = ( drums[i].speed * dt ) + ( 0.5f * DRUM_DECELERATION * dt * dt );
		vec2_AddScaled( &( drums[i].position ), &( drums[i].forward ), displacement, &newPos );

		// spawn particles along the path
		drums[i].emitter += dt;
		if( drums[i].emitter >= EMIT_TIME ) {
			Vector2 rightEmit;
			Vector2 leftEmit;

			Vector2 base;
			vec2_AddScaled( &newPos, &( drums[i].forward ), 32.0f, &base );
			vec2_AddScaled( &base, &( drums[i].right ), 14.0f, &rightEmit );
			vec2_AddScaled( &base, &( drums[i].right ), -14.0f, &leftEmit );

			spawnParticle( rightEmit, VEC2_ZERO, randFloat( 0.0f, 2 * M_PI_F ), VEC2_ZERO, 0.0f, 1.0f, 0.5f, smallSmokePrtImg, 1, 50 );
			spawnParticle( leftEmit, VEC2_ZERO, randFloat( 0.0f, 2 * M_PI_F ), VEC2_ZERO, 0.0f, 1.0f, 0.5f, smallSmokePrtImg, 1, 50 );

			spawnParticle( rightEmit, VEC2_ZERO, randFloat( 0.0f, 2 * M_PI_F ), VEC2_ZERO, 0.0f, 0.25f, 0.15f, firePrtImg, 1, 49 );
			spawnParticle( leftEmit, VEC2_ZERO, randFloat( 0.0f, 2 * M_PI_F ), VEC2_ZERO, 0.0f, 0.25f, 0.15f, firePrtImg, 1, 49 );

			while( drums[i].emitter >= EMIT_TIME ) {
				drums[i].emitter -= EMIT_TIME;
			}
		}

		updateSprite( drums[i].sprite, newPos, VEC2_ONE, drums[i].rotation );
		drums[i].position = newPos;

		// decrease the speed a bit
		drums[i].speed += DRUM_DECELERATION * dt;

		// update the collision positions
		setColliderPositions( i );

		if( drums[i].speed <= 0.0f ) {
			explode( i );
		}
	}
}

void debugRenderPanjandrums( void )
{
	collisionDebugRendering( panjandrumColliders.firstCollider, 1, panjandrumColliders.stride, panjandrumColliders.count, 1.0f, 0.0f, 1.0f );
}

#include "../resources.h"