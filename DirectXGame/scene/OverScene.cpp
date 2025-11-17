#include "OverScene.h"

void OverScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	EnterWorldTransform_.Initialize();
	viewProjection_.Initialize();
	// 天球の生成
	skydome_ = new Skydome();
	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &viewProjection_);
	Entermodel_ = Model::CreateFromOBJ("Enter", true);
	Overmodel_ = Model::CreateFromOBJ("Over", true);
}

void OverScene::Update() {
	// 天球の更新
	skydome_->Update();
}

void OverScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	Model::PreDraw(commandList);

	// 天球の描画
	skydome_->Draw();

	Entermodel_->Draw(EnterWorldTransform_, viewProjection_);

	Overmodel_->Draw(EnterWorldTransform_, viewProjection_);

	Model::PostDraw();
}