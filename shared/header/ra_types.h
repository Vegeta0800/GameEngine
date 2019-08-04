
#pragma once

// EXTERNAL INCLUDES
#include <stdint.h>

#ifdef DOUBLE_PRECISION
typedef double real;
#else
typedef float real;
#endif

# define M_PI           3.14159265358979323846f

typedef unsigned char byte;

typedef intptr_t iptr;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef uint64_t ui64;

typedef ui32 opSystem;

enum class Meshes
{
	BOX,
	TRIANGLE
};

struct iVec2
{
	ui32 x;
	ui32 y;
};

struct iVec3
{
	ui32 x;
	ui32 y;
	ui32 z;
};

struct ColorRGB
{
	byte r;
	byte g;
	byte b;
};

struct ColorRGBA
{
	byte r;
	byte g;
	byte b;
	byte a;
};

struct fColorRGBA
{
	real r;
	real g;
	real b;
	real a;
};

typedef fColorRGBA Vec4;