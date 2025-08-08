#include "Rulescene.h"
#include <numbers>
#include <TextureManager.h>

void RuleScene::Initialize() { 
	dxCommon_ = DirectXCommon::GetInstance();
	viewProjection_.Initialize();
	WorldTransform_.Initialize();
	// 天球の生成
	skydome_ = new Skydome();
	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &viewProjection_);

	Rule_ = TextureManager::Load("setumei.png");

	sprite_ = Sprite::Create(Rule_, { 100,-150 });
}

void RuleScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}

	Timer_ += 1.0f / 60.0f;
	float param = std::sin(2.0f * std::numbers::pi_v<float> * Timer_ / kWalklMotionTime);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	WorldTransform_.rotation_.y = radian * (std::numbers::pi_v<float> / 90.0f);
	// 行列計算
	WorldTransform_.UpdateMatrix();

	// 天球の更新
	skydome_->Update();
}

void RuleScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	// 天球の描画
	skydome_->Draw();

	Model::PostDraw();

	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	sprite_->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();
}
