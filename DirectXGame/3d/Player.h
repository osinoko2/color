#pragma once

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugText.h"
#include"AABB.h"

class MapChipField;

class Enemy;

/// <summary>
/// 自キャラ
/// </summary>
class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, Model* model2, Model* model3, ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	float x, y, z;

	// += 演算子のオーバーロード
	Vector3& operator+=(const Vector3& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
	}

	//Getter
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3& GetVelocity() {return velocity_; }

	//Setter
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	/// <summary>
	/// 動きの入力
	/// </summary>
	void MoveInput();


	// マップとのあたり判定情報
	struct CollisionMapInfo
	{
		bool CeilingCollisionFlag = false;
		bool LandingFlag = false;
		bool WallConstactFlag = false;
		Vector3 movement_;
	};

	// マップとの当たり判定
	void MapCollision(CollisionMapInfo& info);
	void MapCollisionUp(CollisionMapInfo& info);
	void MapCollisionDown(CollisionMapInfo& info);
	void MapCollisionLeft(CollisionMapInfo& info);
	void MapCollisionRight(CollisionMapInfo& info);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="info"></param>
	void JudgmentMove(const CollisionMapInfo& info);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="info"></param>
	void CeilingContact(const CollisionMapInfo& info);

	/// <summary>
	///	回転判断
	/// </summary>
	void TurnControll();

	/// <summary>
	/// 着地判定
	/// </summary>
	/// <param name="info"></param>
	void SwitchGrandState(const CollisionMapInfo& info);

	/// <summary>
	/// 色判定
	/// </summary>
	void SwitchColorState();

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 角
	enum Corner{
		kRightBottom,    // 右下
		kLeftBottom,     // 左下
		kRightTop,       // 右上
		kLeftTop,        // 左上

		kNumCorner       // 要素数
	};

	enum class ColorState {
		Red,
		Blue,
		Yellow,
	};

	ColorState currentColorState = ColorState::Red;

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	// ワールド座標を取得
	Vector3 GetWorldPosition();


	// AABBを取得
	AABB GetAABB();

	void OnCollision(const Enemy* enemy);


	// デスフラグのGetter
	bool IsDead()const { return isDead_; }

	void SetDeadFlag(bool isDead) { isDead_ = isDead; } 

	bool IsGoalReached() const { return goalReached_; } // Getter for goalReached_

	void SetGoalReached(bool reached) { goalReached_ = reached; } // Setter for goalReached_

private:
	// デスフラグ
	bool  isDead_ = false;

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* modelRed_ = nullptr;
	Model* modelYellow_ = nullptr;
	Model* modelBlue_ = nullptr;
	ViewProjection* viewProjection_ = nullptr;

	Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.2f;
	static inline const float kLimitRunSpeed = 1.0f;
	static inline const float kLimitAttenuation = 1.0f;

	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.5f;

	// 接地状態フラグ
	bool onGround_ = true;
	// 重力加速度
	static inline const float kGravityAcceleration = 0.05f;
	// 最大落下速度
	static inline const float kLimitFallSpeed = 1.0f;
	// ジャンプ初速
	static inline const float kJumpAcceleration = 0.7f;

	static inline const float kAttenuationLanding = 0.7f;

	static inline const float kUsuallyHeight = 0.0f;


	MapChipField* mapChipField_ = nullptr;
	// キャラクターのあたり判定サイズ
	static inline const float kWidth = 0.8f;

	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 18.0f;

	static inline const float firstSpeed = 1.0f;

	bool goalReached_ = false; // ゴールに達したかどうかのフラグ

	const float nowVal = 0.0f;
	const float minVal = 0.0f;
	const float maxVal = 1.0f;
	const float fallLine = -2.5f;

	static inline const float staySpeed = 0.0f;

	static inline const float noDepth = 0.0f; 

	static inline const float noHeight = 0.0f;

	static inline const float noMove = 0.0f;

	static inline const float kMovement = 0.1f;

	static inline const float half = 2.0f;

	static inline const int next = 1;

	// ゴールしたら  Vector3 initialPosition_;
	/*bool goalHit_ = false;*/

	bool reverseColorState = false; // 色の順番を逆にするかどうか

	ObjectColor objectColor_;
	Vector4 color_;

	Vector3 attenuationVector = {0.0f, -0.1f, 0.0f};
};
