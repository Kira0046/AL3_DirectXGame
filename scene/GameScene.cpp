#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete spriteBG_;
	delete modelPlayer_;
	delete modelStage_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	//テクスチャ読み込み
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	textureHandlestage_ = TextureManager::Load("stage.jpg");
	textureHandlePlayer_ = TextureManager::Load("player.png");
	//背景スプライト生成
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});
	//プレイヤー
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();
	//ステージ
	modelStage_ = Model::Create();
	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};
	worldTransformStage_.Initialize();

	//カメラ
	viewProjection_.eye = {0, 1, -6};
	viewProjection_.target = {0, 1, 0};
	viewProjection_.Initialize();
}

void GameScene::Update() {

	PlayerUpdate();
	// value_++;
	// std::string strDebug = std::string("Value:") + std::to_string(value_);
	// debugText_->Print(strDebug, 50, 50, 1.0f);
}

void GameScene::PlayerUpdate() { 
	//右移動
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
		//移動制限
		if (worldTransformPlayer_.translation_.x > 4) {
			worldTransformPlayer_.translation_.x = 4;
		}
	}
	//左移動
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
		//移動制限
		if (worldTransformPlayer_.translation_.x < -4) {
			worldTransformPlayer_.translation_.x = -4;
		}
	}
	worldTransformPlayer_.UpdateMatrix();
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
	spriteBG_->Draw();
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

	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandlestage_);

	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
