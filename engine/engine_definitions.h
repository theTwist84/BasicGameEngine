#ifndef ENGINE_ENGINE_DEFINITIONS_H 
#define ENGINE_ENGINE_DEFINITIONS_H


enum
{
	k_maximum_string_length = 32,
	k_maximum_long_string_length = 256,

	k_debug_fill_data = 0xBA,

};


typedef char sbyte;
typedef unsigned char byte;
typedef sbyte int8;
typedef byte uint8;
typedef short int16;
typedef unsigned short uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef long long int64;
typedef unsigned long long uint64;

typedef float float32;
typedef double float64;

static_assert(sizeof(int8) == 0x1, "int8 size != 0x1");
static_assert(sizeof(uint8) == 0x1, "uint8 size != 0x1");
static_assert(sizeof(sbyte) == 0x1, "sbyte size != 0x1");
static_assert(sizeof(byte) == 0x1, "byte size != 0x1");
static_assert(sizeof(bool) == 0x1, "bool size != 0x1");
static_assert(sizeof(int16) == 0x2, "int16 size != 0x2");
static_assert(sizeof(uint16) == 0x2, "uint16 size != 0x2");
static_assert(sizeof(int32) == 0x4, "int32 size != 0x4");
static_assert(sizeof(uint32) == 0x4, "uint32 size != 0x4");
static_assert(sizeof(int64) == 0x8, "int64 size != 0x8");
static_assert(sizeof(uint64) == 0x8, "uint64 size != 0x8");
static_assert(sizeof(float32) == 0x4, "float32 size != 0x4");
static_assert(sizeof(float64) == 0x8, "float64 size != 0x8");


#endif