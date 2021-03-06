#include "particles.h"
#include "Graphics/graphics.h"
#include "Math/MathUtil.h"

#include <string.h>

struct Particle {
	Vector2 currRenderPos;
	Vector2 futureRenderPos;
	Vector2 gravity;
	Vector2 velocity;
	float lifeTime;
	float lifeElapsed;
	float fadeStart;
	Color currColor;
	Color futureColor;
	float currScale;
	float futureScale;
	float drag;
	float rotation;
	int image;
	char layer;
	unsigned int camFlags;
};

#define MAX_NUM_PARTICLES 2048

int lastParticle;
static struct Particle particles[MAX_NUM_PARTICLES];

void initParticles( )
{
	lastParticle = -1;
}

void spawnParticle( Vector2 startPos, Vector2 startVel, float rotation, Vector2 gravity, float drag,
	float lifeTime, float fadeStart, int image, unsigned int camFlags, char layer )
{
	if( lastParticle >= ( MAX_NUM_PARTICLES - 1 ) ) {
		return;
	}

	++lastParticle;
	particles[lastParticle].currRenderPos = startPos;
	particles[lastParticle].futureRenderPos = startPos;
	particles[lastParticle].velocity = startVel;
	particles[lastParticle].gravity = gravity;
	particles[lastParticle].lifeTime = lifeTime;
	particles[lastParticle].fadeStart = MIN( fadeStart, lifeTime );
	particles[lastParticle].lifeElapsed = 0;
	particles[lastParticle].image = image;
	particles[lastParticle].layer = layer;
	particles[lastParticle].camFlags = camFlags;
	particles[lastParticle].drag = drag;

	particles[lastParticle].currColor.r = particles[lastParticle].futureColor.r = 1.0f;
	particles[lastParticle].currColor.g = particles[lastParticle].futureColor.g = 1.0f;
	particles[lastParticle].currColor.b = particles[lastParticle].futureColor.b = 1.0f;
	particles[lastParticle].currColor.a = particles[lastParticle].futureColor.a = 1.0f;

	particles[lastParticle].currScale = particles[lastParticle].futureScale = 1.0f;
}

void particlesPhysicsTick( float dt )
{
	int i;
	float fadeAmt;

	/* update the positions of all the particles */
	for( i = 0; i <= lastParticle; ++i ) {
		particles[i].lifeElapsed += dt;

		fadeAmt = inverseLerp( particles[i].fadeStart, particles[i].lifeTime, particles[i].lifeElapsed );
		particles[i].futureColor.a = lerp( 1.0f, 0.0f, fadeAmt );
		particles[i].futureScale = lerp( 1.0f, 0.0f, fadeAmt );

		particles[i].currRenderPos = particles[i].futureRenderPos;
		vec2_AddScaled( &particles[i].velocity, &particles[i].gravity, dt, &particles[i].velocity );
		vec2_AddScaled( &particles[i].futureRenderPos, &particles[i].velocity, dt, &particles[i].futureRenderPos );
		vec2_Scale( &particles[i].velocity, 1.0f - ( particles[i].drag * dt ), &particles[i].velocity );
	}

	/* destroy all the dead particles, we won't worry about preserving order but should do some tests to see
	    if doing so will make it more efficient */
	for( i = 0; i <= lastParticle; ++i ) {
		if( particles[i].lifeElapsed >= particles[i].lifeTime ) {
			particles[i] = particles[lastParticle];
			--lastParticle;
			--i;
		}
	}
}

void particlesDraw( )
{
	int i;
	Color color = { 255, 255, 255, 255 };

	for( i = 0; i <= lastParticle; ++i ) {
		queueRenderImage_r_c( particles[i].image, particles[i].camFlags, particles[i].currRenderPos, particles[i].futureRenderPos,
			particles[i].rotation, particles[i].rotation, particles[i].currColor, particles[i].futureColor, particles[i].layer );
		particles[i].currRenderPos = particles[i].futureRenderPos;
		particles[i].currColor = particles[i].futureColor;
		particles[i].currScale = particles[i].futureScale;
	}
}