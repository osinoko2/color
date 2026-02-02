#include "ClearScene.h"
#include <numbers>

void ClearScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	EnterWorldTransform_.Initialize();
	viewProjection_.Initialize();
	// 天球の生成
	skydome_ = new Skydome();
	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_,&viewProjection_);
	Entermodel_ = Model::CreateFromOBJ("Enter", true);
	ClearBestmodel_ = Model::CreateFromOBJ("ClearBest", true);
	Clearmodel_ = Model::CreateFromOBJ("StageClear", true);
}

void ClearScene::Update() {

	// 天球の更新
	skydome_->Update();

	Alpha -= numf;

	if (Alpha <= 0.0f && numf > 0.0f) {
		numf = -numf;
	}

	if (Alpha >= 1.0f && numf < 0.0f) {
		numf = -numf;
	}

	Entermodel_->SetAlpha(Alpha);
}

void ClearScene::Draw() {

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	Model::PreDraw(commandList);

	// 天球の描画
	skydome_->Draw();

	Entermodel_->Draw(EnterWorldTransform_, viewProjection_);

	//ClearBestmodel_->Draw(EnterWorldTransform_, viewProjection_);
	
	Clearmodel_->Draw(EnterWorldTransform_, viewProjection_);

	Model::PostDraw();
}