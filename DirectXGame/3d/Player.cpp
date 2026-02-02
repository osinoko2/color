#define NOMINMAX
#include "Player.h"
#include "Input.h"
#include "mapChipField.h"
#include <algorithm>
#include <cassert>
#include <numbers>

void Player::Initialize(Model* model, Model* model2, Model* model3, ViewProjection* viewProjection, const Vector3& position) {

	// NULLチェック
	assert(model);

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;
	worldTransform_.translation_.z =0.0f;


	// 引数の内容をメンバ変数に記録
	modelRed_ = model;
	modelYellow_ = model2;
	modelBlue_ = model3;
	viewProjection_ = viewProjection;

	objectColor_.Initialize();
	color_ = { 1,1,1,1 };

	
}

void Player::Update() {

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	//*--------- 1,移動入力------------*/
	MoveInput();

	//*----------2,移動量を加味して衝突判定する------------*
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.movement_ = velocity_;
	collisionMapInfo.LandingFlag = false;
	collisionMapInfo.WallConstactFlag = false;
	// マップ衝突チェック
	MapCollision(collisionMapInfo);

	//*----------3,判定結果を反映して移動させる------------*
	JudgmentMove(collisionMapInfo);

	//*----------4,天井に接触している場合の処理------------*
	CeilingContact(collisionMapInfo);

	//*----------5,壁に接触している場合の処理-----------*

	//*----------6,接地状態の切り替え-----------*
	SwitchGrandState(collisionMapInfo);

	//*----------7,旋回制御-----------*
	TurnControll();

	//-----------8,行列計算----------*
	// 行列計算
	worldTransform_.UpdateMatrix();

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		SwithColorState();
	}

	color_.w = std::clamp(nowVal, minVal, maxVal);
	objectColor_.SetColor(color_);
	objectColor_.TransferMatrix();

	if (worldTransform_.translation_.y <= fallLine) {
		isDead_ = true;
	}
}

void Player::Draw() {
	switch (currentColorState) {
	case ColorState::Red:
		// 3Dモデルを描画
		modelRed_->Draw(worldTransform_, *viewProjection_);
		break;
	case ColorState::Blue:
		// 黄色の描画処理
		modelYellow_->Draw(worldTransform_, *viewProjection_);
		break;
	case ColorState::Yellow:
		// 青色の描画処理
		modelBlue_->Draw(worldTransform_, *viewProjection_);
		break;
	}
}

void Player::MoveInput() {


	// 移動入力
	// 接地状態
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHTARROW) || Input::GetInstance()->PushKey(DIK_LEFTARROW) || Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHTARROW) || Input::GetInstance()->PushKey(DIK_D)) {

				// 左移動中の右入力
				if (velocity_.x < staySpeed) {

					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (kLimitAttenuation - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFTARROW) || Input::GetInstance()->PushKey(DIK_A) ){

				// 右移動中の左入力
				if (velocity_.x > staySpeed) {

					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (kLimitAttenuation - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
				}
			}
			// 加速/減速
			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.z += acceleration.z;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (kLimitAttenuation - kAttenuation);
			velocity_.y *= (kLimitAttenuation - kAttenuation);
			velocity_.z *= (kLimitAttenuation - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UPARROW) || Input::GetInstance()->PushKey(DIK_W)) {
			// ジャンプ初速
			velocity_.y += kJumpAcceleration;
			// 空中
		}
	}else {
		// 落下速度
		velocity_.y += -kGravityAcceleration;
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

	}

	//// 移動
	// worldTransform_.translation_.x += velocity_.x;
	// worldTransform_.translation_.y += velocity_.y;
	// worldTransform_.translation_.z += velocity_.z;


}

void Player::CeilingContact(const CollisionMapInfo& info) {
	// 天井に当たった?
	if (info.CeilingCollisionFlag) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = kUsuallyHeight;
	}
}

void Player::SwitchGrandState(const CollisionMapInfo& info) {
	// 接地状態の切り替え処理
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > kUsuallyHeight) {
			// 空中状態の移行
			onGround_ = false;
		}else {
			std::array<Vector3, kNumCorner> positonsNew;

			for (uint32_t i = 0; i < positonsNew.size(); ++i) {
				positonsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement_, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			bool hit = false;

			// 左下点の判定
			MapChipField::IndexSet indexSet;
			indexSet =mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kLeftBottom] + attenuationVector);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kRightBottom] + attenuationVector);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			indexSet =mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kLeftBottom] + attenuationVector);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kRedBlock && currentColorState == ColorState::Red) {
				hit = true;
			}

			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kRightBottom] + attenuationVector);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kRedBlock && currentColorState == ColorState::Red) {
				hit = true;
			}

			indexSet =mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kLeftBottom] + attenuationVector);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlueBlock && currentColorState == ColorState::Blue) {
				hit = true;
			}

			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kRightBottom] + attenuationVector);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlueBlock && currentColorState == ColorState::Blue) {
				hit = true;
			}

			indexSet =mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kLeftBottom] + attenuationVector);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kYellowBlock && currentColorState == ColorState::Yellow) {
				hit = true;
			}

			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kRightBottom] + attenuationVector);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kYellowBlock && currentColorState == ColorState::Yellow) {
				hit = true;
			}

			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	}
	else {
		// 着地
		if (info.LandingFlag) {
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (kLimitAttenuation - kAttenuationLanding);
			// 下方向速度をリセット
			velocity_.y = kUsuallyHeight;
			// 着地状態に切り替える
			onGround_ = true;
		}
	}
}

void Player::SwithColorState()
{
	if (currentColorState == ColorState::Red) {
		currentColorState = ColorState::Blue;
	} else if (currentColorState == ColorState::Blue) {
		currentColorState = ColorState::Yellow;
	} else {
		currentColorState = ColorState::Red;
	}
}

void Player::MapCollision(CollisionMapInfo& info) {
	MapCollisionUp(info);
	MapCollisionDown(info);
	MapCollisionLeft(info);
	MapCollisionRight(info);
}

void Player::MapCollisionUp(CollisionMapInfo& info) {
	// 上昇アリ？
	if (info.movement_.y <= noMove) {
		return;
	}
	// 移動後４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(noMove, info.movement_.y, noMove), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kRedBlock && currentColorState == ColorState::Red) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kRedBlock && currentColorState == ColorState::Red) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlueBlock && currentColorState == ColorState::Blue) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlueBlock && currentColorState == ColorState::Blue) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kYellowBlock && currentColorState == ColorState::Yellow) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kYellowBlock && currentColorState == ColorState::Yellow) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		MapChipField::IndexSet indexSetNow;
		// めり込みを排除する方向に移動量を設定する
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(noHeight, +kHeight / half, noHeight));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(noMove, info.movement_.y + kMovement, noMove) + Vector3(noHeight, kHeight / half, noHeight));
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.movement_.y = std::max(noMove, (rect.bottom - worldTransform_.translation_.y) - ((kHeight / half) + kBlank));
			// 天井に当たったことを記録する
			info.CeilingCollisionFlag = true;
		}
	}
}

void Player::MapCollisionDown(CollisionMapInfo& info) {
	// 下降アリ？
	if (info.movement_.y >= noMove) {
		return;
	}
	// 移動後４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(noMove, info.movement_.y, noMove), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - next);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - next);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - next);
	if (mapChipType == MapChipType::kRedBlock && mapChipTypeNext != MapChipType::kRedBlock && currentColorState == ColorState::Red) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - next);
	if (mapChipType == MapChipType::kRedBlock && mapChipTypeNext != MapChipType::kRedBlock && currentColorState == ColorState::Red) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - next);
	if (mapChipType == MapChipType::kBlueBlock && mapChipTypeNext != MapChipType::kBlueBlock && currentColorState == ColorState::Blue) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - next);
	if (mapChipType == MapChipType::kBlueBlock && mapChipTypeNext != MapChipType::kBlueBlock && currentColorState == ColorState::Blue) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - next);
	if (mapChipType == MapChipType::kYellowBlock && mapChipTypeNext != MapChipType::kYellowBlock && currentColorState == ColorState::Yellow) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - next);
	if (mapChipType == MapChipType::kYellowBlock && mapChipTypeNext != MapChipType::kYellowBlock && currentColorState == ColorState::Yellow) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(noHeight, -kHeight / half, noHeight));
		if (indexSetNow.yIndex != indexSet.yIndex)
		{
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.movement_ + Vector3(noHeight, -kHeight / half, noHeight));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.movement_.y = std::min(noMove, (rect.top - worldTransform_.translation_.y) + ((kHeight / half) + kBlank));
			// 地面に当たったことを記録する
			info.LandingFlag = true;
		}
	}
}

void Player::MapCollisionLeft(CollisionMapInfo& info) {
	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(info.movement_.x, noMove, noMove), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 左上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kRedBlock && currentColorState == ColorState::Red) {
		hit = true;
	}

	// 左上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kRedBlock && currentColorState == ColorState::Red) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlueBlock && currentColorState == ColorState::Blue) {
		hit = true;
	}

	// 左上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlueBlock && currentColorState == ColorState::Blue) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kYellowBlock && currentColorState == ColorState::Yellow) {
		hit = true;
	}

	// 左上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kYellowBlock && currentColorState == ColorState::Yellow) {
		hit = true;
	}

	if (hit) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement_.x = std::max(noMove, (rect.right - worldTransform_.translation_.x) - (kWidth / half + kBlank));
	}

}

void Player::MapCollisionRight(CollisionMapInfo& info) {
	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(info.movement_.x, noMove, noMove), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	// 右下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kRedBlock && currentColorState == ColorState::Red) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kRedBlock && currentColorState == ColorState::Red) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlueBlock && currentColorState == ColorState::Blue) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlueBlock && currentColorState == ColorState::Blue) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kYellowBlock && currentColorState == ColorState::Yellow) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kYellowBlock && currentColorState == ColorState::Yellow) {
		hit = true;
	}

	if (hit) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement_.x = std::min(noMove, (rect.left - worldTransform_.translation_.x) + (kWidth / half + kBlank));
	}
}

void Player::JudgmentMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_.x += info.movement_.x;
	worldTransform_.translation_.y += info.movement_.y;
	worldTransform_.translation_.z += info.movement_.z;
}

void Player::TurnControll() {
	// 旋回制御
	if (turnTimer_ > 0.0f) {
		const float turnSpeed = 1.0f / 60.0f;
		const float angleSpeed = 3.0f;

		turnTimer_ -= turnSpeed;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / half, std::numbers::pi_v<float> * angleSpeed / half};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定するa
		worldTransform_.rotation_.y = -destinationRotationY;
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {
		{+kWidth / half, -kHeight / half, noDepth}, // kRightBottom
		{-kWidth / half, -kHeight / half, noDepth}, // kLeftBottom
		{+kWidth / half, +kHeight / half, noDepth}, // kRightTop
		{-kWidth / half, +kHeight / half, noDepth}  // kleftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

Vector3 Player::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	//  ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

AABB Player::GetAABB()
{
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = { worldPos.x - kWidth / half, worldPos.y - kHeight / half, worldPos.z - kWidth / half };
	aabb.max = { worldPos.x + kWidth / half, worldPos.y + kHeight / half, worldPos.z + kWidth / half };

	return aabb;
}

void Player::OnCollision(const Enemy* enemy)
{
	(void)enemy;


	isDead_ = true;
}

