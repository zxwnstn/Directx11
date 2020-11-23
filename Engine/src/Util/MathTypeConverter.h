#pragma once

namespace Engine::Util {

	DirectX::XMVECTOR ToXMVector(const vec4& vec);
	DirectX::XMVECTOR ToXMVector(const vec3& vec);
	DirectX::XMVECTOR ToQuaternion(const vec3& vec);
	DirectX::XMVECTOR ToQuaternion(const vec4& vec);
	DirectX::XMFLOAT3 ToXMFloat3(const vec3& vec);
	DirectX::XMFLOAT4 ToXMFloat4(const vec4& vec);
	DirectX::XMMATRIX ToXMMatrix(const mat4& mat);

	vec4 ToVector4(const DirectX::XMVECTOR& vec);
	vec3 ToVector3(const DirectX::XMVECTOR& vec);
	vec4 ToVector4(const DirectX::XMFLOAT4& vec);
	vec3 ToVector3(const DirectX::XMFLOAT3& vec);
	mat4 ToMatrix(const DirectX::XMMATRIX& mat);

}