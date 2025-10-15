#pragma once

#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "GameScene2.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "TitleScene.h"
#include "WinApp.h"
#include "ClearScene.h"
#include "Rulescene.h"

GameScene* gameScene = nullptr;
GameScene2* gameScene2 = nullptr;
TitleScene* titleScene = nullptr;
ClearScene* clearScene = nullptr;
RuleScene* ruleScene = nullptr;

enum class Scene {

	kUnknown = 0,

	kTitle,
	kRule,
	kGame,
	kGame2,
	kClear,
};

Scene scene = Scene::kUnknown;

void ChangeScene();

void UpdataScene();

void DrawScene();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"LE3C_08_サクライ_ショウセイ");

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();


	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	// ゲームシーンの初期化
	/*gameScene = new GameScene();
	gameScene->Initialize();*/

	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();

		// シーン切り替え
		ChangeScene();
		// 現在シーン更新
		UpdataScene();
		// 軸表示の更新
		axisIndicator->Update();
		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();
		// ゲームシーンの描画
		//gameScene->Draw();
		// タイトルシーンの描画
		//titleScene->Draw();
		// 現在シーンの描画
		DrawScene();
		// 軸表示の描画
		axisIndicator->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// 各種解放
	delete titleScene;
	delete ruleScene;
	delete gameScene;
	delete gameScene2;
	delete clearScene;

	// 3Dモデル解放
	Model::StaticFinalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}

void ChangeScene()
{
	switch (scene)
	{
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kRule;
			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			ruleScene = new RuleScene;
			ruleScene->Initialize();
		}
		break;
	case Scene::kRule:
		if (ruleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			delete ruleScene;
			ruleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			if (gameScene->IsClear()) {
				// シーン変更
				scene = Scene::kGame2;
				// 旧シーンの解放
				delete gameScene;
				gameScene = nullptr;
				// 新シーンの生成と初期化
				gameScene2 = new GameScene2;
				gameScene2->Initialize();
			}
			else{
				// シーン変更
				scene = Scene::kGame;
				// 旧シーンの解放
				delete gameScene;
				gameScene = nullptr;
				// 新シーンの生成と初期化
				gameScene = new GameScene;
				gameScene->Initialize();
			}
		}
		break;
	case Scene::kGame2:
		if (gameScene2->IsFinished()) {
			if (gameScene2->IsClear()) {
				// シーン変更
				scene = Scene::kClear;
				// 旧シーンの変更
				delete gameScene2;
				gameScene2 = nullptr;
				// 新シーンの生成と初期化
				clearScene = new ClearScene;
				clearScene->Initialize();
			}
			else{
				// シーン変更
				scene = Scene::kGame2;
				// 旧シーンの変更
				delete gameScene2;
				gameScene2 = nullptr;
				// 新シーンの生成と初期化
				gameScene2 = new GameScene2;
				gameScene2->Initialize();
			}
		}
		break;
	case Scene::kClear:
		if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの変更
			delete clearScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	}
}

void UpdataScene()
{
	switch (scene)
	{
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kRule:
		ruleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kGame2:
		gameScene2->Update();
		break;
	case Scene::kClear:
		clearScene->Update();
		break;
	}
}

void DrawScene()
{
	switch (scene)
	{
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kRule:
		ruleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kGame2:
		gameScene2->Draw();
		break;
	case Scene::kClear:
		clearScene->Draw();
		break;
	}
	
}
