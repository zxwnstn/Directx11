#pragma once

//dx
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//fbx
#ifdef _DEBUG
	#pragma comment(lib, "../../vendor/FbxSDK/lib/x64/debug/libfbxsdk-md.lib")
#else
	#pragma comment(lib, "../../vendor/FbxSDK/lib/x64/release/libfbxsdk-md.lib")
#endif

//boost
#ifdef _DEBUG
	#pragma comment(lib, "../../vendor/boost/stage/lib/libboost_serialization-vc141-mt-gd-x64-1_70.lib")
#else
	#pragma comment(lib, "../../vendor/boost/stage/lib/libboost_serialization-vc141-mt-x64-1_70.lib")
#endif

#include <Windows.h>

#include <d3d11.h>
#include <d3dcommon.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3dcompiler.h>

//std
#include <iostream>
#include <vector>
#include <type_traits>
#include <unordered_map>
#include <string>
#include <set>
#include <variant>
#include <any>
#include <filesystem>
#include <fstream>
#include <memory>
#include <queue>
#include <algorithm>
#include <thread>

//vendor
#include "stb_image.h"

#define BIT(x) 1 << x
