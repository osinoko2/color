#include "Enemy.h"
#include <numbers>
#include <algorithm>

void Enemy::Initialize(Model* model, Model* model2, Model* model3, ViewProjection* viewProjection, const Vector3& position,ColorState color)
{
	// モデル変数に記録
	model_ = model;
	model2_ = model2;
	model3_ = model3;
	viewProjection_ = viewProjection;
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.translation_.x += 15.0f;
	worldTransform_.rotation_.y = 3.14f / 2.0f;

	// 速度を設定する
	//velocity_ = { -kWalkSpeed,0,0 };

	// 時間を設定する
	walkTimer_ = 0.0f;

	objectColor_.Initialize();

	currentColorState_ = color;
}

void Enemy::Update()
{
	// 移動
	//worldTransform_.translation_ += velocity_;

	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	float param = std::sin(walkTimer_ * 3.14f);
	float radian = (kWalkMotionAngleStart + kWalkMotionAngleEnd) * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = radian;

	// 行列計算
	worldTransform_.UpdateMatrix();

	color_.w = std::clamp(0.0f, 0.0f, 1.0f);
	objectColor_.SetColor(color_);
	objectColor_.TransferMatrix();
}

void Enemy::Draw()
{
	if (currentColorState_ == ColorState::Red) {
		// 3Dモデルを描画
		model_->Draw(worldTransform_, *viewProjection_);
	}
	if (currentColorState_ == ColorState::Blue) {
		// 3Dモデルを描画
		model2_->Draw(worldTransform_, *viewProjection_);
	}
	if (currentColorState_ == ColorState::Yellow) {
		// 3Dモデルを描画
		model3_->Draw(worldTransform_, *viewProjection_);
	}
}

Vector3 Enemy::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	//  ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

AABB Enemy::GetAABB()
{
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = { worldPos.x - kWidth / 2.0f,worldPos.y - kHeight / 2.0f,worldPos.z - kWidth / 2.0f };
	aabb.max = { worldPos.x + kWidth / 2.0f,worldPos.y + kHeight / 2.0f,worldPos.z + kWidth / 2.0f };

	return aabb;
}

void Enemy::OnCollision(const Player* player)
{
	(void)player;
}

