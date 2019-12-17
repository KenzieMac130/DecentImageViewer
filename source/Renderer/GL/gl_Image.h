#include "divCommon.h"
#include "../abstraction.h"

/*
If your GPU doesn't support this extension (Get a better GPU)
0: Convert to an acceptable format on the CPU.
1: Adds hardware packed pixel support.
*/
#define EXPECT_PACKED_PIXEL_EXT 0

/*
If your GPU doesn't support this extension (Get a better GPU)
0: Convert to an acceptable format on the CPU.
1: Adds hardware B10G11R11 format support.
*/
#define EXPECT_PACKED_FLOAT_EXT 0

/*
If your GPU doesn't support this extension (Get a better GPU)
0: Support for these format will be lost!
1: Adds BC1/BC3/BC5 format support.
*/
#define EXPECT_S3TC_EXT 0

/*
If your GPU doesn't support this extension (Written in 2010... Still Get a better GPU)
0: Support for this format will be lost!
1: Adds BC6H/BC7 format support
*/
#define EXPECT_BCPT_EXT 0

/*
1: Allows 64 bit floating points
0: Disallows 64 bit floating points
*/
#define ALLOW_LARGE_FORMATS 1

typedef struct {
	unsigned int texture;
	GpuImageInfo imageInfo;
} GL_Texture;