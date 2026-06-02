#include "GameScene.h"
#include "TextureManager.h"
#include "myMath.h"
#include <cassert>
#include <algorithm>
#include <ImGuiManager.h>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformRedBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlueBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformYellowBlocks_) {
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

	worldTransformRedBlocks_.clear();

	worldTransformBlueBlocks_.clear();

	worldTransformYellowBlocks_.clear();

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

void GameScene::Initialize() {

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
	mapChipField_->LoadMapChipCsv("Resources/map.csv");

	GenerateFieldObjects();

	// 天球の生成
	skydome_ = new Skydome();
	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_,&viewProjection_);

	// 自キャラの生成
	player_ = new Player();
	modelPlayer_ = Model::CreateFromOBJ("RedPlayer", true);
	modelPlayer2_ = Model::CreateFromOBJ("BluePlayer", true);
	modelPlayer3_ = Model::CreateFromOBJ("YellowPlayer", true);

	// 自キャラの初期化
	player_->Initialize(modelPlayer_, modelPlayer2_, modelPlayer3_, &viewProjection_, playerPosition);
	player_->SetMapChipField(mapChipField_);
	player_->SetDeadFlag(false);

	// カメラコントロールの初期化
	cameraController_ = new CameraController();// 生成
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();
	CameraController::Rect movableArea_ = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(movableArea_);
	
	modelEnemy_ = Model::CreateFromOBJ("RedEnemy", true);
	modelEnemy2_ = Model::CreateFromOBJ("BlueEnemy", true);
	modelEnemy3_ = Model::CreateFromOBJ("YellowEnemy", true);

	enemyPositions = {
	    {6.0f,  1.0f, 0.2f},
	    //{6.0f, 3.0f, 0.2f},
	    //{6.0f, 5.0f, 0.2f},
	    //{15.0f, 1.0f, 0.2f},  // 新しい敵の位置
	    //{15.0f, 3.0f, 0.2f},  // 新しい敵の位置
	    //{15.0f, 5.0f, 0.2f},  // 新しい敵の位置
	    {22.0f, 3.0f, 0.2f}, // 新しい敵の位置
	    //{22.0f, 5.0f, 0.2f},  // 新しい敵の位置
	    //{22.0f, 7.0f, 0.2f},   // 新しい敵の位置
	    //{28.0f, 5.0f, 0.2f},  // 新しい敵の位置
	    {28.0f, 7.0f, 0.2f}, // 新しい敵の位置
	    //{28.0f, 9.0f, 0.2f}   // 新しい敵の位置
	};

	enemyColor = {
	    // Enemy::ColorState::Yellow,
	    // Enemy::ColorState::Yellow,
	    Enemy::ColorState::Red,
	    // Enemy::ColorState::Red,    // 新しい敵の色
	    // Enemy::ColorState::Red,   // 新しい敵の色
	    // Enemy::ColorState::Blue,  // 新しい敵の色
	    // Enemy::ColorState::Blue,    // 新しい敵の色
	    Enemy::ColorState::Blue, // 新しい敵の色
	    // Enemy::ColorState::Yellow,   // 新しい敵の色
	    // Enemy::ColorState::Yellow,    // 新しい敵の色
	    Enemy::ColorState::Yellow, // 新しい敵の色
	    // Enemy::ColorState::Red   // 新しい敵の色
	};

	// 敵
	for (int32_t k = 0; k < /*12*/3; ++k) {
		Enemy* newEnemy = new Enemy();

		newEnemy->Initialize(modelEnemy_, modelEnemy2_, modelEnemy3_, &viewProjection_, enemyPositions[k], enemyColor[k]);
		enemies_.push_back(newEnemy);
	}

	modelDeathParticlse_ = Model::CreateFromOBJ("deathParticle", true);

	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(modelDeathParticlse_, &viewProjection_, playerPosition);

	modelGoal_ = Model::CreateFromOBJ("Goal",true);

	BG_ = TextureManager::Load("menuBG.png");
	backGame = TextureManager::Load("menuChoice1.png");
	backTitle = TextureManager::Load("menuChoice2.png");

	phase_ = Phase::kPlay;

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280,720);

	// 色
	objectColor_.Initialize();
	color_ = { 1,1,1,0.5f };

	activeObjectColor_.Initialize();
	activeColor_ = {1, 1, 1, 1};

	BGsprite_ = Sprite::Create(BG_, point0_, {color_});
	choiceSprite1_ = Sprite::Create(backGame, point0_);
	choiceSprite2_ = Sprite::Create(backTitle, point0_);

	backTitleFlag = false;
	choice[0] = true;
	choice[1] = false;
}

void GameScene::Update() {

	ChangePhase();

	switch (phase_){
	case Phase::kPlay:

		// 自キャラの更新
		player_->Update();

		// 天球の更新
		skydome_->Update();

		// カメラコントローラの更新
		cameraController_->Update();

		//color_.w = std::clamp(0.5f, 0.5f, 0.5f);
		objectColor_.SetColor(color_);
		objectColor_.TransferMatrix();

		activeObjectColor_.SetColor(activeColor_);
		activeObjectColor_.TransferMatrix();

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// 縦横ブロック更新
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko) {
					continue;
				}
				
				// アフィン変換行列の作成
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 赤
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformRedBlocks_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko) {
					continue;
				}

				// アフィン変換行列の作成
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 青
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlueBlocks_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko) {
					continue;
				}
				
				// アフィン変換行列の作成
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 黄色
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformYellowBlocks_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko) {
					continue;
				}

				// アフィン変換行列の作成
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 ゴール
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformGoal_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko) {
					continue;
				}

				// アフィン変換行列の作成
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

		if (input_->TriggerKey(DIK_ESCAPE) && pauseRequested_ == false) {
			pauseRequested_ = true;
		}

		if (pauseRequested_ == true) {
			phase_ = Phase::kPause;
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
	case Phase::kPause:
		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W)) {
			choice[0] = true;
			choice[1] = false;
		}

		if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S)) {
			choice[0] = false;
			choice[1] = true;
		}

		if (choice[0] == true && input_->TriggerKey(DIK_RETURN)) {
			pauseRequested_ = false;
			phase_ = Phase::kPlay;
		}

		if (choice[1] == true && input_->TriggerKey(DIK_RETURN)) {
			pauseRequested_ = false;
			backTitleFlag = true;
		}

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
				if (!worldTransformBlockYoko) {
					continue;
				}

				// アフィン変換行列の作成
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 赤
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformRedBlocks_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko) {
					continue;
				}

				// アフィン変換行列の作成
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 青
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlueBlocks_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko) {
					continue;
				}

				// アフィン変換行列の作成
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// 縦横ブロック更新 黄色
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformYellowBlocks_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko) {
					continue;
				}

				// アフィン変換行列の作成
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

	#ifdef DEBUG
	ImGui::
	// リロードボタン
	if (ImGui::) {
		reloadRequested_ = true;
	}
	#endif // DEBUG
}


void GameScene::Draw() {

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

	switch (phase_) {
	case Phase::kPlay:
		// 自キャラの描画
		player_->Draw();
		break;
	case Phase::kPause:
		// 自キャラの描画
		player_->Draw();

		break;
	case Phase::kDeath:
		break;
	}

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	//縦横ブロック描画
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko) {
				continue;
			}

			modelBlock_->Draw(*worldTransformBlockYoko, viewProjection_);
		}
	}

	//縦横ブロック描画　赤
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformRedBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko) 
				continue;
			
			if (player_->currentColorState == Player::ColorState::Red) {
				modelRedBlock_->Draw(*worldTransformBlockYoko, viewProjection_, &activeObjectColor_);
			} else {
				modelRedBlock_->Draw(*worldTransformBlockYoko, viewProjection_, &objectColor_);
			}
		}
	}

	////縦横ブロック描画　青
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlueBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko) {
				continue;
			}

			if (player_->currentColorState == Player::ColorState::Blue) {
				modelBlueBlock_->Draw(*worldTransformBlockYoko, viewProjection_, &activeObjectColor_);
			} else {
				modelBlueBlock_->Draw(*worldTransformBlockYoko, viewProjection_, &objectColor_);
			}		
		}
	}

	//縦横ブロック描画　黄色
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformYellowBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko) {
				continue;
			}

			if (player_->currentColorState == Player::ColorState::Yellow) {
				modelYellowBlock_->Draw(*worldTransformBlockYoko, viewProjection_, &activeObjectColor_);
			} else {
				modelYellowBlock_->Draw(*worldTransformBlockYoko, viewProjection_, &objectColor_);
			}
		}
	}

	//縦横ブロック描画　ゴール
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformGoal_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko) {
				continue;
			}

			modelGoal_->Draw(*worldTransformBlockYoko, viewProjection_);
		}
	}

	if (deathParticles_) {
		deathParticles_->Draw();
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
	
	switch (phase_) {
	case Phase::kPlay:
		break;
	case Phase::kPause:
		BGsprite_->Draw();
		if (choice[0] == true) {
			choiceSprite1_->Draw();
		} else {
			choiceSprite2_->Draw();
		}
		break;
	case Phase::kDeath:
		break;
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion

}

void GameScene::GenerateFieldObjects()
{
	uint32_t numBlockVirticle = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	worldTransformBlocks_.resize(numBlockVirticle);

	// 要素数を変更する赤
	worldTransformRedBlocks_.resize(numBlockVirticle);

	// 要素数を変更する青
	worldTransformBlueBlocks_.resize(numBlockVirticle);

	// 要素数を変更する黄色
	worldTransformYellowBlocks_.resize(numBlockVirticle);

	// 要素数を変更する黄色
	worldTransformGoal_.resize(numBlockVirticle);

	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// キューブの生成赤
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformRedBlocks_[i].resize(numBlockHorizontal);
	}

	// キューブの生成青
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformBlueBlocks_[i].resize(numBlockHorizontal);
	}

	// キューブの生成黄色
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformYellowBlocks_[i].resize(numBlockHorizontal);
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

			/*mapChipType = mapChipField_->GetMapChipTypeByIndex(j, i);

			uint8_t subID = mapChipField_->GetMapChipSubIDByIndex(j, i);

			 switch (mapChipType) {
			case MapChipType::kBlank:
				
				break;
			case MapChipType::kBlock:
				WorldTransform* worldTransform;
				switch (subID) {
				case 1:// 通常ブロック
					worldTransform = new WorldTransform();
					worldTransform->Initialize();
					worldTransformBlocks_[i][j] = worldTransform;
					worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
					break;
				case 2:// 赤いブロック
					worldTransform = new WorldTransform();
					worldTransform->Initialize();
					worldTransformRedBlocks_[i][j] = worldTransform;
					worldTransformRedBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
					break;
				case 3:// 青いブロック
					worldTransform = new WorldTransform();
					worldTransform->Initialize();
					worldTransformBlueBlocks_[i][j] = worldTransform;
					worldTransformBlueBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
					break;
				case 4:// 黄色のブロック
					worldTransform = new WorldTransform();
					worldTransform->Initialize();
					worldTransformYellowBlocks_[i][j] = worldTransform;
					worldTransformYellowBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
					break;
				case 5:// ゴールポイント
					worldTransform = new WorldTransform();
					worldTransform->Initialize();
					worldTransformGoal_[i][j] = worldTransform;
					worldTransformGoal_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
					break;

				case 6:

					break;
				}
				break;
			case MapChipType::kPlayer:
				assert(player_ == nullptr && "自キャラを二重に配置しようとしています");
				// 自キャラの生成
				player_ = new Player();
				modelPlayer_ = Model::CreateFromOBJ("RedPlayer", true);
				modelPlayer2_ = Model::CreateFromOBJ("BluePlayer", true);
				modelPlayer3_ = Model::CreateFromOBJ("YellowPlayer", true);

				// 自キャラの初期化
				player_->Initialize(modelPlayer_, modelPlayer2_, modelPlayer3_, &viewProjection_, playerPosition);
				player_->SetMapChipField(mapChipField_);
				player_->SetDeadFlag(false);

				break;
			case MapChipType::kEnemy:
				switch (subID) {
				case 0:
				modelEnemy_ = Model::CreateFromOBJ("RedEnemy", true);
				modelEnemy2_ = Model::CreateFromOBJ("BlueEnemy", true);
				modelEnemy3_ = Model::CreateFromOBJ("YellowEnemy", true);

				enemyPositions = {
					{6.0f,  1.0f, 0.0f},
					//{6.0f, 3.0f, 0.2f},
					//{6.0f, 5.0f, 0.2f},						//{15.0f, 1.0f, 0.2f},  // 新しい敵の位置
					//{15.0f, 3.0f, 0.2f},  // 新しい敵の位置
					//{15.0f, 5.0f, 0.2f},  // 新しい敵の位置
					{22.0f, 3.0f, 0.0f}, // 新しい敵の位置
					//{22.0f, 5.0f, 0.0f},  // 新しい敵の位置
					//{22.0f, 7.0f, 0.0f},   // 新しい敵の位置
					//{28.0f, 5.0f, 0.0f},  // 新しい敵の位置						{28.0f, 7.0f, 0.0f}, // 新しい敵の位置
					//{28.0f, 9.0f, 0.0f}   // 新しい敵の位置
				};

				enemyColor = {
					// Enemy::ColorState::Yellow,
					// Enemy::ColorState::Yellow,
					Enemy::ColorState::Red,
					// Enemy::ColorState::Red,    // 新しい敵の色
					// Enemy::ColorState::Red,   // 新しい敵の色
					// Enemy::ColorState::Blue,  // 新しい敵の色
					// Enemy::ColorState::Blue,    // 新しい敵の色
					Enemy::ColorState::Blue, // 新しい敵の色
					// Enemy::ColorState::Yellow,   // 新しい敵の色
					// Enemy::ColorState::Yellow,    // 新しい敵の色
					Enemy::ColorState::Yellow, // 新しい敵の色
					// Enemy::ColorState::Red   // 新しい敵の色
					};

				// 敵
				for (int32_t k = 0; k < 3; ++k) {
					Enemy* newEnemy = new Enemy();

					newEnemy->Initialize(modelEnemy_, modelEnemy2_, modelEnemy3_, &viewProjection_, enemyPositions[k], enemyColor[k]);
					enemies_.push_back(newEnemy);
					}
				break;
				}
			break;
			}*/
		}
	}

	// 赤
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j,i)==MapChipType::kRedBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformRedBlocks_[i][j] = worldTransform;
				worldTransformRedBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}

	// 青
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j,i)==MapChipType::kBlueBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlueBlocks_[i][j] = worldTransform;
				worldTransformBlueBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}

	// 黄色
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j,i)==MapChipType::kYellowBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformYellowBlocks_[i][j] = worldTransform;
				worldTransformYellowBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
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

void GameScene::CheckAllCollisions(){
	
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
				if (enemy->currentColorState_ == Enemy::ColorState::Red && player_->currentColorState == Player::ColorState::Red) {
					// 自キャラの衝突判定コールバックを呼び出す
					player_->OnCollision(enemy);
				}
				if (enemy->currentColorState_ == Enemy::ColorState::Blue && player_->currentColorState == Player::ColorState::Blue) {
					// 自キャラの衝突判定コールバックを呼び出す
					player_->OnCollision(enemy);
				}
				if (enemy->currentColorState_ == Enemy::ColorState::Yellow && player_->currentColorState == Player::ColorState::Yellow) {
					// 自キャラの衝突判定コールバックを呼び出す
					player_->OnCollision(enemy);
				}
				// 敵弾の衝突判定コールバックを呼び出す
				enemy->OnCollision(player_);
			}
		}
#pragma endregion
}

void GameScene::ChangePhase()
{
	switch (phase_){
	case Phase::kPlay:

		// 自キャラの状態をチェック
		if (player_->IsDead() == true) {
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
	case Phase::kPause:

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

void GameScene::AdvanceToNextStage()
{

	phase_ = Phase::kNextStage;
}

