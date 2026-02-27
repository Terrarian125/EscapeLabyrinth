#pragma once
#include "../Library/GameObject.h"
#include "global.h"
#include <vector>
#include "Compass.h"

class Stage : public GameObject {
public:
    Stage();
    virtual ~Stage();
    void Update() override;
    void Draw() override;

    //外部（敵のAIなど）から道かどうかを判定するための関数
    int GetMazeData(int x, int y) const;

    //デバッグ用
    //当たり判定の有効・無効を切り替える関数
    void SetCollisionEnable(bool enable) { m_collisionEnable = enable; }
    bool GetCollisionEnable() const { return m_collisionEnable; }
    //鍵の状態
    bool GetHasKey() const { return m_hasKey; }
    void SetHasKey(bool has) { m_hasKey = has; }

    //外部から座標を取得するための関数
    VECTOR GetKeyPosition() const { return m_KeyPos; }
    VECTOR GetExitPosition() const { return m_ExitPos; }
private:
   
    //座標を保存しておく変数
    VECTOR m_KeyPos = VGet(0, 0, 0);
    VECTOR m_ExitPos = VGet(0, 0, 0);

    void GenerateMaze();                //迷路生成メイン処理
    void Dig(int x, int y);            //穴掘り法（再帰）
    void BreakWalls(int breakCount);    //ループ作成用の壁破壊
    void PlaceEnemies();               //敵の配置
    void DrawMinimap();                   //ミニマップの描画

    int m_mazeData[STAGE_HEIGHT][STAGE_WIDTH]; //0:通路, 1:壁

    int m_floorGraph; //床の画像ハンドル
    int m_wallGraph;  //壁の画像ハンドル
	int m_bgGraph; //背景の画像ハンドル

    int m_keyGraph;      //鍵の画像用
    bool m_hasKey;       //鍵を拾ったかどうかのフラグ
    int m_seKeyGet;   //鍵を拾った音
    VECTOR m_keyPos;     //鍵を置く座標

    //出口
	int m_doorGraph; //閉じた扉の画像ハンドル
    int m_doorOpenGraph; //開いた扉の画像ハンドル
    int m_exitX, m_exitY; //出口の場所
    int m_seDoorOpen; //扉が開いた音

	//コンパス
	Compass m_compass;

    float bgScrollX;

	bool m_collisionEnable;//当たり判定の有効・無効フラグ（デバッグ用）
};