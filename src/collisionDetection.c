#include "collisionDetection.h"

#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <float.h>

#include "Graphics/graphics.h"
#include "Math/vector2.h"
#include "Math/MathUtil.h"

/*
Determines if the collider primary overlaps with fixed, and if they do how much primary has to be moved to stop colliding.
*/
int AABBvAABB( union Collider* primary, union Collider* fixed, Vector2* outSeparation )
{
	Vector2 penetration;
	Vector2 sumDim;
	Vector2 diffDim;

	vec2_Add( &( primary->aabb.halfDim ), &( fixed->aabb.halfDim ), &sumDim );
	vec2_Subtract( &( primary->aabb.center ), &( fixed->aabb.center ), &diffDim );
	
	diffDim.v[0] = fabsf( diffDim.v[0] );
	diffDim.v[1] = fabsf( diffDim.v[1] );
	vec2_Subtract( &( sumDim ), &( diffDim ), &penetration );
	if( ( penetration.v[0] <= 0.0f ) || ( penetration.v[1] <= 0.0f ) ) {
		return 0;
	}

	/* collision on both axi, figure out how much we have to move to stop colliding */
	if( penetration.v[1] <= penetration.v[0] ) {
		outSeparation->v[0] = 0.0f;
		if( primary->aabb.center.v[1] < fixed->aabb.center.v[1] ) {
			outSeparation->v[1] = -penetration.v[1];
		} else {
			outSeparation->v[1] = penetration.v[1];
		}
	} else {
		if( primary->aabb.center.v[0] < fixed->aabb.center.v[0] ) {
			outSeparation->v[0] = -penetration.v[0];
		} else {
			outSeparation->v[0] = penetration.v[0];
		}
		outSeparation->v[1] = 0.0f;
	}

	return 1;
}

int CirclevAABB( union Collider* primary, union Collider* fixed, Vector2* outSeparation  )
{
	float xPenetration;
	float yPenetration;
	Vector2 diff;
	Vector2 corner;
	float sumDim;
	float diffLen;
	float separationLen;

	vec2_Subtract( &( primary->aabb.center ), & ( fixed->aabb.center ), &diff );

	/* first check to make sure collision is possible */
	sumDim = primary->circle.radius + fixed->aabb.halfDim.v[0];
	xPenetration = sumDim - fabsf( diff.v[0] );
	if( xPenetration <= 0 ) {
		return 0;
	}

	sumDim = primary->circle.radius + fixed->aabb.halfDim.v[1];
	yPenetration = sumDim - fabsf( diff.v[1] );
	if( yPenetration <= 0 ) {
		return 0;
	}

	/* if the circle is in one of the corner zones for the box then check against that corner
		that's all we should need to check */
	if( ( fabsf( diff.v[0] ) >= fixed->aabb.halfDim.v[0] ) && ( fabsf( diff.v[1] ) >= fixed->aabb.halfDim.v[1] ) ) {
		/* first figure out which corner the circle is closest to */
		if( diff.v[0] < 0.0f ) {
			corner.v[0] = fixed->aabb.center.v[0] - fixed->aabb.halfDim.v[0];
		} else {
			corner.v[0] = fixed->aabb.center.v[0] + fixed->aabb.halfDim.v[0];
		}

		if( diff.v[1] < 0.0f ) {
			corner.v[1] = fixed->aabb.center.v[1] - fixed->aabb.halfDim.v[1];
		} else {
			corner.v[1] = fixed->aabb.center.v[1] + fixed->aabb.halfDim.v[1];
		}

		vec2_Subtract( &( primary->circle.center ), &corner, &diff );

		/* check to see if the distance from the corner is smaller enough for a collision */
		if( vec2_MagSqrd( &diff ) >= ( primary->circle.radius * primary->circle.radius ) ) {
			return 0;
		}

		/* now calculate the actual separation amount needed */
		diffLen = vec2_Mag( &diff );
		separationLen = primary->circle.radius - diffLen;
		outSeparation->v[0] = ( diff.v[0] / diffLen ) * separationLen;
		outSeparation->v[1] = ( diff.v[1] / diffLen ) * separationLen;
	} else {
		/* otherwise just check like a normal the circle was an aabb with half lengths equal to it's radius */
		/* collision on both axi, figure out how much we have to move to stop colliding */
		if( yPenetration <= xPenetration ) {
			outSeparation->v[0] = 0.0f;
			if( primary->aabb.center.v[1] < fixed->aabb.center.v[1] ) {
				outSeparation->v[1] = -yPenetration;
			} else {
				outSeparation->v[1] = yPenetration;
			}
		} else {
			if( primary->aabb.center.v[0] < fixed->aabb.center.v[0] ) {
				outSeparation->v[0] = -xPenetration;
			} else {
				outSeparation->v[0] = xPenetration;
			}
			outSeparation->v[1] = 0.0f;
		}
	}

	return 1;
}

int AABBvCircle( union Collider* primary, union Collider* fixed, Vector2* outSeparation  )
{
	/* use existing code and just flip the separation */
	if( CirclevAABB( fixed, primary, outSeparation ) ) {
		outSeparation->v[0] = -( outSeparation->v[0] );
		outSeparation->v[1] = -( outSeparation->v[1] );
		return 1;
	}

	return 0;
}

int CirclevCircle( union Collider* primary, union Collider* fixed, Vector2* outSeparation )
{
	float radiiSum;
	float dist;

	radiiSum = ( primary->circle.radius + fixed->circle.radius );
	radiiSum *= radiiSum;
	vec2_Subtract( &( primary->circle.center ), &( fixed->circle.center ), outSeparation );
	dist = vec2_MagSqrd( outSeparation );

	if( dist >= radiiSum ) {
		return 0;
	}

	/* lots of square roots, better way to do this? */
	radiiSum = sqrtf( radiiSum );
	dist = sqrtf( dist );
	vec2_Normalize( outSeparation );
	vec2_Scale( outSeparation, ( radiiSum - dist ), outSeparation );

	return 1;
}

int DeactivatedvAny( union Collider* primary, union Collider* fixed, Vector2* outSeparation )
{
	return 0;
}

/* define all the collider functions, the first should always be the responding object, the second the fixed object */
typedef int(*CollisionCheck)( union Collider* primary, union Collider* fixed, Vector2* outSeparation );
CollisionCheck collisionChecks[NUM_COLLIDER_TYPES][NUM_COLLIDER_TYPES] = {
	{ DeactivatedvAny, DeactivatedvAny, DeactivatedvAny },
	{ DeactivatedvAny, AABBvAABB, AABBvCircle },
	{ DeactivatedvAny, CirclevAABB, CirclevCircle }
};

/* raycasting intersection functions */
int RayCastvAABBAxis( float rayStart, float dir, float boxMin, float boxMax, float* tMin, float* tMax )
{
	float ood;
	float t1, t2;
	float swap;

	if( fabsf( dir ) < 0.001f ) {
		/* ray parallel to axis, no hit unless origin within slab */
		if( ( rayStart < boxMin ) || ( rayStart > boxMax ) ) {
			return 0;
		}
	} else {
		/* compute intersection of the ray with the near and far line of the slab */
		ood = 1.0f / dir;
		t1 = ( boxMin - rayStart ) * ood;
		t2 = ( boxMax - rayStart ) * ood;

		/* make t1 the intersection with the near plane, t2 with the far plane */
		if( t1 > t2 ) {
			swap = t1;
			t1 = t2;
			t2 = swap;
		}

		/* compute intersection */
		(*tMin) = MAX( (*tMin), t1 );
		(*tMax) = MIN( (*tMax), t2 );

		if( (*tMin) > (*tMax) ) {
			return 0;
		}
	}

	return 1;
}

int RayCastvAABB( Vector2* start, Vector2* dir, union Collider* collider, float* t, Vector2* pt )
{
	float tMin, tMax;

	tMin = 0.0f;
	tMax = 1.0f;

	if( !RayCastvAABBAxis( start->v[0], dir->v[0],
							( collider->aabb.center.v[0] - collider->aabb.halfDim.v[0] ), ( collider->aabb.center.v[0] + collider->aabb.halfDim.v[0] ),
							&tMin, &tMax ) ) {
		return 0;
	}

	if( !RayCastvAABBAxis( start->v[1], dir->v[1],
							( collider->aabb.center.v[1] - collider->aabb.halfDim.v[1] ), ( collider->aabb.center.v[1] + collider->aabb.halfDim.v[1] ),
							&tMin, &tMax ) ) {
		return 0;
	}

	vec2_AddScaled( start, dir, tMin, pt );
	(*t) = tMin;

	return 1;
}

int RayCastvCircle( Vector2* start, Vector2* dir, union Collider* collider, float* t, Vector2* pt )
{
	Vector2 startCircOrig;
	float a, b, c, discr;


	/* translate the sphere to the origin */
	vec2_Subtract( start, &( collider->circle.center ), &startCircOrig );
	b = 2.0f * 	vec2_DotProduct( &startCircOrig, dir );
	c = vec2_DotProduct( &startCircOrig, &startCircOrig ) - ( collider->circle.radius * collider->circle.radius );

	/* rays origin is outside the circle, and it's pointing away, so no chance of hitting */
	if( ( c > 0.0f ) && ( b > 0.0f ) ) {
		return 0;
	}

	a = vec2_DotProduct( dir, dir );
	discr = ( b * b ) - ( 4 * a * c );

	/* ray missed the circle */
	if( discr < 0.0f ) {
		return 0;
	}

	(*t) = ( -b - sqrtf( discr ) ) / ( 2 * a );

	/* if the ray starts inside the sphere */
	if( (*t) < 0.0f ) {
		(*t) = 0.0f;
	}

	vec2_AddScaled( start, dir, (*t), pt );

	return 1;
}

int RayCastvDeactivated( Vector2* start, Vector2* dir, union Collider* collider, float* t, Vector2* pt )
{
	return 0;
}

typedef int(*RayCastCheck)( Vector2* start, Vector2* dir, union Collider* collider, float* t, Vector2* pt );
RayCastCheck rayCastChecks[NUM_COLLIDER_TYPES] = { RayCastvDeactivated, RayCastvAABB, RayCastvCircle };

/*
Finds and handles the all collisions between mainCollider and the colliders in the list.
 NOTE: You shouldn't modify the passed in list in the response.
*/
void detectCollisions( union Collider* mainCollider, union Collider* listFirst, size_t listStride, int listCount,
	CollisionResponse response, int passThroughIdx )
{
	Vector2 separation = VEC2_ZERO;
	int idx;
	union Collider* current;
	char* data = (char*)listFirst;

	/* if there's no response then there's no reason to detect any collisions */
	if( response == NULL ) {
		return;
	}

	if( mainCollider->type == CT_DEACTIVATED ) {
		return;
	}

	for( idx = 0; idx < listCount; ++idx ) {
		current = (union Collider*)( data + ( idx * listStride ) );

		if( collisionChecks[mainCollider->type][current->type]( mainCollider, current, &separation ) ) {
			response( passThroughIdx, idx, separation );
		}
	}
}

/*
Finds and handles all collisions between both lists of colliders.
 NOTE: You shouldn't modify the passed in list in the response.
*/
void detectAllCollisions( CollisionCollection firstList, CollisionCollection secondList, CollisionResponse response )
{
	Vector2 separation = VEC2_ZERO;
	int firstIdx, secondIdx;
	union Collider* firstCurrent;
	union Collider* secondCurrent;
	char* firstData = (char*)firstList.firstCollider;
	char* secondData = (char*)secondList.firstCollider;

	/* no response means no reason to even check */
	if( response == NULL ) {
		return;
	}

	for( firstIdx = 0; firstIdx < firstList.count; ++firstIdx ) {
		firstCurrent = (union Collider*)( firstData + ( firstIdx * firstList.stride ) );
		for( secondIdx = 0; secondIdx < secondList.count; ++secondIdx ) {
			secondCurrent = (union Collider*)( secondData + ( secondIdx * secondList.stride ) );

			if( collisionChecks[firstCurrent->type][secondCurrent->type]( firstCurrent, secondCurrent, &separation ) ) {
				response( firstIdx, secondIdx, separation );
			}
		}
	}
}

/*
Finds if the specified line segment hits anything in the list. Returns 1 if it did, 0 otherwise. Puts the
 collision point into out, if out is NULL it'll exit once it detects any collision instead of finding the first.
*/
int detectRayCastHit( Vector2 start, Vector2 end, union Collider* listFirst, size_t listStride, int listCount, Vector2* out )
{
	int idx;
	union Collider* current;
	Vector2 dir;
	char* data = (char*)listFirst;
	float t = 0.0f;
	Vector2 collPt = VEC2_ZERO;
	float currClosestT = 1.0f;
	int ret = 0;

	vec2_Subtract( &end, &start, &dir );

	for( idx = 0; idx < listCount; ++idx ) {
		current = (union Collider*)( data + ( idx * listStride ) );

		if( rayCastChecks[current->type]( &start, &dir, current, &t, &collPt ) ) {
			if( t <= currClosestT ) {
				ret = 1;
				currClosestT = t;
				if( out == NULL ) {
					return 1;
				}
				(*out) = collPt;
			}
		}
	}

	return ret;
}

/*
Renders all the collider boxes.
*/
void collisionDebugRendering( union Collider* first, unsigned int camFlags, size_t stride, int count,
	float red, float green, float blue )
{
	int idx;
	union Collider* current;
	char* data = (char*)first;
	Vector2 size;
	Vector2 position;

	for( idx = 0; idx < count; ++idx ) {
		current = (union Collider*)( data + ( idx * stride ) );
		switch( current->type ) {
		case CT_AABB:
			size.v[0] = current->aabb.halfDim.v[0] * 2.0f;
			size.v[1] = current->aabb.halfDim.v[1] * 2.0f;
			position.v[0] = current->aabb.center.v[0] - current->aabb.halfDim.v[0];
			position.v[1] = current->aabb.center.v[1] - current->aabb.halfDim.v[1];
			queueDebugDraw_AABB( camFlags, position, size, red, green, blue );
			break;
		case CT_CIRCLE:
			queueDebugDraw_Circle( camFlags, current->circle.center, current->circle.radius, red, green, blue );
			break;
		}
	}
}

void collisionCollectionDebugRendering( CollisionCollection colliders, unsigned int camFlags, float red, float green, float blue )
{
	collisionDebugRendering( colliders.firstCollider, camFlags, colliders.stride, colliders.count, red, green, blue );
}