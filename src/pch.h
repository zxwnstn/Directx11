#pragma once

//dx
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//fbx
#pragma comment(lib, "vendor/Fbx/lib/x64/libfbxsdk-md.lib")
#pragma comment(lib, "vendor/Fbx/lib/x64/libxml2-md.lib")
#pragma comment(lib, "vendor/Fbx/lib/x64/zlib-md.lib")

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

//vendor
#include "../vendor/stb_image/stb_image.h"

#define BIT(x) 1 << x
