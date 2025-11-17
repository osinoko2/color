#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugText.h"
#include "AABB.h"

/// <summary>
/// 自キャラ
/// </summary>

class MapChipField;

class Player;

class Enemy {
public:
	enum class ColorState {
		Red,
		Blue,
		Yellow,
	};

	void Initialize(Model* model, Model* model2, Model* model3, ViewProjection* viewProjection, const Vector3& positionconst,ColorState color);

	void Update();

	void Draw();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	

	// AABBを取得
	AABB GetAABB();

	float x, y, z;

	// += 演算子のオーバーロード
	Vector3& operator+=(const Vector3& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
	}

	void OnCollision(const Player* player);


	ColorState currentColorState_ = ColorState::Red;


private:
    // ワールドトランスフォーム
	WorldTransform worldTransform_;
	// モデルのポインタ
	Model* model_ = nullptr;
	Model* model2_ = nullptr;
	Model* model3_ = nullptr;
	// ビュープロジェクションのポインタ
	ViewProjection* viewProjection_ = nullptr;
	// マップチップフィールドのポインタ
	MapChipField* mapChipField_ = nullptr;

	// 歩行の速さ
	//static inline const float kWalkSpeed = 0.01f;
	// 速度
	Vector3 velocity_ = {};

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = -7.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 8.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 5.0f;
	// 経過時間
	float walkTimer_ = 0.0f;

	static inline const float kWidth = 0.8f;

	static inline const float kHeight = 0.8f;

	ObjectColor objectColor_;
	Vector4 color_;
};
