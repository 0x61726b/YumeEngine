




























//---------------------------------------------------------------------------------
#ifndef __YumeConfig_h__
#define __YumeConfig_h__
//---------------------------------------------------------------------------------
#include "Core/YumeBuildSettings.h"
//---------------------------------------------------------------------------------

#define YUME_MEMORY_ALLOCATOR_STD 1
#define YUME_MEMORY_ALLOCATOR_NED 2
#define YUME_MEMORY_ALLOCATOR_USER 3
#define YUME_MEMORY_ALLOCATOR_NEDPOOLING 4

#ifndef YUME_MEMORY_ALLOCATOR
#define YUME_MEMORY_ALLOCATOR YUME_MEMORY_ALLOCATOR_NEDPOOLING
#endif

#define YUME_SSE
#define YUME_THREADING
//---------------------------------------------------------------------------------
#endif
//~End of YumeConfig.h