
#pragma once
//EXTERNAL INCLUDES
#include <stdint.h>
#include <string>
//INTERNAL INCLUDES

//Define PI
#define M_PI 3.14159265358979323846f

//Typedefs for easier naming

//byte as unsigned char
typedef unsigned char byte;

//Int pointer
typedef intptr_t iptr;

//Signed integers
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

//Unsinged integers
typedef uint8_t ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef uint64_t ui64;

//Mesh types for standard creation
enum class Meshes
{
	BOX,
	TRIANGLE
};

//Login data without padding
#pragma pack(push, 1)
struct LoginData
{
	char name[32];
	char password[32];
};
#pragma pack(pop)

//Login data without padding
#pragma pack(push, 1)
struct RoomData
{
	char name[32];
	bool created;
	bool deleted;
};
#pragma pack(pop)

//Int vector2
struct iVec2
{
	ui32 x;
	ui32 y;
};

//Int vector3
struct iVec3
{
	ui32 x;
	ui32 y;
	ui32 z;
};

//Color RGB with byte values
struct ColorRGB
{
	byte r;
	byte g;
	byte b;
};

//Color RGBA with byte values
struct ColorRGBA
{
	byte r;
	byte g;
	byte b;
	byte a;
};

//Color RGBA with float values
struct fColorRGBA
{
	float r;
	float g;
	float b;
	float a;
};

//Typedef a fColorRGBA to Vec4
typedef fColorRGBA Vec4;