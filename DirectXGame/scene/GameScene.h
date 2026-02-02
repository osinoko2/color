#pragma once

#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include "DebugCamera.h"
#include "Skydome.h"
#include "mapChipField.h"
#include "CameraController.h"
#include "Enemy.h"
#include "AABB.h"
#include "DeathParticles.h"
#include "myMath.h"
#include "Goal.h"
#include <vector>
#include "ClearScene.h"


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

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

	void GenerateFieldObjects();

	/// <summary>
	// 全てのあたり判定を行う
	/// </summary>
	void CheckAllCollisions();

	/// <summary>
	/// フェーズの切り替え
	/// </summary>
	void ChangePhase();

	void AdvanceToNextStage();

	void Reset();

	bool IsFinished() const { return finished_; }

	/// <summary>
	// AABB同士の交差判定
	/// </summary>
	bool IsCollision(const AABB& a, const AABB& b) {
		// 重なっているかどうかを判定する条件式
		return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
			(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
			(a.min.z <= b.max.z && a.max.z >= b.min.z);
	}

	bool IsClear()const { return isClear_; }

	// リロード要求フラグのgetter
	//bool GetReloadRequested() { return reloadRequested_; }

	bool GetBackTitleFlag() { return backTitleFlag; }

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデル
	Model* model_ = nullptr;
	Model* modelBlock_ = nullptr;
	Model* modelRedBlock_ = nullptr;// 追加
	Model* modelBlueBlock_ = nullptr;// 追加
	Model* modelYellowBlock_ = nullptr;// 追加
	Model* modelPlayer_ = nullptr;
	Model* modelPlayer2_ = nullptr;
	Model* modelPlayer3_ = nullptr;
	Model* modelEnemy_ = nullptr;
	Model* modelEnemy2_ = nullptr;
	Model* modelEnemy3_ = nullptr;
	Model* modelGoal_ = nullptr;
	Model* modelDeathParticlse_ = nullptr;

	// スプライト
	Sprite* BGsprite_ = nullptr;
	Sprite* choiceSprite1_ = nullptr;
	Sprite* choiceSprite2_ = nullptr;
	uint32_t BG_ = 0;
	uint32_t backGame = 0;
	uint32_t backTitle = 0;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 自キャラ
	Player* player_ = nullptr;


	// 縦横ブロック配列
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// 縦横ブロック配列 赤
	std::vector<std::vector<WorldTransform*>> worldTransformRedBlocks_;

	// 縦横ブロック配列 青
	std::vector<std::vector<WorldTransform*>> worldTransformBlueBlocks_;

	// 縦横ブロック配列 黄
	std::vector<std::vector<WorldTransform*>> worldTransformYellowBlocks_;

	// 縦横ブロック配列 ゴール
	std::vector<std::vector<WorldTransform*>> worldTransformGoal_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	//天球
	Skydome* skydome_ = nullptr;
	// 3Dモデル
	Model* modelSkydome_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	MapChipType mapChipType;

	CameraController* cameraController_=nullptr;

	std::list<Enemy*>enemies_;

	DeathParticles* deathParticles_ = nullptr;

	// ゲームのフェーズ(型)
	enum class Phase{
		kPlay,   // ゲームプレイ
		kPause,  // ポーズ画面
		kDeath,  // デス演出
		kNextStage,
	};

	bool isClear_ = false;

	// ゲームの現在フェーズ(変数)
	Phase phase_;

	bool finished_ = false;

	ObjectColor objectColor_;
	Vector4 color_;

	ObjectColor activeObjectColor_;
	Vector4 activeColor_;

	Vector2 point0_ = {0, 0};

	std::vector<Vector3> enemyPositions;
	std::vector<Enemy::ColorState> enemyColor;

	bool choice[2] = {true, false}; 

	// リロード要求フラグ
	bool reloadRequested_ = false;

	// ポーズフラグ
	bool pauseRequested_ = false;

	bool backTitleFlag = false;
};
