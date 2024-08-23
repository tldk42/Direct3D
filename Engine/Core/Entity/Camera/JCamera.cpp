#include "common_pch.h"
#include "JCamera.h"
#include "Core/Window/Application.h"

uint32_t JCamera::s_CameraNum = 0;

JCamera::JCamera() noexcept
	: mName(std::format(L"JCam_{}", s_CameraNum++)),
	  mView{},
	  mProj{},
	  mDefaultEye(0, 0, 0),
	  mDefaultLookAt(0, 0, 0),
	  mEye(0, 0, 0),
	  mLookAt(0, 0, 0),
	  mYaw(0.f),
	  mPitch(0.f),
	  mFOV(0),
	  mAspect(0),
	  mNearPlane(0),
	  mFarPlane(1),
	  mRotationValue(0.01f),
	  mTranslationValue(5.f),
	  mInputKeyboard()
{
	JCamera::SetViewParams(g_XMZero, g_XMIdentityR2);
	const float aspect = static_cast<float>(MainApp.GetWindowWidth()) / static_cast<float>(MainApp.GetWindowHeight());
	JCamera::SetProjParams(XM_PI / 4, aspect, 1.f, 1000.f);
}

JCamera::JCamera(const JWText& InName)
	: JCamera()
{
	mName = InName;
}

void JCamera::Initialize()
{
	mInputKeyboard.Initialize();
}

void JCamera::Update(float_t DeltaTime)
{

	mInputKeyboard.Update();
	UpdateInput();
	UpdateRotation(DeltaTime);
	UpdateVelocity(DeltaTime);

	float yawDelta   = mRotVelocity.x;
	float pitchDelta = mRotVelocity.y;

	mYaw += yawDelta;
	mPitch += pitchDelta;

	// Limit pitch to straight up or straight down
	mPitch = max(-XM_PI / 2.0f, mPitch);
	mPitch = min(+XM_PI / 2.0f, mPitch);

	XMVECTOR velocity   = XMLoadFloat3(&mVelocity);
	XMVECTOR posDelta   = velocity * DeltaTime;
	XMMATRIX mCameraRot = XMMatrixRotationRollPitchYaw(mPitch, mYaw, 0);
	// Transform vectors based on camera's rotation matrix
	XMVECTOR worldUp       = XMVector3TransformCoord(M_UpVector, mCameraRot);
	XMVECTOR worldAhead    = XMVector3TransformCoord(M_ForwardVector, mCameraRot);
	XMVECTOR posDeltaWorld = XMVector3TransformCoord(posDelta, mCameraRot);


	// Move the lookAt position 
	XMVECTOR vLookAt = XMLoadFloat3(&mLookAt);
	vLookAt += posDeltaWorld;
	XMStoreFloat3(&mLookAt, vLookAt);

	// Update the eye point based on a radius away from the lookAt position
	XMVECTOR vEye = vLookAt - worldAhead;
	XMStoreFloat3(&mEye, vEye);

	// Update the view matrix
	XMMATRIX view = XMMatrixLookAtLH(vEye, vLookAt, worldUp);
	XMStoreFloat4x4(&mView, view);

	XMMATRIX mTrans = XMMatrixTranslation(0, 0, mVelocity.z * DeltaTime);
	XMStoreFloat4x4(&mWorld, mTrans);
}

void JCamera::Release()
{}

void JCamera::Reset()
{
	XMVECTOR defaultEye    = XMLoadFloat3(&mDefaultEye);
	XMVECTOR defaultLookAt = XMLoadFloat3(&mDefaultLookAt);

	XMStoreFloat4x4(&mWorld, XMMatrixIdentity());

	SetViewParams(defaultEye, defaultLookAt);
}

void JCamera::SetViewParams(FXMVECTOR InEyeVec, FXMVECTOR InLookAtVec)
{
	XMStoreFloat3(&mEye, InEyeVec);
	XMStoreFloat3(&mDefaultEye, InEyeVec);

	XMStoreFloat3(&mLookAt, InLookAtVec);
	XMStoreFloat3(&mDefaultLookAt, InLookAtVec);

	XMMATRIX viewMat = XMMatrixLookAtLH(InEyeVec, InLookAtVec, M_UpVector);
	XMStoreFloat4x4(&mView, viewMat);

	XMMATRIX inverseViewMat = XMMatrixInverse(nullptr, viewMat);

	XMFLOAT3 zBasis;
	XMStoreFloat3(&zBasis, inverseViewMat.r[2]);

	mYaw      = atan2f(zBasis.x, zBasis.z);
	float len = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
	mPitch    = -asinf(zBasis.y / len);
}

void JCamera::SetProjParams(float InFOV, float InAspect, float InNearPlane, float InFarPlane)
{
	mFOV       = InFOV;
	mAspect    = InAspect;
	mNearPlane = InNearPlane;
	mFarPlane  = InFarPlane;

	XMMATRIX projMat = XMMatrixPerspectiveFovLH(InFOV, InAspect, InNearPlane, InFarPlane);
	XMStoreFloat4x4(&mProj, projMat);
}


void JCamera::UpdateVelocity(float DeltaTime)
{
	XMVECTOR vMouseDelta  = XMLoadFloat2(&mMouseDelta);
	XMVECTOR vRotVelocity = vMouseDelta * mRotationValue;

	XMStoreFloat2(&mRotVelocity, vRotVelocity);


	XMVECTOR keyboardDirection = XMLoadFloat3(&mInputDirection);
	XMVECTOR acceleration      = keyboardDirection;

	acceleration = XMVector3Normalize(acceleration);

	acceleration *= mTranslationValue;

	if (bMovementDrag)
	{
		if (XMVectorGetX(XMVector3LengthSq(acceleration)) > 0)
		{
			XMStoreFloat3(&mVelocity, acceleration);

			mDragTimer = mTotalDragTimeToZero;

			XMStoreFloat3(&mDragVelocity, acceleration / mDragTimer);
		}
		else
		{
			if (mDragTimer > 0)
			{
				// Drag until timer is <= 0
				XMVECTOR vVelocity     = XMLoadFloat3(&mVelocity);
				XMVECTOR vVelocityDrag = XMLoadFloat3(&mDragVelocity);

				vVelocity -= vVelocityDrag * DeltaTime;

				XMStoreFloat3(&mVelocity, vVelocity);

				mDragTimer -= DeltaTime;
			}
			else
			{
				// Zero velocity
				mVelocity = FVector::ZeroVector;
			}
		}
	}
	else
	{
		XMStoreFloat3(&mVelocity, acceleration);
	}
}


void JCamera::UpdateRotation(float DeltaTime)
{
	if (IsKeyPressed(EKeyCode::LButton) || IsKeyPressed(EKeyCode::RButton))
	{
		float fPercentOfNew = 1.0f / 2.f;
		float fPercentOfOld = 1.0f - fPercentOfNew;
		mMouseDelta.x       = mMouseDelta.x * fPercentOfOld + mInputKeyboard.GetCurMouseDelta().x * fPercentOfNew;
		mMouseDelta.y       = mMouseDelta.y * fPercentOfOld + mInputKeyboard.GetCurMouseDelta().y * fPercentOfNew;

		mRotVelocity.x = mMouseDelta.x * mRotationValue;
		mRotVelocity.y = mMouseDelta.y * mRotationValue;
	}
	else
	{
		mMouseDelta.x  = 0.f;
		mMouseDelta.y  = 0.f;
		mRotVelocity.x = 0.f;
		mRotVelocity.y = 0.f;
	}
}


void JCamera::UpdateInput()
{
	mInputDirection = FVector::ZeroVector;

	if (IsKeyPressed(EKeyCode::W))
		mInputDirection.z += 1.f;
	else if (IsKeyPressed(EKeyCode::S))
		mInputDirection.z -= 1.f;
	if (IsKeyPressed(EKeyCode::D))
		mInputDirection.x += 1.f;
	else if (IsKeyPressed(EKeyCode::A))
		mInputDirection.x -= 1.f;
}
