#include "Goal.h"

Goal::Goal() {}

Goal::~Goal() {
	delete model_;
}

void Goal::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {

	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	viewProjection_ = viewProjection;
}

void Goal::Update() {
	// 行列計算
	worldTransform_.UpdateMatrix();
}

void Goal::Draw() {
	model_->Draw(worldTransform_, *viewProjection_);
}
