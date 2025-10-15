#include "GameScene2.h"
#include "TextureManager.h"
#include "myMath.h"
#include <cassert>
#include <algorithm>

GameScene2::GameScene2() {}

GameScene2::~GameScene2() {
	delete model_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks2_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks3_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks4_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformGoal_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();

	worldTransformBlocks2_.clear();

	worldTransformBlocks3_.clear();

	worldTransformBlocks4_.clear();

	worldTransformGoal_.clear();

	delete debugCamera_;

	delete modelSkydome_;

	delete mapChipField_;

	delete modelPlayer_;

	delete modelPlayer2_;

	delete modelPlayer3_;

	delete modelEnemy_;

	delete modelGoal_;

	delete deathParticles_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
}

void GameScene2::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	//textureHandle_ = TextureManager::Load("uvChecker.png");
	// 3Dモデルの生成
	model_ = Model::Create();
	modelBlock_ = Model::CreateFromOBJ("block", true);
	modelRedBlock_ = Model::CreateFromOBJ("Redblock", true);// 追加
	modelBlueBlock_ = Model::CreateFromOBJ("Blueblock", true);
	modelYellowBlock_ = Model::CreateFromOBJ("Yellowblock", true);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 18);
	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(9, 18);

	// マップチップフィールドの生成
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map2.csv");

	// 自キャラの生成
	player_ = new Player();
	modelPlayer_ = Model::CreateFromOBJ("RedPlayer",true);
	modelPlayer2_ = Model::CreateFromOBJ("BluePlayer",true);
	modelPlayer3_ = Model::CreateFromOBJ("YellowPlayer",true);

	// 自キャラの初期化
	player_->Initialize(modelPlayer_,modelPlayer2_,modelPlayer3_,&viewProjection_,playerPosition);
	player_->SetMapChipField(mapChipField_);


	// 天球の生成
	skydome_ = new Skydome();
	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_,&viewProjection_);

	GenerateBlcoks();

	// カメラコントロールの初期化
	cameraController_ = new CameraController();// 生成
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();
	CameraController::Rect movableArea_ = {12.0f, 100 - 12.0f, 6.0f, 12.0f};
	cameraController_->SetMovableArea(movableArea_);


	modelEnemy_ = Model::CreateFromOBJ("RedEnemy", true);
	modelEnemy2_ = Model::CreateFromOBJ("BlueEnemy",true);
	modelEnemy3_ = Model::CreateFromOBJ("YellowEnemy", true);

	std::vector<Vector3> enemyPositions = {
		{8.0f, 18.0f, 0.0f},
		{8.0f, 16.0f, 0.0f},
		{8.0f, 14.0f, 0.0f},
		{27.5f, 8.0f, 0.0f},  // 新しい敵の位置
		{33.5f, 12.0f, 0.0f},  // 新しい敵の位置
		{30.0f, 8.0f, 0.0f},  // 新しい敵の位置
		{20.0f, 5.0f, 0.0f},  // 新しい敵の位置
		{20.0f, 4.0f, 0.0f},  // 新しい敵の位置
		{20.0f, 6.0f, 0.0f}   // 新しい敵の位置
	};

	std::vector<Enemy::ColorState>enemyColor = {
		Enemy::ColorState::Yellow,
		Enemy::ColorState::Yellow,
		Enemy::ColorState::Yellow,
		Enemy::ColorState::Blue,    // 新しい敵の色
		Enemy::ColorState::Blue,   // 新しい敵の色
		Enemy::ColorState::Blue,  // 新しい敵の色
		Enemy::ColorState::Red,    // 新しい敵の色
		Enemy::ColorState::Blue,   // 新しい敵の色
		Enemy::ColorState::Yellow   // 新しい敵の色
	};

	// 敵
	for (int32_t i= 0; i < 9; ++i) {
		Enemy*newEnemy = new Enemy();

		newEnemy->Initialize(modelEnemy_,modelEnemy2_,modelEnemy3_, &viewProjection_, enemyPositions[i],enemyColor[i]);
		enemies_.push_back(newEnemy);
	}

	modelDeathParticlse_ = Model::CreateFromOBJ("deathParticle", true);

	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(modelDeathParticlse_, &viewProjection_, playerPosition);

	modelGoal_ = Model::CreateFromOBJ("Goal",true);


	phase_ = Phase::kPlay;

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280,720);

	// 色
	objectColor_.Initialize();
	color_ = { 1,1,1,1 };
}

void GameScene2::Update() {

	ChangePhase();

	//AdvanceToNextStage();

	switch (phase_){
	case Phase::kPlay:

		// 自キャラの更新
		player_->Update();

		// 天球の更新
		skydome_->Update();

		// カメラコントローラの更新
		cameraController_->Update();

		color_.w = std::clamp(0.5f, 0.5f, 0.5f);
		objectColor_.SetColor(color_);
		objectColor_.TransferMatrix();

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// 縦横ブロック更新
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko)
					continue;

				// アフィン変換行列の作成
				//(MakeAffineMatrix：自分で作った数学系関数)
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 赤
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks2_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko)
					continue;

				// アフィン変換行列の作成
				//(MakeAffineMatrix：自分で作った数学系関数)
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 青
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks3_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko)
					continue;

				// アフィン変換行列の作成
				//(MakeAffineMatrix：自分で作った数学系関数)
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 黄色
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks4_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko)
					continue;

				// アフィン変換行列の作成
				//(MakeAffineMatrix：自分で作った数学系関数)
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 ゴール
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformGoal_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko)
					continue;

				// アフィン変換行列の作成
				//(MakeAffineMatrix：自分で作った数学系関数)
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// カメラ処理
		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			viewProjection_.matView = cameraController_->GetViewProjection().matView;
			viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクションの転送
			viewProjection_.TransferMatrix();
		}

		if (player_->IsGoalReached()) {
			AdvanceToNextStage(); // 次のステージに進む
			isClear_ = true;
		}

#ifdef _DEBUG
		if (input_->TriggerKey(DIK_SPACE)) {
			if (isDebugCameraActive_ == true)
				isDebugCameraActive_ = false;
			else
				isDebugCameraActive_ = true;
		}
#endif


		// 全てのあたり判定を行う
		CheckAllCollisions();
		break;
	case Phase::kDeath:

		// 天球の更新
		skydome_->Update();


		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// 縦横ブロック更新
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko)
					continue;

				// アフィン変換行列の作成
				//(MakeAffineMatrix：自分で作った数学系関数)
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 赤
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks2_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko)
					continue;

				// アフィン変換行列の作成
				//(MakeAffineMatrix：自分で作った数学系関数)
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 青
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks3_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko)
					continue;

				// アフィン変換行列の作成
				//(MakeAffineMatrix：自分で作った数学系関数)
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 黄色
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks4_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko)
					continue;

				// アフィン変換行列の作成
				//(MakeAffineMatrix：自分で作った数学系関数)
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// カメラ処理
		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			viewProjection_.matView = cameraController_->GetViewProjection().matView;
			viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクションの転送
			viewProjection_.TransferMatrix();
		}

		if (deathParticles_) {
			deathParticles_->Update();
		}
		break;
	}
}


void GameScene2::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dモデル描画
	//	model_->Draw(worldTransform_, viewProjection_, textureHandle_);


	// 天球の描画
	skydome_->Draw();

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	//縦横ブロック描画
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			modelBlock_->Draw(*worldTransformBlockYoko, viewProjection_);
		}
	}

	//縦横ブロック描画　赤
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks2_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			modelRedBlock_->Draw(*worldTransformBlockYoko, viewProjection_,&objectColor_);
		}
	}

	////縦横ブロック描画　青
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks3_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			modelBlueBlock_->Draw(*worldTransformBlockYoko, viewProjection_,&objectColor_);
		}
	}

	//縦横ブロック描画　黄色
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks4_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			modelYellowBlock_->Draw(*worldTransformBlockYoko, viewProjection_,&objectColor_);
		}
	}

	//縦横ブロック描画　ゴール
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformGoal_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			modelGoal_->Draw(*worldTransformBlockYoko, viewProjection_);
		}
	}


	if (!player_->IsDead()) {
		player_->Draw();
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	switch (phase_)
	{
	case GameScene2::Phase::kPlay:
		// 自キャラの描画
		player_->Draw();
		break;
	case GameScene2::Phase::kDeath:
		break;
	}


	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion

}

void GameScene2::GenerateBlcoks()
{
	uint32_t numBlockVirticle = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	worldTransformBlocks_.resize(numBlockVirticle);

	// 要素数を変更する赤
	worldTransformBlocks2_.resize(numBlockVirticle);

	// 要素数を変更する青
	worldTransformBlocks3_.resize(numBlockVirticle);

	// 要素数を変更する黄色
	worldTransformBlocks4_.resize(numBlockVirticle);

	// 要素数を変更する黄色
	worldTransformGoal_.resize(numBlockVirticle);

	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// キューブの生成赤
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformBlocks2_[i].resize(numBlockHorizontal);
	}

	// キューブの生成青
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformBlocks3_[i].resize(numBlockHorizontal);
	}

	// キューブの生成黄色
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformBlocks4_[i].resize(numBlockHorizontal);
	}

	// キューブの生成ゴール
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformGoal_[i].resize(numBlockHorizontal);
	}

	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j,i)==MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}

	// 赤
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j,i)==MapChipType::kRedBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks2_[i][j] = worldTransform;
				worldTransformBlocks2_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}

	// 青
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j,i)==MapChipType::kBlueBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks3_[i][j] = worldTransform;
				worldTransformBlocks3_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}

	// 黄色
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j,i)==MapChipType::kYellowBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks4_[i][j] = worldTransform;
				worldTransformBlocks4_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}

	// ゴール
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j,i)==MapChipType::kGoal) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformGoal_[i][j] = worldTransform;
				worldTransformGoal_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene2::CheckAllCollisions()
{
	{
		// プレイヤーとゴールの当たり判定
		AABB playerAABB = player_->GetAABB();
		for (std::vector<WorldTransform*>& goalLine : worldTransformGoal_) {
			for (WorldTransform* goalTransform : goalLine) {
				if (goalTransform == nullptr)
					continue;

				AABB goalAABB;
				goalAABB.min = {goalTransform->translation_.x - 0.5f, goalTransform->translation_.y - 0.5f, goalTransform->translation_.z - 0.5f};
				goalAABB.max = {goalTransform->translation_.x + 0.5f, goalTransform->translation_.y + 0.5f, goalTransform->translation_.z + 0.5f};

				if (IsCollision(playerAABB, goalAABB)) {
					player_->SetGoalReached(true); // ゴールに到達したことを設定
					return;                        // ゴールに達したので、これ以上の判定は必要ない
				}
			}
		}
#pragma region 自キャラと敵キャラの当たり判定

		// 判定対象1と2の座標
		AABB aabb1, aabb2;


		// 自キャラの座標
		aabb1 = player_->GetAABB();




		for (Enemy* enemy : enemies_) {
			// 敵弾の座標
			aabb2 = enemy->GetAABB();

			// AABB同士の交差判定(
			if (IsCollision(aabb1, aabb2)) {
				if (enemy->currentColorState_ == Enemy::ColorState::Red) {
					if (player_->currentColorState == Player::ColorState::Red) {
						// 自キャラの衝突判定コールバックを呼び出す
						player_->OnCollision(enemy);
					}
				}
				if (enemy->currentColorState_ == Enemy::ColorState::Blue) {
					if (player_->currentColorState == Player::ColorState::Blue) {
						// 自キャラの衝突判定コールバックを呼び出す
						player_->OnCollision(enemy);
					}
				}
				if (enemy->currentColorState_ == Enemy::ColorState::Yellow) {
					if (player_->currentColorState == Player::ColorState::Yellow) {
						// 自キャラの衝突判定コールバックを呼び出す
						player_->OnCollision(enemy);
					}
				}
				// 敵弾の衝突判定コールバックを呼び出す
				enemy->OnCollision(player_);
			}
		}
	}
#pragma endregion
}

void GameScene2::ChangePhase()
{
	switch (phase_){
	case Phase::kPlay:

		// 自キャラの状態をチェック
		if (player_->IsDead()) {
			// 死亡フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			// 自キャラの座標にデスパーティクルを発生、初期化
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(modelDeathParticlse_, &viewProjection_, deathParticlesPosition);
		}

		// 全てのあたり判定を行う
		CheckAllCollisions();

		break;
	case Phase::kDeath:
		// デス演出フェーズの処理
		if (deathParticles_ && deathParticles_->IsFinished()) {
			finished_=true;
		}
		break;
	case Phase::kNextStage:
		finished_ = true;


		break;
	}
}

void GameScene2::AdvanceToNextStage()
{

	phase_ = Phase::kNextStage;
}
