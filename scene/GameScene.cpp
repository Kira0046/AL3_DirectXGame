#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <time.h>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete spriteBG_;
	delete spriteTitle_;
	delete modelPlayer_;
	delete modelStage_;
	delete modelBeam_;
	delete modelEnemy_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	//////////////////////////////////////////////////:ゲーム中
	//テクスチャ読み込み
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	textureHandlestage_ = TextureManager::Load("stage.jpg");
	textureHandlePlayer_ = TextureManager::Load("player.png");
	textureHandleBeam_ = TextureManager::Load("beam.png");
	textureHandleEnemy_ = TextureManager::Load("enemy.png");

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

	//ビーム
	modelBeam_ = Model::Create();
	worldTransformBeam_.scale_ = {0.3f, 0.3f, 0.3f};
	worldTransformBeam_.Initialize();

	//敵
	modelEnemy_ = Model::Create();
	worldTransformEnemy_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformEnemy_.translation_ = {0, 0, 40};
	worldTransformEnemy_.Initialize();

	//カメラ
	viewProjection_.eye = {0, 1, -6};
	viewProjection_.target = {0, 1, 0};
	viewProjection_.Initialize();
	//乱数初期化
	srand(time(NULL));
	/////////////////////////////////////////////////:タイトル
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});

	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 500});

	/////////////////////////////////////////////////:ゲームオーバー
	textureHandleGameOver_ = TextureManager::Load("gameover.png");
	spriteGameOver_ = Sprite::Create(textureHandleGameOver_, {0, 75});
}

void GameScene::Update() {
	
	switch (sceneMode_) {
	case 0:
		TitleUpdate();
		break;
	case 1:
		GamePlayUpdate();
		break;
	case 2:
		GameOverUpdate();
		break;
	}

	// value_++;
	// std::string strDebug = std::string("Value:") + std::to_string(value_);
	// debugText_->Print(strDebug, 50, 50, 1.0f);
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
	
	//GamePlayDraw2DBack();

	switch (sceneMode_) {
	case 1:
		GamePlayDraw2DBack();
		break;
	case 2:
		GameOverDraw2DBack();
		break;
	}


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
	switch (sceneMode_) {
	case 1:
		GamePlayDraw3D();
		break;
	case 2:
		GameOverDraw3D();
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
	switch (sceneMode_) {
	case 0:
		TitleDraw2DNear();
		break;
	case 1:
		GamePlayDraw2DNear();
		break;
	case 2:
		GameOverDraw2DNear();
		break;
	}
	

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

////////////////////////////////////////////////////////:ゲームプレイ
void GameScene::GamePlayUpdate() {
	PlayerUpdate();
	BeamUpdate();
	EnemyUpdate();
	Collision();
	if (playerLife_ <= 0) {
		sceneMode_ = 2;
	}

}
//ゲームプレイ表示
void GameScene::GamePlayDraw3D() {
	//ステージ描画
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandlestage_);
	//プレイヤー描画
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	//ビーム描画
	if (Beamflag_ == 1) {
		modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
	}
	//敵描画
	if (Enemyflag_ == 1) {
		modelEnemy_->Draw(worldTransformEnemy_, viewProjection_, textureHandleEnemy_);
	}
}

void GameScene::GamePlayDraw2DBack() { 
	spriteBG_->Draw(); 
}

void GameScene::GamePlayDraw2DNear() {
	//ゲームスコア
	char strS[100];
	sprintf_s(strS, "SCORE:%d", gameScore_);
	debugText_->Print(strS, 200, 10, 2);
	//プレイヤーライフ
	char strL[100];
	sprintf_s(strL, "LIFE:%d", playerLife_);
	debugText_->Print(strL, 1000, 10, 2);
}



//各種更新
///////////////////////////////////////////////////////////////////////:プレイヤー
//プレイヤー更新
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
////////////////////////////////////////////////////////////////////////:ビーム
//ビーム更新
void GameScene::BeamUpdate() {
	//移動
	BeamMove();
	//発生
	BeamBorn();
	//ビーム消失
	if (worldTransformBeam_.translation_.z > 40) {
		Beamflag_ = 0;
	}
	worldTransformBeam_.UpdateMatrix();
}
//ビーム移動
void GameScene::BeamMove() {

	worldTransformBeam_.translation_.z += 0.2f;
	worldTransformBeam_.rotation_.x += 0.1f;
}
//ビーム発生
void GameScene::BeamBorn() {
	if (input_->PushKey(DIK_SPACE)) {
		if (Beamflag_ == 0) {
			worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
			worldTransformBeam_.translation_.y = worldTransformPlayer_.translation_.y;
			worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;

			Beamflag_ = 1;
		}
	}
}

///////////////////////////////////////////////////////////////////////////:敵
void GameScene::EnemyUpdate() {
	//移動
	EnemyMove();
	//発生
	EnemyBorn();

	if (worldTransformEnemy_.translation_.z < -5) {
		Enemyflag_ = 0;
	}

	worldTransformEnemy_.UpdateMatrix();
}
//敵移動
void GameScene::EnemyMove() {
	worldTransformEnemy_.translation_.z -= 0.2f;
	worldTransformEnemy_.rotation_.x -= 0.1f;
}
//敵発生
void GameScene::EnemyBorn() {
	if (Enemyflag_ == 0) {
		worldTransformEnemy_.translation_.z = 40;
		Enemyflag_ = 1;

		int x = rand() % 80;
		float x2 = (float)x / 10 - 4;
		worldTransformEnemy_.translation_.x = x2;
	}
}

//////////////////////////////////////////////////////////////////////////////:当たり判定
void GameScene::Collision() {
	//プレイヤー、敵当たり判定
	CollisionPlayerEnemy();
	//ビーム、敵当たり判定
	CollisionBeamEnemy();
}
//プレイヤー、敵当たり判定
void GameScene::CollisionPlayerEnemy() {

	if (Enemyflag_ == 1) {
		//差計算
		float dx = abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_.translation_.x);
		float dz = abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_.translation_.z);
		//当たり判定
		if (dx < 1 && dz < 1) {
			Enemyflag_ = 0;
			playerLife_ -= 1;
		}
	}
}
//ビーム、敵当たり判定
void GameScene::CollisionBeamEnemy() {
	if (Enemyflag_ == 1 && Beamflag_ == 1) {
		//差計算
		float bx = abs(worldTransformBeam_.translation_.x - worldTransformEnemy_.translation_.x);
		float bz = abs(worldTransformBeam_.translation_.z - worldTransformEnemy_.translation_.z);
		//当たり判定
		if (bx < 1 && bz < 1) {
			Enemyflag_ = 0;
			Beamflag_ = 0;
			gameScore_ += 1;
		}
	}
}

//////////////////////////////////////////////////////////////////////:ゲームタイトル
//タイトル更新
void GameScene::TitleUpdate() { 
	gameTimer_ += 1;
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneMode_ = 1;
		GamePlayStart();
	}
}
//タイトル表示
void GameScene::TitleDraw2DNear() { 
	spriteTitle_->Draw(); 

	if (gameTimer_ % 80 >= 20) {
		spriteEnter_->Draw();
	}
}
//初期化
void GameScene::GamePlayStart() { 
	//表示
	gameScore_ = 0;
	playerLife_ = 3;
	//プレイヤー
	worldTransformPlayer_.translation_ = {0, 0, 0};
	//敵
	Enemyflag_ = 0;
	//ビーム
	Beamflag_ = 0;
}

//////////////////////////////////////////////////////////:ゲームオーバー
//ゲームオーバー更新
void GameScene::GameOverUpdate() { 
	gameTimer_ += 1;
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneMode_ = 0;
	}
}
//ゲームオーバー表示
void GameScene::GameOverDraw2DNear() { 	
	spriteGameOver_->Draw(); 
	if (gameTimer_ % 80 >= 20) {
		spriteEnter_->Draw();
	}
	char strS[100];
	sprintf_s(strS, "SCORE:%d", gameScore_);
	debugText_->Print(strS, 200, 10, 2);

	char strL[100];
	sprintf_s(strL, "LIFE:%d", playerLife_);
	debugText_->Print(strL, 1000, 10, 2);
}

void GameScene::GameOverDraw2DBack() { 
spriteBG_->Draw();
}

void GameScene::GameOverDraw3D() {
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandlestage_);
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	if (Beamflag_ == 1) {
		modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
	}
	if (Enemyflag_ == 1) {
		modelEnemy_->Draw(worldTransformEnemy_, viewProjection_, textureHandleEnemy_);
	}
}