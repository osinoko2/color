
#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "GameScene.h"
/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {
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

	//最初の角度[度]
	static inline const float kMoveMotionAngleStart = 5.0f;
	// 最後の角度[度]
	static inline const float kMoveMotionAngleEnd = -5.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kMoveMotionTime = 2.0f;
	//タイマー
	float Timer_ = 0.0f;

	bool finished_ = false;
	DirectXCommon* dxCommon_ = nullptr;
	WorldTransform titleWorldTransform_;
	WorldTransform PushSpaceWorldTransform_;
	ViewProjection viewProjection_;
	ViewProjection pushSpaceViewProjection_;
	Model* titlemodel_ = nullptr;
	Model* pushSpaceModel_ = nullptr;

	//天球
	Skydome* skydome_ = nullptr;
	// 3Dモデル
	Model* modelSkydome_ = nullptr;
};