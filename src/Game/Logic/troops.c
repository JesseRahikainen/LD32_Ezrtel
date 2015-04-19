#include "troops.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "../../Graphics/color.h"
#include "../../Graphics/sprites.h"
#include "../resources.h"
#include "../../Math/mathUtil.h"
#include "../../particles.h"
#include "pits.h"
#include "../../Graphics/graphics.h"
#include "../../sound.h"

static enum TroopState {
	TS_WALKING,
	TS_SHOOTING,
	TS_CROUCHING,
	TS_DEAD
};

typedef struct {
	Vector2 position;
	Vector2 velocity;
	float rotation;
	int side;
	union Collider collider;
	int sprite;

	float despawn;
	float shootWait;

	int target;
	enum TroopState state;
	int pitIdx;
} Troop;

#define MAX_TROOPS 256
static Troop troops[MAX_TROOPS];

#define SHOOT_DELAY_MIN 0.5f
#define SHOOT_DELAY_MAX 1.5f
#define SHOOT_HIT_CHANCE 0.5f
#define TARGET_COS_ANGLE 0.707f
#define TARGET_DIST 100.0f

void initTroops( void )
{
	memset( troops, 0, sizeof( troops ) );
	for( int i = 0; i < MAX_TROOPS; ++i ) {
		troops[i].sprite = -1;
		troops[i].collider.type = CT_DEACTIVATED;
		troops[i].collider.circle.radius = 10.0f;
	}

	troopColliders.firstCollider = &( troops[0].collider );
	troopColliders.count = MAX_TROOPS;
	troopColliders.stride = sizeof( Troop );
}

int getTroopSide( int idx )
{
	return troops[idx].side;
}

int troopImmuneToPanjaDrum( int idx )
{
	return ( troops[idx].state == TS_CROUCHING );
}

void enterPit( int idx, int pitIdx )
{
	troops[idx].state = TS_CROUCHING;
	troops[idx].pitIdx = pitIdx;
	pitEntered( pitIdx );
}

void spawnTroop( Vector2 position, int side )
{
	int idx = -1;
	for( int i = 0; ( i < MAX_TROOPS ) && ( idx == -1 ); ++i ) {
		if( troops[i].sprite == -1 ) {
			idx = i;
		}
	}

	if( idx == -1 ) {
		return;
	}

	troops[idx].position = position;
	troops[idx].side = side;
	troops[idx].rotation = DEG_TO_RAD( ( side == 0 ) ? -90.0f : 90.0f );
	troops[idx].sprite = createSprite( troopWalkImg, 1,troops[idx].position, VEC2_ONE, troops[idx].rotation, sideColors[side], 75 );
	troops[idx].collider.circle.center = position;
	troops[idx].collider.type = CT_CIRCLE;
	troops[idx].despawn = -1.0f;
	troops[idx].state = TS_WALKING;
	troops[idx].pitIdx = -1;

	if( side == 0 ) {
		troops[idx].velocity.y = -20.0f;
	} else {
		troops[idx].velocity.y = 20.0f;
	}
}

void destroyTroop( int idx )
{
	destroySprite( troops[idx].sprite );
	troops[idx].sprite = -1;
	troops[idx].collider.type = CT_DEACTIVATED;

	if( troops[idx].pitIdx >= 0 ) {
		pitLeft( troops[idx].pitIdx );
	}
}

void killTroop( int idx, Vector2 direction )
{
	if( ( troops[idx].state == TS_DEAD ) ||
		( troops[idx].collider.type == CT_DEACTIVATED ) ) {
		return;
	}
	// just go to their kill animation, leave them there and fade them out eventually
	//  want the player to be able to run over dead troops and gib them
	troops[idx].despawn = 2.0f;
	troops[idx].velocity = VEC2_ZERO;
	troops[idx].state = TS_DEAD;
	playSound( troopKilledSnd );

	int numBlood = ( rand( ) % 2 ) + 1;
	for( int i = 0; i < numBlood; ++i ) {
		Vector2 velocity;
		velocity.x = ( -signWithZero( direction.x ) * randFloat( 20.0f, 50.0f ) ) + randFloat( -30.0f, 30.0f );
		velocity.y = ( -signWithZero( direction.y ) * randFloat( 20.0f, 50.0f ) ) + randFloat( -30.0f, 30.0f );
		spawnParticle( troops[idx].position, velocity, 0.0f, VEC2_ZERO, 1.0f, 0.5f, 0.25f, bloodPrtImg, 1, 64 );
	}
}

void gibTroop( int idx, Vector2 direction )
{
	int numGibs = ( rand( ) % 4 ) + 2;
	for( int i = 0; i < numGibs; ++i ) {
		Vector2 velocity;
		velocity.x = direction.x * randFloat( -50.0f, -20.0f );
		velocity.y = direction.y * randFloat( -50.0f, -20.0f );
		spawnParticle( troops[idx].position, velocity, randFloat( 0.0f, 2.0f * M_PI_F ),VEC2_ZERO, 3.5f, 2.0f, 1.9f, gibsPrtImg, 1, 65 );
	}

	int numBlood = ( rand( ) % 10 ) + 5;
	for( int i = 0; i < numBlood; ++i ) {
		Vector2 velocity;
		velocity.x = randFloat( 20.0f, 50.0f ) * sign( randFloat( -1.0f, 1.0f ) );
		velocity.y = randFloat( 20.0f, 50.0f ) * sign( randFloat( -1.0f, 1.0f ) );
		spawnParticle( troops[idx].position, velocity, 0.0f, VEC2_ZERO, 1.0f, 1.0f, 0.75f, bloodPrtImg, 1, 64 );
	}

	destroyTroop( idx );
}

int getBestTargetFor( int idx )
{
	int preferredTarget = troops[idx].target;
	if( ( troops[preferredTarget].collider.type == CT_DEACTIVATED ) ||
		( troops[preferredTarget].state == TS_DEAD ) ) {
		preferredTarget = -1;
	}

	float bestScore = FLT_MAX;
	int bestIdx = -1;
	Vector2 direction;
	direction.x = 0.0f;
	direction.y = ( troops[idx].side == 0 ) ? -1.0f : 1.0f;

	for( int i = 0; i < MAX_TROOPS; ++i ) {
		if( ( troops[i].sprite == -1 ) ||
			( troops[i].side == troops[idx].side ) ||
			( troops[i].state == TS_DEAD ) ) {
			continue;
		}

		Vector2 diff;
		vec2_Subtract( &( troops[i].position ), &( troops[idx].position ), &diff );
		float dist = vec2_MagSqrd( &diff );
		if( dist <= ( TARGET_DIST * TARGET_DIST ) ) {
			dist = sqrtf( dist );
			diff.x /= dist;
			diff.y /= dist;

			float dot = inverseLerp( TARGET_COS_ANGLE, 1.0f, vec2_DotProduct( &direction, &diff ) );
			float score = dot * dist;

			if( i == preferredTarget ) {
				score /= 2.0f;
			}

			if( troops[i].state == TS_CROUCHING ) {
				score *= 4.0f;
			}

			if( ( score > 0.0f ) && ( score <= bestScore ) ) {
				bestIdx = i;
				bestScore = score;
			}
		}
	}

	return bestIdx;
}

static void processWalking( int idx )
{
	troops[idx].target = getBestTargetFor( idx );
	if( troops[idx].target >= 0 ) {
		troops[idx].shootWait = randFloat( SHOOT_DELAY_MIN, SHOOT_DELAY_MAX );
		troops[idx].state = TS_SHOOTING;
	}
}

static void processShooting( int idx )
{
	troops[idx].target = getBestTargetFor( idx );
	if( troops[idx].target < 0 ) {
		troops[idx].state = TS_WALKING;
	}
}

static void processCrouching( int idx )
{
	troops[idx].target = getBestTargetFor( idx );
}

void processTroops( void )
{
	for( int i = 0; i < MAX_TROOPS; ++i ) {
		if( troops[i].sprite == -1 ) {
			continue;
		}

		// troop AI
		switch( troops[i].state ) {
		case TS_WALKING:
			setImageForSprite( troops[i].sprite, troopWalkImg );
			processWalking( i );
			break;
		case TS_SHOOTING:
			setImageForSprite( troops[i].sprite, troopShootImg );
			processShooting( i );
			break;
		case TS_CROUCHING:
			setImageForSprite( troops[i].sprite, troopCrouchImg );
			processCrouching( i );
			break;
		case TS_DEAD:
			setImageForSprite( troops[i].sprite, troopDeadImg );
			break;
		}
	}
}

static void walkingUpdate( int idx, float dt )
{
	troops[idx].rotation = DEG_TO_RAD( ( troops[idx].side == 0 ) ? -90.0f : 90.0f );
	vec2_AddScaled( &( troops[idx].position ), &( troops[idx].velocity ), dt, &( troops[idx].position ) );
}


static Vector2 getMuzzlePos( int idx )
{
	Vector2 muzzlePos = troops[idx].position;
	Vector2 forward;
	Vector2 right;
	forward.x = cosf( troops[idx].rotation );
	forward.y = sinf( troops[idx].rotation );

	right.x = -forward.y;
	right.y = forward.x;

	switch( troops[idx].state ) {
	case TS_SHOOTING:
		vec2_AddScaled( &muzzlePos, &forward, 10.0f, &muzzlePos );
		vec2_AddScaled( &muzzlePos, &right, 3.0f, &muzzlePos );
		break;
	case TS_CROUCHING:
		vec2_AddScaled( &muzzlePos, &forward, 13.0f, &muzzlePos );
		vec2_AddScaled( &muzzlePos, &right, 4.0f, &muzzlePos );
		break;
	}

	return muzzlePos;
}

static void shootAtTarget( int shooterIdx, int targetIdx )
{
	// can't hit crouching troops, these have to be dealt with by the player
	if( troops[targetIdx].state == TS_CROUCHING ) {
		return;
	}

	// muzzle flash
	Vector2 muzzlePos = getMuzzlePos( shooterIdx );
	spawnParticle( muzzlePos, VEC2_ZERO, troops[shooterIdx].rotation, VEC2_ZERO, 0.0f, 0.25f, 0.20f, muzzleFlashPrtImg, 1, 64 );
	playSound( shootSnd );

	if( randFloat( 0.0f, 1.0f ) <= SHOOT_HIT_CHANCE ) {
		Vector2 direction;
		vec2_Subtract(&( troops[shooterIdx].position ),  &( troops[targetIdx].position ), &direction );
		killTroop( targetIdx, direction );
	}
}

static void faceTowardsTarget( int idx )
{
	if( troops[idx].target < 0 ) {
		troops[idx].rotation = DEG_TO_RAD( ( troops[idx].side == 0 ) ? -90.0f : 90.0f );
		return;
	}

	// face towards our target
	Vector2 direction;
	int targetIdx = troops[idx].target;
	vec2_Subtract( &( troops[targetIdx].position ), &( troops[idx].position ), &direction );
	troops[idx].rotation = atan2f( direction.y, direction.x );
}

static void shootingUpdate( int idx, float dt )
{
	faceTowardsTarget( idx );

	// try and hit them
	troops[idx].shootWait -= dt;
	if( troops[idx].shootWait <= 0.0f ) {
		// shoot at our current target
		shootAtTarget( idx, troops[idx].target );
		troops[idx].shootWait = randFloat( SHOOT_DELAY_MIN, SHOOT_DELAY_MAX );
	}
}

static void crouchingUpdate( int idx, float dt )
{
	// keep heading towards the center of the pit, but slower
	Vector2 pitTarget = pitPos( troops[idx].pitIdx );
	vec2_Lerp( &( troops[idx].position ), &pitTarget, 0.85f * dt, &( troops[idx].position ) );

	faceTowardsTarget( idx );

	if( troops[idx].target >= 0 ) {
		troops[idx].shootWait -= dt;
		if( troops[idx].shootWait <= 0.0f ) {
			shootAtTarget( idx, troops[idx].target );
			troops[idx].shootWait = SHOOT_DELAY_MIN;
		}
	}
}

static void deadUpdate( int idx, float dt )
{
	troops[idx].despawn -= dt;
	if( troops[idx].despawn <= 0.0f ) {
		destroyTroop( idx );
	}
}

void updateTroops( float dt )
{
	for( int i = 0; i < MAX_TROOPS; ++i ) {
		if( troops[i].sprite == -1 ) {
			continue;
		}

		switch( troops[i].state ) {
		case TS_WALKING:
			walkingUpdate( i, dt );
			break;
		case TS_SHOOTING:
			shootingUpdate( i, dt );
			break;
		case TS_CROUCHING:
			crouchingUpdate( i, dt );
			break;
		case TS_DEAD:
			deadUpdate( i, dt );
			break;
		}
			
		troops[i].collider.circle.center = troops[i].position;
		updateSprite( troops[i].sprite, troops[i].position, VEC2_ONE, troops[i].rotation );
	}
}

void debugRenderTroops( void )
{
	collisionDebugRendering( troopColliders.firstCollider, 1, troopColliders.stride, troopColliders.count, 1.0f, 0.0f, 1.0f );
}