#include "gameScreen.h"

#include <stdlib.h>

#include "resources.h"
#include "../gameState.h"
#include "../Graphics/graphics.h"
#include "../Graphics/sprites.h"
#include "../Graphics/camera.h"
#include "../particles.h"
#include "../sound.h"

#include "../collisionDetection.h"

#include "Logic/panjandrum.h"
#include "Logic/troops.h"
#include "Logic/explosions.h"
#include "Logic/mines.h"
#include "Logic/barbedWire.h"
#include "Logic/pits.h"

#include "gameOverScreen.h"

#define OUR_SIDE 0
#define THEIR_SIDE 1
#define LEFT_SIDE 2
#define RIGHT_SIDE 3
union Collider sideCollisions[4];
CollisionCollection sideColliders;

#define MIN_ATTACKER_SPAWN 2
#define MAX_ATTACKER_SPAWN 5

#define MIN_DEFENDER_SPAWN 2
#define MAX_DEFENDER_SPAWN 6

#define MIN_SPAWN_DELAY 2.5f
#define MAX_SPAWN_DELAY 5.0f

#define MIN_PIT_SPAWN 10
#define MAX_PIT_SPAWN 20

#define MIN_MINE_SPOT_SPAWN 4
#define MAX_MIN_SPOT_SPAWN 8
#define MIN_MINES_PER_SPOT 4
#define MAX_MINES_PER_SPOT 6

#define MIN_WIRE_GROUP_SPAWN 4
#define MAX_WIRE_GROUP_SPAWN 8

#define CHANCE_MINES_IN_WIRE 0.33f
#define CHANCE_PIT_IN_WIRE 0.25f

#define CHANCE_OF_WIRE_GROUP 0.30f
#define CHANCE_OF_MINE_GROUP 0.30f
#define CHANCE_OF_PIT 0.30f

static Vector2 center = { 500.0f, 300.0f };

static int launchToSprite;

static Vector2 launchFromPosition;
static Vector2 launchToPosition;

static float ourSpawnDelay;
static float theirSpawnDelay;

#define MAX_DRUM_COUNT 4
#define MIN_DRUM_RECHARGE_TIME 5.0f
#define MAX_DRUM_RECHARGE_TIME 10.0f

static int drumCount;
static float drumRechargeAmount;

#define MAX_HEALTH 10
static int defenderHealth;
static int attackerHealth;

/* intro specific variables */
static int introMode;
union Collider startCollider;
/* end intro specific variables */

static void getLaunchToPosition( void )
{
	int mouseX;
	int mouseY;
	SDL_GetMouseState( &mouseX, &mouseY );
	launchToPosition.x = (float)mouseX;
	launchToPosition.y = (float)mouseY;
}

static void resetEverything( void )
{
	initSprites( );

	launchFromPosition.x = 500.0f;
	launchFromPosition.y = 600.0f - 16.0f;

	getLaunchToPosition( );
	launchToSprite = createSprite( launchToImg, 1, launchToPosition, VEC2_ONE, 0.0f, COL_WHITE, 0 );

	initPanjandrums( );
	initTroops( );
	initExplosions( );
	initMines( );
	initBarbedWire( );
	initPits( );

	initParticles( );

	attackerHealth = MAX_HEALTH;
	defenderHealth = MAX_HEALTH;

	drumCount = MAX_DRUM_COUNT;
}

static void layMineGroup( Vector2 basePos )
{
	int type = rand( ) % 4;

	Vector2 spawnPos;
	switch( type ) {
	case 0:
		// three mines
		spawnPos = basePos;
		spawnPos.x += randFloat( -10.0f, 10.0f );
		spawnPos.y += 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );

		spawnPos = basePos;
		spawnPos.x += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );

		spawnPos = basePos;
		spawnPos.x -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );
		break;
	case 1:
		spawnPos = basePos;
		spawnPos.x += randFloat( -10.0f, 10.0f );
		spawnPos.y -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );

		spawnPos = basePos;
		spawnPos.x += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y += 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );

		spawnPos = basePos;
		spawnPos.x -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y += 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );
		break;

	case 2:
		// four mines
		spawnPos = basePos;
		spawnPos.x += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );

		spawnPos = basePos;
		spawnPos.x -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );

		spawnPos = basePos;
		spawnPos.x += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y += 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );

		spawnPos = basePos;
		spawnPos.x -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y += 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );
		break;
	default:
		spawnPos = basePos;
		spawnPos.x += randFloat( -10.0f, 10.0f );
		spawnPos.y += randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );

		spawnPos = basePos;
		spawnPos.x += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );

		spawnPos = basePos;
		spawnPos.x -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );

		spawnPos = basePos;
		spawnPos.x += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y += 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );

		spawnPos = basePos;
		spawnPos.x -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y += 30.0f + randFloat( -10.0f, 10.0f );
		spawnMine( spawnPos );
		// five mines
		break;
	}
}

static void layPit( Vector2 basePos )
{
	int type = rand( ) % 3;

	Vector2 spawnPos;
	switch( type ) {
	case 0:
		spawnPos = basePos;
		spawnPos.x += randFloat( -30.0f, 30.0f );
		spawnPos.y += randFloat( -30.0f, 30.0f );
		spawnPit( spawnPos );
		spawnTroop( spawnPos, 1 );
		break;
	case 1:
		spawnPos = basePos;
		spawnPos.x += randFloat( -10.0f, 10.0f );
		spawnPos.y -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPit( spawnPos );
		spawnTroop( spawnPos, 1 );

		spawnPos = basePos;
		spawnPos.x += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPit( spawnPos );
		spawnTroop( spawnPos, 1 );

		spawnPos = basePos;
		spawnPos.x -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPit( spawnPos );
		spawnTroop( spawnPos, 1 );

		break;
	default:
		spawnPos = basePos;
		spawnPos.x += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPit( spawnPos );
		spawnTroop( spawnPos, 1 );

		spawnPos = basePos;
		spawnPos.x -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPit( spawnPos );
		spawnTroop( spawnPos, 1 );

		spawnPos = basePos;
		spawnPos.x += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPit( spawnPos );
		spawnTroop( spawnPos, 1 );

		spawnPos = basePos;
		spawnPos.x -= 30.0f + randFloat( -10.0f, 10.0f );
		spawnPos.y += 30.0f + randFloat( -10.0f, 10.0f );
		spawnPit( spawnPos );
		spawnTroop( spawnPos, 1 );

		break;
	}
}

static void layWireGroup( Vector2 basePos )
{
	int type = rand( ) % 4;
	Vector2 spawnPos;

	int putMines = ( randFloat( 0.0f, 1.0f ) < CHANCE_MINES_IN_WIRE );
	int putPit = ( randFloat( 0.0f, 1.0f ) < CHANCE_PIT_IN_WIRE );

	switch( type ) {
	case 0:
		spawnPos = basePos;
		spawnPos.x += 40 + randFloat( -4.0f, 4.0f );
		spawnBarbedWire( spawnPos );

		spawnPos = basePos;
		spawnPos.x -= 40 + randFloat( -4.0f, 4.0f );
		spawnBarbedWire( spawnPos );

		spawnPos = basePos;
		spawnPos.y += 20 + randFloat( -4.0f, 4.0f );
		spawnBarbedWire( spawnPos );

		spawnPos = basePos;
		spawnPos.y -= 20 + randFloat( -4.0f, 4.0f );
		spawnBarbedWire( spawnPos );

		if( putPit ) {
			spawnPit( basePos );
			spawnTroop( basePos, 1 );
		} else if( putMines ) {
			layMineGroup( basePos );
		}
		break;
	case 1:
		spawnPos = basePos;
		spawnPos.x += 40 + randFloat( -4.0f, 4.0f );
		spawnPos.y += randFloat( -10.0f, 10.0f );
		spawnBarbedWire( spawnPos );

		spawnPos = basePos;
		spawnPos.x -= 40 + randFloat( -4.0f, 4.0f );
		spawnPos.y += randFloat( -10.0f, 10.0f );
		spawnBarbedWire( spawnPos );

		if( putMines ) {
			layMineGroup( basePos );
		}
		break;
	case 2:
		spawnPos = basePos;
		spawnPos.x += randFloat( -20.0f, 20.0f );
		spawnPos.y += 35 + randFloat( -4.0f, 4.0f );
		spawnBarbedWire( spawnPos );

		spawnPos = basePos;
		spawnPos.x += randFloat( -20.0f, 20.0f );
		spawnPos.y += randFloat( -4.0f, 4.0f );
		spawnBarbedWire( spawnPos );

		spawnPos = basePos;
		spawnPos.x += randFloat( -20.0f, 20.0f );
		spawnPos.y -= 35 + randFloat( -4.0f, 4.0f );
		spawnBarbedWire( spawnPos );

		if( putMines ) {
			layMineGroup( basePos );
		}
		break;
	default:
		spawnPos = basePos;
		spawnPos.x += randFloat( -20.0f, 20.0f );
		spawnPos.y += 35 + randFloat( -4.0f, 4.0f );
		spawnBarbedWire( spawnPos );

		spawnPos = basePos;
		spawnPos.x += 40.0f + randFloat( -20.0f, 20.0f );
		spawnPos.y -= 35 + randFloat( -4.0f, 4.0f );
		spawnBarbedWire( spawnPos );

		spawnPos = basePos;
		spawnPos.x -= 40.0f + randFloat( -20.0f, 20.0f );
		spawnPos.y -= 35 + randFloat( -4.0f, 4.0f );
		spawnBarbedWire( spawnPos );

		if( putPit ) {
			spawnPit( basePos );
			spawnTroop( basePos, 1 );
		} else if( putMines ) {
			layMineGroup( basePos );
		}
		break;
	}
}

static void createGameLayout( void )
{
	playSong( "Music/play.it" );
	introMode = 0;
	resetEverything( );

	Vector2 basePos = { 50.0f, 50.0f };
	for( basePos.x = 50.0f; basePos.x < 1000.0f; basePos.x += 100.0f ) {
		for( basePos.y = 50.0f; basePos.y < 500.0f; basePos.y += 100.0f ) {
			float chance = randFloat( 0.0f, 1.0f );

			chance -= CHANCE_OF_WIRE_GROUP;
			if( chance <= 0.0f ) {
				layWireGroup( basePos );
				continue;
			}

			chance -= CHANCE_OF_MINE_GROUP;
			if( chance <= 0.0f ) {
				layMineGroup( basePos );
				continue;
			}

			chance -= CHANCE_OF_PIT;
			if( chance <= 0.0f ) {
				layPit( basePos );
			}
		}
	}
}

static void createIntroLayout( void )
{
	introMode = 1;
	resetEverything( );

	startCollider.type = CT_AABB;
	startCollider.aabb.center.x = 481.0f;
	startCollider.aabb.center.y = 198.0f;
	startCollider.aabb.halfDim.x = 72.0f;
	startCollider.aabb.halfDim.y = 27.0f;

	Vector2 spawnPos;
	spawnPos.x = 500.0f;

	spawnPos.y = 334.0f;
	spawnBarbedWire( spawnPos );

	spawnPos.y = 495.0f;
	spawnPit( spawnPos );
	spawnTroop( spawnPos, 1 );

	spawnPos.y = 410.0f;
	layMineGroup( spawnPos );
}

static int gameScreen_Enter( void )
{
	cam_TurnOnFlags( 0, 1 );
	SDL_ShowCursor( SDL_DISABLE );

	setRendererClearColor( 0.878f, 0.807f, 0.667f, 1.0f );

	resetEverything( );

	sideCollisions[OUR_SIDE].type = CT_AABB;
	sideCollisions[OUR_SIDE].aabb.center.x = 500.0f;
	sideCollisions[OUR_SIDE].aabb.center.y = 600.0f + 10.0f;
	sideCollisions[OUR_SIDE].aabb.halfDim.x = 1000.0f;
	sideCollisions[OUR_SIDE].aabb.halfDim.y = 10.0f;

	sideCollisions[THEIR_SIDE].type = CT_AABB;
	sideCollisions[THEIR_SIDE].aabb.center.x = 500.0f;
	sideCollisions[THEIR_SIDE].aabb.center.y = -10.0f;
	sideCollisions[THEIR_SIDE].aabb.halfDim.x = 1000.0f;
	sideCollisions[THEIR_SIDE].aabb.halfDim.y = 10.0f;

	sideCollisions[LEFT_SIDE].type = CT_AABB;
	sideCollisions[LEFT_SIDE].aabb.center.x = -10.0f;
	sideCollisions[LEFT_SIDE].aabb.center.y = 300.0f;
	sideCollisions[LEFT_SIDE].aabb.halfDim.x = 10.0f;
	sideCollisions[LEFT_SIDE].aabb.halfDim.y = 1000.0f;

	sideCollisions[RIGHT_SIDE].type = CT_AABB;
	sideCollisions[RIGHT_SIDE].aabb.center.x = 1000.0f + 10.0f;
	sideCollisions[RIGHT_SIDE].aabb.center.y = 300.0f;
	sideCollisions[RIGHT_SIDE].aabb.halfDim.x = 10.0f;
	sideCollisions[RIGHT_SIDE].aabb.halfDim.y = 1000.0f;

	sideColliders.firstCollider = sideCollisions;
	sideColliders.count = 4;
	sideColliders.stride = sizeof( union Collider );

	//createGameLayout( );
	createIntroLayout( );

	playSong( "Music/intro.it" );

	return 1;
}

static int gameScreen_Exit( void )
{
	return 1;
}

static void updatePanjandrumsReady( float dt )
{
	// we'll do it like this:
	//  you currently have D drums in storage
	//  you can have up to N drums in storage
	//  there's a minium time of S
	//  there's a maximum time of T
	//  when at 0 drums it takes T seconds to get a drum
	//  it should take lerp( T, S, ( D / N ) ) to spawn the next drum

	if( drumCount >= MAX_DRUM_COUNT ) {
		return;
	}

	float timeToRecharge = lerp( MAX_DRUM_RECHARGE_TIME, MIN_DRUM_RECHARGE_TIME, (float)drumCount / (float)( MAX_DRUM_COUNT - 1 ) );
	drumRechargeAmount += dt / timeToRecharge;

	if( drumRechargeAmount >= 1.0f ) {
		playSound( rechargeCompleteSnd );
		++drumCount;
		drumRechargeAmount = 0.0f;
	}
}

static int testAndDeductDrum( void )
{
	if( drumCount > 0 ) {
		--drumCount;
		return 1;
	}

	return 0;
}

static void drawPanjandrumsReady( void )
{
	Vector2 drawPos;
	Vector2 scale;
	float baseX = 7.0f;
	float baseY = 450.0f;

	float maxBarHeight = 300.0f;
	float minBarHeight = 150.0f;

	scale.x = 1.0f;
	for( int i = 0; i < drumCount; ++i ) {
		// draw the full bars, white with the icon
		float barHeight = lerp( maxBarHeight, minBarHeight, (float)i / (float)MAX_DRUM_COUNT );
		scale.y = ( barHeight / 15.0f );

		drawPos.y = 600.0f - ( barHeight / 2.0f );
		drawPos.x = baseX;
		queueRenderImage_sv( barBaseImg, 1, drawPos, drawPos, scale, scale, -50 );

		drawPos.y = 600 - barHeight;
		drawPos.x = 10.0f;
		queueRenderImage( drumIconImg, 1, drawPos, drawPos, -60 );
	}

	if( drumCount >= MAX_DRUM_COUNT ) {
		return;
	}

	// draw the currently filling bar, black
	float barHeight = lerp( maxBarHeight, minBarHeight, (float)( drumCount + 1 ) / (float)MAX_DRUM_COUNT );
	barHeight *= drumRechargeAmount;
	scale.y = ( barHeight / 15.0f );

	drawPos.y = 600.0f - ( barHeight / 2.0f );
	drawPos.x = baseX;

	queueRenderImage_sv_c( barBaseImg, 1, drawPos, drawPos, scale, scale, COL_BLACK, COL_BLACK, -70 );
}

static void gameScreen_ProcessEvents( SDL_Event* e )
{
	if( e->type == SDL_MOUSEBUTTONDOWN ) {
		int mouseX;
		int mouseY;
		SDL_GetMouseState( &mouseX, &mouseY );

		if( e->button.button == SDL_BUTTON_RIGHT ) {
			launchFromPosition.x = (float)mouseX;
		} else if( e->button.button == SDL_BUTTON_LEFT ) {
			if( testAndDeductDrum( ) >= 1.0f ) {
				launchPanjandrum( launchFromPosition, launchToPosition );
			}
		}
	}

	if( e->type == SDL_KEYDOWN ) {
		if( e->key.keysym.sym == SDLK_r ) {
			resetEverything( );
		}
	}
}

/* COLLISION RESPONSE */
//typedef void(*CollisionResponse)( int collideeIdx, int colliderIdx, Vector2 separation );
// NOTE: All drums have two collision volumes, so you need to divide the
//  passed in index by 2 to get the actual index for the drum
void drumTroopCollision( int drumIdx, int troopIdx, Vector2 separation )
{
	if( troopImmuneToPanjaDrum( troopIdx ) ) {
		return;
	}

	playSound( troopRunOverSnd );
	gibTroop( troopIdx, separation );
	slowPanjanDrum( ( drumIdx / 2 ), 1.0f );
}

void drumExplosionCollision( int drumIdx, int explosionIdx, Vector2 separation )
{
	panjanDrumHitByExplosion( ( drumIdx / 2 ), separation );
}

void explosionTroopCollision( int explosionIdx, int troopIdx, Vector2 separation )
{
	gibTroop( troopIdx, separation );
}

void drumMineCollision( int drumIdx, int mineIdx, Vector2 separation )
{
	explodeMine( mineIdx );
}

void explosionMineCollision( int explosionIdx, int mineIdx, Vector2 separation )
{
	// only if the explosion is over the center of the mine
	float radius = getMineRadius( );
	radius *= radius;
	if( vec2_MagSqrd( &separation ) >= radius ) {
		explodeMine( mineIdx );
	}
}

void mineTroopCollision( int mineIdx, int troopIdx, Vector2 separation )
{
	// don't explode their own troops
	if( getTroopSide( troopIdx ) == 1 ) {
		return;
	}
	explodeMine( mineIdx );
}

void drumWireCollision( int drumIdx, int wireIdx, Vector2 separation )
{
	destroyBarbedWire( wireIdx );
	slowPanjanDrum( ( drumIdx / 2 ), 100.0f );
}

void wireTroopCollision( int wireIdx, int troopIdx, Vector2 separation )
{
	// barbed wire doesn't hurt it's own side
	if( getTroopSide( troopIdx ) == 1 ) {
		return;
	}

	killTroop( troopIdx, separation );
}

void pitTroopCollision( int pitIdx, int troopIdx, Vector2 seperation )
{
	if( !pitInhabited( pitIdx ) ) {
		enterPit( troopIdx, pitIdx );
	}
}

void sideTroopCollision( int sideIdx, int troopIdx, Vector2 separation )
{
	int troopSide = getTroopSide( troopIdx );
	if( troopSide == sideIdx ) {
		return;
	}

	if( sideIdx == OUR_SIDE ) {
		--attackerHealth;
		playSound( attackersHitSnd );
		if( attackerHealth <= 0 ) {
			if( introMode ) {
				attackerHealth = MAX_HEALTH;
			} else {
				gameOverWon = 0;
				gsmEnterState( &globalGSM, &gameOverScreenState );
			}
		}
	} else if( sideIdx == THEIR_SIDE ) {
		--defenderHealth;
		playSound( defendersHitSnd );
		if( defenderHealth <= 0 ) {
			if( introMode ) {
				defenderHealth = MAX_HEALTH;
			} else {
				gameOverWon = 1;
				gsmEnterState( &globalGSM, &gameOverScreenState );
			}
		}
	}

	destroyTroop( troopIdx );
}

void startExplosionCollision( int startIdx, int explosionIdx, Vector2 separation )
{
	createGameLayout( );
}

/* END COLLISION RESPONSE */

static void gameScreen_Process( void )
{
	getLaunchToPosition( );
	processTroops( );

	// check all collisions
	detectAllCollisions( panjandrumColliders, troopColliders, drumTroopCollision );
	detectAllCollisions( panjandrumColliders, explosionColliders, drumExplosionCollision );
	detectAllCollisions( explosionColliders, troopColliders, explosionTroopCollision );
	detectAllCollisions( panjandrumColliders, barbedWireColliders, drumWireCollision );
	detectAllCollisions( panjandrumColliders, mineColliders, drumMineCollision );
	detectAllCollisions( explosionColliders, mineColliders, explosionMineCollision );
	detectAllCollisions( barbedWireColliders, troopColliders, wireTroopCollision );
	detectAllCollisions( mineColliders, troopColliders, mineTroopCollision );
	detectAllCollisions( pitColliders, troopColliders, pitTroopCollision );
	detectAllCollisions( sideColliders, troopColliders, sideTroopCollision );

	if( introMode ) {
		detectCollisions( &startCollider, explosionColliders.firstCollider, explosionColliders.stride, explosionColliders.count,
			startExplosionCollision, 0 );
	}
}

static void gameScreen_Draw( void )
{
	particlesDraw( );
	drawSprites( );
	drawMines( );
	drawBarbedWire( );
	drawPits( );

	queueRenderImage( sandImg, 1, center, center, 100 );
	queueRenderImage( launchFromImg, 1, launchFromPosition, launchFromPosition, 0 );
	queueRenderImage( launchToImg, 1, launchToPosition, launchToPosition, 0 );

	// draw healths
	//  base image is 15x15
	Vector2 barPos;
	Vector2 scale;
	float percentHealth;
	float barScale;

	percentHealth = (float)attackerHealth / (float)MAX_HEALTH;
	barScale = ( 300.0f * percentHealth ) / 15.0f;
	barPos.x = 1000.0f - 7.0f;
	barPos.y = 600.0f - ( ( 15.0f * barScale ) / 2.0f );
	scale.x = 1.0f;
	scale.y = barScale;
	queueRenderImage_sv_c( barBaseImg, 1, barPos, barPos, scale, scale, sideColors[0], sideColors[0], -10 );

	percentHealth = (float)defenderHealth / (float)MAX_HEALTH;
	barScale = ( 300.0f * percentHealth ) / 15.0f;
	barPos.x = 1000.0f - 7.0f;
	barPos.y = ( ( 15.0f * barScale ) / 2.0f );
	scale.x = 1.0f;
	scale.y = barScale;
	queueRenderImage_sv_c( barBaseImg, 1, barPos, barPos, scale, scale, sideColors[0], sideColors[1], -10 );

	// draw recharge bars
	drawPanjandrumsReady( );

	if( introMode ) {
		queueRenderImage( instructionsImg, 1, center, center, -100 );
	}

	/*
	debugRenderPanjandrums( );
	debugRenderTroops( );
	debugRenderExplosions( );
	debugRenderMines( );
	debugRenderBarbedWire( );
	debugRenderPits( );
	collisionCollectionDebugRendering( sideColliders, 1, 1.0f, 1.0f, 1.0f );
	if( introMode ) {
		collisionDebugRendering( &startCollider, 1, 0, 1, 1.0f, 1.0f, 1.0f );
	}
	// */
}

static void spawnTroops( int side, int minCnt, int maxCnt )
{
	int numToSpawn = minCnt + ( rand( ) % ( maxCnt - minCnt ) );

	float baseX;
	if( introMode ) {
		baseX = 500.0f + randFloat( 200.0f, 480.0f ) * sign( randFloat( -1.0f, 1.0f ) );
	} else {
		baseX = randFloat( 20.0f, 980.0f );
	}
	float baseY = ( side == 0 ) ? 650.0f : -60.0f;

	Vector2 spawnPos;
	spawnPos.x = baseX;


	float stepX = ( baseX < 500.0f ) ? 25.0f : -25.0f;
	float signY = ( side == 0 ) ? 1.0f : -1.0f;
	while( numToSpawn > 0 ) {
		spawnPos.y = baseY + ( randFloat( 0.0f, 30.0f ) * signY );
		spawnTroop( spawnPos, side );
		spawnPos.x += stepX + randFloat( -10.0f, 10.0f );
		--numToSpawn;
	}
}

static void gameScreen_PhysicsTick( float dt )
{
	ourSpawnDelay -= dt;
	if( ourSpawnDelay <= 0.0f ) {
		spawnTroops( 0, MIN_ATTACKER_SPAWN, MAX_ATTACKER_SPAWN );
		ourSpawnDelay = randFloat( MIN_SPAWN_DELAY, MAX_SPAWN_DELAY );
	}

	theirSpawnDelay -= dt;
	if( theirSpawnDelay <=0.0f ) {
		spawnTroops( 1, MIN_DEFENDER_SPAWN, MAX_DEFENDER_SPAWN );
		theirSpawnDelay = randFloat( MIN_SPAWN_DELAY, MAX_SPAWN_DELAY );
	}

	updatePanjandrumsReady( dt );

	updatePanjandrums( dt );
	updateTroops( dt );
	updateExplosions( );
	updateMines( dt );
	particlesPhysicsTick( dt );

	updateSprite( launchToSprite, launchToPosition, VEC2_ONE, 0.0f );
}

struct GameState gameScreenState = { gameScreen_Enter, gameScreen_Exit, gameScreen_ProcessEvents,
	gameScreen_Process, gameScreen_Draw, gameScreen_PhysicsTick };