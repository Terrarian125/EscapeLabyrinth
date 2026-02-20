#pragma once
#include "../Library/GameObject.h"
#include "global.h"
#include <vector>

class Stage : public GameObject {
public:
    Stage();
    virtual ~Stage();

    void Update() override;
    void Draw() override;

    //外部（敵のAIなど）から道かどうかを判定するための関数
    int GetMazeData(int x, int y) const { return m_mazeData[y][x]; }

private:
    void GenerateMaze();                //迷路生成メイン処理
    void Dig(int x, int y);            //穴掘り法（再帰）
    void BreakWalls(int breakCount);    //ループ作成用の壁破壊
    void PlaceEnemies();               //敵の配置
	void WidenPaths();              //通路を広げる

    int m_mazeData[STAGE_HEIGHT][STAGE_WIDTH]; //0:通路, 1:壁

    int m_floorGraph; // 床の画像ハンドル
    int m_wallGraph;  // 壁の画像ハンドル
};