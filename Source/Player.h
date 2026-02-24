#pragma once
#include "../Library/GameObject.h"

class Player : public GameObject {
public:
    Player();
    void Update() override;
    void Draw() override {}

    VECTOR GetPosition() const { return m_pos; }
    float GetAngle() const { return m_angle; }
    //デバッグ用、座標の設定
    void SetPosition(VECTOR pos) { m_pos = pos; }
private:
    VECTOR m_pos;   //座標
    float m_angle;  //向き
};