#include "pch.h"
#include "Animation.h"

void AnimationClip::Interpolate(float dt)
{
	using namespace DirectX;
	cur += dt;

	if (cur < KeyFrames.front().start)
	{
		XMVECTOR S = XMLoadFloat3(&KeyFrames.front().Scale);
		XMVECTOR P = XMLoadFloat3(&KeyFrames.front().Translation);
		XMVECTOR Q = XMLoadFloat4(&KeyFrames.front().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&InterpoledMat, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else if (cur >= KeyFrames.back().end)
	{
		XMVECTOR S = XMLoadFloat3(&KeyFrames.back().Scale);
		XMVECTOR P = XMLoadFloat3(&KeyFrames.back().Translation);
		XMVECTOR Q = XMLoadFloat4(&KeyFrames.back().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&InterpoledMat, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else
	{
		for (uint32_t i = 0; i < KeyFrames.size() - 1; ++i)
		{
			if (KeyFrames[i].start <= cur && cur < KeyFrames[i].end)
			{
				float lerpPercent = (cur - KeyFrames[i].start) / (KeyFrames[i].end - KeyFrames[i].start);

				XMVECTOR s0 = XMLoadFloat3(&KeyFrames[i].Scale);
				XMVECTOR s1 = XMLoadFloat3(&KeyFrames[i + 1].Scale);

				XMVECTOR p0 = XMLoadFloat3(&KeyFrames[i].Translation);
				XMVECTOR p1 = XMLoadFloat3(&KeyFrames[i + 1].Translation);

				XMVECTOR q0 = XMLoadFloat4(&KeyFrames[i].RotationQuat);
				XMVECTOR q1 = XMLoadFloat4(&KeyFrames[i + 1].RotationQuat);

				XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
				XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
				XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);

				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4x4(&InterpoledMat, XMMatrixAffineTransformation(S, zero, Q, P));

				break;
			}
		}
	}
}

void Joint::Update(float dt, std::vector<DirectX::XMFLOAT4X4>& finalMat)
{
	Anim["Take 001"].Interpolate(dt);
	finalMat.push_back(Anim["Take 001"].InterpoledMat);
}


void Skeleton::Update(float dt)
{
	using namespace DirectX;

	FinalMat.clear();
	for (auto& joint : Joints)
	{
		joint.Update(dt, FinalMat);
	}
		
	// Premultiply by the bone offset transform to get the final transform.
	for (UINT i = 0; i < Joints.size(); ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&Joints[i].OffsetMat);
		XMMATRIX toRoot = XMLoadFloat4x4(&FinalMat[i]);
		XMMATRIX finalTransform = XMMatrixMultiply(offset, toRoot);
		finalTransform *= XMMatrixScaling(0.01f, 0.01f, 0.01f);

		XMStoreFloat4x4(&FinalMat[i], XMMatrixTranspose(finalTransform));
	}
}

