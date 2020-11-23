#include "pch.h"

#include "MathTypeConverter.h"

namespace Engine::Util {

	DirectX::XMVECTOR ToXMVector(const vec4 & vec)
	{
		DirectX::XMVECTOR ret;
		ret.m128_f32[0] = vec.x;
		ret.m128_f32[1] = vec.y;
		ret.m128_f32[2] = vec.z;
		ret.m128_f32[3] = vec.w;
		return ret;
	}

	DirectX::XMVECTOR ToXMVector(const vec3 & vec)
	{
		DirectX::XMVECTOR ret;
		ret.m128_f32[0] = vec.x;
		ret.m128_f32[1] = vec.y;
		ret.m128_f32[2] = vec.z;
		ret.m128_f32[3] = 0.0f;
		return ret;
	}

	DirectX::XMVECTOR ToQuaternion(const vec3 & vec)
	{
		DirectX::XMVECTOR ret;
		ret = DirectX::XMQuaternionRotationRollPitchYaw(vec.x, vec.y, vec.z);
		return ret;
	}

	DirectX::XMVECTOR ToQuaternion(const vec4 & vec)
	{
		DirectX::XMVECTOR ret;
		ret = DirectX::XMQuaternionRotationRollPitchYaw(vec.x, vec.y, vec.z);
		return ret;
	}

	DirectX::XMFLOAT3 ToXMFloat3(const vec3 & vec)
	{
		DirectX::XMFLOAT3 ret;
		ret.x = vec.x;
		ret.y = vec.y;
		ret.z = vec.z;
		return ret;
	}

	DirectX::XMFLOAT4 ToXMFloat4(const vec4 & vec)
	{
		DirectX::XMFLOAT4 ret;
		ret.x = vec.x;
		ret.y = vec.y;
		ret.z = vec.z;
		ret.w = vec.w;
		return ret;
	}


	DirectX::XMMATRIX ToXMMatrix(const mat4 & mat)
	{
		DirectX::XMMATRIX ret;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ret.r[i].m128_f32[j] = mat.m[i][j];
			}
		}
		return ret;
	}

	vec4 ToVector4(const DirectX::XMVECTOR & vec)
	{
		vec4 ret;
		ret.x = vec.m128_f32[0];
		ret.y = vec.m128_f32[1];
		ret.z = vec.m128_f32[2];
		ret.w = vec.m128_f32[3];
		return ret;
	}

	vec3 ToVector3(const DirectX::XMVECTOR & vec)
	{
		vec3 ret;
		ret.x = vec.m128_f32[0];
		ret.y = vec.m128_f32[1];
		ret.z = vec.m128_f32[2];
		return ret;
	}

	vec4 ToVector4(const DirectX::XMFLOAT4 & vec)
	{
		vec4 ret;
		ret.x = vec.x;
		ret.y = vec.y;
		ret.z = vec.z;
		ret.w = vec.w;
		return ret;
	}

	vec3 ToVector3(const DirectX::XMFLOAT3 & vec)
	{
		vec3 ret;
		ret.x = vec.x;
		ret.y = vec.y;
		ret.z = vec.z;
		return ret;
	}

	mat4 ToMatrix(const DirectX::XMMATRIX & mat)
	{
		mat4 ret;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ret.m[i][j] = mat.r[i].m128_f32[j];
			}
		}
		return ret;
	}


}