#ifndef COLOR_H
#define COLOR_H

/*
Standard 32-bit colors. Doesn't handle HDR or any fancy stuff.
*/

#include <stdint.h>

typedef struct {
	union {
		struct {
			float r;
			float g;
			float b;
			float a;
		};
		float col[4];
	};
} Color;

static const Color COL_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
static const Color COL_BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };

Color* col_Lerp( const Color* from, const Color* to, float t, Color* out );

#endif // inclusion guard