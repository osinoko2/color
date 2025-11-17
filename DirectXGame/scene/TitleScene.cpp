#include "TitleScene.h"
#include <numbers>

void TitleScene::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();
	titlemodel_ = Model::CreateFromOBJ("title", true);
	pushSpaceModel_ = Model::CreateFromOBJ("PushSpace", true);
	titleWorldTransform_.Initialize();
	viewProjection_.Initialize();
	PushSpaceWorldTransform_.Initialize();
	pushSpaceViewProjection_.Initialize();
	Timer_ = 0.0f;

	// タイトルを近づける
	titleWorldTransform_.translation_ = { 0.0f, 5.0f, -40.0f };  // z値を調整して近づける
	PushSpaceWorldTransform_.translation_ = { 0.0f,0.0f,-20.0f };

	// 天球の生成
	skydome_ = new Skydome();
	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_,&viewProjection_);
	
}

void TitleScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}

	if (titleWorldTransform_.translation_.y > 0.0f) {
		titleWorldTransform_.translation_.y -= 0.1f;
	}

	Timer_ += 1.0f / 60.0f;
	float param = std::sin(2.0f * std::numbers::pi_v<float> * Timer_ / kMoveMotionTime);
	float radian = kMoveMotionAngleStart + kMoveMotionAngleEnd * (param + 1.0f) / 2.0f;
	if(titleWorldTransform_.translation_.y <= 0.0f){
		titleWorldTransform_.rotation_.x = radian * (std::numbers::pi_v<float> / 90.0f);
		titleWorldTransform_.rotation_.y = radian * (std::numbers::pi_v<float> / 90.0f);
	}

	/*if (1.0f > PushSpaceWorldTransform_.translation_.y > -1.0f) {
		if(){
			PushSpaceWorldTransform_.translation_.y -= 0.2f;
		} else if () {
			PushSpaceWorldTransform_.translation_.y += 0.2f;
		}
	}*/

	// 行列計算
	titleWorldTransform_.UpdateMatrix();
	//PushSpaceWorldTransform_.UpdateMatrix();

	// 天球の更新
	skydome_->Update();

}

void TitleScene::Draw()
{
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	titlemodel_->Draw(titleWorldTransform_, viewProjection_);
	pushSpaceModel_->Draw(PushSpaceWorldTransform_, pushSpaceViewProjection_);

	// 天球の描画
	skydome_->Draw();

	Model::PostDraw();

}
