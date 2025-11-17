#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "GameScene.h"

/// <summary>
/// ルールシーン
/// </summary>
class RuleScene {
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

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 5.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = -5.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalklMotionTime = 1.0f;
	// タイマー
	float Timer_ = 0.0f;

	bool finished_ = false;
	DirectXCommon* dxCommon_ = nullptr;
	WorldTransform WorldTransform_;
	ViewProjection viewProjection_;

	// 天球
	Skydome* skydome_ = nullptr;
	// 3Dモデル
	Model* modelSkydome_ = nullptr;

	uint32_t Rule_ = 0;
	Sprite* sprite_ = nullptr;
};
