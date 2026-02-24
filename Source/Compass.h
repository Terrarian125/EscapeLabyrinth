/// <summary>
/// コンパス（目的地を示すUI）
/// 他のゲームでも使えるようにしたい
/// </summary>

#pragma once
#include <vector>
#include <DxLib.h>

// 目的地情報の構造体
struct CompassTarget {
    VECTOR pos;       // ワールド座標
    int graphHandle;  // 表示したいアイコン画像
};

class Compass {
public:
    Compass();
    ~Compass();

    //初期化（バーの画像と、デフォルトのマーカー画像を読み込む）
    void Init();

    //描画
    // centerAngle: プレイヤーの向いている角度（ラジアン）
    // playerPos: プレイヤーの現在地
    // targets: 表示したいターゲットのリスト
    void Draw(float centerAngle, const VECTOR& playerPos, const std::vector<CompassTarget>& targets);

private:
    int m_barGraph;    // コンパスの土台（横長のバー）
    int m_baseMarker;  // 基本のマーカー画像
};