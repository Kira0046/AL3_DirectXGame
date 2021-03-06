#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <time.h>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete spriteBG_;
	delete spriteTitle_;
	for (int i = 0; i < 5; i++) {
		delete spriteNumber_[i];
	}
	for (int i = 0; i < 3; i++) {
		delete spritePlayerLife_[i];
	}
	delete spriteScore_;
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
	//サウンド
	soundDataHandleTitleBGM_ = audio_->LoadWave("Audio/Ring05.wav");
	soundDataHandleGamePlayBGM_ = audio_->LoadWave("Audio/Ring08.wav");
	soundDataHandleGameOverBGM_ = audio_->LoadWave("Audio/Ring09.wav");
	soundDataHandleEnemyHitSE_ = audio_->LoadWave("Audio/chord.wav");
	soundDataHandlePlayerHitSE_ = audio_->LoadWave("Audio/tada.wav");
	//タイトルBGM再生
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
	
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
	textureHandlePlayerLife_ = TextureManager::Load("playerLife.png");
	for (int i = 0; i < 3; i++) {
		spritePlayerLife_[i] = Sprite::Create(textureHandlePlayerLife_, {900.0f + i * 50, 5.0f});
	}
	//ステージ
	modelStage_ = Model::Create();
	for (int i = 0; i < 20; i++) {

		worldTransformStage_[i].translation_ = {0, -1.5f, 2.0f * i - 5};
		worldTransformStage_[i].scale_ = {4.5f, 1, 1};
		worldTransformStage_[i].Initialize();
	}
	//ビーム
	modelBeam_ = Model::Create();
	for (int i = 0; i < 10; i++) {

		worldTransformBeam_[i].scale_ = {0.3f, 0.3f, 0.3f};
		worldTransformBeam_[i].Initialize();
	}
	//敵
	modelEnemy_ = Model::Create();
	for (int i = 0; i < 10; i++) {

		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].translation_ = {0, 0, 40};
		worldTransformEnemy_[i].Initialize();
	}
	//カメラ
	viewProjection_.eye = {0, 1, -6};
	viewProjection_.target = {0, 1, 0};
	viewProjection_.Initialize();
	//スコア
	textureHandleNumber_ = TextureManager::Load("number.png");
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i] = Sprite::Create(textureHandleNumber_, {300.0f + i * 26, 0});
	}
	textureHandleScore_ = TextureManager::Load("score.png");
	spriteScore_ = Sprite::Create(textureHandleScore_, {150.0f,0});
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
	StageUpdate();
	Collision();
	if (playerLife_ <= 0) {
		sceneMode_ = 2;
		//サウンド
		audio_->StopWave(voiceHandleBGM_);
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGameOverBGM_, true);
	}

}
//ゲームプレイ表示
void GameScene::GamePlayDraw3D() {
	//ステージ描画
	for (int i = 0; i < 20; i++) {
		modelStage_->Draw(worldTransformStage_[i], viewProjection_, textureHandlestage_);
	}
	//プレイヤー描画
	if (playerTimer_ % 4 < 2) {
		modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	}
	//ビーム描画
	for (int i = 0; i < 10; i++) {

		if (Beamflag_[i] == 1) {
			modelBeam_->Draw(worldTransformBeam_[i], viewProjection_, textureHandleBeam_);
		}
	}
	//敵描画
	for (int i = 0; i < 10; i++) {
		if (Enemyflag_[i] == 1 || Enemyflag_[i] == 2) {
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		}
	}
}

void GameScene::GamePlayDraw2DBack() { 
	spriteBG_->Draw(); 
}

void GameScene::GamePlayDraw2DNear() {
	DrawScore();
	//ゲームスコア
	/*char strS[100];
	sprintf_s(strS, "SCORE:%d", gameScore_);
	debugText_->Print(strS, 200, 10, 2);*/
	//プレイヤーライフ
	/*char strL[100];
	sprintf_s(strL, "LIFE:%d", playerLife_);
	debugText_->Print(strL, 1000, 10, 2);*/

	for (int i = 0; i < playerLife_; i++) {
		spritePlayerLife_[i]->Draw();
	}

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
	//衝突時点滅タイマー現象
	if (playerTimer_ > 0) {
		playerTimer_ -= 1;
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
	//ビーム
	for (int i = 0; i < 10; i++) {

		if (worldTransformBeam_[i].translation_.z > 40) {
			Beamflag_[i] = 0;
		}
		worldTransformBeam_[i].UpdateMatrix();
	}
}
//ビーム移動
void GameScene::BeamMove() {
	for (int i = 0; i < 10; i++) {

		worldTransformBeam_[i].translation_.z += 0.2f;
		worldTransformBeam_[i].rotation_.x += 0.1f;
	}
}
//ビーム発生
void GameScene::BeamBorn() {
	if (beamTimer_ == 0) {

		if (input_->PushKey(DIK_SPACE)) {
			for (int i = 0; i < 10; i++) {

				if (Beamflag_[i] == 0) {
					worldTransformBeam_[i].translation_.x = worldTransformPlayer_.translation_.x;
					worldTransformBeam_[i].translation_.y = worldTransformPlayer_.translation_.y;
					worldTransformBeam_[i].translation_.z = worldTransformPlayer_.translation_.z;

					Beamflag_[i] = 1;
					beamTimer_ = 1;
					break;
				}
			}
		}
	} else {
		beamTimer_++;
		if (beamTimer_ > 20) {
			beamTimer_ = 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////:敵
void GameScene::EnemyUpdate() {
	//移動
	EnemyMove();
	//発生
	EnemyBorn();
	//消滅
	EnemyJump();

	for (int i = 0; i < 10; i++) {
		if (worldTransformEnemy_[i].translation_.z < -5) {
			Enemyflag_[i] = 0;
		}
		worldTransformEnemy_[i].UpdateMatrix();
	}
}
//敵移動
void GameScene::EnemyMove() {
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].translation_.z -= 0.08f;
		worldTransformEnemy_[i].translation_.z -= gameTimer_ / 10000.0f;

		worldTransformEnemy_[i].translation_.x += enemySpeed_[i];
		worldTransformEnemy_[i].rotation_.x -= 0.1f;
		if (worldTransformEnemy_[i].translation_.x > 5.0f) {
			enemySpeed_[i] = -0.1f;
		}
		if (worldTransformEnemy_[i].translation_.x < -5.0f) {
			enemySpeed_[i] = 0.1f;
		}
	}
}
//敵発生
void GameScene::EnemyBorn() {
	if (rand() % 10 == 0) {

		for (int i = 0; i < 10; i++) {

			if (Enemyflag_[i] == 0) {
				worldTransformEnemy_[i].translation_.z = 40;
				worldTransformEnemy_[i].translation_.y = 0;
				enemyJumpSpeed_[i] = 1.0f;
				Enemyflag_[i] = 1;

				if (rand() % 2 == 0) {
					enemySpeed_[i] = 0.1f;
				} else {
					enemySpeed_[i] = -0.1f;
				}

				int x = rand() % 80;
				float x2 = (float)x / 10 - 4;
				worldTransformEnemy_[i].translation_.x = x2;

				break;
			}
		}
	}
	
}
//敵死亡演出
void GameScene::EnemyJump() {
	for (int i = 0; i < 10; i++) {
		if (Enemyflag_[i] == 2) {

			worldTransformEnemy_[i].translation_.y += enemyJumpSpeed_[i];

			enemyJumpSpeed_[i] -= 0.1f;

			worldTransformEnemy_[i].translation_.x += enemySpeed_[i] * 2;

			if (worldTransformEnemy_[i].translation_.y < -3) {
				Enemyflag_[i] = 0;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////:ステージ
void GameScene::StageUpdate() {
	for (int i = 0; i < 20; i++) {
		//移動
		worldTransformStage_[i].translation_.z -= 0.05f;
		
		if (worldTransformStage_[i].translation_.z < -5) {
			worldTransformStage_[i].translation_.z += 40;
		}
		//行列更新
		worldTransformStage_[i].UpdateMatrix();
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
	for (int i = 0; i < 10; i++) {
		if (Enemyflag_[i] == 1) {
			//差計算
			float dx =
			  abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
			float dz =
			  abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);
			//当たり判定
			if (dx < 1 && dz < 1) {
				Enemyflag_[i] = 0;
				playerLife_ -= 1;
				playerTimer_ = 60;
				audio_->PlayWave(soundDataHandlePlayerHitSE_);
			}
		}
	}
}
//ビーム、敵当たり判定
void GameScene::CollisionBeamEnemy() {
	for (int e = 0; e < 10; e++) {

		if (Enemyflag_[e] == 1) {

			for (int b = 0; b < 10; b++) {
				
				if (Beamflag_[b]==1) {

					//差計算
					float bx = abs(
					  worldTransformBeam_[b].translation_.x -
					  worldTransformEnemy_[e].translation_.x);
					float bz = abs(
					  worldTransformBeam_[b].translation_.z -
					  worldTransformEnemy_[e].translation_.z);
					//当たり判定
					if (bx < 1 && bz < 1) {
						//敵死亡演出へ
						Enemyflag_[e] = 2;

						Beamflag_[b] = 0;
						gameScore_ += 1;
						audio_->PlayWave(soundDataHandleEnemyHitSE_);
					}
				}
			}		
		}
	}
}
//スコア
void GameScene::DrawScore() {

	char eachNumber[5] = {};
	int number = gameScore_;

	int keta = 10000;
	for (int i = 0; i < 5; i++) {
		eachNumber[i] = number / keta;
		number = number % keta;
		keta = keta / 10;
	}

	for (int i = 0; i < 5; i++) {
		spriteNumber_[i]->SetSize({32, 64});
		spriteNumber_[i]->SetTextureRect({32.0f * eachNumber[i], 0}, {32, 64});
		spriteNumber_[i]->Draw();
	}
	spriteScore_->Draw();
}

//////////////////////////////////////////////////////////////////////:ゲームタイトル
//タイトル更新
void GameScene::TitleUpdate() { 
	gameTimer_ += 1;
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneMode_ = 1;
		//初期化
		GamePlayStart();
		//サウンド
		audio_->StopWave(voiceHandleBGM_);
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGamePlayBGM_, true);
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
	for (int i = 0; i < 10; i++) {
		Enemyflag_[i] = 0;
	}
	//ビーム
	for (int i = 0; i < 10; i++) {

		Beamflag_[i] = 0;
	}
	//タイマー
	gameTimer_ = 0;
	//点滅タイマー
	playerTimer_ = 0;
}

//////////////////////////////////////////////////////////:ゲームオーバー
//ゲームオーバー更新
void GameScene::GameOverUpdate() { 
	gameTimer_ += 1;
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneMode_ = 0;
		//サウンド
		audio_->StopWave(voiceHandleBGM_);
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
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
	for (int i = 0; i < 20; i++) {
		modelStage_->Draw(worldTransformStage_[i], viewProjection_, textureHandlestage_);
	}
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	for (int i = 0; i < 10; i++) {

		if (Beamflag_[i] == 1) {
			modelBeam_->Draw(worldTransformBeam_[i], viewProjection_, textureHandleBeam_);
		}
	}
	for (int i = 0; i < 10; i++) {
		if (Enemyflag_[i] == 1 || Enemyflag_[i] == 2){
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		}
	}
}