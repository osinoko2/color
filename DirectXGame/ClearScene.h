#pragma once

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "GameScene.h"

class ClearScene{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	bool IsFinished() const { return finished_; }
private:

	bool finished_ = false;

	DirectXCommon* dxCommon_ = nullptr;
	WorldTransform EnterWorldTransform_;
	ViewProjection viewProjection_;
	//天球
	Skydome* skydome_ = nullptr;
	// 3Dモデル
	Model* modelSkydome_ = nullptr;
	Model* Entermodel_ = nullptr;
	Model* ClearBestmodel_ = nullptr;
};

