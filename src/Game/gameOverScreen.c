#include "gameOverScreen.h"

#include "resources.h"
#include "../sound.h"
#include "../Graphics/sprites.h"
#include "../Graphics/graphics.h"
#include "gameScreen.h"
#include "../Math/mathUtil.h"

Color cloudColor;
Vector2 cloudScale;
static int foreground;

Vector2 center = { 500.0f, 300.0f };
Vector2 anyKeyPos = { 500.0f, 550.0f };

typedef struct {
	int sprite;
	Vector2 pos;
	Vector2 vel;
} Cloud;

float timeAlive;

#define NUM_CLOUDS 32
Cloud clouds[NUM_CLOUDS];

void spawnCloud( int idx, float xPos )
{
	if( clouds[idx].sprite != -1 ) {
		destroySprite( clouds[idx].sprite );
	}

	Vector2 pos;
	pos.x = xPos;
	pos.y = randFloat( -20, 400.0f );
	clouds[idx].sprite = createSprite( cloudImg, 1, pos, cloudScale, 0.0f, cloudColor, 10 );
	clouds[idx].pos = pos;
	clouds[idx].vel.x = randFloat( -60.0f, -40.0f );
}

void updateClouds( float dt )
{
	for( int i = 0; i < NUM_CLOUDS; ++i ) {
		vec2_AddScaled( &( clouds[i].pos ), &( clouds[i].vel ), dt, &( clouds[i].pos ) );
		updateSprite( clouds[i].sprite, clouds[i].pos, cloudScale, 0.0f );

		// if the sprite is off screen then spawn it again on the other side
		if( clouds[i].pos.x <= -560.0f ) {
			spawnCloud( i, 1560.0f );
		}
	}
}

static int gameOverScreen_Enter( void )
{
	timeAlive = 0.0f;
	if( gameOverWon ) {
		foreground = winForeImg;
		cloudScale.x = 1.0f;
		cloudScale.y = 1.0f;
		cloudColor = COL_WHITE;

		playSong( "Music/win.it" );

		setRendererClearColor( 0.0f, 0.58f, 1.0f, 1.0f );
	} else {
		foreground = loseForeImg;
		cloudScale.x = 3.0f;
		cloudScale.y = 1.0f;
		cloudColor.r = 0.5f;
		cloudColor.g = 0.5f;
		cloudColor.b = 0.5f;
		cloudColor.a = 1.0f;

		playSong( "Music/lose.it" );

		setRendererClearColor( 0.25f, 0.25f, 0.25f, 1.0f );
	}

	initSprites( );

	createSprite( foreground, 1, center, VEC2_ONE, 0.0f, COL_WHITE, 0 );
	createSprite( pressAnyImg, 1, anyKeyPos, VEC2_ONE, 0.0f, COL_WHITE, -10 );

	for( int i = 0; i < NUM_CLOUDS; ++i ) {
		clouds[i].sprite = -1;
		clouds[i].vel.y = 0.0f;
		spawnCloud( i, randFloat( -500.0f, 1500.0f ) );
	}

	return 1;
}

static int gameOverScreen_Exit( void )
{
	return 1;
}

static void gameOverScreen_ProcessEvents( SDL_Event* e )
{
	if( timeAlive < 1.5f ) {
		return;
	}

	if( ( e->type == SDL_MOUSEBUTTONDOWN ) ||
		( e->type == SDL_KEYDOWN ) ){
		gsmEnterState( &globalGSM, &gameScreenState );
	}
}

static void gameOverScreen_Process( void )
{
}

static void gameOverScreen_Draw( void )
{
	drawSprites( );
}

static void gameOverScreen_PhysicsTick( float dt )
{
	timeAlive += dt;
	updateClouds( dt );
}

struct GameState gameOverScreenState = { gameOverScreen_Enter, gameOverScreen_Exit, gameOverScreen_ProcessEvents,
	gameOverScreen_Process, gameOverScreen_Draw, gameOverScreen_PhysicsTick };