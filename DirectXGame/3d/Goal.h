#pragma once

#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>

/// <summary>
/// ゴール
/// </summary>
class Goal {
public:

	Goal();

	~Goal();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	/// <param name="viewProjection"></param>
	/// <param name="position"></param>
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	
private:

	WorldTransform worldTransform_;            // ワールド変換データ
	Model* model_ = nullptr;                   // モデル
	ViewProjection* viewProjection_ = nullptr; // ViewProjection

};