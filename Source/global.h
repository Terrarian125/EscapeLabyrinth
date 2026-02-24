#pragma once

//2次元ベクトル（整数）
struct Point
{
	int x;
	int y;
};
//2次元ベクトル（浮動小数点数）
struct Pointf
{
	float x;
	float y;
};

//矩形を表すよ
struct Rect {
	int x;//左上x
	int y;//左上y
	int w;//幅
	int h;//高さ
};

enum DIR
{
	UP = 0,
	DOWN,
	LEFT,
	RIGHT,
	NONE,
	MAX_DIR
};

//迷路のサイズとキャラのサイズ
//迷路のsizeは奇数でないとダメ
const int STAGE_WIDTH = 29;
const int STAGE_HEIGHT = 21;
const int CHA_SIZE = 32; //キャラのサイズ 32*32
const float STAGE_SCALE = 400.0f;

//ゲームクリアのフラグ
extern bool isGameClear; // 外部からも参照できるようにする