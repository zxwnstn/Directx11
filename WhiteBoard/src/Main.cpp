#include <iostream>

#include <DirectXMath.h>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>

int main()
{
	DirectX::XMVECTOR rotate{ 0.0f, 0.0f, 0.0f, 0.0f };
	auto rot = DirectX::XMMatrixRotationRollPitchYaw(rotate.m128_f32[0], rotate.m128_f32[1], rotate.m128_f32[2]);


	DirectX::XMVECTOR up{ 0.0f, 1.0f, 0.0f, 0.0f };
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotate.m128_f32[0], rotate.m128_f32[1], rotate.m128_f32[2]);
	up = DirectX::XMVector3TransformCoord(up, rotationMatrix);

	DirectX::XMMATRIX newRotationMatrix = DirectX::XMMatrixRotationAxis(up, 1.14f);
	DirectX::XMVECTOR newRotate;

	newRotate.m128_f32[0] = (float)atan2(newRotationMatrix.r[0].m128_f32[1], newRotationMatrix.r[1].m128_f32[1]);
	newRotate.m128_f32[1] = (float)atan2(newRotationMatrix.r[2].m128_f32[0], newRotationMatrix.r[2].m128_f32[2]);
	newRotate.m128_f32[2] = (float)asin(-newRotationMatrix.r[2].m128_f32[1]);
	DirectX::XMMATRIX rotationMatrix2 = DirectX::XMMatrixRotationRollPitchYaw(newRotate.m128_f32[0], newRotate.m128_f32[1], newRotate.m128_f32[2]);

}