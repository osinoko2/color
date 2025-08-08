#pragma once

#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>



class Goal {
public:

	Goal();

	~Goal();

	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

	
private:

	WorldTransform worldTransform_;            // ワールド変換データ
	Model* model_ = nullptr;                   // モデル
	ViewProjection* viewProjection_ = nullptr; // ViewProjection

};