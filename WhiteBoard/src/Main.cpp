#include <iostream>

#include <DirectXMath.h>

int main()
{
	float x = 0.1f;
	float y = 0.1f;
	float z = 0.1f;

	DirectX::XMVECTOR t, r, s, zero;
	t.m128_f32[0] = 1.0f;
	t.m128_f32[1] = 1.0f;
	t.m128_f32[2] = 1.0f;
	t.m128_f32[3] = 0.0f;
	
	zero.m128_f32[0] = 0.0f;
	zero.m128_f32[1] = 0.0f;
	zero.m128_f32[2] = 0.0f;
	zero.m128_f32[3] = 1.0f;

	r = DirectX::XMQuaternionRotationRollPitchYaw(2.0f, 1.0f, 4.0f);
	//r = DirectX::XMMatrixRotationRollPitchYaw(2.0f, 1.0f, 4.0f);
	
	s.m128_f32[0] = 1.0f;
	s.m128_f32[1] = 1.0f;
	s.m128_f32[2] = 1.0f;
	s.m128_f32[3] = 0.0f;
	auto v = DirectX::XMMatrixAffineTransformation(s, zero, r, t);


	auto tm = DirectX::XMMatrixTranslationFromVector(t);
	auto rm = DirectX::XMMatrixRotationRollPitchYaw(2.0f, 1.0f, 4.0f);
	auto sm = DirectX::XMMatrixScalingFromVector(s);
	auto transform = sm * rm * tm;

	auto ret1 = DirectX::XMVector3TransformCoord(t, v);
	auto ret2 = DirectX::XMVector3TransformCoord(t, transform);

}