#include "resources.h"

#include <SDL_log.h>
#include "../Graphics/graphics.h"
#include "../sound.h"

#define LOAD_AND_TEST( file, func, id ) \
	id = func( file ); if( id < 0 ) { \
		SDL_LogError( SDL_LOG_CATEGORY_APPLICATION, "Error loading resource file %s.", file ); }

Color sideColors[2] = { { 0.455f, 0.282f, 0.145f, 1.0f }, { 0.337f, 0.388f, 0.35f, 1.0f } };

int loadAllResources( void )
{
	LOAD_AND_TEST( "Images/sand.png", loadImage, sandImg );

	LOAD_AND_TEST( "Images/launch_from_indicator.png", loadImage, launchFromImg );
	Vector2 launchFromOffset = { 0.0f, 8.0f };
	setImageOffset( launchFromImg, launchFromOffset );

	LOAD_AND_TEST( "Images/launch_to_indicator.png", loadImage, launchToImg );

	LOAD_AND_TEST( "Images/panjandrum.png", loadImage, panjandrumImg );

	LOAD_AND_TEST( "Images/troop_walk.png", loadImage, troopWalkImg );
	LOAD_AND_TEST( "Images/troop_shoot.png", loadImage, troopShootImg );
	LOAD_AND_TEST( "Images/troop_crouch.png", loadImage, troopCrouchImg );
	LOAD_AND_TEST( "Images/troop_dead.png", loadImage, troopDeadImg );

	LOAD_AND_TEST( "Images/mine.png", loadImage, mineImg );
	LOAD_AND_TEST( "Images/barbed_wire.png", loadImage, barbedWireImg );
	LOAD_AND_TEST( "Images/pit.png", loadImage, pitImg );

	LOAD_AND_TEST( "Images/smoke_particle.png", loadImage, smokePrtImg );
	LOAD_AND_TEST( "Images/fire_particle.png", loadImage, firePrtImg );
	LOAD_AND_TEST( "Images/blood_particle.png", loadImage, bloodPrtImg );
	LOAD_AND_TEST( "Images/gib_particle.png", loadImage, gibsPrtImg );
	LOAD_AND_TEST( "Images/muzzle_flash.png", loadImage, muzzleFlashPrtImg );
	LOAD_AND_TEST( "Images/smoke_particle_small.png", loadImage, smallSmokePrtImg );

	LOAD_AND_TEST( "Images/drum_icon.png", loadImage, drumIconImg );
	LOAD_AND_TEST( "Images/bar_base.png", loadImage, barBaseImg );

	LOAD_AND_TEST( "Images/instructions.png", loadImage, instructionsImg );

	LOAD_AND_TEST( "Sounds/attackers_hit.wav", loadSound, attackersHitSnd );
	LOAD_AND_TEST( "Sounds/defenders_hit.wav", loadSound, defendersHitSnd );
	LOAD_AND_TEST( "Sounds/drum_explosion.wav", loadSound, drumExplosionSnd );
	LOAD_AND_TEST( "Sounds/mine_explosion.wav", loadSound, mineExplosionSnd );
	LOAD_AND_TEST( "Sounds/recharge_complete.wav", loadSound, rechargeCompleteSnd );
	LOAD_AND_TEST( "Sounds/shoot.wav", loadSound, shootSnd );
	LOAD_AND_TEST( "Sounds/troop_killed.wav", loadSound, troopKilledSnd );
	LOAD_AND_TEST( "Sounds/troop_gibbed.wav", loadSound, troopRunOverSnd );

	LOAD_AND_TEST( "Images/win_foreground.png", loadImage, winForeImg );
	LOAD_AND_TEST( "Images/lose_foreground.png", loadImage, loseForeImg );
	LOAD_AND_TEST( "Images/press_any.png", loadImage, pressAnyImg );
	LOAD_AND_TEST( "Images/cloud.png", loadImage, cloudImg );

	return 0;
}