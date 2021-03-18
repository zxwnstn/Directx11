#pragma once

//boost
#ifdef _DEBUG
#pragma comment(lib, "../../vendor/boost/stage/lib/libboost_serialization-vc141-mt-gd-x64-1_70.lib")
#else
#pragma comment(lib, "../../vendor/boost/stage/lib/libboost_serialization-vc141-mt-x64-1_70.lib")
#endif

#include "Engine.h"

extern int g_Width;
extern int g_Height;
