#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include "vector2.h"
#include "vector3.h"
#include <stdint.h>

#ifndef M_PI_F
	#define M_PI_F 3.14159265358979323846f
#endif

#define DEG_TO_RAD( d ) ( ( ( d ) * ( (float)M_PI_F / 180.0f ) ) )
#define RAD_TO_DEG( d ) ( ( ( d ) * ( 180.0f / (float)M_PI_F ) ) )

#define MIN( a, b ) ( ((a)>(b))?(b):(a) )
#define MAX( a, b ) ( ((a)>(b))?(a):(b) )

int isPowerOfTwo( int x );
float lerp( float from, float to, float t );
uint8_t lerp_uint8_t( uint8_t from, uint8_t to, float t );
float inverseLerp( float from, float to, float val );
float clamp( float min, float max, float val );
float randFloat( float min, float max );
float sign( float val );
float signWithZero( float val );
Vector3* vec2ToVec3( const Vector2* vec2, float z, Vector3* out );

#endif // inclusion guard