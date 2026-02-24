#include "Compass.h"
#include <cmath>

Compass::Compass() : m_barGraph(-1), m_baseMarker(-1) {}

Compass::~Compass() {}

void Compass::Init() {
    m_barGraph = LoadGraph("Data/Image/compass_bar.png");
    m_baseMarker = LoadGraph("Data/Image/marker_default.png");
}

void Compass::Draw(float centerAngle, const VECTOR& playerPos, const std::vector<CompassTarget>& targets) {
    int screenW = 1280; //画面解像度に合わせて調整
    int barW = 600;     //コンパスの表示幅
    int barH = 40;
    int barX = (screenW - barW) / 2;
    int barY = 30;

    //土台のバーを描画
    DrawExtendGraph(barX, barY, barX + barW, barY + barH, m_barGraph, TRUE);

    //ターゲットを計算
    for (const auto& target : targets) {
        //プレイヤーからターゲットへの角度を計算
        //atan2(x2-x1, z2-z1)
        float targetAngle = atan2f(target.pos.x - playerPos.x, target.pos.z - playerPos.z);

        //向きの差を求める（-PI ～ PI）
        float diff = targetAngle - centerAngle;
        while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;
        while (diff > DX_PI_F)  diff -= DX_PI_F * 2.0f;

        //視野角（左右90度以内なら表示）
        float viewRange = DX_PI_F / 2.0f;
        if (fabs(diff) < viewRange) {
            //角度差を画面上のX座標に変換 (-300px ～ +300px)
            float ratio = diff / viewRange; //-1.0 ～ 1.0
            int markerX = (screenW / 2) + static_cast<int>(ratio * (barW / 2));

            //指定された画像があれば使い、なければデフォルト
            int gh = (target.graphHandle != -1) ? target.graphHandle : m_baseMarker;

            //アイコンを描画（サイズは適宜調整）
            DrawGraph(markerX - 10, barY + 5, gh, TRUE);
        }
    }
}