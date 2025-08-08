#include <algorithm>
#include "CameraController.h"
#include "Player.h"


// 線形補間関数
template <typename T>
T Lerp(const T& a, const T& b, float t) {
	return a * (1 - t) + b * t;
}

void CameraController::Initialize()
{
	viewProjection_.Initialize();
}

void CameraController::Update()
{
	// 追従対象のワールドトランスフォームを取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	// 追従対象の座標、移動速度とオフセットで目標座標を計算
	dest_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	dest_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias;
	dest_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity.z * kVelocityBias;

	// 座標補間によりゆったり追従
	viewProjection_.translation_.x = Lerp(viewProjection_.translation_.x, dest_.x, kInterpolationRate);
	viewProjection_.translation_.y = Lerp(viewProjection_.translation_.y, dest_.y, kInterpolationRate);
	viewProjection_.translation_.z = Lerp(viewProjection_.translation_.z, dest_.z, kInterpolationRate);

	// 追従対象が画面外に出ないように補正
	viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x, targetWorldTransform.translation_.x + margin.left, targetWorldTransform.translation_.x + margin.right);
	viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, targetWorldTransform.translation_.y + margin.bottom, targetWorldTransform.translation_.y + margin.top);
	// 移動範囲制限
	viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x, movableArea_.left, movableArea_.right);
	viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, movableArea_.bottom, movableArea_.top);

	/*
	viewProjection_.translation_.x =
	max(viewProjection_.translation_.x, targetWorldTransform.translation_.x + margin.left);
	viewProjection_.translation_.x =
	min(viewProjection_.translation_.x, targetWorldTransform.translation_.x + margin.right);
	viewProjection_.translation_.y =
	max(viewProjection_.translation_.y, targetWorldTransform.translation_.y + margin.bottom);
	viewProjection_.translation_.y =
	min(viewProjection_.translation_.y, targetWorldTransform.translation_.y+ margin.top);
	*/
	// 移動範囲制限
	viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x, movableArea_.left, movableArea_.right);
	viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, movableArea_.bottom, movableArea_.top);
	viewProjection_.UpdateMatrix();
}

void CameraController::Reset()
{
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	viewProjection_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	viewProjection_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	viewProjection_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}

ViewProjection CameraController::GetViewPosition()
{
    return ViewProjection();
}

