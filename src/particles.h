#ifndef ENGINE_PARTICLES_H
#define ENGINE_PARTICLES_H

#include "Math/Vector2.h"

void initParticles( );
void spawnParticle( Vector2 startPos, Vector2 startVel, float rotation, Vector2 gravity, float drag,
	float lifeTime, float fadeStart, int image, unsigned int camFlags, char layer );
void particlesPhysicsTick( float dt );
void particlesDraw( );

#endif /* inclusion guard */
