﻿#pragma once

#include "Audio.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXMath.h>

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

  private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	//プレイヤー更新宣言
	void PlayerUpdate();
	//ビーム更新宣言
	void BeamUpdate();
	void BeamMove();
	void BeamBorn();
	//敵更新宣言
	void EnemyUpdate();
	void EnemyMove();
	void EnemyBorn();
	//当たり判定宣言
	void Collision();
	//プレイヤー、敵当たり判定宣言
	void CollisionPlayerEnemy();
	//ビーム、敵当たり判定宣言
	void CollisionBeamEnemy();


	//ビーム存在フラグ
	int Beamflag_ = 0;
	//的存在フラグ
	int Enemyflag_ = 1;
	//スコア
	int gameScore_ = 0;
	//プレイヤーライフ
	int playerLife_ = 3;

	/*int32_t value_ = 0;*/
	//テクスチャ宣言
	uint32_t textureHandlePlayer_ = 0;
	uint32_t textureHandleBG_ = 0;
	uint32_t textureHandlestage_ = 0;
	uint32_t textureHandleBeam_ = 0;
	uint32_t textureHandleEnemy_ = 0;
	//スプライト宣言
	Sprite* spriteBG_ = nullptr;
	//モデル宣言
	Model* modelStage_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* modelBeam_ = nullptr;
	Model* modelEnemy_ = nullptr;
	//座標宣言
	WorldTransform worldTransformStage_;
	WorldTransform worldTransformPlayer_;
	WorldTransform worldTransformBeam_;
	WorldTransform worldTransformEnemy_;
	ViewProjection viewProjection_;
};
