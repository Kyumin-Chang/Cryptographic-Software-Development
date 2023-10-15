#pragma once

typedef unsigned __int64 UINT64;
typedef __int64 SINT64;

typedef unsigned int UINT32;
typedef int SINT32;

typedef unsigned short UINT16;
typedef short SINT16;

typedef unsigned char UINT8;
typedef char SINT8;

typedef struct {
	SINT8 sign;  //1 : positive, -1 : negative, 0 : zero
	UINT32 *dat;
	SINT16 len;
}MPZ;



